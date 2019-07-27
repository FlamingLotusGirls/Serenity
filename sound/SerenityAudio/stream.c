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


/* Stream draining complete */
void stream_drain_complete(pa_stream *s, int success, void *userdata) {
    sa_soundplay_t *splay = (sa_soundplay_t *) userdata;
    pa_operation *o;

    if (!success) {
        fprintf(stderr, "Failed to drain stream: %s\n", pa_strerror(pa_context_errno(g_context)));
        quit(1);
    }

    if (splay->verbose)
        fprintf(stderr, "Playback stream %s drained.\n",splay->stream_name );

    pa_stream_disconnect(splay->stream);
    pa_stream_unref(splay->stream);
    splay->stream = NULL;

}

/* This is called whenever new data may be written to the stream */
void stream_write_callback(pa_stream *s, size_t length, void *userdata) {
    
	sa_soundplay_t *splay = (sa_soundplay_t *)userdata;

    sf_count_t bytes;
    void *data;

	//if (splay->verbose) fprintf(stderr,"stream write callback\n");

    assert(s && length);

    if (!splay->sndfile) {
		if (splay->verbose) fprintf(stderr, "write callback with no sndfile %s\n",splay->stream_name);
        return;
	}

    data = pa_xmalloc(length);

    if (splay->readf_function) {
        size_t k = pa_frame_size(&splay->sample_spec);

        if ((bytes = (splay->readf_function) (splay->sndfile, data, (sf_count_t) (length/k))) > 0)
            bytes *= (sf_count_t) k;

    } else {
        bytes = sf_read_raw(splay->sndfile, data, (sf_count_t) length);
	}

    if (bytes > 0)
        pa_stream_write(s, data, (size_t) bytes, pa_xfree, 0, PA_SEEK_RELATIVE);
    else
        pa_xfree(data);

    if (bytes < (sf_count_t) length) {
        sf_close(splay->sndfile);
        splay->sndfile = NULL;
        pa_operation_unref(pa_stream_drain(s, stream_drain_complete, userdata));
    }
}

/* This routine is called whenever the stream state changes */
void stream_state_callback(pa_stream *s, void *userdata) {
	sa_soundplay_t *splay = (sa_soundplay_t *)userdata;

	if (splay->verbose) {
		fprintf(stderr, "stream state callback: %d\n",pa_stream_get_state(s) );
	}

	// just making sure
    assert(s);

    switch (pa_stream_get_state(s)) {
        case PA_STREAM_CREATING:
        	break;
        case PA_STREAM_TERMINATED:
        	if (splay->verbose) fprintf(stderr, "stream %s terminated\n",splay->stream_name);
            break;

        case PA_STREAM_READY:
            if (splay->verbose)
                fprintf(stderr, "Stream successfully created\n");
            break;

        case PA_STREAM_FAILED:
        default:
            fprintf(stderr, "Stream errror: %s\n", pa_strerror(pa_context_errno(pa_stream_get_context(s))));
            quit(1);
    }
}
