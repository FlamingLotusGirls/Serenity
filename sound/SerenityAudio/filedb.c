/***
  SerenityAudio

  This sound service, written to work with PulseAudio,
  is an HTTP driven service which will response to REST / HTTP
  commands from an interactive sculpture and will drive a set of speakers.
  It is written intending to be run on a Raaspberry Pi.

This file gets loaded with a JSON file, and provides
a very simple interface for finding the filenames of the audio files to play.

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

// Jannson for parsing Json
#include <jansson.h>

#include "saplay.h"


#define MAX_REGISTERED_EFFECTS 20
#define MAX_REGISTERED_BACKGROUNDS 40


struct effect
{
    char name[MAX_NAME_SZ];
    char file_i1[MAX_FILE_SZ];
    char file_i2[MAX_FILE_SZ];
    char file_i3[MAX_FILE_SZ];
};

struct background
{
    char name[MAX_NAME_SZ];
    char file[MAX_NAME_SZ];
};

int n_effects = 0;
struct effect effects[MAX_REGISTERED_EFFECTS] = {0};

int n_backgrounds = 0;
struct background backgrounds[MAX_REGISTERED_BACKGROUNDS] = {0};

static bool inited = false;


// files - quick accessors to find the files for the different Effects and Backgrounds
bool sa_filedb_effect_get(const char *effect, int intensity, char **filename )
{
    if (!inited)
    {
        fprintf(stderr, "filedb use without init\n");
        return false;
    }
    for (int i = 0; i < n_effects; i++)
    {
        if (0 == strcmp(effect, effects[i].name))
        {
            switch (intensity)
            {
            case 1:
                *filename = effects[i].file_i1;
                return true;
            case 2:
                *filename = effects[i].file_i2;
                return true;
            case 3:
                *filename = effects[i].file_i3;
                return true;
            default:
                fprintf(stderr, " requested bad effect intensity %d\n", intensity);
                return false;
            }

        }
    }
    return false;

}

bool sa_filedb_background_get(const char *background, char **filename)
{
    if (!inited)
    {
        fprintf(stderr, "filedb use without init\n");
        return false;
    }

    for (int i = 0; i < n_backgrounds; i++)
    {
        if (0 == strcmp(background, backgrounds[i].name))
        {
            *filename = backgrounds[i].file;
            return(true);
        }
    }
    return(false);

}


// Grabs the config filename from the global
//

bool sa_filedb_init(const char *config_filename)
{

    json_error_t    js_err;

    // don't call twice. Willg et confused.
    if ( inited ) return false;
    inited = true;

    // load in the adminConfig file
    json_auto_t *js_root = json_load_file(config_filename, JSON_DECODE_ANY | JSON_DISABLE_EOF_CHECK, &js_err);

    if (js_root == NULL)
    {
        fprintf(stderr, "JSON config parse failed on %s\n", config_filename);
        fprintf(stderr, "position: (%d,%d)  %s\n", js_err.line, js_err.column, js_err.text);
        return(false);
    }

    // used in looping names array
    size_t index;
    json_t *value;

    // Backgrounds
    int bg_index = 0;

    json_auto_t *bg = json_object_get(js_root, "backgrounds");
    json_auto_t *bg_n = json_object_get(bg, "names");
    json_array_foreach( bg_n, index, value )
    {
        const char *bg_name = json_string_value(value);
        json_auto_t *o = json_object_get(bg, bg_name);
        json_auto_t *fn = json_object_get(o, "file");

        strncpy(backgrounds[bg_index].name, bg_name, sizeof( backgrounds[bg_index].name) );
        strncpy(backgrounds[bg_index].file, json_string_value(fn), sizeof( backgrounds[bg_index].file ));


        if (g_verbose) fprintf(stderr, " parsed background: name %s file %s\n",
                                   backgrounds[bg_index].name, backgrounds[bg_index].file);

        json_decref(value);

        bg_index++;
        if (bg_index > MAX_REGISTERED_BACKGROUNDS)
            return(false);

    }
    n_backgrounds = bg_index;

    // effects
    int ef_index = 0;

    json_auto_t *ef = json_object_get(js_root, "effects");
    json_auto_t *ef_n = json_object_get(ef, "names");
    json_array_foreach( ef_n, index, value )
    {
        const char *ef_name = json_string_value(value);
        json_auto_t *o = json_object_get(ef, ef_name);
        json_auto_t *f1 = json_object_get(o, "file1");
        json_auto_t *f2 = json_object_get(o, "file2");
        json_auto_t *f3 = json_object_get(o, "file3");

        strncpy(effects[ef_index].name, ef_name, sizeof( effects[ef_index].name) );
        strncpy(effects[ef_index].file_i1, json_string_value(f1), sizeof( effects[ef_index].file_i1 ));
        strncpy(effects[ef_index].file_i2, json_string_value(f2), sizeof( effects[ef_index].file_i2 ));
        strncpy(effects[ef_index].file_i3, json_string_value(f3), sizeof( effects[ef_index].file_i3 ));


        if (g_verbose) fprintf(stderr, " parsed effects: name %s file1 %s file2 %s file3 %s\n",
                                   effects[ef_index].name, effects[ef_index].file_i2, effects[ef_index].file_i1, effects[ef_index].file_i3);

        json_decref(value);

        ef_index++;
        if (ef_index > MAX_REGISTERED_EFFECTS)
            return(false);

    }
    n_effects = ef_index;

}


