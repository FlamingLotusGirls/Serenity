/***
  SerenityAudio

  This sound service, written to work with PulseAudio,
  is an HTTP driven service which will response to REST / HTTP
  commands from an interactive sculpture and will drive a set of speakers.
  It is written intending to be run on a Raaspberry Pi.

  Based on a PulseAudio example file by Lennart Poettering and Pierre Ossman.
  However, very little remains of the original code.

*
* Written by Brian Bulkowski (bbulkow) 7/7/2019 to compile and run with PulseAudio 10.0
*

Copyright (c) 2019 Brian Bulkowski

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

***/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <signal.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <locale.h>
#include <stdbool.h>

// Jannson for parsing Json
#include <jansson.h>


#include "saplay.h"

int g_verbose = 0;


static char *g_sound_directory = NULL; // loaded from the config file
static char *g_admin_config_filename = NULL;
char *g_zone = NULL;
static char *g_config_filename = "config.json";
int g_http_port = 0;

static char *g_admin_url = NULL; // the URL to pull scapes from
static char *g_scape_data = NULL; // most recent data fetches ( or, first time only? )
static size_t g_scape_data_len = 0;

static sa_sink_t g_sa_sinks[MAX_SA_SINKS] = {0}; // null terminated array of pointers
static bool g_sa_sinks_configured = false;

pa_context *g_context = NULL;
bool g_context_connected = false;

static pa_mainloop_api *g_mainloop_api = NULL;

static char *g_client_name = NULL, *g_device = NULL;

static pa_volume_t g_volume = PA_VOLUME_NORM;

static pa_time_event *g_timer = NULL;

// My timer will fire every 50ms
//#define TIME_EVENT_USEC 50000
#define TIME_EVENT_USEC 100000


/* A shortcut for terminating the application */
void quit(int ret)
{
    assert(g_mainloop_api);
    g_mainloop_api->quit(g_mainloop_api, ret);
}

/* Connection draining complete */
static void context_drain_complete(pa_context *c, void *userdata)
{
    pa_context_disconnect(c);
}


/* This is called whenever the context status changes */
/* todo: creating the stream as soon as the context comes available is kinda fun, but
** we really want something else
*/
static void context_state_callback(pa_context *c, void *userdata)
{

    if (g_verbose)
    {
        fprintf(stderr, "context state callback, new state %d\n", pa_context_get_state(c) );
    }

    // just making sure???
    assert(c);

    switch (pa_context_get_state(c))
    {
    case PA_CONTEXT_CONNECTING:
    case PA_CONTEXT_AUTHORIZING:
    case PA_CONTEXT_SETTING_NAME:
        break;

    case PA_CONTEXT_READY:
    {
        assert(c);

        if (g_verbose)
            fprintf(stderr, "Connection established.\n");
        g_context_connected = true;

        break;
    }

    case PA_CONTEXT_TERMINATED:
        quit(0);
        break;

    case PA_CONTEXT_FAILED:
    default:
        fprintf(stderr, "Connection failure: %s\n", pa_strerror(pa_context_errno(c)));
        quit(1);
    }
}

/* UNIX signal to quit recieved */
static void exit_signal_callback(pa_mainloop_api *m, pa_signal_event *e, int sig, void *userdata)
{
    if (g_verbose)
        fprintf(stderr, "Got SIGINT, exiting.\n");
    quit(0);
}


// open it and set it for async playing
// eventually can add delays and whatnot

// Filename of null means use stdin... or is always passed in?
// filename is a static and not to be freed

static sa_soundplay_t *sa_soundplay_new( char *filename, char *dev, pa_volume_t volume )
{

    sa_soundplay_t *splay = malloc(sizeof(sa_soundplay_t));
    memset(splay, 0, sizeof(sa_soundplay_t) );  // typically don't do this, do every field, but doing it this time

    SF_INFO sfinfo;

    // initialize many things from the globals at this point
    pa_channel_map_init_stereo(&splay->channel_map); // bug waiting to happen. Might not be a stereo file.
    splay->volume = volume;
    splay->verbose = g_verbose;

    // open file
    memset(&sfinfo, 0, sizeof(sfinfo));

    splay->sndfile = sf_open(filename, SFM_READ, &sfinfo);
    splay->filename = strdup(filename);
    splay->dev = strdup(dev);

    // Todo: have an error code
    if (!splay->sndfile)
    {
        fprintf(stderr, "Failed to open file '%s'\n", filename);
        sa_soundplay_free(splay);
        return(NULL);
    }

    splay->sample_spec.rate = (uint32_t) sfinfo.samplerate;
    splay->sample_spec.channels = (uint8_t) sfinfo.channels;

    splay->readf_function = NULL;

    switch (sfinfo.format & 0xFF)
    {
    case SF_FORMAT_PCM_16:
    case SF_FORMAT_PCM_U8:
    case SF_FORMAT_PCM_S8:
        splay->sample_spec.format = PA_SAMPLE_S16NE;
        splay->readf_function = (sf_count_t (*)(SNDFILE * _sndfile, void *ptr, sf_count_t frames)) sf_readf_short;
        break;

    case SF_FORMAT_ULAW:
        splay->sample_spec.format = PA_SAMPLE_ULAW;
        break;

    case SF_FORMAT_ALAW:
        splay->sample_spec.format = PA_SAMPLE_ALAW;
        break;

    case SF_FORMAT_FLOAT:
    case SF_FORMAT_DOUBLE:
    default:
        splay->sample_spec.format = PA_SAMPLE_FLOAT32NE;
        splay->readf_function = (sf_count_t (*)(SNDFILE * _sndfile, void *ptr, sf_count_t frames)) sf_readf_float;
        break;
    }

    if (!splay->stream_name)
    {
        const char *n, *sn;

        // WARNING. No information about whether this function is returning
        // newly allocated memory that must be freed, or a pointer inside
        // the soundfile. Could thus be a memory leak
        n = sf_get_string(splay->sndfile, SF_STR_TITLE);

        if (!n)
            n = filename;
        // this returns a string that must be freed with pa_xfree()
        splay->stream_name = pa_locale_to_utf8(n);
        if (!sn)
            splay->stream_name = pa_utf8_filter(n);

    }

    // better have had a context - don't know if it's connected though?
    assert(g_context);

    if (splay->verbose)
    {
        char t[PA_SAMPLE_SPEC_SNPRINT_MAX];
        pa_sample_spec_snprint(t, sizeof(t), &splay->sample_spec);
        fprintf(stderr, "created play file using sample spec '%s'\n", t);
    }

    return(splay);

}

bool sa_soundplay_playing(sa_soundplay_t *splay)
{
    if ( splay->stream ) return true;
    return false;
}

void sa_soundplay_play( sa_soundplay_t *splay)
{

    if (splay->stream)
    {
        fprintf(stderr, "Called start on already playing stream %s\n", splay->stream_name);
        return;
    }
    if (g_context == NULL)
    {
        // THIS IS OK, there's a startup period where it will keep trying
        return;
    }
    if (splay->verbose) fprintf(stderr, "soundplay start: %s\n", splay->stream_name);

    // have to open a new soundfile, but already have the key parameters
    if (splay->sndfile == NULL)
    {

        SF_INFO sfinfo;
        memset(&sfinfo, 0, sizeof(sfinfo));

        splay->sndfile = sf_open(splay->filename, SFM_READ, &sfinfo);
        assert(splay->sndfile);
    }

    splay->stream = pa_stream_new(g_context, splay->stream_name, &splay->sample_spec, &splay->channel_map );
    assert(splay->stream);

    pa_cvolume cv;

    pa_stream_set_state_callback(splay->stream, stream_state_callback, splay);
    pa_stream_set_write_callback(splay->stream, stream_write_callback, splay);
    pa_stream_connect_playback(splay->stream, splay->dev, NULL/*buffer_attr*/, 0/*flags*/,
                               pa_cvolume_set(&cv, splay->sample_spec.channels, splay->volume),
                               NULL/*sync stream*/);

    // Test code: what is the stream index? Can I use that to inedpendantly control
    // volume?
    //fprintf(stderr,"stream %s: index %d\n",splay->stream_name, pa_stream_get_index(splay->stream));

}

// terminate a given sound
void sa_soundplay_terminate( sa_soundplay_t *splay)
{
    if (splay->stream)
    {
        pa_stream_disconnect(splay->stream);
        if (splay->verbose)
        {
            fprintf(stderr, "terminating stream %s will get a callback for draining", splay->stream_name);
        }
    }
    else
    {
        fprintf(stderr, "soundplay_terminate but no stream in progress");
    }

}

void sa_soundplay_free( sa_soundplay_t *splay )
{
    if (splay->stream) pa_stream_unref(splay->stream);
    if (splay->stream_name) pa_xfree(splay->stream_name);
    if (splay->sndfile) sf_close(splay->sndfile);
    if (splay->dev) free(splay->dev);
    if (splay->filename) free(splay->filename);

    free(splay);
}

/*
** sa_soundscape
** a "soundscape" is made of all the speakers playing a particular loop.
**
** NOTE This should be refactors, because we now think of a 'scape'
** as a collection of loops, and this is only one loop.
** which is either a background or an effect.
** we don't have a general name for the single thing, and this
** should be refactored to that.... someday....
*/

static bool sa_soundscape_playing(sa_soundscape_t *scape) {
    return(scape->is_playing);
}

static pa_volume_t to_pavolume(int vol) {
    if (vol <= 0) return( PA_VOLUME_MUTED );
    double pct = (double ) vol / 100.0 ;
    // bump?
    pct += 0.4;
    // if you don't want floats, use ( vol * PA_VOLUME_NORM ) / 100 
    //fprintf(stderr, "input vol %d norm %d pct %f output vol %d\n",vol, (int) PA_VOLUME_NORM, pct, (pa_volume_t) (  PA_VOLUME_NORM * pct ) );
    return ( pct * PA_VOLUME_NORM );
}


static void sa_soundscape_play(sa_soundscape_t *scape)
{
    if (false == g_sa_sinks_configured) return;

    for(int i = 0 ; i < MAX_SA_SINKS ; i++)
    {
        if (g_sa_sinks[i].active)
        {
            if (g_verbose) fprintf(stderr, "new soundscape: new soundplay: sink %s\n", g_sa_sinks[i].dev);
            scape->splays[i] = sa_soundplay_new(scape->filename, g_sa_sinks[i].dev, to_pavolume( scape->volume ) );
            if (!scape->splays[i]) {
                fprintf(stderr, "could not create soundplay from soundscape, kinda fucked\n");
                return;
            }
            sa_soundplay_play(scape->splays[i]);
            scape->n_splays++;
        }
    }
    scape->is_playing = true;
}

sa_soundscape_t *sa_soundscape_new(char *filename, int volume)
{

    sa_soundscape_t *scape = malloc(sizeof(sa_soundscape_t));
    // defaults all happen to be zero
    memset( scape, 0, sizeof(sa_soundscape_t) );

    if (g_verbose) fprintf(stderr, "new soundscape: %s vol %d\n", filename,volume);

    // Filename is relative to sound directory. Slap on the prefix here.
    char full_filename[120];
    strcpy(full_filename, g_sound_directory);
    strcat(full_filename, filename);
    fprintf(stderr, "soundscape new: full filename %s\n", full_filename);
    scape->filename = strdup(full_filename);

    scape->volume = volume;

    sa_soundscape_play(scape);

    return(scape);

}

// Take an existing player, stop the existing loops,
// start new loops with the new filename

bool sa_soundscape_filename_change(sa_soundscape_t *scape, char *filename, int volume)
{


    return true;
}

void sa_soundscape_volume_change(sa_soundscape_t *scape, int  volume)
{

    scape->volume = volume;

    fprintf(stderr, "soundscape: change volume %d\n", volume);

    for (int i = 0; i < scape->n_splays; i++)
    {
        stream_volume_set(scape->splays[i], to_pavolume(  volume ) );
    }

}

void sa_soundscape_timer(sa_soundscape_t *scape)
{

    if (g_verbose) fprintf(stderr, "soundscape timer: scape %p n_splays %d\n", scape, scape->n_splays);

    if (false == sa_soundscape_playing(scape)) {
        sa_soundscape_play(scape);
    }

    for (int i = 0 ; i < scape->n_splays ; i++)
    {
        if (false == sa_soundplay_playing(scape->splays[i]))
        {
            sa_soundplay_play(scape->splays[i]);
        }
    }

}

void sa_soundscape_free( sa_soundscape_t *scape)
{
    if(scape->filename) free(scape->filename);

    for (int i = 0; i < scape->n_splays; i++)
    {
        if (scape->splays[i])
        {
            sa_soundplay_free(scape->splays[i]);
        }
    }

    free(scape);
}


/*
** timer - this is called frequently, and where we decide to start and stop effects.
** or loop them because they were completed or whatnot.
*/

static struct timeval g_start_time;
static bool g_started = false;

static bool g_volume_timer_set = false;
static struct timeval g_volume_timer_next;
static int g_volume_next;

/* pa_time_event_cb_t */
static void
sa_timer(pa_mainloop_api *a, pa_time_event *e, const struct timeval *tv, void *userdata)
{
    if (g_verbose) fprintf(stderr, "time event called: sec %d usec %d\n", tv->tv_sec, tv->tv_usec);

    // FIRST TIME AFTER CONTEXT IS CONNECTED
    if ( (g_started == false) && (g_context_connected == true))
    {

        if (g_verbose) fprintf(stderr, "first time started\n");

        // this will call the sinks to populate, and when that's done, call the
        // next function ( next function a placeholder these days?)
        sa_sinks_populate(g_context, NULL);

        // Kick off fetching the HTTP to load the initial JSON
        // Should have the initial JSON by this point, call the code to act on it
        if (g_scape_data)
        {
            if ( sa_scape_process(g_scape_data) )
            {
                fprintf(stderr, "successfully processed inital scape data\n");
            }
            else
            {
                fprintf(stderr, " initial scape processing failed\n");
            }
        }

        g_started = true;
        goto NEXT;
    }

    // This happens every time after the first
    sa_scape_timer();

    // put the things you want to happen in here

NEXT:
    ;

    struct timeval now;
    gettimeofday(&now, NULL);
    pa_timeval_add(&now, TIME_EVENT_USEC);
    a->time_restart(e, &now);

}

//
// This populates the static structures with teh indexes. It does not start with 0 and 1,
// the indexes ( which are the easiest way to talk about sinks ) increment as things are plugged
// and unplugged. Thus we want to iterate the structure and find out what's currently around.
//

static void sa_sink_list_cb(pa_context *c, const pa_sink_info *info, int eol, void *userdata)
{

    if (eol) {
        g_sa_sinks_configured = true;
        return;
    }

    callback_fn_t next_fn = (callback_fn_t) userdata;

    if (g_verbose) fprintf(stderr, "sink list callback:\n");

    // find next inactive sink, set it
    int i;
    for (i = 0; i < MAX_SA_SINKS; i++)
    {
        if (g_sa_sinks[i].active == false)
        {
            g_sa_sinks[i].active = true;
            g_sa_sinks[i].index = info->index;
            g_sa_sinks[i].dev = strdup(info->name);
            if (g_verbose) fprintf(stderr, "popuated index %d with idx %d dev %s\n", i, info->index, info->name);
            break;
        }
    }
    if (i == MAX_SA_SINKS)
    {
        fprintf(stderr, " WARNING: large number of sinks ( more than MAX_SINKS ), some ignored\n");
    }

    if (next_fn)
    {
        next_fn();
    }

    return;

}

void sa_sinks_populate( pa_context *c, callback_fn_t next_fn )
{

    // cleanup array
    for (int i = 0; i < MAX_SA_SINKS; i++)
    {
        if (g_sa_sinks[i].active && g_sa_sinks[i].dev)
        {
            free(g_sa_sinks[i].dev);
            g_sa_sinks[i].dev = 0;
        }
        g_sa_sinks[i].active = false;
    }

    pa_operation *o = pa_context_get_sink_info_list ( c, sa_sink_list_cb, next_fn /*userdata*/ );
    pa_operation_unref(o);

}

//
// Config
//

static bool config_load(const char *filename)
{

    // nice to have for debugging
    json_error_t    js_err;

    // note, the auto directories have fancieness because they free themselves
    // outside of scope
    json_auto_t *js_root = json_load_file(filename, JSON_DECODE_ANY | JSON_DISABLE_EOF_CHECK, &js_err);

    if (js_root == NULL)
    {
        fprintf(stderr, "JSON config parse failed on %s\n", filename);
        fprintf(stderr, "position: (%d,%d)  %s\n", js_err.line, js_err.column, js_err.text);
        return(false);
    }

    json_auto_t *js_sdir = json_object_get(js_root, "soundDir");
    if (!js_sdir)
    {
        fprintf(stderr, "dirctory not found in config file, fail");
        goto quit;
    }
    else
    {
        g_sound_directory = strdup( json_string_value(js_sdir) );
    }

    json_auto_t *js_zone = json_object_get(js_root, "zone");
    if (!js_zone)
    {
        fprintf(stderr, "zone not found in config file, fail");
        goto quit;
    }
    else
    {
        g_zone = strdup( json_string_value(js_zone) );
    }

    json_auto_t *js_admin_config_filename = json_object_get(js_root, "adminConfig");
    if (!js_admin_config_filename)
    {
        fprintf(stderr, "adminConfig not found in config file, fail");
        goto quit;
    }
    else
    {
        g_admin_config_filename = strdup( json_string_value(js_admin_config_filename) );
    }

    json_auto_t *js_admin_url = json_object_get(js_root, "adminUrl");
    if (!js_admin_url)
    {
        fprintf(stderr, "adminConfig not found in config file, fail");
        goto quit;
    }
    else
    {
        g_admin_url = strdup( json_string_value(js_admin_url) );
    }

    json_auto_t *js_http_port = json_object_get(js_root, "httpPort");
    if (!js_http_port) {
        fprintf(stderr, "httpPort not found in config file, using 8000\n");
        g_http_port = 8000;
    }
    else {
        g_http_port = json_integer_value(js_http_port);
    }

    if (g_verbose) fprintf(stderr, "config json file loaded successfully\n");
    return(true);

quit:
    return(false);

}


static void help(const char *argv0)
{

    printf("%s [options] [FILE]\n\n"
           "  -h, --help                            Show this help\n"
           "      --version                         Show version\n\n"
           "  -v, --verbose                         Enable verbose operation\n\n"
           "  -s, --server                          The name of the server to connect to\n"
           "  -n, --client-name=NAME                How to call this client on the server\n"
           "      --stream-name=NAME                How to call this stream on the server\n"
           "      --volume=VOLUME                   Specify the initial (linear) volume in range 0...65536\n"
           "      --channel-map=CHANNELMAP          Set the channel map to the use\n",
           argv0);
}

enum
{
    ARG_VERSION = 256,
    ARG_STREAM_NAME,
    ARG_CHANNELMAP
};

int main(int argc, char *argv[])
{
    pa_mainloop *m = NULL;
    int ret = 1, r, c;
    char *bn = NULL;
    char *server = NULL;
    char *stream_name = NULL;

    SF_INFO sfinfo;

    static const struct option long_options[] =
    {
        {"server",          1, NULL, 's'},
        {"client-name", 1, NULL, 'n'},
        {"stream-name", 1, NULL, ARG_STREAM_NAME},
        {"version",     0, NULL, ARG_VERSION},
        {"help",        0, NULL, 'h'},
        {"verbose",     0, NULL, 'v'},
        {NULL,          0, NULL, 0}
    };

    if (!(bn = strrchr(argv[0], '/')))
        bn = argv[0];
    else
        bn++;

    while ((c = getopt_long(argc, argv, "d:s:n:h", long_options, NULL)) != -1)
    {

        switch (c)
        {
        case 'h' :
            help(bn);
            ret = 0;
            goto quit;

        case 's':
            pa_xfree(server);
            server = pa_xstrdup(optarg);
            break;

        case 'n':
            pa_xfree(g_client_name);
            g_client_name = pa_xstrdup(optarg);
            break;

        case ARG_STREAM_NAME:
            pa_xfree(stream_name);
            stream_name = pa_xstrdup(optarg);
            break;

        case 'v':
            g_verbose = 1;
            break;

        default:
            goto quit;
        }
    }

    if (!g_client_name)
    {
        // must be freed with pa_xfree
        g_client_name = pa_locale_to_utf8(bn);
        if (!g_client_name)
            g_client_name = pa_utf8_filter(bn);
    }

    if (! config_load(g_config_filename))
    {
        goto quit;
    }

    if (sa_filedb_init(g_admin_config_filename) == false )
    {
        fprintf(stderr, "could not load admin config to get filenames");
        goto quit;
    }

    if (g_verbose)
    {
        fprintf(stderr, "about to set up mainloop\n");
    }

    /* set up the http server */
    if ( ! sa_http_start() )
    {
        fprintf(stderr, "could not start HTTP server\n");
        goto quit;
    }

    /* do the one initial HTTP request to get the older state */
    char scape_url[120];
    scape_url[0] = 0;
    strcpy(scape_url, g_admin_url);
    strcat(scape_url, "soundscape");
    fprintf(stderr, "pulling from url: %s\n", scape_url);
    if (sa_http_request(scape_url, &g_scape_data, &g_scape_data_len) == false)
    {
        fprintf(stderr, "could not fetch initial scapes, will wait for push\n");
    }
    else
    {
        fprintf(stderr, "initial fetch of scapes succeeded\n");
    }

    /* Set up a new main loop */
    if (!(m = pa_mainloop_new()))
    {
        fprintf(stderr, "pa_mainloop_new() failed.\n");
        goto quit;
    }

    g_mainloop_api = pa_mainloop_get_api(m);

    r = pa_signal_init(g_mainloop_api);
    assert(r == 0);
    pa_signal_new(SIGINT, exit_signal_callback, NULL);
#ifdef SIGPIPE
    signal(SIGPIPE, SIG_IGN);
#endif

    if (g_verbose)
    {
        fprintf(stderr, "about to create new context \n");
    }

    /* Create a new connection context */
    /* note: documentation says post 0.9, use with_proplist() and specify some defaults */
    g_context = pa_context_new(g_mainloop_api, g_client_name);
    if (!g_context)
    {
        fprintf(stderr, "pa_context_new() failed.\n");
        goto quit;
    }

    pa_context_set_state_callback(g_context, context_state_callback, NULL);

    /* Connect the context */
    if (pa_context_connect(g_context, server, 0, NULL) < 0)
    {
        fprintf(stderr, "pa_context_connect() failed: %s", pa_strerror(pa_context_errno(g_context)));
        goto quit;
    }

    if (g_verbose)
    {
        fprintf(stderr, "about to run mainloop\n");
    }

    /* set up our timer */
    struct timeval now;
    gettimeofday(&now, NULL);
    pa_timeval_add(&now, TIME_EVENT_USEC);
    g_timer = (* g_mainloop_api->time_new) (g_mainloop_api, &now, sa_timer, NULL);
    if (g_timer == NULL)
    {
        fprintf(stderr, "time_new failed!!!\n");
    }




    /* Run the main loop - hangs here forever? */
    if (pa_mainloop_run(m, &ret) < 0)
    {
        fprintf(stderr, "pa_mainloop_run() failed.\n");
        goto quit;
    }

quit:
    if (g_verbose)
    {
        fprintf(stderr, "quitting and cleaning up\n");
    }

    sa_http_terminate();

    if (g_context)
        pa_context_unref(g_context);

    if (g_sound_directory)      free(g_sound_directory);
    if (g_zone)                 free(g_zone);
    if (g_admin_config_filename) free(g_admin_config_filename);
    if (g_admin_url)            free(g_admin_url);
    if (g_scape_data)          free(g_scape_data);

    if (m)
    {
        pa_signal_done();
        pa_mainloop_free(m);
    }

    sa_scape_free();

    pa_xfree(server);
    pa_xfree(g_device);
    pa_xfree(g_client_name);
    pa_xfree(stream_name);

    return ret;
}
