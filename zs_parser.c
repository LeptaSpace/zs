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
        self->charmap [(uint) *chars++] = event;
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
        uint char_nbr;
        self->charmap [0] = finished_event;
        for (char_nbr = 1; char_nbr < 256; char_nbr++)
            self->charmap [char_nbr] = other_event;
        //  There are two ways to do this; either we define character
        //  classes that produce generic events depending on the current
        //  state (e.g. hyphen_event in function names, or minus_event in
        //  numbers), or else we define lower level events that the FSM
        //  sorts out. I've chosen the second design so decisions stay in
        //  the FSM.
        s_set_charmap (self, "ABCDEFGHIJKLMNOPQRSTUVWXYZ", letter_event);
        s_set_charmap (self, "abcdefghijklmnopqrstuvwxyz", letter_event);
        s_set_charmap (self, "0123456789", digit_event);
        s_set_charmap (self, "-", hyphen_event);
        s_set_charmap (self, "+", plus_event);
        s_set_charmap (self, "/", slash_event);
        s_set_charmap (self, "_", underscore_event);
        s_set_charmap (self, ".", period_event);
        s_set_charmap (self, ",", comma_event);
        s_set_charmap (self, ":", colon_event);
        s_set_charmap (self, "*", asterisk_event);
        s_set_charmap (self, "^", caret_event);
        s_set_charmap (self, "%", percent_event);
        s_set_charmap (self, "[", open_quote_event);
        s_set_charmap (self, "]", close_quote_event);
        s_set_charmap (self, "(", open_paren_event);
        s_set_charmap (self, ")", close_paren_event);
        s_set_charmap (self, " \t", white_space_event);
        s_set_charmap (self, "\n", newline_event);
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
//  Parse and execute a buffer of one or more lines of code.

void
zs_parser_execute (zs_parser_t *self, const char *input)
{
//     printf (">%s\n", input);
    self->input = input;
    self->input_ptr = self->input;
    self->current = *self->input_ptr++;
    self->line_nbr++;
    fsm_execute (self->fsm, self->charmap [(uint) self->current]);
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
    fsm_set_next_event (self->fsm, self->charmap [(uint) self->current]);
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
//  store_newline_character
//

static void
store_newline_character (zs_parser_t *self)
{
    self->current = '\n';
    store_the_character (self);
}


//  ---------------------------------------------------------------------------
//  have_function
//

static void
have_function (zs_parser_t *self)
{
    printf ("have function: %s\n", self->token);
}


//  ---------------------------------------------------------------------------
//  have_number_candidate
//

static void
have_number_candidate (zs_parser_t *self)
{
    //  Check that number fits the various patterns we accept
    //  All digits
    //  A single period at start, or embedded in number
    //  Commas, used for thousand seperators, in the right place
    //  +- as unary sign operators
    //  +-/:*x^v binary operators, evaluated ^v then *x/: then +-
    //  () to force priority
    //  [0-9]+[eE][+-]?[0-9]+ used once as exponent
    //  Ki Mi Gi Ti Pi Ei used as suffix
    //  h k M G T P E Z Y used as suffix
    //  d c m u n p f a z y used as suffix
    //  Store as int, 64 bit, or double; typed primitives plusi plus64 plusd
    printf ("have number: %s\n", self->token);
}


//  ---------------------------------------------------------------------------
//  have_string
//

static void
have_string (zs_parser_t *self)
{
    printf ("have string: %s\n", self->token);
}


//  ---------------------------------------------------------------------------
//  have_punctuation
//

static void
have_punctuation (zs_parser_t *self)
{
    printf ("have punctuation: %s\n", self->token);
}


//  ---------------------------------------------------------------------------
//  count_line_processed
//

static void
count_line_processed (zs_parser_t *self)
{
    self->line_nbr++;
}


//  ---------------------------------------------------------------------------
//  report_unfinished_string
//

static void
report_unfinished_string (zs_parser_t *self)
{
    printf ("unfinished string: %s\n", self->token);
}


//  ---------------------------------------------------------------------------
//  report_unexpected_input
//

static void
report_unexpected_input (zs_parser_t *self)
{
    printf ("%s\n", self->input);
    printf ("%*c\n", (uint) (self->input_ptr - self->input), '^');
    printf ("Line %u: unexpected input '%c'\n", self->line_nbr, self->current);
}


//  ---------------------------------------------------------------------------
//  Return number of processing cycles used so far

uint64_t
zs_parser_cycles (zs_parser_t *self)
{
    return fsm_cycles (self->fsm);
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
    zs_parser_execute (parser, "echo ([Hello, World])");
    zs_parser_execute (parser, "echo ([Hello, ]\n[World])");
    zs_parser_execute (parser, "echo (1234 5678)");
//     zs_parser_execute (parser, "1 +1 -1 .1 0.1");
//     zs_parser_execute (parser, "3.141592653589793238462643383279502884197169");
//     zs_parser_execute (parser, "1/2 1:2 1024*1024 10^10 1v2 99:70");
//     zs_parser_execute (parser, "1E10 3.14e+000");
//     zs_parser_execute (parser, "2k 2M 2G 2T 2P 2E 2Z 2Y");
//     zs_parser_execute (parser, "2Ki 2Mi 2Gi 2Ti 2Pi 2Ei");
//     zs_parser_execute (parser, "2d 2c 2m 2u 2n 2p 2f 2a 2z 2y");
//     zs_parser_execute (parser, "[Here is a long string");
//     zs_parser_execute (parser, " which continues over two lines]");

//     printf ("%ld cycles done\n", (long) zs_parser_cycles (parser));
    zs_parser_destroy (&parser);
    //  @end
    printf ("OK\n");
}
