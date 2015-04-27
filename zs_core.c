/*  =========================================================================
    zs_core - the ZeroScript core engine

    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of the ZeroScript language, http://zeroscript.org.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

/*
@header
    The core engine processes input lines.
@discuss
@end
*/

#include "zs_classes.h"
#include "zs_core_fsm.h"        //  Finite state machine engine

//  This holds an item in our value queue
typedef struct {
    char *string;               //  It's either a string or
    int64_t number;             //      an integer
} value_t;

static value_t *
value_new (const char *string, int64_t number)
{
    value_t *self = (value_t *) zmalloc (sizeof (value_t));
    if (self) {
        if (string)
            self->string = strdup (string);
        else
            self->number = number;
    }
    return self;
}

static void
s_value_destroy (value_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        value_t *self = *self_p;
        free (self->string);
        free (self);
        *self_p = NULL;
    }
}

//  This holds an entry in the dictionary
typedef struct {
    char *name;                 //  Function name
    void *exec;                 //  Function body
} entry_t;


//  Structure of our class

struct _zs_core_t {
    fsm_t *fsm;                 //  Our finite state machine
    event_t events [zs_lex_tokens];
    zs_lex_t *lex;              //  Lexer instance
    const char *input;          //  Line of text we're parsing
    int status;                 //  0 = OK, -1 = error
    zlistx_t *values;           //  Value queue
};

//  ---------------------------------------------------------------------------
//  Create a new core engine, return the reference if successful, or NULL
//  if construction failed due to lack of available memory.

zs_core_t *
zs_core_new (void)
{
    zs_core_t *self = (zs_core_t *) zmalloc (sizeof (zs_core_t));
    if (self) {
        self->fsm = fsm_new (self);
        self->lex = zs_lex_new ();
        self->values = zlistx_new ();
        zlistx_set_destructor (self->values, (czmq_destructor *) s_value_destroy);
        
        //  Set token type to event map
        self->events [zs_lex_invoke] = invoke_event;
        self->events [zs_lex_compose] = compose_event;
        self->events [zs_lex_string] = string_event;
        self->events [zs_lex_number] = number_event;
        self->events [zs_lex_open] = open_event;
        self->events [zs_lex_close] = close_event;
        self->events [zs_lex_invalid] = invalid_event;
        self->events [zs_lex_null] = null_event;
    }
    return self;
}


//  ---------------------------------------------------------------------------
//  Destroy the zs_core and free all memory used by the object.

void
zs_core_destroy (zs_core_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        zs_core_t *self = *self_p;
        fsm_destroy (&self->fsm);
        zs_lex_destroy (&self->lex);
        free (self);
        *self_p = NULL;
    }
}


//  ---------------------------------------------------------------------------
//  Enable verbose tracing of engine

void
zs_core_verbose (zs_core_t *self, bool verbose)
{
    fsm_set_animate (self->fsm, verbose);
}


//  ---------------------------------------------------------------------------
//  Execute a buffer of code; to reset the engine you destroy it and create a
//  new one. Returns 0 if OK, -1 on syntax errors or cataclysmic implosions of
//  the Sun (can be resolved from context).

int
zs_core_execute (zs_core_t *self, const char *input)
{
    self->input = input;
    zs_lex_token_t token = zs_lex_first (self->lex, self->input);
    assert (token < zs_lex_tokens);
    fsm_set_next_event (self->fsm, self->events [token]);
    fsm_execute (self->fsm);
    return self->status;
}


//  *************************  Finite State Machine  *************************
//  These actions are called from the generated FSM code.

//  ---------------------------------------------------------------------------
//  get_next_token
//

static void
get_next_token (zs_core_t *self)
{
    zs_lex_token_t token = zs_lex_next (self->lex);
    assert (token < zs_lex_tokens);
    fsm_set_next_event (self->fsm, self->events [token]);
}


//  ---------------------------------------------------------------------------
//  queue_numeric_value
//

static void
queue_numeric_value (zs_core_t *self)
{
    zlistx_add_end (self->values, value_new (NULL, atol (zs_lex_token (self->lex))));
}


//  ---------------------------------------------------------------------------
//  queue_string_value
//

static void
queue_string_value (zs_core_t *self)
{
    zlistx_add_end (self->values, value_new (zs_lex_token (self->lex), 0));
}


//  ---------------------------------------------------------------------------
//  open_composition
//

static void
open_composition (zs_core_t *self)
{
}


//  ---------------------------------------------------------------------------
//  record_numeric_value
//

static void
record_numeric_value (zs_core_t *self)
{
}


//  ---------------------------------------------------------------------------
//  record_string_value
//

static void
record_string_value (zs_core_t *self)
{
}


//  ---------------------------------------------------------------------------
//  record_function
//

static void
record_function (zs_core_t *self)
{
}


//  ---------------------------------------------------------------------------
//  close_composition
//

static void
close_composition (zs_core_t *self)
{
}


//  ---------------------------------------------------------------------------
//  invoke_function
//

static void
invoke_function (zs_core_t *self)
{
}


//  ---------------------------------------------------------------------------
//  name_must_be_unknown
//

static void
name_must_be_unknown (zs_core_t *self)
{
}


//  ---------------------------------------------------------------------------
//  signal_success
//

static void
signal_success (zs_core_t *self)
{
    self->status = 0;
}


//  ---------------------------------------------------------------------------
//  signal_syntax_error
//

static void
signal_syntax_error (zs_core_t *self)
{
    self->status = -1;
}


//  ---------------------------------------------------------------------------
//  Selftest

void
zs_core_test (bool verbose)
{
    printf (" * zs_core: ");
    if (verbose)
        printf ("\n");

    //  @selftest
    zs_core_t *core = zs_core_new ();
    zs_core_verbose (core, verbose);

    zs_core_execute (core, "1 2 3 sum");
    zs_core_execute (core, "sum (1 2 3)");
    zs_core_execute (core, "a: (sum (1 2 3))");
    zs_core_execute (core, "b: (a 4 5 6 sum)");
    zs_core_execute (core, "c: (a b sum)");
    zs_core_execute (core, "a b c");
    zs_core_execute (core, "<hello> <world>");
    zs_core_execute (core, "echo (<hello> <world>)");
    zs_core_execute (core, "pi: (22/7)");
    zs_core_execute (core, "twopi: (pi 2 times)");
    
    zs_core_destroy (&core);
    //  @end
    printf ("OK\n");
}
