/*  =========================================================================
    zs_pipe - ZeroScript data pipe

    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of the ZeroScript language, http://zeroscript.org.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

/*
@header
    A pipe is an ordered list of strings and numbers.
@discuss
@end
*/

#include "zs_classes.h"

//  This holds an item in our value queue
typedef struct {
    int64_t number;
    char *string;
    char type;
} value_t;

//  Structure of our class
struct _zs_pipe_t {
    zlistx_t *values;           //  Simple stupid implementation
    value_t *current;           //  Last held value
    char number [20];           //  Current numeric value as string
};

static value_t *
s_value_new (const char *string, int64_t number)
{
    value_t *self = (value_t *) zmalloc (sizeof (value_t) + (string? strlen (string) + 1: 0));
    if (self) {
        if (string) {
            self->type = 's';
            self->string = &self->type + 1;
            strcpy (self->string, string);
        }
        else {
            self->type = 'n';
            self->number = number;
        }
    }
    return self;
}

static void
s_value_destroy (value_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        value_t *self = *self_p;
        free (self);
        *self_p = NULL;
    }
}


//  ---------------------------------------------------------------------------
//  Create a new zs_pipe, return the reference if successful, or NULL
//  if construction failed due to lack of available memory.

zs_pipe_t *
zs_pipe_new (void)
{
    zs_pipe_t *self = (zs_pipe_t *) zmalloc (sizeof (zs_pipe_t));
    if (self) {
        self->values = zlistx_new ();
        zlistx_set_destructor (self->values, (czmq_destructor *) s_value_destroy);
    }
    return self;
}


//  ---------------------------------------------------------------------------
//  Destroy the zs_pipe and free all memory used by the object.

void
zs_pipe_destroy (zs_pipe_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        zs_pipe_t *self = *self_p;
        zlistx_destroy (&self->values);
        s_value_destroy (&self->current);
        free (self);
        *self_p = NULL;
    }
}


//  ---------------------------------------------------------------------------
//  Add numeric value to pipe

void
zs_pipe_put_number (zs_pipe_t *self, int64_t number)
{
    zlistx_add_end (self->values, s_value_new (NULL, number));
}


//  ---------------------------------------------------------------------------
//  Add string value to pipe

void
zs_pipe_put_string (zs_pipe_t *self, const char *string)
{
    zlistx_add_end (self->values, s_value_new (string, 0));
}


//  ---------------------------------------------------------------------------
//  Return number of values in pipe

size_t
zs_pipe_size (zs_pipe_t *self)
{
    return zlistx_size (self->values);
}


//  ---------------------------------------------------------------------------
//  Return true if pipe is not empty, and next value off pipe is numeric

bool
zs_pipe_isnumber (zs_pipe_t *self)
{
    value_t *value = zlistx_first (self->values);
    if (value)
        return value->type == 'n';
    else
        return false;
}


//  ---------------------------------------------------------------------------
//  Return true if next value off pipe is numeric

bool
zs_pipe_isstring (zs_pipe_t *self)
{
    value_t *value = zlistx_first (self->values);
    if (value)
        return value->type == 's';
    else
        return false;
}


//  ---------------------------------------------------------------------------
//  Return next value off pipe as number; if the value is a string it's
//  converted to a number, quite brutally.

int64_t
zs_pipe_get_number (zs_pipe_t *self)
{
    s_value_destroy (&self->current);
    self->current = (value_t *) zlistx_detach (self->values, NULL);
    if (self->current) {
        if (self->current->type == 'n')
            return self->current->number;
        else 
            return atol (self->current->string);
    }
    else
        return 0;
}


//  ---------------------------------------------------------------------------
//  Return next value off pipe as string (converting if needed). Caller
//  should not modify value; this is managed by pipe class.

const char *
zs_pipe_get_string (zs_pipe_t *self)
{
    s_value_destroy (&self->current);
    self->current = (value_t *) zlistx_detach (self->values, NULL);
    if (self->current) {
        if (self->current->type == 's')
            return self->current->string;
        else {
            snprintf (self->number, 20, "%" PRId64, self->current->number);
            return self->number;
        }
    }
    else
        return NULL;
}


//  ---------------------------------------------------------------------------
//  Return pipe contents, as string. Caller must free it when done. Values are
//  separated by spaces.

char *
zs_pipe_contents (zs_pipe_t *self)
{
    //  Calculate length of resulting string
    size_t result_size = 1;
    value_t *value = (value_t *) zlistx_first (self->values);
    while (value) {
        if (value->type == 's')
            result_size += strlen (value->string) + 1;
        else {
            char formatted [20];
            snprintf (formatted, 20, "%" PRId64, value->number);
            result_size += strlen (formatted) + 1;
        }
        value = (value_t *) zlistx_next (self->values);
    }
    //  Now format the result
    char *result = zmalloc (result_size);
    value = (value_t *) zlistx_first (self->values);
    while (value) {
        if (*result)
            strcat (result, " ");
        if (value->type == 's')
            strcat (result, value->string);
        else {
            char formatted [20];
            snprintf (formatted, 20, "%" PRId64, value->number);
            strcat (result, formatted);
        }
        value = (value_t *) zlistx_next (self->values);
    }
    return result;
}


//  ---------------------------------------------------------------------------
//  Empty the pipe of any values it might contain.

void
zs_pipe_purge (zs_pipe_t *self)
{
    zlistx_purge (self->values);
}


//  ---------------------------------------------------------------------------
//  Selftest

void
zs_pipe_test (bool verbose)
{
    printf (" * zs_pipe: ");
    if (verbose)
        printf ("\n");

    //  @selftest
    zs_pipe_t *pipe = zs_pipe_new ();

    zs_pipe_put_number (pipe, 12345);
    assert (zs_pipe_isnumber (pipe));
    zs_pipe_put_string (pipe, "Hello World");
    assert (zs_pipe_size (pipe) == 2);

    assert (zs_pipe_get_number (pipe) == 12345);
    const char *string = zs_pipe_get_string (pipe);
    assert (streq (string, "Hello World"));
    assert (zs_pipe_size (pipe) == 0);

    char *results = zs_pipe_contents (pipe);
    assert (streq (results, ""));

    zs_pipe_put_number (pipe, 12345);
    zs_pipe_put_number (pipe, 12345);
    zs_pipe_put_number (pipe, 12345);
    assert (zs_pipe_size (pipe) == 3);
    zs_pipe_purge (pipe);
    assert (zs_pipe_size (pipe) == 0);
    
    zs_pipe_destroy (&pipe);
    //  @end
    printf ("OK\n");
}
