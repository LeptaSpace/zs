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

#define SCOPE_MAX   256         //  Arbitrary max.depth

//  Structure of our class

struct _zs_repl_t {
    fsm_t *fsm;                 //  Our finite state machine
    event_t events [zs_lex_tokens];
    zs_lex_t *lex;              //  Lexer instance
    zs_lex_token_t token;       //  Lexer token value
    const char *input;          //  Line of text we're parsing
    int status;                 //  0 = OK, -1 = error
    zs_vm_t *vm;                //  Execution context
    bool completed;             //  Input formed a complete phrase
    size_t scope;               //  Nesting scope, 0..n
    //  Stack matching closing token
    zs_lex_token_t scope_stack [SCOPE_MAX];
    //  Loop function at each open scope
    char *loop_function [SCOPE_MAX];
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
        self->completed = true;

        self->vm = zs_vm_new ();
        s_register_atomics (self->vm);
        s_register_zs_units_si (self->vm);
        s_register_zs_units_misc (self->vm);

        //  Set token type to event map
        self->events [zs_lex_fn_inline] = fn_inline_event;
        self->events [zs_lex_fn_nested] = fn_nested_event;
        self->events [zs_lex_fn_define] = fn_define_event;
        self->events [zs_lex_fn_close] = fn_close_event;
        self->events [zs_lex_start_menu] = start_menu_event;
        self->events [zs_lex_end_menu] = end_menu_event;
        self->events [zs_lex_start_loop] = start_loop_event;
        self->events [zs_lex_end_loop] = end_loop_event;
        self->events [zs_lex_string] = string_event;
        self->events [zs_lex_number] = number_event;
        self->events [zs_lex_phrase] = phrase_event;
        self->events [zs_lex_sentence] = sentence_event;
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
        while (self->scope)
            zstr_free (&self->loop_function [self->scope--]);
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
    zs_lex_set_verbose (self->lex, verbose);
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
    self->token = zs_lex_first (self->lex, self->input);
    assert (self->token < zs_lex_tokens);
    fsm_set_next_event (self->fsm, self->events [self->token]);
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
    self->token = zs_lex_next (self->lex);
    assert (self->token < zs_lex_tokens);
    fsm_set_next_event (self->fsm, self->events [self->token]);
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
    char *number = strdup (zs_lex_value (self->lex));
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
    zs_vm_compile_string (self->vm, zs_lex_value (self->lex));
}


//  ---------------------------------------------------------------------------
//  compile_inline_call
//

static void
compile_inline_call (zs_repl_t *self)
{
    if (zs_vm_compile_inline (self->vm, zs_lex_value (self->lex)))
        fsm_set_exception (self->fsm, invalid_event);
}


//  ---------------------------------------------------------------------------
//  compile_nested_call
//

static void
compile_nested_call (zs_repl_t *self)
{
    assert (self->scope < SCOPE_MAX);
    self->scope_stack [self->scope++] = zs_lex_fn_close;
    if (zs_vm_compile_nest (self->vm, zs_lex_value (self->lex)))
        fsm_set_exception (self->fsm, invalid_event);
}


//  ---------------------------------------------------------------------------
//  pop_and_check_scope
//

static void
pop_and_check_scope (zs_repl_t *self)
{
    if (self->scope == 0
    ||  self->scope_stack [--self->scope] != self->token)
        fsm_set_exception (self->fsm, invalid_event);
}


//  ---------------------------------------------------------------------------
//  compile_unnest
//

static void
compile_unnest (zs_repl_t *self)
{
    zs_vm_compile_xnest (self->vm);
}


//  ---------------------------------------------------------------------------
//  compile_define
//

static void
compile_define (zs_repl_t *self)
{
    assert (self->scope < SCOPE_MAX);
    self->scope_stack [self->scope++] = zs_lex_fn_close;
    zs_vm_compile_define (self->vm, zs_lex_value (self->lex));
}


//  ---------------------------------------------------------------------------
//  compile_unnest_or_commit
//  If we are closing a scope, we've successfully compiled a new function
//  definition, so we can commit it. We do not allow function definitions
//  inside each other, they are always at scope = 0.
//

static void
compile_unnest_or_commit (zs_repl_t *self)
{
    if (self->scope)
        zs_vm_compile_xnest (self->vm);
    else {
        zs_vm_commit (self->vm);
        fsm_set_exception (self->fsm, committed_event);
    }
}


//  ---------------------------------------------------------------------------
//  compile_start_menu
//

static void
compile_start_menu (zs_repl_t *self)
{
    assert (self->scope < SCOPE_MAX);
    self->scope_stack [self->scope++] = zs_lex_end_menu;
    zs_vm_compile_menu (self->vm);
}


//  ---------------------------------------------------------------------------
//  compile_end_menu
//

static void
compile_end_menu (zs_repl_t *self)
{
    zs_vm_compile_xmenu (self->vm);
}


//  ---------------------------------------------------------------------------
//  remember_loop_function
//

static void
remember_loop_function (zs_repl_t *self)
{
    zstr_free (&self->loop_function [self->scope]);
    self->loop_function [self->scope] = strdup (zs_lex_value (self->lex));
}


//  ---------------------------------------------------------------------------
//  require_loop_function
//

static void
require_loop_function (zs_repl_t *self)
{
    if (!self->loop_function [self->scope])
        fsm_set_exception (self->fsm, invalid_event);
}


//  ---------------------------------------------------------------------------
//  compile_start_loop
//

static void
compile_start_loop (zs_repl_t *self)
{
    zs_vm_compile_loop (self->vm, self->loop_function [self->scope]);
    assert (self->scope < SCOPE_MAX);
    self->scope_stack [self->scope++] = zs_lex_end_loop;
}


//  ---------------------------------------------------------------------------
//  compile_end_loop
//

static void
compile_end_loop (zs_repl_t *self)
{
    zs_vm_compile_xloop (self->vm);
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
    if (self->scope == 0) {
        fsm_set_exception (self->fsm, completed_event);
        self->completed = true;
    }
}


//  ---------------------------------------------------------------------------
//  compile_commit_shell
//

static void
compile_commit_shell (zs_repl_t *self)
{
    zs_vm_commit (self->vm);
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
    zs_vm_rollback (self->vm);
    self->scope = 0;
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
//  Return latest function by name; use with first to iterate through
//  functions. Returns function name or NULL if there are none defined.

const char *
zs_repl_first (zs_repl_t *self)
{
    return zs_vm_function_first (self->vm);
}


//  ---------------------------------------------------------------------------
//  Return previous function by name; use after first to iterate through
//  functions. Returns function name or NULL if there are no more.

const char *
zs_repl_next (zs_repl_t *self)
{
    return zs_vm_function_next (self->vm);
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
s_repl_assert (zs_repl_t *self, const char *input, const char *expected)
{
    if (zs_repl_execute (self, input)) {
        printf ("E: syntax error: '%s'\n", input);
        assert (false);
    }
    const char *results = zs_repl_results (self);
    if (strneq (results, expected)) {
        printf ("input='%s' results='%s' expected='%s'\n", input, results, expected);
        assert (false);
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
    s_repl_assert (repl, "1 2 3 sum", "6");
    s_repl_assert (repl, "1 2, k", "1000 2000");
    s_repl_assert (repl, "1 2 3, sum", "6");
    s_repl_assert (repl, "1 2 3, 4 5 6 sum", "1 2 3 15");
    s_repl_assert (repl, "1 2 3, 4 5 6, sum", "21");
    s_repl_assert (repl, "1 2 3 count, 1 1 sum, min", "2");
    s_repl_assert (repl, "sum (1 2 3)", "6");
    s_repl_assert (repl, "sum (sum (1 2 3) count (4 5 6))", "9");
    s_repl_assert (repl, "sum (1 2 3", "");
    s_repl_assert (repl, ")", "6");
    s_repl_assert (repl, "sub: (<hello>)", "");
    s_repl_assert (repl, "sub", "hello");
    s_repl_assert (repl, "sum (k (1 2 3) M (2))", "2006000");
    s_repl_assert (repl, "k", "1000");
    s_repl_assert (repl, "fn: (sum)", "");
    s_repl_assert (repl, "1 2 3 fn", "6");
    s_repl_assert (repl, "1 2 3, fn", "6");
    s_repl_assert (repl, "fn (1 2 3)", "6");
    s_repl_assert (repl, "K: (1000 *)", "");
    s_repl_assert (repl, "K (1 2 3)", "1000 2000 3000");
    s_repl_assert (repl, "12.0 .1 +", "12.1");
    s_repl_assert (repl, "1 [1 2] 0.5 [1 2] 0.49 [1 2] count", "4");
    s_repl_assert (repl, "times (1 k) { 1 } count", "1000");
    zs_repl_destroy (&repl);
    //  @end
    printf ("OK\n");
}
