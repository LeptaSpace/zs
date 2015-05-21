/*  =========================================================================
    zs_repl - the ZeroScript read-evaluate-print loop

    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of the ZeroScript language, http://zeroscript.org.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

/*
@header
    The repl engine processes input lines.
@discuss
@end
*/

#include "zs_classes.h"
#include "zs_repl_fsm.h"        //  Finite state machine engine
#include "zs_atomics.h"         //  Core atomics
#include "zs_units_si.h"        //  SI scaling functions
#include "zs_units_misc.h"      //  Miscellaneous scaling functions
#include "zs_strtod.c"          //  Coerced strtod function

//  This holds an entry in the dictionary
typedef struct {
    char *name;                 //  Function name
    void *exec;                 //  Function body
} entry_t;


//  Structure of our class

struct _zs_repl_t {
    fsm_t *fsm;                 //  Our finite state machine
    event_t events [zs_lex_tokens];
    zs_lex_t *lex;              //  Lexer instance
    const char *input;          //  Line of text we're parsing
    int status;                 //  0 = OK, -1 = error
    zs_vm_t *vm;                //  Execution context
    bool completed;             //  Input formed a complete phrase
    uint scope;                 //  Nesting scope, 0..n
};

//  ---------------------------------------------------------------------------
//  Create a new repl engine, return the reference if successful, or NULL
//  if construction failed due to lack of available memory.

zs_repl_t *
zs_repl_new (void)
{
    zs_repl_t *self = (zs_repl_t *) zmalloc (sizeof (zs_repl_t));
    if (self) {
        self->fsm = fsm_new (self);
        self->lex = zs_lex_new ();
        self->vm = zs_vm_new ();
        s_register_atomics (self->vm);
        s_register_zs_units_si (self->vm);
        s_register_zs_units_misc (self->vm);

        //  Set token type to event map
        self->events [zs_lex_inline_fn] = inline_fn_event;
        self->events [zs_lex_nested_fn] = nested_fn_event;
        self->events [zs_lex_define_fn] = define_fn_event;
        self->events [zs_lex_string] = string_event;
        self->events [zs_lex_number] = number_event;
        self->events [zs_lex_phrase] = phrase_event;
        self->events [zs_lex_sentence] = sentence_event;
        self->events [zs_lex_close_list] = close_list_event;
        self->events [zs_lex_invalid] = invalid_event;
        self->events [zs_lex_null] = finished_event;
    }
    return self;
}


//  ---------------------------------------------------------------------------
//  Destroy the zs_repl and free all memory used by the object.

void
zs_repl_destroy (zs_repl_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        zs_repl_t *self = *self_p;
        fsm_destroy (&self->fsm);
        zs_lex_destroy (&self->lex);
        zs_vm_destroy (&self->vm);
        free (self);
        *self_p = NULL;
    }
}


//  ---------------------------------------------------------------------------
//  Enable verbose tracing of engine

void
zs_repl_verbose (zs_repl_t *self, bool verbose)
{
    fsm_set_animate (self->fsm, verbose);
    zs_vm_set_verbose (self->vm, verbose);
}


//  ---------------------------------------------------------------------------
//  Execute a buffer of code; to reset the engine you destroy it and create a
//  new one. Returns 0 if OK, -1 on syntax errors or cataclysmic implosions of
//  the Sun (can be resolved from context).

int
zs_repl_execute (zs_repl_t *self, const char *input)
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
get_next_token (zs_repl_t *self)
{
    zs_lex_token_t token = zs_lex_next (self->lex);
    assert (token < zs_lex_tokens);
    fsm_set_next_event (self->fsm, self->events [token]);
}


//  ---------------------------------------------------------------------------
//  compile_define_shell
//

static void
compile_define_shell (zs_repl_t *self)
{
    zs_vm_compile_define (self->vm, "$shell$");
}


//  ---------------------------------------------------------------------------
//  compile_number
//

static void
compile_number (zs_repl_t *self)
{
    char *number = strdup (zs_lex_token (self->lex));
    assert (strlen (number) > 0);

    //  Check if it's a percentage; this also coerces number to real
    bool percentage = false;
    if (number [strlen (number) - 1] == '%') {
        number [strlen (number) - 1] = 0;
        percentage = true;
    }
    //  Try to convert as whole number
    char *end = number;
    int64_t whole = (int64_t) strtoll (number, &end, 10);
    if (*end == 0 && !percentage)
        zs_vm_compile_whole (self->vm, whole);
    else {
        //  Try to convert as real number
        end = number;
        double real = zs_strtod (number, &end);
        if (*end == 0)
            zs_vm_compile_real (self->vm, percentage? real / 100: real);
        else
            fsm_set_exception (self->fsm, invalid_event);
    }
    free (number);
}


//  ---------------------------------------------------------------------------
//  compile_string
//

static void
compile_string (zs_repl_t *self)
{
    zs_vm_compile_string (self->vm, zs_lex_token (self->lex));
}


//  ---------------------------------------------------------------------------
//  compile_inline_call
//

static void
compile_inline_call (zs_repl_t *self)
{
    const char *function = zs_lex_token (self->lex);
    switch (zs_vm_function_type (self->vm, function)) {
        //  VM takes care of plumbing and function call
        case zs_type_strict:
            if (zs_vm_compile_strict (self->vm, function))
                fsm_set_exception (self->fsm, invalid_event);
            break;
        case zs_type_modest:
            if (zs_vm_compile_modest (self->vm, function))
                fsm_set_exception (self->fsm, invalid_event);
            break;
        case zs_type_greedy:
            if (zs_vm_compile_greedy (self->vm, function))
                fsm_set_exception (self->fsm, invalid_event);
            break;
        case zs_type_unknown:
            fsm_set_exception (self->fsm, invalid_event);
            break;
    }
}


//  ---------------------------------------------------------------------------
//  compile_nested_call
//

static void
compile_nested_call (zs_repl_t *self)
{
    self->scope++;
    if (zs_vm_compile_nested (self->vm, zs_lex_token (self->lex)))
        fsm_set_exception (self->fsm, invalid_event);
}


//  ---------------------------------------------------------------------------
//  compile_unnest
//

static void
compile_unnest (zs_repl_t *self)
{
    if (self->scope) {
        self->scope--;
        zs_vm_compile_unnest (self->vm);
    }
    else
        fsm_set_exception (self->fsm, invalid_event);
}


//  ---------------------------------------------------------------------------
//  compile_define
//

static void
compile_define (zs_repl_t *self)
{
    self->scope++;
    zs_vm_compile_define (self->vm, zs_lex_token (self->lex));
}


//  ---------------------------------------------------------------------------
//  compile_unnest_or_commit
//

static void
compile_unnest_or_commit (zs_repl_t *self)
{
    assert (self->scope);
    if (--self->scope)
        zs_vm_compile_unnest (self->vm);
    else {
        zs_vm_compile_commit (self->vm);
        fsm_set_exception (self->fsm, completed_event);
    }
}


//  ---------------------------------------------------------------------------
//  compile_end_of_phrase
//

static void
compile_end_of_phrase (zs_repl_t *self)
{
    zs_vm_compile_phrase (self->vm);
}


//  ---------------------------------------------------------------------------
//  compile_end_of_sentence
//

static void
compile_end_of_sentence (zs_repl_t *self)
{
    zs_vm_compile_sentence (self->vm);
}


//  ---------------------------------------------------------------------------
//  check_if_completed
//

static void
check_if_completed (zs_repl_t *self)
{
    if (self->scope == 0)
        fsm_set_exception (self->fsm, completed_event);
}


//  ---------------------------------------------------------------------------
//  compile_commit_shell
//

static void
compile_commit_shell (zs_repl_t *self)
{
    zs_vm_compile_commit (self->vm);
}


//  ---------------------------------------------------------------------------
//  run_virtual_machine
//

static void
run_virtual_machine (zs_repl_t *self)
{
    zs_vm_run (self->vm);
}


//  ---------------------------------------------------------------------------
//  rollback_the_function
//

static void
rollback_the_function (zs_repl_t *self)
{
    zs_vm_compile_rollback (self->vm);
    self->scope = 0;
}


//  ---------------------------------------------------------------------------
//  signal_completed
//

static void
signal_completed (zs_repl_t *self)
{
    self->completed = true;
}


//  ---------------------------------------------------------------------------
//  signal_syntax_error
//

static void
signal_syntax_error (zs_repl_t *self)
{
    self->status = -1;
    self->completed = true;
}


//  ---------------------------------------------------------------------------
//  Return true if the input formed a complete phrase that was successfully
//  evaulated. If not, the repl expects more input.

bool
zs_repl_completed (zs_repl_t *self)
{
    return self->completed;
}


//  ---------------------------------------------------------------------------
//  Return pipe results as string, after successful execution. Caller must
//  not modify returned value.

const char *
zs_repl_results (zs_repl_t *self)
{
    return zs_vm_results (self->vm);
}


//  ---------------------------------------------------------------------------
//  After a syntax error, return position of syntax error in text.

uint
zs_repl_offset (zs_repl_t *self)
{
    return zs_lex_offset (self->lex);
}


static void
s_repl_assert (zs_repl_t *self, const char *input, const char *output)
{
    int rc = zs_repl_execute (self, input);
    assert (rc == 0);
    if (strneq (zs_repl_results (self), output)) {
        printf ("input='%s' results='%s' expected='%s'\n",
                input, zs_repl_results (self), output);
        exit (-1);
    }
}


//  ---------------------------------------------------------------------------
//  Selftest

void
zs_repl_test (bool verbose)
{
    printf (" * zs_repl: ");
    if (verbose)
        printf ("\n");

    //  @selftest
    zs_repl_t *repl = zs_repl_new ();
    zs_repl_verbose (repl, verbose);
    s_repl_assert (repl, "1 2 3 add", "6");
    s_repl_assert (repl, "1 2, k", "1000 2000");
    s_repl_assert (repl, "1 2 3, add", "6");
    s_repl_assert (repl, "1 2 3, 4 5 6 add", "1 2 3 15");
    s_repl_assert (repl, "1 2 3, 4 5 6, add", "21");
    s_repl_assert (repl, "1 2 3 count, 1 1 add, subtract", "1");
    s_repl_assert (repl, "add (1 2 3)", "6");
    s_repl_assert (repl, "add (add (1 2 3) count (4 5 6))", "9");
    s_repl_assert (repl, "add (1 2 3", "");
    s_repl_assert (repl, ")", "6");
    s_repl_assert (repl, "sub: (<hello>)", "");
    s_repl_assert (repl, "sub", "hello");
    s_repl_assert (repl, "add (k (1 2 3) M (2))", "2006000");
    zs_repl_destroy (&repl);
    //  @end
    printf ("OK\n");
}
