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
    zs_primitive_t *stack [256];    //  Call stack
    uint stack_ptr;             //  Current size of stack
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
        zs_exec_probe (self->exec, s_sum);
        zs_exec_probe (self->exec, s_count);
        zs_exec_probe (self->exec, s_echo);
        zs_exec_probe (self->exec, s_selftest);
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
//  call_function
//

static void
call_function (zs_core_t *self)
{
    //  This won't work properly for nested functions yet
    zs_exec_cycle (self->exec);
    (self->function) (self->exec);
}


//  ---------------------------------------------------------------------------
//  push_function
//

static void
push_function (zs_core_t *self)
{
    self->stack [self->stack_ptr++] = self->function;
}


//  ---------------------------------------------------------------------------
//  pop_function
//

static void
pop_function (zs_core_t *self)
{
    if (self->stack_ptr)
        self->function = self->stack [--self->stack_ptr];
    else
        fsm_set_exception (self->fsm, invalid_event);
}


//  ---------------------------------------------------------------------------
//  show_pipe_contents
//

static void
show_pipe_contents (zs_core_t *self)
{
    printf ("=> ");
    zs_pipe_print (zs_exec_output (self->exec));
    zs_exec_cycle (self->exec);
    printf ("OK\n");
}


//  ---------------------------------------------------------------------------
//  name_must_be_unknown
//

static void
name_must_be_unknown (zs_core_t *self)
{
}


//  ---------------------------------------------------------------------------
//  open_composition
//

static void
open_composition (zs_core_t *self)
{
}


//  ---------------------------------------------------------------------------
//  compose_number_value
//

static void
compose_number_value (zs_core_t *self)
{
}


//  ---------------------------------------------------------------------------
//  compose_string_value
//

static void
compose_string_value (zs_core_t *self)
{
}


//  ---------------------------------------------------------------------------
//  compose_function
//

static void
compose_function (zs_core_t *self)
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
    zs_exec_cycle (self->exec);
    self->status = -1;
}


//  ---------------------------------------------------------------------------
//  After a syntax error, return position of syntax error in text.

uint
zs_core_offset (zs_core_t *self)
{
    return zs_lex_offset (self->lex);
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

    int rc = zs_core_execute (core, "1 2 3 sum");
    assert (rc == 0);
    rc = zs_core_execute (core, "sum (1 2 3)");
    assert (rc == 0);
    rc = zs_core_execute (core, "sum (sum (1 2 3) count (4 5 6))");
    assert (rc == 0);
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
