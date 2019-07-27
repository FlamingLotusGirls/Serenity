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

typedef struct sa_soundplay {

	pa_stream *stream; // gets reset to NULL when file is over

	char *stream_name;
	char *filename;
    char *dev; // device

	int verbose;

	pa_volume_t volume;

  SNDFILE* sndfile;
  pa_sample_spec sample_spec; // is this valid c?  
  pa_channel_map channel_map;
  bool channel_map_set;

	sf_count_t (*readf_function)(SNDFILE *_sndfile, void *ptr, sf_count_t frames);
} sa_soundplay_t;


typedef struct sa_sink {
    bool active;
    char *dev; // also known as "name" in some interfaces, malloc'd
                // have to pass this to pa_stream_connect_playback
    int index;
    // oh, I'm sure there are more things to map
} sa_sink_t;

// the scape plays on all speakers attached to this pi
typedef struct sa_soundscape {

    int n_splays;

    sa_soundplay_t *splays[MAX_SA_SINKS];

} sa_soundscape_t;

typedef struct sa_sound_ambient {

  

} sa_sound_ambient_t;

// useful type, a void function returning void
typedef void (*callback_fn_t) (void);

/* Forward References */

// Saplay

extern pa_context *g_context;
extern bool g_context_connected;


extern void quit(int ret);

extern void sa_soundplay_start(sa_soundplay_t *);
extern void sa_soundplay_free(sa_soundplay_t *);

extern sa_soundscape_t *sa_soundscape_new(char *filename);

extern void sa_sinks_populate( pa_context *c, callback_fn_t next_fn );

// httpd
extern bool sa_http_start(void); // false if fail
extern void sa_http_terminate(void);

// Stream
extern void stream_drain_complete(pa_stream *s, int success, void *userdata);
extern void stream_write_callback(pa_stream *s, size_t length, void *userdata);
extern void stream_state_callback(pa_stream *s, void *userdata);

extern int g_verbose;

#endif // _SAPLAY_H_
