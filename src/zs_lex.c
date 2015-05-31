/*  =========================================================================
    zs_lex - break input into lexical tokens

    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of the ZeroScript language, http://zeroscript.org.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

/*
@header
    The lexer breaks an input stream into words, which are function
    compositions and invocations, strings, numbers, and open or close
    lists. It does not validate any semantics.
@discuss
    Functions contain letters, digits, hyphens, slashes, and underscores,
    and may not start with a digit.

    A inline function is a name. A nested function is a name followed
    by a list. A definition is a function followed by ':' and a list.

    Lists start with ( and end with ).

    Strings are quoted by < and >.

    Menus start with [ and end with ], and choices are separated by |.

    Loops start with { and end with }.

    Accepts real or whole numbers:
        [sign]integer(.,)fraction(Ee)[sign]exponent (strtod format)
        whole number preceded by + or -
        % at end of number divides by 100

    , and . are used as punctuation.

    # marks a comment to the end of the line

@end
*/

#include "zs_classes.h"
#include "zs_lex_fsm.h"         //  Finite state machine engine

//  Structure of our class

struct _zs_lex_t {
    fsm_t *fsm;                 //  Our finite state machine
    event_t events [256];       //  Map characters to events
    const char *input;          //  Line of text we're parsing
    const char *input_ptr;      //  Next character to process
    uint token_size;            //  Size of token so far
    zs_lex_token_t type;        //  Token type
    char token [1025];          //  Current token, max size 1K
    char current;               //  Current character
};

static void
s_set_events (zs_lex_t *self, const char *chars, event_t event)
{
    while (*chars)
        self->events [(uint) *chars++] = event;
}

//  ---------------------------------------------------------------------------
//  Create a new zs_lex, return the reference if successful, or NULL
//  if construction failed due to lack of available memory.

zs_lex_t *
zs_lex_new (void)
{
    zs_lex_t *self = (zs_lex_t *) zmalloc (sizeof (zs_lex_t));
    if (self) {
        self->fsm = fsm_new (self);
        uint char_nbr;
        self->events [0] = finished_event;
        for (char_nbr = 1; char_nbr < 256; char_nbr++)
            self->events [char_nbr] = other_event;
        //  There are two ways to do this; either we define character
        //  classes that produce generic events depending on the current
        //  state (e.g. hyphen_event in function names, or minus_event in
        //  numbers), or else we define lower level events that the FSM
        //  sorts out. I've chosen the second design so decisions stay in
        //  the FSM.
        s_set_events (self, "ABCDEFGHIJKLMNOPQRSTUVWXYZ", letter_event);
        s_set_events (self, "abcdefghijklmnopqrstuvwxyz", letter_event);
        s_set_events (self, "0123456789", digit_event);
        s_set_events (self, "/*^_", usable_event);
        s_set_events (self, "-", hyphen_event);
        s_set_events (self, "+", plus_event);
        s_set_events (self, ".", period_event);
        s_set_events (self, ",", comma_event);
        s_set_events (self, ":", colon_event);
        s_set_events (self, "%", percent_event);
        s_set_events (self, "<", open_quote_event);
        s_set_events (self, ">", close_quote_event);
        s_set_events (self, "(", open_paren_event);
        s_set_events (self, ")", close_paren_event);
        s_set_events (self, "[", open_square_event);
        s_set_events (self, "]", close_square_event);
        s_set_events (self, "{", open_curly_event);
        s_set_events (self, "}", close_curly_event);
        s_set_events (self, "|", vertical_bar_event);
        s_set_events (self, "#", comment_event);
        s_set_events (self, " \t", whitespace_event);
        s_set_events (self, "\n", newline_event);
    }
    return self;
}


//  ---------------------------------------------------------------------------
//  Destroy the zs_lex and free all memory used by the object.

void
zs_lex_destroy (zs_lex_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        zs_lex_t *self = *self_p;
        fsm_destroy (&self->fsm);
        free (self);
        *self_p = NULL;
    }
}


//  ---------------------------------------------------------------------------
//  Enable verbose tracing of lexer

void
zs_lex_set_verbose (zs_lex_t *self, bool verbose)
{
    fsm_set_animate (self->fsm, verbose);
}


//  ---------------------------------------------------------------------------
//  Start parsing buffer, return type of first token

zs_lex_token_t
zs_lex_first (zs_lex_t *self, const char *input)
{
    self->input = input;
    self->input_ptr = self->input;
    return zs_lex_next (self);
}


//  ---------------------------------------------------------------------------
//  Continue parsing buffer, return type of next token

zs_lex_token_t
zs_lex_next (zs_lex_t *self)
{
    parse_next_character (self);
    fsm_execute (self->fsm);
    return self->type;
}


//  ---------------------------------------------------------------------------
//  Return actual token value, if any

const char *
zs_lex_value (zs_lex_t *self)
{
    return self->token;
}


//  ---------------------------------------------------------------------------
//  Return position of last processed character in text

uint
zs_lex_offset (zs_lex_t *self)
{
    return (self->input_ptr - self->input);
}


//  *************************  Finite State Machine  *************************
//  These actions are called from the generated FSM code.

//  ---------------------------------------------------------------------------
//  start_new_token
//

static void
start_new_token (zs_lex_t *self)
{
    self->token_size = 0;
    self->type = zs_lex_null;
}


//  ---------------------------------------------------------------------------
//  store_the_character
//

static void
store_the_character (zs_lex_t *self)
{
    self->token [self->token_size++] = self->current;
    self->token [self->token_size] = 0;
}


//  ---------------------------------------------------------------------------
//  parse_next_character
//

static void
parse_next_character (zs_lex_t *self)
{
    self->current = *self->input_ptr;
    if (self->current)
        self->input_ptr++;      //  Don't advance past end of input
    fsm_set_next_event (self->fsm, self->events [(uint) self->current]);
}


//  ---------------------------------------------------------------------------
//  push_back_to_previous
//

static void
push_back_to_previous (zs_lex_t *self)
{
    //  This lets us handle tokens that are glued together
    if (self->input_ptr > self->input)
        self->input_ptr--;
}


//  ---------------------------------------------------------------------------
//  store_comma_character
//

static void
store_comma_character (zs_lex_t *self)
{
    self->token [self->token_size++] = ',';
    self->token [self->token_size] = 0;
}


//  ---------------------------------------------------------------------------
//  store_period_character
//

static void
store_period_character (zs_lex_t *self)
{
    self->token [self->token_size++] = '.';
    self->token [self->token_size] = 0;
}


//  ---------------------------------------------------------------------------
//  store_newline_character
//

static void
store_newline_character (zs_lex_t *self)
{
    self->token [self->token_size++] = '\n';
    self->token [self->token_size] = 0;
}


//  ---------------------------------------------------------------------------
//  have_fn_inline_token
//

static void
have_fn_inline_token (zs_lex_t *self)
{
    self->type = zs_lex_fn_inline;
}


//  ---------------------------------------------------------------------------
//  have_fn_nested_token
//

static void
have_fn_nested_token (zs_lex_t *self)
{
    self->type = zs_lex_fn_nested;
}


//  ---------------------------------------------------------------------------
//  have_fn_define_token
//

static void
have_fn_define_token (zs_lex_t *self)
{
    self->type = zs_lex_fn_define;
}


//  ---------------------------------------------------------------------------
//  have_number_token
//

static void
have_number_token (zs_lex_t *self)
{
   self->type = zs_lex_number;
}


//  ---------------------------------------------------------------------------
//  have_string_token
//

static void
have_string_token (zs_lex_t *self)
{
    self->type = zs_lex_string;
}


//  ---------------------------------------------------------------------------
//  have_close_list_token
//

static void
have_close_list_token (zs_lex_t *self)
{
    self->type = zs_lex_fn_close;
}


//  ---------------------------------------------------------------------------
//  have_start_menu_token
//

static void
have_start_menu_token (zs_lex_t *self)
{
    self->type = zs_lex_start_menu;
}


//  ---------------------------------------------------------------------------
//  have_end_menu_token
//

static void
have_end_menu_token (zs_lex_t *self)
{
    self->type = zs_lex_end_menu;
}


//  ---------------------------------------------------------------------------
//  have_choice_token
//

static void
have_choice_token (zs_lex_t *self)
{
    self->type = zs_lex_choice;
}


//  ---------------------------------------------------------------------------
//  have_start_loop_token
//

static void
have_start_loop_token (zs_lex_t *self)
{
    self->type = zs_lex_start_loop;
}


//  ---------------------------------------------------------------------------
//  have_end_loop_token
//

static void
have_end_loop_token (zs_lex_t *self)
{
    self->type = zs_lex_end_loop;
}


//  ---------------------------------------------------------------------------
//  have_phrase_token
//

static void
have_phrase_token (zs_lex_t *self)
{
    self->type = zs_lex_phrase;
}


//  ---------------------------------------------------------------------------
//  have_sentence_token
//

static void
have_sentence_token (zs_lex_t *self)
{
    self->type = zs_lex_sentence;
}


//  ---------------------------------------------------------------------------
//  have_null_token
//

static void
have_null_token (zs_lex_t *self)
{
    self->type = zs_lex_null;
}


//  ---------------------------------------------------------------------------
//  have_invalid_token
//

static void
have_invalid_token (zs_lex_t *self)
{
    self->type = zs_lex_invalid;
}


//  ---------------------------------------------------------------------------
//  Return number of processing cycles used so far

uint64_t
zs_lex_cycles (zs_lex_t *self)
{
    return fsm_cycles (self->fsm);
}


//  ---------------------------------------------------------------------------
//  Selftest

void
zs_lex_test (bool verbose)
{
    printf (" * zs_lex: ");
    if (verbose)
        printf ("\n");

    //  @selftest
    zs_lex_t *lex = zs_lex_new ();
    zs_lex_set_verbose (lex, verbose);

    //  Simple numbers and strings
    assert (zs_lex_first (lex, "1234") == zs_lex_number);
    assert (zs_lex_next (lex) == zs_lex_null);
    assert (zs_lex_next (lex) == zs_lex_null);

    assert (zs_lex_first (lex, "1234 4567") == zs_lex_number);
    assert (zs_lex_next (lex) == zs_lex_number);
    assert (zs_lex_next (lex) == zs_lex_null);

    assert (zs_lex_first (lex, "<Hello, World>") == zs_lex_string);
    assert (zs_lex_next (lex) == zs_lex_null);

    assert (zs_lex_first (lex, "<Hello,>\n<World>") == zs_lex_string);
    assert (zs_lex_next (lex) == zs_lex_string);
    assert (zs_lex_next (lex) == zs_lex_null);

    assert (zs_lex_first (lex, "<Here is a long string") == zs_lex_null);
    assert (zs_lex_first (lex, " which continues over two lines>") == zs_lex_string);
    assert (zs_lex_next (lex) == zs_lex_null);

    //  Calling inline functions
    assert (zs_lex_first (lex, "something(22.7e2)") == zs_lex_fn_nested);
    assert (streq (zs_lex_value (lex), "something"));
    assert (zs_lex_next (lex) == zs_lex_number);
    assert (zs_lex_next (lex) == zs_lex_fn_close);
    assert (zs_lex_next (lex) == zs_lex_null);

    //  Defining functions
    assert (zs_lex_first (lex, "pi: ( 22.7 )") == zs_lex_fn_define);
    assert (zs_lex_next (lex) == zs_lex_number);
    assert (zs_lex_next (lex) == zs_lex_fn_close);
    assert (zs_lex_next (lex) == zs_lex_null);

    assert (zs_lex_first (lex, "twopi:( pi 2 times)") == zs_lex_fn_define);
    assert (streq (zs_lex_value (lex), "twopi"));
    assert (zs_lex_next (lex) == zs_lex_fn_inline);
    assert (streq (zs_lex_value (lex), "pi"));
    assert (zs_lex_next (lex) == zs_lex_number);
    assert (zs_lex_next (lex) == zs_lex_fn_inline);
    assert (streq (zs_lex_value (lex), "times"));
    assert (zs_lex_next (lex) == zs_lex_fn_close);
    assert (zs_lex_next (lex) == zs_lex_null);

    //  Various numeric forms
    assert (zs_lex_first (lex, "1 +1 -1 0.1") == zs_lex_number);
    assert (zs_lex_next (lex) == zs_lex_number);
    assert (zs_lex_next (lex) == zs_lex_number);
    assert (zs_lex_next (lex) == zs_lex_number);
    assert (zs_lex_next (lex) == zs_lex_null);

    assert (zs_lex_first (lex, "3.141592653589793238462643383279502884197169") == zs_lex_number);
    assert (zs_lex_next (lex) == zs_lex_null);

    assert (zs_lex_first (lex, "66%") == zs_lex_number);
    assert (zs_lex_next (lex) == zs_lex_null);

    assert (zs_lex_first (lex, "1E10 3.14e+000 1,000,000") == zs_lex_number);
    assert (zs_lex_next (lex) == zs_lex_number);
    assert (zs_lex_next (lex) == zs_lex_number);
    assert (zs_lex_next (lex) == zs_lex_null);

    assert (zs_lex_first (lex, "2,3 2, 3") == zs_lex_number);
    assert (zs_lex_next (lex) == zs_lex_number);
    assert (zs_lex_next (lex) == zs_lex_phrase);
    assert (zs_lex_next (lex) == zs_lex_number);
    assert (zs_lex_next (lex) == zs_lex_null);

    assert (zs_lex_first (lex, "2.3 2. 3") == zs_lex_number);
    assert (zs_lex_next (lex) == zs_lex_number);
    assert (zs_lex_next (lex) == zs_lex_sentence);
    assert (zs_lex_next (lex) == zs_lex_number);
    assert (zs_lex_next (lex) == zs_lex_null);

    //  Inline comments
    assert (zs_lex_first (lex, "# This is a comment") == zs_lex_null);

    //  Mathematical operators
    assert (zs_lex_first (lex, "+ - * / ^") == zs_lex_fn_inline);
    assert (zs_lex_next (lex) == zs_lex_fn_inline);
    assert (zs_lex_next (lex) == zs_lex_fn_inline);
    assert (zs_lex_next (lex) == zs_lex_fn_inline);
    assert (zs_lex_next (lex) == zs_lex_fn_inline);
    assert (zs_lex_next (lex) == zs_lex_null);

    //  These also work, which may need tightening
    assert (zs_lex_first (lex, "++ -- ** // ^^ *2") == zs_lex_fn_inline);
    assert (zs_lex_next (lex) == zs_lex_fn_inline);
    assert (zs_lex_next (lex) == zs_lex_fn_inline);
    assert (zs_lex_next (lex) == zs_lex_fn_inline);
    assert (zs_lex_next (lex) == zs_lex_fn_inline);
    assert (zs_lex_next (lex) == zs_lex_fn_inline);
    assert (zs_lex_next (lex) == zs_lex_null);

    //  Menus are marked by square braces
    assert (zs_lex_first (lex, "1 [<hello>]") == zs_lex_number);
    assert (zs_lex_next (lex) == zs_lex_start_menu);
    assert (zs_lex_next (lex) == zs_lex_string);
    assert (zs_lex_next (lex) == zs_lex_end_menu);
    assert (zs_lex_next (lex) == zs_lex_null);

    //  loops are marked by curly braces
    assert (zs_lex_first (lex, "{ 4 }") == zs_lex_start_loop);
    assert (zs_lex_next (lex) == zs_lex_number);
    assert (zs_lex_next (lex) == zs_lex_end_loop);
    assert (zs_lex_next (lex) == zs_lex_null);

    //  Test various invalid tokens
    assert (zs_lex_first (lex, "!Hello, World>") == zs_lex_invalid);
    assert (zs_lex_first (lex, "<Hello,>?<World>") == zs_lex_string);
    assert (zs_lex_next (lex) == zs_lex_invalid);
    assert (zs_lex_first (lex, "echo ( some text >") == zs_lex_fn_nested);
    assert (streq (zs_lex_value (lex), "echo"));
    assert (zs_lex_next (lex) == zs_lex_fn_inline);
    assert (streq (zs_lex_value (lex), "some"));
    assert (zs_lex_next (lex) == zs_lex_fn_inline);
    assert (streq (zs_lex_value (lex), "text"));
    assert (zs_lex_next (lex) == zs_lex_invalid);
    assert (zs_lex_next (lex) == zs_lex_null);
    assert (zs_lex_first (lex, "1?2") == zs_lex_invalid);

    if (verbose)
        printf ("%ld cycles done\n", (long) zs_lex_cycles (lex));
    zs_lex_destroy (&lex);
    //  @end
    printf ("OK\n");
}
