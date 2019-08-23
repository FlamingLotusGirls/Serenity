/***
  SerenityAudio

  This sound service, written to work with PulseAudio,
  is an HTTP driven service which will response to REST / HTTP
  commands from an interactive sculpture and will drive a set of speakers.
  It is written intending to be run on a Raaspberry Pi.

This file parses an incoming JSON object, which it got from the HTTP server
or from making an HTTP request, and acts on it.


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




// Local Debug
static bool ldebug = true;

static pthread_mutex_t sink_submit_lock = PTHREAD_MUTEX_INITIALIZER;

typedef struct submit_queue_e_s {
  struct submit_queue_e_s *next;
  const char *data;
} submit_queue_e;

static submit_queue_e * submit_queue_head = NULL;


void sa_sink_timer(void)
{

    // process the queue
    char *sink_data;
    do {

      const char *sink_data;

      pthread_mutex_lock(&sink_submit_lock);

      sink_data = NULL;

      if (submit_queue_head) {
        submit_queue_e *t = submit_queue_head;
        sink_data = t->data;
        submit_queue_head = t->next;
        free(t);
      }

      pthread_mutex_unlock(&sink_submit_lock);

      if (sink_data) sa_sink_process(sink_data);
    }
    while (sink_data);


}

// The HTTP server is on a different thread, so I have to make a copy
// of the data and process it in the timer thread.
// A fancier system would parse inline and act on it later....
bool sa_sink_submit( const char *sink_str) {

  if (ldebug) fprintf(stderr, "sink received a submit: %s\n",sink_str);

  // might as well copy the data outside the loop
  const char *data = strdup( sink_str );
  submit_queue_e *e = malloc(sizeof(submit_queue_e));
  e->data = data;
  e->next = 0;

  // put on queue, to be fair, do it in order ( insert to tail )
  pthread_mutex_lock(&sink_submit_lock);

  submit_queue_e **tail = &submit_queue_head;
  while (*tail) {
    tail = & ((*tail)->next);
  }
  *tail = e;

  pthread_mutex_unlock(&sink_submit_lock);

  return true;

}


// Grabs the config filename from the global and resets sound players and volumes based on that
//

bool sa_sink_process(const char *sink_str)
{

    json_error_t    js_err;

    if ( ldebug ) fprintf(stderr, "sink process: data %s\n", sink_str);

    // load in the adminConfig file
    json_auto_t *js_root = json_loads(sink_str, JSON_DECODE_ANY | JSON_DISABLE_EOF_CHECK, &js_err);

    if (js_root == NULL)
    {
        fprintf(stderr, "JSON sink parse failed on %s\n", sink_str);
        fprintf(stderr, "position: (%d,%d)  %s\n", js_err.line, js_err.column, js_err.text);
        return(false);
    }

    // TODO: parse the sinks. The format is simpler than some:
    // an object of { "sinkName": { "volume": 45}, "sinkName2": { "volume": 56} }
    const char *key;
    json_t *value;
    json_object_foreach(js_root, key, value) {

        if (ldebug) fprintf(stderr, "received sink object %s\n",key);
        json_auto_t *j_vol = json_object_get(value, "volume");
        if (j_vol) {
            int vol = (int) json_integer_value(j_vol);

            sa_sinks_volume_set( key, vol );

        }

    }

}


