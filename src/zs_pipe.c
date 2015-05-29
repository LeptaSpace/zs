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
#include "zs_strtod.c"


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
    value_t *value;             //  Register value, if any
    char string_value [30];     //  Register value as string
    size_t nbr_reals;           //  Number of reals on the pipe
};

static value_t *
s_value_new (const char *string)
{
    value_t *self = (value_t *) zmalloc (sizeof (value_t) + (string? strlen (string) + 1: 0));
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
        s_value_destroy (&self->value);
        free (self);
        *self_p = NULL;
    }
}


//  ---------------------------------------------------------------------------
//  Sets pipe register to contain a specified whole number; any previous
//  value in the register is lost.

void
zs_pipe_set_whole (zs_pipe_t *self, int64_t whole)
{
    if (!self->value)
        self->value = s_value_new (NULL);
    self->value->type = 'w';
    self->value->whole = whole;
}


//  ---------------------------------------------------------------------------
//  Sets pipe register to contain a specified real number; any previous
//  value in the register is lost.

void
zs_pipe_set_real (zs_pipe_t *self, double real)
{
    if (!self->value)
        self->value = s_value_new (NULL);

    self->value->type = 'r';
    self->value->real = real;
}


//  ---------------------------------------------------------------------------
//  Sets pipe register to contain a specified string; any previous value
//  in the register is lost.

void
zs_pipe_set_string (zs_pipe_t *self, const char *string)
{
    if (!self->value
    ||  self->value->type != 's'
    ||  strlen (self->value->string) < strlen (string)) {
        s_value_destroy (&self->value);
        self->value = s_value_new (string);
        self->value->type = 's';
        self->value->string = &self->value->type + 1;
    }
    strcpy (self->value->string, string);
}


//  ---------------------------------------------------------------------------
//  Sends current pipe register to the pipe; returns 0 if successful, or
//  -1 if the pipe register was empty. Clears the register.

int
zs_pipe_send (zs_pipe_t *self)
{
    if (self->value) {
        zlistx_add_end (self->values, self->value);
        self->nbr_reals += self->value->type == 'r'? 1: 0;
        self->value = NULL;
        return 0;
    }
    else
        return -1;
}


//  ---------------------------------------------------------------------------
//  Send whole number to pipe; this wipes the current pipe register.

void
zs_pipe_send_whole (zs_pipe_t *self, int64_t whole)
{
    zs_pipe_set_whole (self, whole);
    zs_pipe_send (self);
}


//  ---------------------------------------------------------------------------
//  Send real number to pipe; this wipes the current pipe register.

void
zs_pipe_send_real (zs_pipe_t *self, double real)
{
    zs_pipe_set_real (self, real);
    zs_pipe_send (self);
}


//  ---------------------------------------------------------------------------
//  Send string to pipe; this wipes the current pipe register.

void
zs_pipe_send_string (zs_pipe_t *self, const char *string)
{
    zs_pipe_set_string (self, string);
    zs_pipe_send (self);
}


//  ---------------------------------------------------------------------------
//  Returns true if the pipe contains at least one real number. Returns false
//  otherwise.

bool
zs_pipe_has_real (zs_pipe_t *self)
{
    return (self->nbr_reals > 0);
}


//  ---------------------------------------------------------------------------
//  Receives the next value off the pipe, into the register. Any previous
//  value in the register is lost. Returns true if a value was successfully
//  received. If no values were received, returns false. This method does
//  not block.

bool
zs_pipe_recv (zs_pipe_t *self)
{
    //  Skip any marks
    while (true) {
        s_value_destroy (&self->value);
        self->value = (value_t *) zlistx_detach (self->values, NULL);

        if (!self->value)
            return false;       //  Pipe is empty
        else
        if (isalpha (self->value->type)) {
            if (self->value->type == 'r')
                self->nbr_reals--;
            return true;        //  We had a normal value
        }
    }
}


//  ---------------------------------------------------------------------------
//  Returns the type of the register, 'w' for whole, 'r' for real, or 's' for
//  string. Returns -1 if the register is empty.

char
zs_pipe_type (zs_pipe_t *self)
{
    return self->value? self->value->type: -1;
}


//  ---------------------------------------------------------------------------
//  Returns the value of the register, coerced to a whole number. This can
//  cause loss of precision. If no conversion was possible, or the register
//  is empty, returns zero.

int64_t
zs_pipe_whole (zs_pipe_t *self)
{
    if (self->value) {
        if (self->value->type == 'w')
            return self->value->whole;
        else
        if (self->value->type == 'r')
            return self->value->real > 0?
                (int64_t) (self->value->real + 0.5):
                (int64_t) (self->value->real - 0.5);
        else
        if (self->value->type == 's') {
            errno = 0;
            int64_t whole = (int64_t) strtoll (self->value->string, NULL, 10);
            return errno == 0? whole: 0;
        }
    }
    return 0;
}


//  ---------------------------------------------------------------------------
//  Returns the value of the register, coerced to a real number. This can
//  cause loss of precision. If no conversion was possible, or the register
//  is empty, returns zero.

double
zs_pipe_real (zs_pipe_t *self)
{
    if (self->value) {
        if (self->value->type == 'w')
            return (double) self->value->whole;
        else
        if (self->value->type == 'r')
            return self->value->real;
        else
        if (self->value->type == 's') {
            char *end = self->value->string;
            double real = zs_strtod (self->value->string, &end);
            return end > self->value->string? real: 0;
        }
    }
    return 0;
}


//  ---------------------------------------------------------------------------
//  Returns the value of the register, coerced to a string if needed. If the
//  register is empty, returns an empty string "". The caller must not modify
//  or free the string.

char *
zs_pipe_string (zs_pipe_t *self)
{
    if (self->value) {
        if (self->value->type == 'w') {
            snprintf (self->string_value, sizeof (self->string_value),
                      "%" PRId64, self->value->whole);
            return self->string_value;
        }
        else
        if (self->value->type == 'r') {
            snprintf (self->string_value, sizeof (self->string_value),
                      "%.9g", self->value->real);
            return self->string_value;
        }
        else
        if (self->value->type == 's')
            return self->value->string;
    }
    return "";
}


//  ---------------------------------------------------------------------------
//  Receives the next value off the pipe, into the register, and coerces it
//  to a whole if needed. If there is no value to receive, returns 0.

int64_t
zs_pipe_recv_whole (zs_pipe_t *self)
{
    if (zs_pipe_recv (self))
        return zs_pipe_whole (self);
    else
        return 0;
}


//  ---------------------------------------------------------------------------
//  Receives the next value off the pipe, into the register, and coerces it
//  to a real if needed. If there is no value to receive, returns 0.

double
zs_pipe_recv_real (zs_pipe_t *self)
{
    if (zs_pipe_recv (self))
        return zs_pipe_real (self);
    else
        return 0;
}


//  ---------------------------------------------------------------------------
//  Receives the next value off the pipe, into the register, and coerces it
//  to a string if needed. If there is no value to receive, returns "". The
//  The caller must not modify or free the string.

char *
zs_pipe_recv_string (zs_pipe_t *self)
{
    if (zs_pipe_recv (self))
        return zs_pipe_string (self);
    else
        return 0;
}


//  ---------------------------------------------------------------------------
//  Marks an end of phrase in the pipe. This is used to delimit the pipe
//  as input for later function calls. Marks are ignored when receiving
//  values off a pipe.

void
zs_pipe_mark (zs_pipe_t *self)
{
    value_t *value = s_value_new (NULL);
    value->type = '|';          //  Non-alphabetic
    zlistx_add_end (self->values, value);
}

static void
s_pull_values (zs_pipe_t *self, zs_pipe_t *source)
{
    //  Pull values from cursor onwards
    while (true) {
        value_t *value = (value_t *) zlistx_detach_cur (source->values);
        if (!value)
            break;
        if (isalpha (value->type)) {
            zlistx_add_end (self->values, value);
            if (value->type == 'r') {
                source->nbr_reals--;
                self->nbr_reals++;
            }
        }
        else
            s_value_destroy (&value);

        value = (value_t *) zlistx_next (source->values);
        if (!value)
            break;
    }
}


//  ---------------------------------------------------------------------------
//  Pulls a list of values from the source pipe into the pipe. This function
//  does a "modest" pull: in a phrase, pulls the last single value. After a
//  phrase, pulls the preceding phrase. If the input is empty, provides a
//  constant '1' input.

void
zs_pipe_pull_modest (zs_pipe_t *self, zs_pipe_t *source)
{
    value_t *value = (value_t *) zlistx_last (source->values);
    if (value) {
        if (value->type == '|') {
            //  Pull last phrase; skip back until we hit the start of the
            //  pipe or a mark (before the current mark)
            value = (value_t *) zlistx_prev (source->values);
            while (value && value->type != '|')
                value = (value_t *) zlistx_prev (source->values);
        }
        //  Ensure cursor points to first valid value, if any
        if (!value)
            zlistx_first (source->values);
        else
        if (value->type == '|')
            zlistx_next (source->values);

        s_pull_values (self, source);
    }
    //  Push a constant 1 if the input is empty; a modest function always
    //  gets at least one input value
    if (zlistx_size (self->values) == 0)
        zs_pipe_send_whole (self, 1);
}


//  ---------------------------------------------------------------------------
//  Pulls a list of values from the source pipe into the pipe. This function
//  does a "greedy" pull: in a phrase, pulls the current phrase. After a
//  phrase, pulls the preceding entire sentence.

void
zs_pipe_pull_greedy (zs_pipe_t *self, zs_pipe_t *source)
{
    value_t *value = (value_t *) zlistx_last (source->values);
    if (!value)
        return;                 //  Nothing to do
    if (value->type == '|')
        //  Pull entire sentence
        value = (value_t *) zlistx_first (source->values);
    else {
        //  Pull current phrase; skip back until we hit the start of
        //  pipe or a mark
        while (value && value->type != '|')
            value = (value_t *) zlistx_prev (source->values);
    }
    //  Ensure cursor points to first valid value, if any
    if (!value)
        zlistx_first (source->values);
    else
    if (value->type == '|')
        zlistx_next (source->values);

    s_pull_values (self, source);
}


//  ---------------------------------------------------------------------------
//  Pulls a list of values from the source pipe into the pipe. This function
//  does an array pull: (a) move last value to input, then (b) move either rest
//  of current phrase, or entire previous phrase, to input.

void
zs_pipe_pull_array (zs_pipe_t *self, zs_pipe_t *source)
{
    value_t *value = (value_t *) zlistx_last (source->values);
    if (!value || value->type == '|')
        return;             //  Invalid, do nothing

    //  Move last value to input; this will be first provided to function
    zlistx_detach_cur (source->values);
    zlistx_add_start (self->values, value);
    if (value->type == 'r') {
        source->nbr_reals--;
        self->nbr_reals++;
    }
    //  Skip to start of this or previous phrase
    while (value && value->type != '|')
        value = (value_t *) zlistx_prev (source->values);

    //  Ensure cursor points to first valid value, if any
    if (!value)
        zlistx_first (source->values);
    else
    if (value->type == '|')
        zlistx_next (source->values);

    s_pull_values (self, source);
}


//  ---------------------------------------------------------------------------
//  Return pipe contents, as string. Caller must free it when done. Values are
//  separated by spaces. This empties the pipe.
//  TODO: make this an atomic (paste)

char *
zs_pipe_paste (zs_pipe_t *self)
{
    //  We use an extensible CZMQ chunk
    zchunk_t *chunk = zchunk_new (NULL, 256);
    while (zs_pipe_recv (self)) {
        char *string = zs_pipe_string (self);
        if (zchunk_size (chunk))
            zchunk_extend (chunk, " ", 1);
        zchunk_extend (chunk, string, strlen (string));
    }
    size_t result_size = zchunk_size (chunk);
    char *result = (char *) malloc (result_size + 1);
    memcpy (result, (char *) zchunk_data (chunk), result_size);
    result [result_size] = 0;
    zchunk_destroy (&chunk);
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
    zs_pipe_t *copy = zs_pipe_new ();

    zs_pipe_send_whole (pipe, 12345);
    zs_pipe_send_string (pipe, "Hello World");

    bool rc = zs_pipe_recv (pipe);
    assert (rc);
    int64_t whole = zs_pipe_whole (pipe);
    assert (whole == 12345);
    rc = zs_pipe_recv (pipe);
    assert (rc);
    const char *string = zs_pipe_string (pipe);
    assert (streq (string, "Hello World"));
    assert (!zs_pipe_recv (pipe));

    char *results = zs_pipe_paste (pipe);
    assert (streq (results, ""));

    zs_pipe_send_whole (pipe, 4);
    zs_pipe_send_whole (pipe, 5);
    zs_pipe_send_whole (pipe, 6);
    whole = zs_pipe_recv_whole (pipe);
    assert (whole == 4);
    zs_pipe_purge (pipe);
    whole = zs_pipe_recv_whole (pipe);
    assert (whole == 0);

    //  Test phrases
    zs_pipe_mark (pipe);
    zs_pipe_send_whole (pipe, 1);
    zs_pipe_send_whole (pipe, 2);
    zs_pipe_send_whole (pipe, 3);
    zs_pipe_mark (pipe);
    zs_pipe_send_whole (pipe, 4);
    zs_pipe_send_whole (pipe, 5);
    zs_pipe_send_whole (pipe, 6);
    zs_pipe_mark (pipe);
    zs_pipe_send_whole (pipe, 7);
    zs_pipe_send_whole (pipe, 8);
    zs_pipe_send_whole (pipe, 9);
    zs_pipe_mark (pipe);
    zs_pipe_send_whole (pipe, 10);

    //  Modest pull should take single last value
    zs_pipe_pull_modest (copy, pipe);
    whole = zs_pipe_recv_whole (copy);
    assert (whole == 10);
    assert (!zs_pipe_recv (copy));

    //  Modest pull should take last phrase
    zs_pipe_pull_modest (copy, pipe);
    whole = zs_pipe_recv_whole (copy);
    assert (whole == 7);
    whole = zs_pipe_recv_whole (copy);
    assert (whole == 8);
    whole = zs_pipe_recv_whole (copy);
    assert (whole == 9);
    assert (!zs_pipe_recv (copy));

    //  Add some more to the pipe...
    zs_pipe_mark (pipe);
    zs_pipe_send_whole (pipe, 7);
    zs_pipe_send_whole (pipe, 8);

    //  Greedy pull should take just those two values now
    zs_pipe_pull_greedy (copy, pipe);
    whole = zs_pipe_recv_whole (copy);
    assert (whole == 7);
    whole = zs_pipe_recv_whole (copy);
    assert (whole == 8);
    assert (!zs_pipe_recv (copy));

    //  Greedy pull should take all six remaining values
    zs_pipe_pull_greedy (copy, pipe);
    whole = zs_pipe_recv_whole (copy);
    assert (whole == 1);
    whole = zs_pipe_recv_whole (copy);
    assert (whole == 2);
    whole = zs_pipe_recv_whole (copy);
    assert (whole == 3);
    whole = zs_pipe_recv_whole (copy);
    assert (whole == 4);
    whole = zs_pipe_recv_whole (copy);
    assert (whole == 5);
    whole = zs_pipe_recv_whole (copy);
    assert (whole == 6);
    assert (!zs_pipe_recv (copy));

    //  Check pipe is empty
    assert (!zs_pipe_recv (copy));

    //  Test array pull
    zs_pipe_send_whole (pipe, 1);
    zs_pipe_send_whole (pipe, 2);
    zs_pipe_send_whole (pipe, 3);
    zs_pipe_mark (pipe);
    zs_pipe_send_whole (pipe, 4);
    zs_pipe_send_whole (pipe, 5);
    zs_pipe_send_whole (pipe, 6);
    zs_pipe_mark (pipe);
    zs_pipe_send_whole (pipe, 7);

    zs_pipe_pull_array (copy, pipe);
    whole = zs_pipe_recv_whole (copy);
    assert (whole == 7);
    whole = zs_pipe_recv_whole (copy);
    assert (whole == 4);
    whole = zs_pipe_recv_whole (copy);
    assert (whole == 5);
    whole = zs_pipe_recv_whole (copy);
    assert (whole == 6);
    assert (!zs_pipe_recv (copy));

    //  Test casting
    zs_pipe_purge (pipe);
    zs_pipe_send_whole (pipe, 1);
    zs_pipe_send_real  (pipe, 2.0);
    zs_pipe_send_whole (pipe, 3);
    assert (zs_pipe_has_real (pipe));
    double real = zs_pipe_recv_real (pipe);
    assert (real == 1.0);
    real = zs_pipe_recv_real (pipe);
    assert (real == 2.0);
    assert (!zs_pipe_has_real (pipe));
    real = zs_pipe_recv_real (pipe);
    assert (real == 3.0);

    zs_pipe_destroy (&copy);
    zs_pipe_destroy (&pipe);
    //  @end
    printf ("OK\n");
}
