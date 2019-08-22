/*
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
*/

#ifndef _SAPLAY_H_
#define _SAPLAY_H_

#include <stdint.h>
#include <stdbool.h>

// external library which understands different formats
#include <sndfile.h>

#include <pulse/pulseaudio.h>

#define MAX_SA_SINKS 6 // having 6 sound inputs seems very reasonable
#define STREAM_INDEX_NULL UINT32_MAX

// max that can play simultaneously. We have 5 or 6 defined now.
#define MAX_EFFECTS 20
#define MAX_ZONES 8
#define MAX_NAME_SZ 40     // bytes in a name
#define MAX_FILE_SZ 120     // bytes in a file
#define MAX_SPEAKER_SZ 40


typedef struct sa_soundplay {

	pa_stream *stream; // gets reset to NULL when file is over
  uint32_t stream_index; // this id is used to change volume with the sink set volume by id call
	char *stream_name;

	char *filename;
  char *dev; // device

	int verbose;

	pa_volume_t volume;

  SNDFILE* sndfile;
  pa_sample_spec sample_spec; // is this valid c?  
  pa_channel_map channel_map;

	sf_count_t (*readf_function)(SNDFILE *_sndfile, void *ptr, sf_count_t frames);
} sa_soundplay_t;


typedef struct sa_sink {
    bool active;
    char *dev; // also known as "name" in some interfaces, malloc'd
                // have to pass this to pa_stream_connect_playback
    int index; // the index is used for setting per-speaker volume
    int position; // this is the position on the back of the raspberry pi by looking at USB bus info 
    char speaker[MAX_SPEAKER_SZ];
    int volume; // if it wasn't connected when got a sink request, stash hear for create time
} sa_sink_t;

// the scape plays on all speakers attached to this pi
typedef struct sa_soundscape {

    bool is_playing;
    char *filename;

    int n_splays;

    sa_soundplay_t *splays[MAX_SA_SINKS];

    int volume; // persists a bit!

} sa_soundscape_t;


// useful type, a void function returning void
typedef void (*callback_fn_t) (void);

/* Forward References */

// http
extern int g_http_port;
extern bool sa_http_request(const char *url, char **result, size_t *result_len);

// files - quick accessors to find the files for the different Effects and Backgrounds
extern bool sa_filedb_effect_get(const char *effect, int intensity, char **filename );
extern bool sa_filedb_background_get(const char *background, char ** filename);
extern bool sa_filedb_init(const char *filename );
// uses all statics. No freeing.

//scape
   // use this from non-pulse audio threads
extern bool sa_scape_submit(const char *scape_string);
   // use this from pulse audio threads
extern bool sa_scape_process(const char *scape_string);
extern void sa_scape_timer();
extern void sa_scape_free();


// Saplay

typedef struct g_speakers_s {
  char name[MAX_SPEAKER_SZ];
  int  position;
} g_speakers_t;

extern int g_n_speakers;
extern g_speakers_t g_speakers[12];


extern pa_context *g_context;
extern bool g_context_connected;

extern char *g_zone;

extern void quit(int ret);

// These volumes are pulse volumes
extern void sa_soundplay_start(sa_soundplay_t *);
extern bool sa_soundplay_playing(sa_soundplay_t *);
extern void sa_soundplay_free(sa_soundplay_t *);
extern void sa_soundplay_volume_set(sa_soundplay_t *, pa_volume_t);

// These volumes are 0 to 100
extern sa_soundscape_t *sa_soundscape_new(char *filename, int volume);
extern bool sa_soundscape_filename_change(sa_soundscape_t *scape, char *filename, int volume);
extern void sa_soundscape_timer(sa_soundscape_t *scape);
extern void sa_soundscape_volume_change(sa_soundscape_t *scape, int volume);
extern void sa_soundscape_free(sa_soundscape_t *scape);

extern void sa_sinks_populate( pa_context *c, callback_fn_t next_fn );

// httpd
extern bool sa_http_start(void); // false if fail
extern void sa_http_terminate(void);

// Stream
extern void stream_volume_set(sa_soundplay_t *splay, pa_volume_t volume);
extern void stream_drain_complete(pa_stream *s, int success, void *userdata);
extern void stream_write_callback(pa_stream *s, size_t length, void *userdata);
extern void stream_state_callback(pa_stream *s, void *userdata);

extern int g_verbose;

#endif // _SAPLAY_H_
