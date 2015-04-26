/*  =========================================================================
    zs_parser_fsm - No title state machine engine

    ** WARNING *************************************************************
    THIS SOURCE FILE IS 100% GENERATED. If you edit this file, you will lose
    your changes at the next build cycle. This is great for temporary printf
    statements. DO NOT MAKE ANY CHANGES YOU WISH TO KEEP. The correct places
    for commits are:

     * The XML model used for this code generation: zs_parser.xml, or
     * The code generation script that built this file: fsm_c
    ************************************************************************
    =========================================================================
*/


//  ---------------------------------------------------------------------------
//  State machine constants

typedef enum {
    expecting_token_state = 1,
    reading_function_state = 2,
    after_function_state = 3,
    reading_number_state = 4,
    reading_string_state = 5,
    defaults_state = 6
} state_t;

typedef enum {
    NULL_event = 0,
    letter_event = 1,
    hyphen_event = 2,
    plus_event = 3,
    period_event = 4,
    digit_event = 5,
    open_quote_event = 6,
    white_space_event = 7,
    newline_event = 8,
    punctuation_event = 9,
    finished_event = 10,
    slash_event = 11,
    underscore_event = 12,
    comma_event = 13,
    colon_event = 14,
    asterisk_event = 15,
    caret_event = 16,
    percent_event = 17,
    open_paren_event = 18,
    close_paren_event = 19,
    close_quote_event = 20,
    other_event = 21
} event_t;

//  Names for state machine logging and error reporting
static char *
s_state_name [] = {
    "(NONE)",
    "expecting_token",
    "reading_function",
    "after_function",
    "reading_number",
    "reading_string",
    "defaults"
};

static char *
s_event_name [] = {
    "(NONE)",
    "letter",
    "hyphen",
    "plus",
    "period",
    "digit",
    "open_quote",
    "white_space",
    "newline",
    "punctuation",
    "finished",
    "slash",
    "underscore",
    "comma",
    "colon",
    "asterisk",
    "caret",
    "percent",
    "open_paren",
    "close_paren",
    "close_quote",
    "other"
};

//  Action prototypes
static void start_new_token (zs_parser_t *self);
static void store_the_character (zs_parser_t *self);
static void read_next_character (zs_parser_t *self);
static void count_line_processed (zs_parser_t *self);
static void have_punctuation (zs_parser_t *self);
static void have_function (zs_parser_t *self);
static void have_number_candidate (zs_parser_t *self);
static void have_string (zs_parser_t *self);
static void store_newline_character (zs_parser_t *self);
static void report_unexpected_input (zs_parser_t *self);

//  This is the context block for a FSM thread; use the setter
//  methods to set the FSM properties.

typedef struct {
    zs_parser_t *parent;        //  Parent class
    bool animate;               //  Animate state machine
    state_t state;              //  Current state
    event_t event;              //  Current event
    event_t next_event;         //  The next event
    event_t exception;          //  Exception event, if any
    uint64_t cycles;            //  Track the work done
} fsm_t;

static fsm_t *
fsm_new (zs_parser_t *parent)
{
    fsm_t *self = (fsm_t *) zmalloc (sizeof (fsm_t));
    if (self) {
        self->state = expecting_token_state;
        self->event = NULL_event;
        self->parent = parent;
    }
    return self;
}

static void
fsm_destroy (fsm_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        fsm_t *self = *self_p;
        free (self);
        *self_p = NULL;
    }
}

static void
fsm_set_next_event (fsm_t *self, event_t next_event)
{
    self->next_event = next_event;
}

static void
fsm_set_exception (fsm_t *self, event_t exception)
{
    self->exception = exception;
}

static void
fsm_set_animate (fsm_t *self, bool animate)
{
    self->animate = animate;
}

static uint64_t
fsm_cycles (fsm_t *self)
{
    return self->cycles;
}


//  Execute state machine until it has no next event; returns control
//  to caller so it can return with a further event.

static void
fsm_execute (fsm_t *self, event_t event)
{
    self->next_event = event;
    while (self->next_event != NULL_event) {
        self->cycles++;
        self->event = self->next_event;
        self->next_event = NULL_event;
        self->exception = NULL_event;
        if (self->animate) {
            zsys_debug ("zs_parser: %s:", s_state_name [self->state]);
            zsys_debug ("zs_parser:         %s", s_event_name [self->event]);
        }
        if (self->state == expecting_token_state) {
            if (self->event == letter_event) {
                if (!self->exception) {
                    //  start_new_token
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ start_new_token");
                    start_new_token (self->parent);
                }
                if (!self->exception) {
                    //  store_the_character
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ store_the_character");
                    store_the_character (self->parent);
                }
                if (!self->exception) {
                    //  read_next_character
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ read_next_character");
                    read_next_character (self->parent);
                }
                if (!self->exception)
                    self->state = reading_function_state;
            }
            else
            if (self->event == hyphen_event) {
                if (!self->exception) {
                    //  start_new_token
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ start_new_token");
                    start_new_token (self->parent);
                }
                if (!self->exception) {
                    //  store_the_character
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ store_the_character");
                    store_the_character (self->parent);
                }
                if (!self->exception) {
                    //  read_next_character
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ read_next_character");
                    read_next_character (self->parent);
                }
                if (!self->exception)
                    self->state = reading_number_state;
            }
            else
            if (self->event == plus_event) {
                if (!self->exception) {
                    //  start_new_token
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ start_new_token");
                    start_new_token (self->parent);
                }
                if (!self->exception) {
                    //  store_the_character
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ store_the_character");
                    store_the_character (self->parent);
                }
                if (!self->exception) {
                    //  read_next_character
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ read_next_character");
                    read_next_character (self->parent);
                }
                if (!self->exception)
                    self->state = reading_number_state;
            }
            else
            if (self->event == period_event) {
                if (!self->exception) {
                    //  start_new_token
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ start_new_token");
                    start_new_token (self->parent);
                }
                if (!self->exception) {
                    //  store_the_character
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ store_the_character");
                    store_the_character (self->parent);
                }
                if (!self->exception) {
                    //  read_next_character
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ read_next_character");
                    read_next_character (self->parent);
                }
                if (!self->exception)
                    self->state = reading_number_state;
            }
            else
            if (self->event == digit_event) {
                if (!self->exception) {
                    //  start_new_token
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ start_new_token");
                    start_new_token (self->parent);
                }
                if (!self->exception) {
                    //  store_the_character
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ store_the_character");
                    store_the_character (self->parent);
                }
                if (!self->exception) {
                    //  read_next_character
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ read_next_character");
                    read_next_character (self->parent);
                }
                if (!self->exception)
                    self->state = reading_number_state;
            }
            else
            if (self->event == open_quote_event) {
                if (!self->exception) {
                    //  start_new_token
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ start_new_token");
                    start_new_token (self->parent);
                }
                if (!self->exception) {
                    //  read_next_character
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ read_next_character");
                    read_next_character (self->parent);
                }
                if (!self->exception)
                    self->state = reading_string_state;
            }
            else
            if (self->event == white_space_event) {
                if (!self->exception) {
                    //  read_next_character
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ read_next_character");
                    read_next_character (self->parent);
                }
            }
            else
            if (self->event == newline_event) {
                if (!self->exception) {
                    //  count_line_processed
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ count_line_processed");
                    count_line_processed (self->parent);
                }
                if (!self->exception) {
                    //  read_next_character
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ read_next_character");
                    read_next_character (self->parent);
                }
            }
            else
            if (self->event == punctuation_event) {
                if (!self->exception) {
                    //  have_punctuation
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ have_punctuation");
                    have_punctuation (self->parent);
                }
                if (!self->exception) {
                    //  read_next_character
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ read_next_character");
                    read_next_character (self->parent);
                }
            }
            else
            if (self->event == finished_event) {
                //  No action - just logging
                if (self->animate)
                    zsys_debug ("zs_parser:             $ finished");
            }
            else
            if (self->event == slash_event) {
                if (!self->exception) {
                    //  report_unexpected_input
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ report_unexpected_input");
                    report_unexpected_input (self->parent);
                }
                if (!self->exception)
                    self->state = expecting_token_state;
            }
            else
            if (self->event == underscore_event) {
                if (!self->exception) {
                    //  report_unexpected_input
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ report_unexpected_input");
                    report_unexpected_input (self->parent);
                }
                if (!self->exception)
                    self->state = expecting_token_state;
            }
            else
            if (self->event == comma_event) {
                if (!self->exception) {
                    //  report_unexpected_input
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ report_unexpected_input");
                    report_unexpected_input (self->parent);
                }
                if (!self->exception)
                    self->state = expecting_token_state;
            }
            else
            if (self->event == colon_event) {
                if (!self->exception) {
                    //  report_unexpected_input
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ report_unexpected_input");
                    report_unexpected_input (self->parent);
                }
                if (!self->exception)
                    self->state = expecting_token_state;
            }
            else
            if (self->event == asterisk_event) {
                if (!self->exception) {
                    //  report_unexpected_input
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ report_unexpected_input");
                    report_unexpected_input (self->parent);
                }
                if (!self->exception)
                    self->state = expecting_token_state;
            }
            else
            if (self->event == caret_event) {
                if (!self->exception) {
                    //  report_unexpected_input
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ report_unexpected_input");
                    report_unexpected_input (self->parent);
                }
                if (!self->exception)
                    self->state = expecting_token_state;
            }
            else
            if (self->event == percent_event) {
                if (!self->exception) {
                    //  report_unexpected_input
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ report_unexpected_input");
                    report_unexpected_input (self->parent);
                }
                if (!self->exception)
                    self->state = expecting_token_state;
            }
            else
            if (self->event == close_quote_event) {
                if (!self->exception) {
                    //  report_unexpected_input
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ report_unexpected_input");
                    report_unexpected_input (self->parent);
                }
                if (!self->exception)
                    self->state = expecting_token_state;
            }
            else
            if (self->event == open_paren_event) {
                if (!self->exception) {
                    //  report_unexpected_input
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ report_unexpected_input");
                    report_unexpected_input (self->parent);
                }
                if (!self->exception)
                    self->state = expecting_token_state;
            }
            else
            if (self->event == close_paren_event) {
                if (!self->exception) {
                    //  report_unexpected_input
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ report_unexpected_input");
                    report_unexpected_input (self->parent);
                }
                if (!self->exception)
                    self->state = expecting_token_state;
            }
            else
            if (self->event == other_event) {
                if (!self->exception) {
                    //  report_unexpected_input
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ report_unexpected_input");
                    report_unexpected_input (self->parent);
                }
                if (!self->exception)
                    self->state = expecting_token_state;
            }
            else {
                //  Handle unexpected internal events
                zsys_warning ("zs_parser: unhandled event %s in %s",
                    s_event_name [self->event], s_state_name [self->state]);
                assert (false);
            }
        }
        else
        if (self->state == reading_function_state) {
            if (self->event == letter_event) {
                if (!self->exception) {
                    //  store_the_character
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ store_the_character");
                    store_the_character (self->parent);
                }
                if (!self->exception) {
                    //  read_next_character
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ read_next_character");
                    read_next_character (self->parent);
                }
            }
            else
            if (self->event == digit_event) {
                if (!self->exception) {
                    //  store_the_character
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ store_the_character");
                    store_the_character (self->parent);
                }
                if (!self->exception) {
                    //  read_next_character
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ read_next_character");
                    read_next_character (self->parent);
                }
            }
            else
            if (self->event == hyphen_event) {
                if (!self->exception) {
                    //  store_the_character
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ store_the_character");
                    store_the_character (self->parent);
                }
                if (!self->exception) {
                    //  read_next_character
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ read_next_character");
                    read_next_character (self->parent);
                }
            }
            else
            if (self->event == period_event) {
                if (!self->exception) {
                    //  store_the_character
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ store_the_character");
                    store_the_character (self->parent);
                }
                if (!self->exception) {
                    //  read_next_character
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ read_next_character");
                    read_next_character (self->parent);
                }
            }
            else
            if (self->event == slash_event) {
                if (!self->exception) {
                    //  store_the_character
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ store_the_character");
                    store_the_character (self->parent);
                }
                if (!self->exception) {
                    //  read_next_character
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ read_next_character");
                    read_next_character (self->parent);
                }
            }
            else
            if (self->event == underscore_event) {
                if (!self->exception) {
                    //  store_the_character
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ store_the_character");
                    store_the_character (self->parent);
                }
                if (!self->exception) {
                    //  read_next_character
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ read_next_character");
                    read_next_character (self->parent);
                }
            }
            else
            if (self->event == white_space_event) {
                if (!self->exception) {
                    //  have_function
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ have_function");
                    have_function (self->parent);
                }
                if (!self->exception) {
                    //  read_next_character
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ read_next_character");
                    read_next_character (self->parent);
                }
                if (!self->exception)
                    self->state = after_function_state;
            }
            else
            if (self->event == newline_event) {
                if (!self->exception) {
                    //  count_line_processed
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ count_line_processed");
                    count_line_processed (self->parent);
                }
                if (!self->exception) {
                    //  read_next_character
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ read_next_character");
                    read_next_character (self->parent);
                }
                if (!self->exception)
                    self->state = after_function_state;
            }
            else
            if (self->event == finished_event) {
                if (!self->exception) {
                    //  have_function
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ have_function");
                    have_function (self->parent);
                }
                if (!self->exception)
                    self->state = after_function_state;
            }
            else
            if (self->event == plus_event) {
                if (!self->exception) {
                    //  report_unexpected_input
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ report_unexpected_input");
                    report_unexpected_input (self->parent);
                }
                if (!self->exception)
                    self->state = expecting_token_state;
            }
            else
            if (self->event == comma_event) {
                if (!self->exception) {
                    //  report_unexpected_input
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ report_unexpected_input");
                    report_unexpected_input (self->parent);
                }
                if (!self->exception)
                    self->state = expecting_token_state;
            }
            else
            if (self->event == colon_event) {
                if (!self->exception) {
                    //  report_unexpected_input
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ report_unexpected_input");
                    report_unexpected_input (self->parent);
                }
                if (!self->exception)
                    self->state = expecting_token_state;
            }
            else
            if (self->event == asterisk_event) {
                if (!self->exception) {
                    //  report_unexpected_input
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ report_unexpected_input");
                    report_unexpected_input (self->parent);
                }
                if (!self->exception)
                    self->state = expecting_token_state;
            }
            else
            if (self->event == caret_event) {
                if (!self->exception) {
                    //  report_unexpected_input
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ report_unexpected_input");
                    report_unexpected_input (self->parent);
                }
                if (!self->exception)
                    self->state = expecting_token_state;
            }
            else
            if (self->event == percent_event) {
                if (!self->exception) {
                    //  report_unexpected_input
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ report_unexpected_input");
                    report_unexpected_input (self->parent);
                }
                if (!self->exception)
                    self->state = expecting_token_state;
            }
            else
            if (self->event == open_quote_event) {
                if (!self->exception) {
                    //  report_unexpected_input
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ report_unexpected_input");
                    report_unexpected_input (self->parent);
                }
                if (!self->exception)
                    self->state = expecting_token_state;
            }
            else
            if (self->event == close_quote_event) {
                if (!self->exception) {
                    //  report_unexpected_input
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ report_unexpected_input");
                    report_unexpected_input (self->parent);
                }
                if (!self->exception)
                    self->state = expecting_token_state;
            }
            else
            if (self->event == open_paren_event) {
                if (!self->exception) {
                    //  report_unexpected_input
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ report_unexpected_input");
                    report_unexpected_input (self->parent);
                }
                if (!self->exception)
                    self->state = expecting_token_state;
            }
            else
            if (self->event == close_paren_event) {
                if (!self->exception) {
                    //  report_unexpected_input
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ report_unexpected_input");
                    report_unexpected_input (self->parent);
                }
                if (!self->exception)
                    self->state = expecting_token_state;
            }
            else
            if (self->event == other_event) {
                if (!self->exception) {
                    //  report_unexpected_input
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ report_unexpected_input");
                    report_unexpected_input (self->parent);
                }
                if (!self->exception)
                    self->state = expecting_token_state;
            }
            else {
                //  Handle unexpected internal events
                zsys_warning ("zs_parser: unhandled event %s in %s",
                    s_event_name [self->event], s_state_name [self->state]);
                assert (false);
            }
        }
        else
        if (self->state == after_function_state) {
            if (self->event == letter_event) {
                if (!self->exception) {
                    //  start_new_token
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ start_new_token");
                    start_new_token (self->parent);
                }
                if (!self->exception) {
                    //  store_the_character
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ store_the_character");
                    store_the_character (self->parent);
                }
                if (!self->exception) {
                    //  read_next_character
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ read_next_character");
                    read_next_character (self->parent);
                }
                if (!self->exception)
                    self->state = reading_function_state;
            }
            else
            if (self->event == hyphen_event) {
                if (!self->exception) {
                    //  start_new_token
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ start_new_token");
                    start_new_token (self->parent);
                }
                if (!self->exception) {
                    //  store_the_character
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ store_the_character");
                    store_the_character (self->parent);
                }
                if (!self->exception) {
                    //  read_next_character
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ read_next_character");
                    read_next_character (self->parent);
                }
                if (!self->exception)
                    self->state = reading_number_state;
            }
            else
            if (self->event == plus_event) {
                if (!self->exception) {
                    //  start_new_token
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ start_new_token");
                    start_new_token (self->parent);
                }
                if (!self->exception) {
                    //  store_the_character
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ store_the_character");
                    store_the_character (self->parent);
                }
                if (!self->exception) {
                    //  read_next_character
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ read_next_character");
                    read_next_character (self->parent);
                }
                if (!self->exception)
                    self->state = reading_number_state;
            }
            else
            if (self->event == period_event) {
                if (!self->exception) {
                    //  start_new_token
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ start_new_token");
                    start_new_token (self->parent);
                }
                if (!self->exception) {
                    //  store_the_character
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ store_the_character");
                    store_the_character (self->parent);
                }
                if (!self->exception) {
                    //  read_next_character
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ read_next_character");
                    read_next_character (self->parent);
                }
                if (!self->exception)
                    self->state = reading_number_state;
            }
            else
            if (self->event == digit_event) {
                if (!self->exception) {
                    //  start_new_token
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ start_new_token");
                    start_new_token (self->parent);
                }
                if (!self->exception) {
                    //  store_the_character
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ store_the_character");
                    store_the_character (self->parent);
                }
                if (!self->exception) {
                    //  read_next_character
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ read_next_character");
                    read_next_character (self->parent);
                }
                if (!self->exception)
                    self->state = reading_number_state;
            }
            else
            if (self->event == open_quote_event) {
                if (!self->exception) {
                    //  start_new_token
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ start_new_token");
                    start_new_token (self->parent);
                }
                if (!self->exception) {
                    //  read_next_character
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ read_next_character");
                    read_next_character (self->parent);
                }
                if (!self->exception)
                    self->state = reading_string_state;
            }
            else
            if (self->event == white_space_event) {
                if (!self->exception) {
                    //  read_next_character
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ read_next_character");
                    read_next_character (self->parent);
                }
            }
            else
            if (self->event == newline_event) {
                if (!self->exception) {
                    //  count_line_processed
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ count_line_processed");
                    count_line_processed (self->parent);
                }
                if (!self->exception) {
                    //  read_next_character
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ read_next_character");
                    read_next_character (self->parent);
                }
            }
            else
            if (self->event == punctuation_event) {
                if (!self->exception) {
                    //  have_punctuation
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ have_punctuation");
                    have_punctuation (self->parent);
                }
                if (!self->exception) {
                    //  read_next_character
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ read_next_character");
                    read_next_character (self->parent);
                }
            }
            else
            if (self->event == finished_event) {
                //  No action - just logging
                if (self->animate)
                    zsys_debug ("zs_parser:             $ finished");
            }
            else
            if (self->event == slash_event) {
                if (!self->exception) {
                    //  report_unexpected_input
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ report_unexpected_input");
                    report_unexpected_input (self->parent);
                }
                if (!self->exception)
                    self->state = expecting_token_state;
            }
            else
            if (self->event == underscore_event) {
                if (!self->exception) {
                    //  report_unexpected_input
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ report_unexpected_input");
                    report_unexpected_input (self->parent);
                }
                if (!self->exception)
                    self->state = expecting_token_state;
            }
            else
            if (self->event == comma_event) {
                if (!self->exception) {
                    //  report_unexpected_input
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ report_unexpected_input");
                    report_unexpected_input (self->parent);
                }
                if (!self->exception)
                    self->state = expecting_token_state;
            }
            else
            if (self->event == colon_event) {
                if (!self->exception) {
                    //  report_unexpected_input
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ report_unexpected_input");
                    report_unexpected_input (self->parent);
                }
                if (!self->exception)
                    self->state = expecting_token_state;
            }
            else
            if (self->event == asterisk_event) {
                if (!self->exception) {
                    //  report_unexpected_input
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ report_unexpected_input");
                    report_unexpected_input (self->parent);
                }
                if (!self->exception)
                    self->state = expecting_token_state;
            }
            else
            if (self->event == caret_event) {
                if (!self->exception) {
                    //  report_unexpected_input
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ report_unexpected_input");
                    report_unexpected_input (self->parent);
                }
                if (!self->exception)
                    self->state = expecting_token_state;
            }
            else
            if (self->event == percent_event) {
                if (!self->exception) {
                    //  report_unexpected_input
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ report_unexpected_input");
                    report_unexpected_input (self->parent);
                }
                if (!self->exception)
                    self->state = expecting_token_state;
            }
            else
            if (self->event == close_quote_event) {
                if (!self->exception) {
                    //  report_unexpected_input
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ report_unexpected_input");
                    report_unexpected_input (self->parent);
                }
                if (!self->exception)
                    self->state = expecting_token_state;
            }
            else
            if (self->event == open_paren_event) {
                if (!self->exception) {
                    //  report_unexpected_input
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ report_unexpected_input");
                    report_unexpected_input (self->parent);
                }
                if (!self->exception)
                    self->state = expecting_token_state;
            }
            else
            if (self->event == close_paren_event) {
                if (!self->exception) {
                    //  report_unexpected_input
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ report_unexpected_input");
                    report_unexpected_input (self->parent);
                }
                if (!self->exception)
                    self->state = expecting_token_state;
            }
            else
            if (self->event == other_event) {
                if (!self->exception) {
                    //  report_unexpected_input
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ report_unexpected_input");
                    report_unexpected_input (self->parent);
                }
                if (!self->exception)
                    self->state = expecting_token_state;
            }
            else {
                //  Handle unexpected internal events
                zsys_warning ("zs_parser: unhandled event %s in %s",
                    s_event_name [self->event], s_state_name [self->state]);
                assert (false);
            }
        }
        else
        if (self->state == reading_number_state) {
            if (self->event == digit_event) {
                if (!self->exception) {
                    //  store_the_character
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ store_the_character");
                    store_the_character (self->parent);
                }
                if (!self->exception) {
                    //  read_next_character
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ read_next_character");
                    read_next_character (self->parent);
                }
            }
            else
            if (self->event == letter_event) {
                if (!self->exception) {
                    //  store_the_character
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ store_the_character");
                    store_the_character (self->parent);
                }
                if (!self->exception) {
                    //  read_next_character
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ read_next_character");
                    read_next_character (self->parent);
                }
            }
            else
            if (self->event == hyphen_event) {
                if (!self->exception) {
                    //  store_the_character
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ store_the_character");
                    store_the_character (self->parent);
                }
                if (!self->exception) {
                    //  read_next_character
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ read_next_character");
                    read_next_character (self->parent);
                }
            }
            else
            if (self->event == plus_event) {
                if (!self->exception) {
                    //  store_the_character
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ store_the_character");
                    store_the_character (self->parent);
                }
                if (!self->exception) {
                    //  read_next_character
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ read_next_character");
                    read_next_character (self->parent);
                }
            }
            else
            if (self->event == slash_event) {
                if (!self->exception) {
                    //  store_the_character
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ store_the_character");
                    store_the_character (self->parent);
                }
                if (!self->exception) {
                    //  read_next_character
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ read_next_character");
                    read_next_character (self->parent);
                }
            }
            else
            if (self->event == period_event) {
                if (!self->exception) {
                    //  store_the_character
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ store_the_character");
                    store_the_character (self->parent);
                }
                if (!self->exception) {
                    //  read_next_character
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ read_next_character");
                    read_next_character (self->parent);
                }
            }
            else
            if (self->event == comma_event) {
                if (!self->exception) {
                    //  store_the_character
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ store_the_character");
                    store_the_character (self->parent);
                }
                if (!self->exception) {
                    //  read_next_character
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ read_next_character");
                    read_next_character (self->parent);
                }
            }
            else
            if (self->event == colon_event) {
                if (!self->exception) {
                    //  store_the_character
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ store_the_character");
                    store_the_character (self->parent);
                }
                if (!self->exception) {
                    //  read_next_character
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ read_next_character");
                    read_next_character (self->parent);
                }
            }
            else
            if (self->event == asterisk_event) {
                if (!self->exception) {
                    //  store_the_character
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ store_the_character");
                    store_the_character (self->parent);
                }
                if (!self->exception) {
                    //  read_next_character
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ read_next_character");
                    read_next_character (self->parent);
                }
            }
            else
            if (self->event == caret_event) {
                if (!self->exception) {
                    //  store_the_character
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ store_the_character");
                    store_the_character (self->parent);
                }
                if (!self->exception) {
                    //  read_next_character
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ read_next_character");
                    read_next_character (self->parent);
                }
            }
            else
            if (self->event == percent_event) {
                if (!self->exception) {
                    //  store_the_character
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ store_the_character");
                    store_the_character (self->parent);
                }
                if (!self->exception) {
                    //  read_next_character
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ read_next_character");
                    read_next_character (self->parent);
                }
            }
            else
            if (self->event == open_paren_event) {
                if (!self->exception) {
                    //  store_the_character
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ store_the_character");
                    store_the_character (self->parent);
                }
                if (!self->exception) {
                    //  read_next_character
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ read_next_character");
                    read_next_character (self->parent);
                }
                if (!self->exception)
                    self->state = expecting_token_state;
            }
            else
            if (self->event == close_paren_event) {
                if (!self->exception) {
                    //  store_the_character
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ store_the_character");
                    store_the_character (self->parent);
                }
                if (!self->exception) {
                    //  read_next_character
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ read_next_character");
                    read_next_character (self->parent);
                }
                if (!self->exception)
                    self->state = expecting_token_state;
            }
            else
            if (self->event == white_space_event) {
                if (!self->exception) {
                    //  have_number_candidate
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ have_number_candidate");
                    have_number_candidate (self->parent);
                }
                if (!self->exception) {
                    //  read_next_character
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ read_next_character");
                    read_next_character (self->parent);
                }
                if (!self->exception)
                    self->state = expecting_token_state;
            }
            else
            if (self->event == newline_event) {
                if (!self->exception) {
                    //  have_number_candidate
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ have_number_candidate");
                    have_number_candidate (self->parent);
                }
                if (!self->exception) {
                    //  count_line_processed
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ count_line_processed");
                    count_line_processed (self->parent);
                }
                if (!self->exception) {
                    //  read_next_character
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ read_next_character");
                    read_next_character (self->parent);
                }
                if (!self->exception)
                    self->state = expecting_token_state;
            }
            else
            if (self->event == finished_event) {
                if (!self->exception) {
                    //  have_number_candidate
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ have_number_candidate");
                    have_number_candidate (self->parent);
                }
                if (!self->exception)
                    self->state = expecting_token_state;
            }
            else
            if (self->event == underscore_event) {
                if (!self->exception) {
                    //  report_unexpected_input
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ report_unexpected_input");
                    report_unexpected_input (self->parent);
                }
                if (!self->exception)
                    self->state = expecting_token_state;
            }
            else
            if (self->event == open_quote_event) {
                if (!self->exception) {
                    //  report_unexpected_input
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ report_unexpected_input");
                    report_unexpected_input (self->parent);
                }
                if (!self->exception)
                    self->state = expecting_token_state;
            }
            else
            if (self->event == close_quote_event) {
                if (!self->exception) {
                    //  report_unexpected_input
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ report_unexpected_input");
                    report_unexpected_input (self->parent);
                }
                if (!self->exception)
                    self->state = expecting_token_state;
            }
            else
            if (self->event == other_event) {
                if (!self->exception) {
                    //  report_unexpected_input
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ report_unexpected_input");
                    report_unexpected_input (self->parent);
                }
                if (!self->exception)
                    self->state = expecting_token_state;
            }
            else {
                //  Handle unexpected internal events
                zsys_warning ("zs_parser: unhandled event %s in %s",
                    s_event_name [self->event], s_state_name [self->state]);
                assert (false);
            }
        }
        else
        if (self->state == reading_string_state) {
            if (self->event == close_quote_event) {
                if (!self->exception) {
                    //  have_string
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ have_string");
                    have_string (self->parent);
                }
                if (!self->exception) {
                    //  read_next_character
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ read_next_character");
                    read_next_character (self->parent);
                }
                if (!self->exception)
                    self->state = expecting_token_state;
            }
            else
            if (self->event == finished_event) {
                if (!self->exception) {
                    //  store_newline_character
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ store_newline_character");
                    store_newline_character (self->parent);
                }
            }
            else
            if (self->event == newline_event) {
                if (!self->exception) {
                    //  store_the_character
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ store_the_character");
                    store_the_character (self->parent);
                }
                if (!self->exception) {
                    //  count_line_processed
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ count_line_processed");
                    count_line_processed (self->parent);
                }
                if (!self->exception) {
                    //  read_next_character
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ read_next_character");
                    read_next_character (self->parent);
                }
                if (!self->exception)
                    self->state = expecting_token_state;
            }
            else {
                //  Handle all other events
                if (!self->exception) {
                    //  store_the_character
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ store_the_character");
                    store_the_character (self->parent);
                }
                if (!self->exception) {
                    //  read_next_character
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ read_next_character");
                    read_next_character (self->parent);
                }
            }
        }
        else
        if (self->state == defaults_state) {
            if (self->event == letter_event) {
                if (!self->exception) {
                    //  report_unexpected_input
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ report_unexpected_input");
                    report_unexpected_input (self->parent);
                }
                if (!self->exception)
                    self->state = expecting_token_state;
            }
            else
            if (self->event == digit_event) {
                if (!self->exception) {
                    //  report_unexpected_input
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ report_unexpected_input");
                    report_unexpected_input (self->parent);
                }
                if (!self->exception)
                    self->state = expecting_token_state;
            }
            else
            if (self->event == hyphen_event) {
                if (!self->exception) {
                    //  report_unexpected_input
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ report_unexpected_input");
                    report_unexpected_input (self->parent);
                }
                if (!self->exception)
                    self->state = expecting_token_state;
            }
            else
            if (self->event == plus_event) {
                if (!self->exception) {
                    //  report_unexpected_input
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ report_unexpected_input");
                    report_unexpected_input (self->parent);
                }
                if (!self->exception)
                    self->state = expecting_token_state;
            }
            else
            if (self->event == slash_event) {
                if (!self->exception) {
                    //  report_unexpected_input
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ report_unexpected_input");
                    report_unexpected_input (self->parent);
                }
                if (!self->exception)
                    self->state = expecting_token_state;
            }
            else
            if (self->event == underscore_event) {
                if (!self->exception) {
                    //  report_unexpected_input
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ report_unexpected_input");
                    report_unexpected_input (self->parent);
                }
                if (!self->exception)
                    self->state = expecting_token_state;
            }
            else
            if (self->event == period_event) {
                if (!self->exception) {
                    //  report_unexpected_input
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ report_unexpected_input");
                    report_unexpected_input (self->parent);
                }
                if (!self->exception)
                    self->state = expecting_token_state;
            }
            else
            if (self->event == comma_event) {
                if (!self->exception) {
                    //  report_unexpected_input
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ report_unexpected_input");
                    report_unexpected_input (self->parent);
                }
                if (!self->exception)
                    self->state = expecting_token_state;
            }
            else
            if (self->event == colon_event) {
                if (!self->exception) {
                    //  report_unexpected_input
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ report_unexpected_input");
                    report_unexpected_input (self->parent);
                }
                if (!self->exception)
                    self->state = expecting_token_state;
            }
            else
            if (self->event == asterisk_event) {
                if (!self->exception) {
                    //  report_unexpected_input
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ report_unexpected_input");
                    report_unexpected_input (self->parent);
                }
                if (!self->exception)
                    self->state = expecting_token_state;
            }
            else
            if (self->event == caret_event) {
                if (!self->exception) {
                    //  report_unexpected_input
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ report_unexpected_input");
                    report_unexpected_input (self->parent);
                }
                if (!self->exception)
                    self->state = expecting_token_state;
            }
            else
            if (self->event == percent_event) {
                if (!self->exception) {
                    //  report_unexpected_input
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ report_unexpected_input");
                    report_unexpected_input (self->parent);
                }
                if (!self->exception)
                    self->state = expecting_token_state;
            }
            else
            if (self->event == open_quote_event) {
                if (!self->exception) {
                    //  report_unexpected_input
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ report_unexpected_input");
                    report_unexpected_input (self->parent);
                }
                if (!self->exception)
                    self->state = expecting_token_state;
            }
            else
            if (self->event == close_quote_event) {
                if (!self->exception) {
                    //  report_unexpected_input
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ report_unexpected_input");
                    report_unexpected_input (self->parent);
                }
                if (!self->exception)
                    self->state = expecting_token_state;
            }
            else
            if (self->event == open_paren_event) {
                if (!self->exception) {
                    //  report_unexpected_input
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ report_unexpected_input");
                    report_unexpected_input (self->parent);
                }
                if (!self->exception)
                    self->state = expecting_token_state;
            }
            else
            if (self->event == close_paren_event) {
                if (!self->exception) {
                    //  report_unexpected_input
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ report_unexpected_input");
                    report_unexpected_input (self->parent);
                }
                if (!self->exception)
                    self->state = expecting_token_state;
            }
            else
            if (self->event == other_event) {
                if (!self->exception) {
                    //  report_unexpected_input
                    if (self->animate)
                        zsys_debug ("zs_parser:             $ report_unexpected_input");
                    report_unexpected_input (self->parent);
                }
                if (!self->exception)
                    self->state = expecting_token_state;
            }
            else {
                //  Handle unexpected internal events
                zsys_warning ("zs_parser: unhandled event %s in %s",
                    s_event_name [self->event], s_state_name [self->state]);
                assert (false);
            }
        }
        //  If we had an exception event, interrupt normal programming
        if (self->exception) {
            if (self->animate)
                zsys_debug ("zs_parser:             ! %s", s_event_name [self->exception]);
            self->next_event = self->exception;
        }
        else
        if (self->animate)
            zsys_debug ("zs_parser:             > %s", s_state_name [self->state]);
    }
}
