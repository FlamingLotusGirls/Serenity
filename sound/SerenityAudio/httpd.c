/***
  SerenityAudio

  This sound service, written to work with PulseAudio,
  is an HTTP driven service which will response to REST / HTTP
  commands from an interactive sculpture and will drive a set of speakers.
  It is written intending to be run on a Raaspberry Pi.

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
#include <sys/select.h>
#include <sys/socket.h>

#include <stdio.h>
#include <string.h>

#include <microhttpd.h>

#include "saplay.h"

// My connection info just accumulates the response by reallocing (so far)

// NOTE there is much talk in the microhttpd documentation about creating a postprocessor.
// This doesn't just try to parse some headers, it tries to parse the data.
// since the content type "application/json" is not known, it doesn't do anything.
// A more sensible system would take any application defined type and ignore it
// It seems if you don't define a postprocessor, you just get called with data over and over,
// which is fine, although it does mean 

struct connection_info {
  size_t  expected_size;
  size_t  alloc_size; // going to alloc +1 longer for a terminating null
  size_t  offset; // current spot you are reading
  char *data;
};

// I will usually get posts less than 1k
#define POST_BUFFER_SIZE (1024 * 2)

int header_iterator (void *cls, enum MHD_ValueKind kind, const char *key, const char *value) {
  struct connection_info *ci = (struct connection_info *) cls;

  fprintf(stderr,"header type %x key %s value %s\n",kind,key,value);
  if (strcmp(key, "Content-Length") == 0) {
    char *endptr;
    ci->expected_size = strtol(value,&endptr,10);
  }
  else if (strcmp(key,"Content-Type")==0) {
    if (strcmp("application/json",value) != 0) {
      fprintf(stderr, "http server expecting only application/json, found %s\n",value);
    }
  }

  return MHD_YES;
}

int httpd_request_handler (void *cls, struct MHD_Connection *connection,
                          const char *url,
                          const char *method, const char *version,
                          const char *upload_data,
                          size_t *upload_data_size, void **con_cls)
{

  struct connection_info *ci;

  fprintf(stderr, "httpd_request_handler called\n");

  // the con_cls is a structure which is used to distinguish between incoming
  // connections. On a new connection, you get called with NULL, and you need
  // to return a unique value. malloc() is pretty good at returning unique values?
  // and after that point you get called with your con_cls, where you can accumulate
  // the post data

  // In order to populate the size in a sensible way, you have to call this 
  // header iterator function. That'll allow you to validate the content
  // length and to determine the size and allocate an optimial buffer
  if (NULL == *con_cls)
  {

      fprintf(stderr, "httpd_request_handler: no con_cls, creating\n");

      ci = malloc (sizeof (struct connection_info));
      memset(ci, 0, sizeof(struct connection_info));

      *con_cls = (void *) ci;

      MHD_get_connection_values(connection, MHD_HEADER_KIND, header_iterator, ci);

      if (ci->expected_size) {
        ci->data = malloc(ci->expected_size + 1);
        ci->alloc_size = ci->expected_size + 1; // for null termination
      }

      fprintf(stderr, "httpd_request_handler: size will be %zu\n",ci->expected_size);

      return MHD_YES;
  }

  // have my connection info.... parse data in
  ci = *con_cls;
  if (*upload_data_size) {
    if (ci->data == NULL) {
      ci->data = malloc((*upload_data_size) + 1);
      ci->alloc_size = (*upload_data_size) + 1;
    }
    if (((*upload_data_size) + ci->offset) < ( ci->alloc_size + 1 ) )   {
      fprintf(stderr, "have to realloc the http buffer\n");
      ci->data = realloc(ci->data, ci->offset + (*upload_data_size) + 1);
      ci->alloc_size = ci->offset + (*upload_data_size) + 1;
    }
    memcpy(ci->data,upload_data,*upload_data_size);
    ci->offset += *upload_data_size;
  }

  bool complete = ci->offset == ci->expected_size ? true : false;

  if (complete == false) {
    // get more responses!
    return MHD_YES;
  }

  // past here, got all the data we are expecting, handle it
  fprintf(stderr, "http request handler called url %s method %s\n",url,method);

  int ret = 0;
  bool parsed = false;

  // null terminate incoming, just makes things more pleasant
  if ( ci->alloc_size <= ci->expected_size) {
    fprintf(stderr, "internal error, not enough space to null terminate incoming http request\n");
    return MHD_NO;
  }
  * ((char *)(ci->data + ci->offset )) = 0;

  if (strcmp(url,"/soundscape") == 0) {
    // should have an extra byte at the end?

    fprintf(stderr, "httpd received soundscape: %s\n",ci->data);

    if (strcmp(method,"PUT") != 0) {
      fprintf(stderr, "server expecting method PUT but no worries\n");
    }

    parsed = sa_scape_process(ci->data);

  }
  else if (strcmp(url,"/sinks") == 0) {
    if (strcmp(method,"PUT") != 0) {
      fprintf(stderr, "server expecting method PUT but no worries\n");
    }
  }

  // send the response off to be parsed by the audio system.

  struct MHD_Response *response = MHD_create_response_from_buffer (strlen ("OK"),
  	                                        (void*) "OK", MHD_RESPMEM_PERSISTENT);

  ret = MHD_queue_response (connection, parsed ? MHD_HTTP_OK : 400, response);
  MHD_destroy_response (response);

  return ret;
}


static void
request_completed (void *cls, struct MHD_Connection *connection,
                   void **con_cls, enum MHD_RequestTerminationCode toe)
{
  struct connection_info *con_info = *con_cls;


  if (NULL == con_info)
    return;

  if (con_info->data) free(con_info->data);

  free (con_info);
  *con_cls = NULL;

}

static struct MHD_Daemon *g_mhd_daemon;

bool sa_http_start(void) {

  fprintf(stderr,"starting HTTP server port %d\n",g_http_port);

  // this kind of start returns immediately and then there is a thread
  // spawned to do epoll
 	g_mhd_daemon = MHD_start_daemon (MHD_USE_EPOLL_INTERNALLY, 
  				g_http_port, NULL, NULL,
                &httpd_request_handler, NULL, 
                MHD_OPTION_NOTIFY_COMPLETED, request_completed,
                NULL, MHD_OPTION_END);

 	if (NULL == g_mhd_daemon) {
    fprintf(stderr, "could not start HTTP server\n");
    return false;
  }

	return(true);
}

void sa_http_terminate(void) {
	if (g_mhd_daemon) {
	  MHD_stop_daemon (g_mhd_daemon);
	  g_mhd_daemon = 0;
	}
}


