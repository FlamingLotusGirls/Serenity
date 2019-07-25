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

#define HTTP_PORT 8000

int http_request_handler (void *cls, struct MHD_Connection *connection,
                          const char *url,
                          const char *method, const char *version,
                          const char *upload_data,
                          size_t *upload_data_size, void **con_cls)
{
	const char *page  = "<html><body>Hello, browser!</body></html>";
	struct MHD_Response *response;
	int ret;

  if (g_verbose) fprintf(stderr, "http request handler called\n");

	response = MHD_create_response_from_buffer (strlen (page),
	                                        (void*) page, MHD_RESPMEM_PERSISTENT);

  ret = MHD_queue_response (connection, MHD_HTTP_OK, response);
  MHD_destroy_response (response);

  return ret;
}

static struct MHD_Daemon *g_mhd_daemon;

bool sa_http_start(void) {

  if (g_verbose) fprintf(stderr,"starting HTTP server\n");

  // this kind of start returns immediately and then there is a thread
  // spawned to do epoll
 	g_mhd_daemon = MHD_start_daemon (MHD_USE_EPOLL_INTERNALLY, 
  				HTTP_PORT, NULL, NULL,
                &http_request_handler, NULL, MHD_OPTION_END);

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


