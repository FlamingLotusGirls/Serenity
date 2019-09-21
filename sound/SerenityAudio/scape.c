/***
  SerenityAudio

  This sound service, written to work with PulseAudio,
  is an HTTP driven service which will response to REST / HTTP
  commands from an interactive sculpture and will drive a set of speakers.
  It is written intending to be run on a Raaspberry Pi.

This file parses an incoming JSON object, which it got from the HTTP server
or from making an HTTP request, and acts on it.

It uses the 'scape' structure,

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
#include <sys/types.h>

#include <stdio.h>
#include <string.h>

// need me some mutex I think
#include <pthread.h>

// Jannson for parsing Json
#include <jansson.h>

#include "saplay.h"

struct effect
{
    char name[MAX_NAME_SZ];
    int intensity;
    int volume;

    sa_soundscape_t *splay; // if currently playing this one
};


//
// This will hold the current value.
// You get something that changes, then you go whack its player

struct scape
{

    char bg_name[MAX_NAME_SZ];
    int bg_volume;
    sa_soundscape_t *bg_splay; // should always have one, really

    int n_effects;
    struct effect effects[MAX_EFFECTS];

    int zone_volume; // only for my zone

    int master_volume;

};

struct scape g_scape = {0};

// Local Debug
static bool ldebug = false;

static pthread_mutex_t scape_submit_lock = PTHREAD_MUTEX_INITIALIZER;

typedef struct submit_queue_e_s {
  struct submit_queue_e_s *next;
  const char *data;
} submit_queue_e;

static submit_queue_e * submit_queue_head = NULL;


//
// WORD TO THE WISE
// The jansson library has a type 'json_int_t'. This is defined in modern systems as a 'long long'
// ( which is 64 bits ). It seems the printf I'm using is savvy to the type conversion issue, and will
// not fix the issue of casting a long long to a %d properly, and gets out of sync. This is annoying
// given how advanced the rest of the compiler is.
// In the case here, we are really working with small values, so some kind of "safe cast" would be nice
// but whatever, but this is why there are a number of strategic casts from a type that looks like an int, into an int

// in this rather manual async system, there is a timer required by every

void sa_scape_timer(void)
{

    // process the queue
    char *scape_data;
    do {

      const char *scape_data;

      pthread_mutex_lock(&scape_submit_lock);

      scape_data = NULL;

      if (submit_queue_head) {
        submit_queue_e *t = submit_queue_head;
        scape_data = t->data;
        submit_queue_head = t->next;
        free(t);
      }

      pthread_mutex_unlock(&scape_submit_lock);

      if (scape_data) sa_scape_process(scape_data);
    }
    while (scape_data);

    if (g_scape.bg_splay) sa_soundscape_timer(g_scape.bg_splay);

    for (int i = 0; i < g_scape.n_effects; i++)
    {
        if (g_scape.effects[i].splay) sa_soundscape_timer(g_scape.effects[i].splay);
    }

}

void sa_scape_free(void)
{
    if (g_scape.bg_splay) sa_soundscape_free(g_scape.bg_splay);

    for (int i = 0; i < g_scape.n_effects; i++)
    {
        if (g_scape.effects[i].splay) sa_soundscape_free(g_scape.effects[i].splay);
    }

}

// The HTTP server is on a different thread, so I have to make a copy
// of the data and process it in the timer thread.
// A fancier system would parse inline and act on it later....
bool sa_scape_submit( const char *scape_str) {

    if (ldebug) fprintf(stderr, "sa_scape_submit: %s\n",scape_str);

  // might as well copy the data outside the loop
  const char *data = strdup( scape_str );
  submit_queue_e *e = malloc(sizeof(submit_queue_e));
  e->data = data;
  e->next = 0;

  // put on queue, to be fair, do it in order ( insert to tail )
  pthread_mutex_lock(&scape_submit_lock);

  submit_queue_e **tail = &submit_queue_head;
  while (*tail) {
    tail = & ((*tail)->next);
  }
  *tail = e;

  pthread_mutex_unlock(&scape_submit_lock);

  return true;

}


// Grabs the config filename from the global and resets sound players and volumes based on that
//

bool sa_scape_process(const char *scape_str)
{

    json_error_t    js_err;

    if ( ldebug ) fprintf(stderr, "scape process: data %s\n", scape_str);

    // load the config json file
    json_auto_t *js_root = json_loads(scape_str, JSON_DECODE_ANY | JSON_DISABLE_EOF_CHECK, &js_err);

    if (js_root == NULL)
    {
        fprintf(stderr, "JSON scape parse failed on %s\n", scape_str);
        fprintf(stderr, "position: (%d,%d)  %s\n", js_err.line, js_err.column, js_err.text);
        return(false);
    }

    json_auto_t *j_background = json_object_get(js_root, "background");
    if (j_background)
    {
        json_auto_t *j_bg_n = json_object_get(j_background, "name");
        const char *n = json_string_value(j_bg_n); // exists as long as j_bg_n
        json_auto_t *j_bg_v = json_object_get(j_background, "volume");
        json_int_t v = j_bg_v ? json_integer_value(j_bg_v) : 0;

        // has the name changed?
        if (0 != strcmp(n, g_scape.bg_name))
        {

            // TODO: stop playing any old background and play this one, with the correct value
            if (ldebug) fprintf(stderr, "TODO: scapeprocess: got new background %s volume %d\n", n, (int) v);

            // get the new filename - static string, don't need to free
            char *filename;
            if (false == sa_filedb_background_get(n, &filename) )
            {
                fprintf(stderr, "unknown background name %s, shouldn't happen\n", n);
                return(false);
            }

            if (ldebug) fprintf(stderr, "scape: background filename %s\n", filename);

            // if there is no background create one & start playing
            if (g_scape.bg_splay == NULL)
            {
                g_scape.bg_splay = sa_soundscape_new(filename, (int) v);
		        if (ldebug) fprintf(stderr, "creating soundscape bg %p\n",g_scape.bg_splay);
            }

            // if we already had one, change the filename
            else
            {
                sa_soundscape_filename_change(g_scape.bg_splay, filename, (int) v);
		        if (ldebug) fprintf(stderr, "bgscape filename change: %p\ni",g_scape.bg_splay);
            }


            strncpy(g_scape.bg_name, n, sizeof(g_scape.bg_name));
            g_scape.bg_volume = v;
        }
        // has the volume changed?
        else if ( j_bg_v && (g_scape.bg_volume != v))
        {

            // TODO: volume changed on bg
            if (ldebug) fprintf(stderr, "new background volume %d\n", (int)v);

            if (g_scape.bg_splay) sa_soundscape_volume_change(g_scape.bg_splay, (int) v);

            g_scape.bg_volume = v;
        }

    }
    else
    {
        if (ldebug) fprintf(stderr, "scapeprocess: no background object found\n");
    }

    json_auto_t *j_effects = json_object_get(js_root, "effects");
    if (j_effects)
    {
        const char *key;
        json_t *value;
        json_object_foreach(j_effects, key, value)
        {
            // skip names
            if (strcmp(key, "names") == 0) {
                continue;
            }

            // broken: allowed to not set either of these.
            json_t *j_intensity = json_object_get(value, "intensity");
            json_int_t intensity = json_integer_value(j_intensity);
            // WARNING. If you put this in, there will be SOME crashes.
            //json_decref(j_intensity);

            json_t *j_volume = json_object_get(value, "volume");
            json_int_t volume = json_integer_value(j_volume);
            // WARNING if you put this in, there might be some crashes?
            //json_decref(j_volume);

            // find the name in effects
            bool found = false;
            for(int eff_idx = 0; eff_idx < g_scape.n_effects; eff_idx++)
            {
	           if (ldebug) fprintf(stderr, "scapeprocess: looking for %s iteratings %d\n",key,eff_idx);

                struct effect *e = &g_scape.effects[eff_idx];
                if (0 == strcmp( e->name, key ) )
                {

                    // todo! Check intensity, check volume, do the things
                    if (ldebug) fprintf(stderr, "scapeprocess: effect %s intensity %d volume %d\n", key, (int)intensity, (int)volume);

                    found = true;

                    // change of intensity changes the filename, but zero also means start and stop
                    if ( intensity != e->intensity )
                    {
                        // new intensity of 0 means kill it
                        if (intensity == 0)
                        {
                            if (ldebug) fprintf(stderr, "scape: effect kill %s splay %p\n",key,e->splay);
                            sa_soundscape_free(e->splay);
                            e->splay = NULL;
                        }
                        // new intensity not zero means a change of file
                        else
                        {
                            char *filename;
                            if (false == sa_filedb_effect_get(e->name, intensity, &filename) )
                            {
                                fprintf(stderr, "unknown2 effect name %s, shouldn't happen\n", e->name);
                                return(false);
                            }
                            if (e->splay == NULL)
                            {
                                if (ldebug) fprintf(stderr, "scape: no splay, create new scape %s filename %s volume %d\n",key,filename,e->volume);
                                e->splay = sa_soundscape_new(filename, (int) volume);
			                    if (ldebug) fprintf(stderr, "scape: effect %s new soundscape %p eptr %p \n",key,e->splay,&(e));
                            }
                            else
                            {
			                    if (ldebug) fprintf(stderr, "filename change: splay %p\n",e->splay);
                                sa_soundscape_filename_change(e->splay, filename, (int) volume);
                            }
                        }

                    }
                    else if (volume != e->volume)
                    {
	                    if (ldebug) fprintf(stderr, "volume change: soundscape %p\n",e->splay);
                        if (e->splay) sa_soundscape_volume_change(e->splay, (int) volume);
                    }
                    e->volume = (int) volume;
                    e->intensity = (int) intensity;

		              break; // found it don't keep looking
                }
            };
            // didn't find it, add it for next time
            if (!found)
            {
		      if (ldebug) fprintf(stderr, "could not find referenced effect, creating \n");

                if (g_scape.n_effects >= MAX_EFFECTS) {
                  fprintf(stderr, "scape: exceeded number of effects should not happen\n");
                  return(false);
                }
                struct effect *e = &g_scape.effects[g_scape.n_effects];
	            if (ldebug) fprintf(stderr, "new scape: e %p\n",e);
                strncpy(e->name, key, sizeof(e->name));
                e->volume = (int)volume;
                e->intensity = (int)intensity;

                // todo: if ntensity greater than zero, start
                if (ldebug) fprintf(stderr, "scapeprocess: new effect: %s intensity %d volume %d\n", e->name, e->intensity, e->volume);

                if ( intensity > 0)
                {

                    char *filename;
                    if (false == sa_filedb_effect_get(e->name, e->intensity, &filename) )
                    {
                        fprintf(stderr, "unknown effect name %s, intensity %d, shouldn't happen\n", e->name, e->intensity);
                        return(false);
                    }

                    e->splay = sa_soundscape_new(filename, (int) e->volume);
		            if (ldebug) fprintf(stderr, "new soundscape: filename %s splay %p\n",filename,e->splay);

                }
                g_scape.n_effects++;

            }

        }
    }
    else
    {
        if (ldebug) fprintf(stderr, "scapeprocess: no effects object found\n");
    }

    json_auto_t *j_zones = json_object_get(js_root, "zones");
    if (j_zones)
    {

        // just looking for my zone
        json_auto_t *j_zone = json_object_get(j_zones, g_zone);
        if (j_zone)
        {
            json_auto_t *j_v = json_object_get(j_zone, "volume");
            if (j_v)
            {
                json_int_t volume = json_integer_value(j_v);

                if (volume != g_scape.zone_volume)
                {

                    // TODO! volume on my zone changed

                    if (ldebug) fprintf(stderr, "my zone volume change: %d\n", volume);

                    g_scape.zone_volume = volume;

                }

            }
        }
    }
    else
    {
        if (ldebug) fprintf(stderr, "scapeprocess: no zones object found\n");
    }

    json_auto_t *j_master = json_object_get(js_root, "master");
    if (j_master)
    {
        json_auto_t *j_mv = json_object_get(j_master, "volume");
        if (j_mv)
        {
            json_int_t volume = json_integer_value(j_mv);
            if (volume != g_scape.master_volume)
            {

                // TODO! volume has changed
                if (ldebug) fprintf(stderr, "new master volume: %d\n", volume);

                g_scape.master_volume = volume;
            }
        }

    }
    else
    {
        if (ldebug) fprintf(stderr, "scapeprocess: no master object found\n");
    }

}


