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
#include "zs_core_lib.h"        //  Core library primitives

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
    zs_exec_t *exec;            //  Execution context
    zs_primitive_t *function;   //  Current function if any
    bool completed;             //  Input formed a complete phrase
    uint scope;                 //  Nesting scope, 0..n
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
        
        //  Set token type to event map
        self->events [zs_lex_function] = function_event;
        self->events [zs_lex_compose] = compose_event;
        self->events [zs_lex_string] = string_event;
        self->events [zs_lex_number] = number_event;
        self->events [zs_lex_open] = open_event;
        self->events [zs_lex_close] = close_event;
        self->events [zs_lex_invalid] = invalid_event;
        self->events [zs_lex_null] = eol_event;

        self->exec = zs_exec_new ();
        s_register_primitives (self->exec);
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
        zs_exec_destroy (&self->exec);
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
    self->completed = false;
    self->status = 0;
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
//  push_number_to_output
//

static void
push_number_to_output (zs_core_t *self)
{
    zs_pipe_put_number (zs_exec_output (self->exec), atoll (zs_lex_token (self->lex)));
}


//  ---------------------------------------------------------------------------
//  push_string_to_output
//

static void
push_string_to_output (zs_core_t *self)
{
    zs_pipe_put_string (zs_exec_output (self->exec), zs_lex_token (self->lex));
}


//  ---------------------------------------------------------------------------
//  resolve_function_name
//

static void
resolve_function_name (zs_core_t *self)
{
    self->function = zs_exec_resolve (self->exec, zs_lex_token (self->lex));
    if (!self->function)
        fsm_set_exception (self->fsm, invalid_event);
}


//  ---------------------------------------------------------------------------
//  call_simple_function
//

static void
call_simple_function (zs_core_t *self)
{
    zs_exec_inline (self->exec, self->function);
}


//  ---------------------------------------------------------------------------
//  open_function_scope
//

static void
open_function_scope (zs_core_t *self)
{
    self->scope++;
    zs_exec_open (self->exec, self->function);
}


//  ---------------------------------------------------------------------------
//  close_function_scope
//

static void
close_function_scope (zs_core_t *self)
{
    if (self->scope) {
        self->scope--;
        if (zs_exec_close (self->exec))
            fsm_set_exception (self->fsm, invalid_event);
    }
    else
        fsm_set_exception (self->fsm, invalid_event);
}


//  ---------------------------------------------------------------------------
//  check_if_completed
//

static void
check_if_completed (zs_core_t *self)
{
    if (self->scope == 0)
        fsm_set_exception (self->fsm, completed_event);
}


//  ---------------------------------------------------------------------------
//  signal_completed
//

static void
signal_completed (zs_core_t *self)
{
    self->completed = true;
}


//  ---------------------------------------------------------------------------
//  signal_syntax_error
//

static void
signal_syntax_error (zs_core_t *self)
{
    self->status = -1;
    self->completed = true;
}


//  ---------------------------------------------------------------------------
//  Return true if the input formed a complete phrase that was successfully
//  evaulated. If not, the core expects more input.

bool
zs_core_completed (zs_core_t *self)
{
    return self->completed;
}


//  ---------------------------------------------------------------------------
//  Return pipe results as string, after successful execution. Caller must
//  free results when finished.

char *
zs_core_results (zs_core_t *self)
{
    return zs_pipe_contents (zs_exec_output (self->exec));
}


//  ---------------------------------------------------------------------------
//  After a syntax error, return position of syntax error in text.

uint
zs_core_offset (zs_core_t *self)
{
    return zs_lex_offset (self->lex);
}


static void
s_core_assert (zs_core_t *self, const char *input, const char *output)
{
    int rc = zs_core_execute (self, input);
    assert (rc == 0);
    char *results = zs_core_results (self);
    if (strneq (results, output)) {
        printf ("input='%s' results='%s' expected='%s'\n",
                input, results, output);
        exit (-1);
    }
    zstr_free (&results);
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

    s_core_assert (core, "1 2 3 sum", "6");
    s_core_assert (core, "sum (1 2 3)", "6");
    s_core_assert (core, "sum (sum (1 2 3) count (4 5 6))", "9");
    s_core_assert (core, "sum (1 2 3", "");
    s_core_assert (core, ")", "6");

//     zs_core_execute (core, "a: (sum (1 2 3))");
//     zs_core_execute (core, "b: (a 4 5 6 sum)");
//     zs_core_execute (core, "c: (a b sum)");
//     zs_core_execute (core, "a b c");
//     zs_core_execute (core, "<hello> <world>");
//     zs_core_execute (core, "echo (<hello> <world>)");
//     zs_core_execute (core, "pi: (22/7)");
//     zs_core_execute (core, "twopi: (pi 2 times)");
    
    zs_core_destroy (&core);
    //  @end
    printf ("OK\n");
}
