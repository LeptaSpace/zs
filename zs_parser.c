/*  =========================================================================
    zs_parser - No title

    =========================================================================
*/

/*
@header
    Description of class for man page.
@discuss
    Detailed discussion of the class, if any.
@end
*/

#include "zs_parser.h"          //  Our own API definitions
#include "zs_parser_fsm.h"      //  Finite state machine engine

//  Structure of our class

struct _zs_parser_t {
    fsm_t *fsm;             //  Our finite state machine
    event_t charmap [256];  //  Map characters to events
    const char *input;      //  Line of text we're parsing
    const char *input_ptr;  //  Next character to process
    uint line_nbr;          //  Number of lines processed
    uint token_size;        //  Size of token so far
    char token [1025];      //  Current token, max size 1K
    char current;           //  Current character
};

static void
s_set_charmap (zs_parser_t *self, const char *chars, event_t event)
{
    while (*chars)
        self->charmap [(int) *chars++] = event;
}

//  ---------------------------------------------------------------------------
//  Create a new zs_parser, return the reference if successful, or NULL
//  if construction failed due to lack of available memory.

zs_parser_t *
zs_parser_new (void)
{
    zs_parser_t *self = (zs_parser_t *) zmalloc (sizeof (zs_parser_t));
    if (self) {
        self->fsm = fsm_new (self);
        s_set_charmap (self, "ABCDEFGHIJKLMNOPQRSTUVWXYZ", letter_event);
        s_set_charmap (self, "abcdefghijklmnopqrstuvwxyz", letter_event);
        s_set_charmap (self, "0123456789", digit_event);
        s_set_charmap (self, "-+", sign_event);
        s_set_charmap (self, "/_", separator_event);
        s_set_charmap (self, ",.:", punctuation_event);
        s_set_charmap (self, "[", open_quote_event);
        s_set_charmap (self, "]", close_quote_event);
        s_set_charmap (self, " \t\n", white_space_event);
        self->charmap [0] = eol_event;
    }
    return self;
}


//  ---------------------------------------------------------------------------
//  Destroy the zs_parser and free all memory used by the object.

void
zs_parser_destroy (zs_parser_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        zs_parser_t *self = *self_p;
        fsm_destroy (&self->fsm);
        free (self);
        *self_p = NULL;
    }
}


//  ---------------------------------------------------------------------------
//  Enable verbose tracing of parser

void
zs_parser_verbose (zs_parser_t *self, bool verbose)
{
    fsm_set_animate (self->fsm, verbose);
}


//  ---------------------------------------------------------------------------
//  Parse and execute a buffer of input text.

void
zs_parser_execute (zs_parser_t *self, const char *input)
{
    printf (">%s\n", input);
    self->input = input;
    fsm_execute (self->fsm, ok_event);
}


//  ---------------------------------------------------------------------------
//  read_first_character
//

static void
read_first_character (zs_parser_t *self)
{
    self->input_ptr = self->input;
    read_next_character (self);
}


//  ---------------------------------------------------------------------------
//  read_next_character
//

static void
read_next_character (zs_parser_t *self)
{
    self->current = *self->input_ptr++;
    event_t event = self->charmap [(int) self->current];
    fsm_set_next_event (self->fsm, event? event: other_event);
}


//  ---------------------------------------------------------------------------
//  start_new_token
//

static void
start_new_token (zs_parser_t *self)
{
    self->token_size = 0;
}


//  ---------------------------------------------------------------------------
//  store_the_character
//

static void
store_the_character (zs_parser_t *self)
{
    self->token [self->token_size++] = self->current;
    self->token [self->token_size] = 0;
}


//  ---------------------------------------------------------------------------
//  have_function
//

static void
have_function (zs_parser_t *self)
{
}


//  ---------------------------------------------------------------------------
//  have_number
//

static void
have_number (zs_parser_t *self)
{
}


//  ---------------------------------------------------------------------------
//  have_string
//

static void
have_string (zs_parser_t *self)
{
}


//  ---------------------------------------------------------------------------
//  have_punctuation
//

static void
have_punctuation (zs_parser_t *self)
{
}


//  ---------------------------------------------------------------------------
//  report_unfinished_string
//

static void
report_unfinished_string (zs_parser_t *self)
{
}


//  ---------------------------------------------------------------------------
//  report_unexpected_input
//

static void
report_unexpected_input (zs_parser_t *self)
{
    printf ("%s\n", self->input);
    printf ("%*c\n", (int) (self->input_ptr - self->input), '^');
    printf ("Line %u: unexpected input [%c]\n", self->line_nbr, self->current);
}


//  ---------------------------------------------------------------------------
//  Selftest

void
zs_parser_test (bool verbose)
{
    printf (" * zs_parser: ");
    if (verbose)
        printf ("\n");

    //  @selftest
    zs_parser_t *parser = zs_parser_new ();
    zs_parser_verbose (parser, verbose);
    uint times;
    for (times = 0; times < 2; times++)
        zs_parser_execute (parser, "This is a string 12345 [Hello] Not again");
    zs_parser_destroy (&parser);
    //  @end
    printf ("OK\n");
}
