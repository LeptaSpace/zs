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
    A pipe is an ordered list of strings and wholes.
@discuss
@end
*/

#include "zs_classes.h"

//  These could be defined as quarks

static double
s_string_to_real (char *string)
{
    char *end = string;
    double real = strtod (string, &end);
    return end > string? real: ZS_NULL_REAL;
}

static int64_t
s_string_to_whole (char *string)
{
    errno = 0;
    int64_t whole = (int64_t) strtoll (string, NULL, 10);
    return errno == 0? whole: ZS_NULL_WHOLE;
}

//  Longest int64_t value has 20 digits
//  http://en.wikipedia.org/wiki/IEEE_754-1985 says max 24 chars
#define NSTR_MAX    25          //  Plus one for null char

static void
s_whole_to_string (int64_t whole, char *result)
{
    int size = snprintf (result, NSTR_MAX, "%" PRId64, whole);
    assert (size < NSTR_MAX);
}

static void
s_real_to_string (double real, char *result)
{
    int size = snprintf (result, NSTR_MAX, "%g", real);
    assert (size < NSTR_MAX);
}


//  This holds an item in our value queue
typedef struct {
    int64_t whole;
    double real;
    char *string;               //  Points to byte after 'type'
    char type;                  //  Keep this at the end of the structure
} value_t;

//  Structure of our class
struct _zs_pipe_t {
    zlistx_t *values;           //  Simple stupid implementation
    value_t *current;           //  Last held value
    char number [NSTR_MAX];     //  Current numeric value as string
};

static value_t *
s_value_new (int64_t whole, double real, const char *string)
{
    value_t *self = (value_t *) zmalloc (sizeof (value_t) + (string? strlen (string) + 1: 0));
    if (self) {
        if (whole != ZS_NULL_WHOLE) {
            self->type = 'w';
            self->whole = whole;
        }
        else
        if (real != ZS_NULL_REAL) {
            self->type = 'r';
            self->real = real;
        }
        else
        if (string) {
            self->type = 's';
            self->string = &self->type + 1;
            strcpy (self->string, string);
        }
        else
            assert (false);
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

//  Convert value_t object to whole
static int64_t
s_value_whole (value_t *self)
{
    if (self) {
        if (self->type == 'w')
            return self->whole;
        else
        if (self->type == 'r')
            return self->real > 0? (int64_t) (self->real + 0.5): (int64_t) (self->real - 0.5);
        else
        if (self->type == 's')
            return s_string_to_whole (self->string);
        else
            assert (false);
    }
    return ZS_NULL_WHOLE;
}

//  Convert value_t object to real
static double
s_value_real (value_t *self)
{
    if (self) {
        if (self->type == 'w')
            return (double) self->whole;
        else
        if (self->type == 'r')
            return self->real;
        else
        if (self->type == 's')
            return s_string_to_real (self->string);
        else
            assert (false);
    }
    return ZS_NULL_WHOLE;
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
//  Send a whole whole to the pipe; values are sent and received in
//  FIFO order, except for the "pull" method (see below), which pulls
//  back the last sent value.

void
zs_pipe_whole_send (zs_pipe_t *self, int64_t whole)
{
    zlistx_add_end (self->values, s_value_new (whole, ZS_NULL_REAL, NULL));
}


//  ---------------------------------------------------------------------------
//  Receive the next whole whole from the pipe. If the pipe is empty or
//  next value cannot be treated as a whole whole, returns ZS_NULL_WHOLE.
//  This call never blocks.

int64_t
zs_pipe_whole_recv (zs_pipe_t *self)
{
    s_value_destroy (&self->current);
    self->current = (value_t *) zlistx_detach (self->values, NULL);

    return self->current? s_value_whole (self->current): ZS_NULL_WHOLE;
}


//  ---------------------------------------------------------------------------
//  Pull the last sent whole whole from the pipe. Returns the value of the
//  whole whole or ZS_NULL_WHOLE if the last value could not be converted,
//  or the pipe was empty.

int64_t
zs_pipe_whole_pull (zs_pipe_t *self)
{
    s_value_destroy (&self->current);
    if (zlistx_last (self->values))
        self->current = (value_t *) zlistx_detach (self->values, zlistx_cursor (self->values));

    return self->current? s_value_whole (self->current): ZS_NULL_WHOLE;
}


//  ---------------------------------------------------------------------------
//  Send a real whole to the pipe; values are sent and received in
//  FIFO order, except for the "pull" method (see below), which pulls
//  back the last sent value.

void
zs_pipe_real_send (zs_pipe_t *self, double real)
{
    zlistx_add_end (self->values, s_value_new (ZS_NULL_WHOLE, real, NULL));
}


//  ---------------------------------------------------------------------------
//  Receive the next real whole from the pipe. If the pipe is empty or
//  next value cannot be treated as a real whole, returns ZS_NULL_REAL.
//  This call never blocks.

double
zs_pipe_real_recv (zs_pipe_t *self)
{
    s_value_destroy (&self->current);
    self->current = (value_t *) zlistx_detach (self->values, NULL);

    return self->current? s_value_real (self->current): ZS_NULL_REAL;
}


//  ---------------------------------------------------------------------------
//  Pull the last sent real whole from the pipe. Returns the value of the
//  real whole or ZS_NULL_REAL if the last value could not be converted,
//  or the pipe was empty.

double
zs_pipe_real_pull (zs_pipe_t *self)
{
    s_value_destroy (&self->current);
    if (zlistx_last (self->values))
        self->current = (value_t *) zlistx_detach (self->values, zlistx_cursor (self->values));

    return self->current? s_value_real (self->current): ZS_NULL_REAL;
}


//  ---------------------------------------------------------------------------
//  Send a string to the pipe; values are sent and received in FIFO order,
//  except for the "pull" method (see below), which pulls back the last sent
//  value.

void
zs_pipe_string_send (zs_pipe_t *self, const char *string)
{
    zlistx_add_end (self->values, s_value_new (ZS_NULL_WHOLE, ZS_NULL_REAL, string));
}


//  ---------------------------------------------------------------------------
//  Receive the next string from the pipe. If the pipe is empty, returns NULL.
//  This call never blocks. Caller should not modify value; this is managed by
//  the pipe class.

char *
zs_pipe_string_recv (zs_pipe_t *self)
{
    s_value_destroy (&self->current);
    self->current = (value_t *) zlistx_detach (self->values, NULL);

    if (self->current) {
        if (self->current->type == 'w') {
            s_whole_to_string (self->current->whole, self->number);
            return self->number;
        }
        else
        if (self->current->type == 'r') {
            s_real_to_string (self->current->real, self->number);
            return self->number;
        }
        else
        if (self->current->type == 's')
            return self->current->string;
        else
            assert (false);
    }
    else
        return NULL;
}


//  ---------------------------------------------------------------------------
//  Pull the last sent string from the pipe. Returns the value of the string
//  or NULL if the pipe was empty. Caller should not modify value; this is
//  managed by the pipe class.

char *
zs_pipe_string_pull (zs_pipe_t *self)
{
    s_value_destroy (&self->current);
    if (zlistx_last (self->values))
        self->current = (value_t *) zlistx_detach (self->values, zlistx_cursor (self->values));

    if (self->current) {
        if (self->current->type == 'w') {
            s_whole_to_string (self->current->whole, self->number);
            return self->number;
        }
        else
        if (self->current->type == 'r') {
            s_real_to_string (self->current->real, self->number);
            return self->number;
        }
        else
        if (self->current->type == 's')
            return self->current->string;
        else
            assert (false);
    }
    else
        return NULL;
}


//  ---------------------------------------------------------------------------
//  Return whole of values in pipe (0 or more)

size_t
zs_pipe_size (zs_pipe_t *self)
{
    return zlistx_size (self->values);
}


//  ---------------------------------------------------------------------------
//  Return pipe contents, as string. Caller must free it when done. Values are
//  separated by spaces. This empties the pipe.
//  TODO: make this an atomic (paste)

char *
zs_pipe_paste (zs_pipe_t *self)
{
    //  Calculate length of resulting string
    size_t result_size = 1;         //  For final NULL
    value_t *value = (value_t *) zlistx_first (self->values);
    char number [NSTR_MAX];         //  Current numeric value as string
    while (value) {
        if (value->type == 'w') {
            s_whole_to_string (value->whole, number);
            result_size += strlen (number) + 1;
        }
        else
        if (value->type == 'r') {
            s_real_to_string (value->real, number);
            result_size += strlen (number) + 1;
        }
        else
        if (value->type == 's')
            result_size += strlen (value->string) + 1;

        value = (value_t *) zlistx_next (self->values);
    }
    //  Now format the result
    char *result = (char *) zmalloc (result_size);
    value = (value_t *) zlistx_first (self->values);
    while (value) {
        if (*result)
            strcat (result, " ");
        if (value->type == 'w') {
            s_whole_to_string (value->whole, number);
            strcat (result, number);
        }
        else
        if (value->type == 'r') {
            s_real_to_string (value->real, number);
            strcat (result, number);
        }
        else
        if (value->type == 's')
            strcat (result, value->string);

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

    zs_pipe_whole_send (pipe, 12345);
    zs_pipe_string_send (pipe, "Hello World");
    assert (zs_pipe_size (pipe) == 2);

    assert (zs_pipe_whole_recv (pipe) == 12345);
    const char *string = zs_pipe_string_recv (pipe);
    puts (string);
    assert (streq (string, "Hello World"));
    assert (zs_pipe_size (pipe) == 0);

    char *results = zs_pipe_paste (pipe);
    assert (streq (results, ""));

    zs_pipe_whole_send (pipe, 4);
    zs_pipe_whole_send (pipe, 5);
    zs_pipe_whole_send (pipe, 6);
    assert (zs_pipe_size (pipe) == 3);
    zs_pipe_purge (pipe);
    assert (zs_pipe_size (pipe) == 0);

    zs_pipe_destroy (&pipe);
    //  @end
    printf ("OK\n");
}
