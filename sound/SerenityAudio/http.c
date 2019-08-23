/***
  SerenityAudio

  This sound service, written to work with PulseAudio,
  is an HTTP driven service which will response to REST / HTTP
  commands from an interactive sculpture and will drive a set of speakers.
  It is written intending to be run on a Raaspberry Pi.

This file makes requests of the controlling server, and gets a JSON file
which will be acted on.

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

#include <curl/curl.h>

#include "saplay.h"

typedef struct write_data_s {
  char *buf;
  size_t alloc_sz;
  size_t len;
} write_data_t;


static size_t curlWriteFunction(void *ptr, size_t size/*always==1*/,
                                size_t nmemb, void* userdata)
{
    fprintf(stderr,"curl write function: ptr %p size %zu nmemb %zu userdata %p\n",ptr,size,nmemb,userdata);

    size_t realSize = size * nmemb; // kinda sumb because size is always 1 but api is api
    write_data_t *buf = (write_data_t *)userdata;

    if(nmemb==0) return 0;

    if (buf->alloc_sz < (buf->len + realSize + 1) ) {
      fprintf(stderr, "reallloc in curl function\n");
      buf->alloc_sz = buf->len + realSize + 1;
      buf->buf = realloc(buf->buf, buf->alloc_sz);
    }
    memcpy(buf->buf + buf->len, ptr, realSize);
    buf->len += realSize;
    buf->buf[buf->len] = 0;

    fprintf(stderr," leaving curl write function\n");

    return realSize;
}

bool sa_http_request(const char *url, char **result, size_t *result_len) {

  fprintf(stderr,"starting HTTP request %s\n",url);

  CURL *curl = 0;
  CURLcode res;
  
  write_data_t write_data = {0};
 
  curl = curl_easy_init();
  if (!curl) {
    fprintf(stderr, " could not init curl ");
  }
  fprintf(stderr," curl object is: %p\n",curl);

  // start with a reasonable buffer. Seems like these usually fit in 1k.
  write_data.buf = malloc(1000);
  write_data.alloc_sz = 1000;
  write_data.len = 0;

  curl_easy_setopt(curl, CURLOPT_URL, url);
  /* example.com is redirected, so we tell libcurl to follow redirection */ 
  curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

  curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&write_data);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curlWriteFunction);

  // signals would be bad
  curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 0);
  // but we're on a local network wtihout DNS. Don't wait long.
  curl_easy_setopt(curl, CURLOPT_TIMEOUT, 2L);

  // start with a reasonable buffer. Seems like these usually fit in 1k.
  write_data.buf = malloc(1000);
  write_data.alloc_sz = 1000;
  write_data.len = 0;

  /* Perform the request, res will get the return code */ 
  fprintf(stderr, "about to curl easy perform %s\n",url);
  res = curl_easy_perform(curl);
  fprintf(stderr, "got curl result\n");

  /* Check for errors */ 
  if(res != CURLE_OK) {
    fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
    curl_easy_cleanup(curl);
    return false;
  }

  long response_code;
  curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
  if (response_code != 200) {
    fprintf(stderr, "curl received response, bad error code %ld\n",response_code);
    curl_easy_cleanup(curl);
    return false;
  }

  //fprintf(stderr, "curl received data %s\n",data);

  /* always cleanup */ 
  curl_easy_cleanup(curl);

  *result = write_data.buf;
  *result_len = write_data.len;

  return true;
}



