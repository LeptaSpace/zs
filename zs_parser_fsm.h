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
    start_state = 1,
    expecting_token_state = 2,
    reading_function_state = 3,
    reading_number_state = 4,
    reading_string_state = 5,
    defaults_state = 6
} state_t;

typedef enum {
    NULL_event = 0,
    ok_event = 1,
    letter_event = 2,
    sign_event = 3,
    digit_event = 4,
    open_quote_event = 5,
    white_space_event = 6,
    punctuation_event = 7,
    eol_event = 8,
    separator_event = 9,
    close_quote_event = 10,
    other_event = 11
} event_t;

//  Names for state machine logging and error reporting
static char *
s_state_name [] = {
    "(NONE)",
    "start",
    "expecting token",
    "reading function",
    "reading number",
    "reading string",
    "defaults"
};

static char *
s_event_name [] = {
    "(NONE)",
    "ok",
    "letter",
    "sign",
    "digit",
    "open_quote",
    "white_space",
    "punctuation",
    "eol",
    "separator",
    "close_quote",
    "other"
};

//  Action prototypes
static void read_first_character (zs_parser_t *self);
static void start_new_token (zs_parser_t *self);
static void store_the_character (zs_parser_t *self);
static void read_next_character (zs_parser_t *self);
static void have_punctuation (zs_parser_t *self);
static void have_function (zs_parser_t *self);
static void have_number (zs_parser_t *self);
static void have_string (zs_parser_t *self);
static void report_unfinished_string (zs_parser_t *self);
static void report_unexpected_input (zs_parser_t *self);

//  This is the context block for a FSM thread; use the setter
//  methods to set the FSM properties.

typedef struct {
    zs_parser_t *parent;        //  Parent class
    bool animate;               //  Animate state machine
    bool terminated;            //  "terminate" action called
    state_t state;              //  Current state
    event_t event;              //  Current event
    event_t next_event;         //  The next event
    event_t exception;          //  Exception event, if any
} fsm_t;

static fsm_t *
fsm_new (zs_parser_t *parent)
{
    fsm_t *self = (fsm_t *) zmalloc (sizeof (fsm_t));
    if (self) {
        self->state = start_state;
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


//  Execute state machine until it is terminated.

static void
fsm_execute (fsm_t *self, event_t event)
{
    self->next_event = event;
    while (!self->terminated) {
        if (self->next_event == NULL_event) {
            zsys_warning ("zs_parser: (%s): no event set",
                s_state_name [self->state]);
            return;
        }
        self->event = self->next_event;
        self->next_event = NULL_event;
        self->exception = NULL_event;
        if (self->animate) {
            zsys_debug ("zs_parser: %s:", s_state_name [self->state]);
            zsys_debug ("zs_parser:         %s", s_event_name [self->event]);
        }
        switch (self->state) {
            case start_state:
                if (self->event == ok_event) {
                    if (!self->exception) {
                        //  read first character
                        if (self->animate)
                            zsys_debug ("zs_parser:             $ read first character");
                        read_first_character (self->parent);
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
                break;

            case expecting_token_state:
                if (self->event == letter_event) {
                    if (!self->exception) {
                        //  start new token
                        if (self->animate)
                            zsys_debug ("zs_parser:             $ start new token");
                        start_new_token (self->parent);
                    }
                    if (!self->exception) {
                        //  store the character
                        if (self->animate)
                            zsys_debug ("zs_parser:             $ store the character");
                        store_the_character (self->parent);
                    }
                    if (!self->exception) {
                        //  read next character
                        if (self->animate)
                            zsys_debug ("zs_parser:             $ read next character");
                        read_next_character (self->parent);
                    }
                    if (!self->exception)
                        self->state = reading_function_state;
                }
                else
                if (self->event == sign_event) {
                    if (!self->exception) {
                        //  start new token
                        if (self->animate)
                            zsys_debug ("zs_parser:             $ start new token");
                        start_new_token (self->parent);
                    }
                    if (!self->exception) {
                        //  store the character
                        if (self->animate)
                            zsys_debug ("zs_parser:             $ store the character");
                        store_the_character (self->parent);
                    }
                    if (!self->exception) {
                        //  read next character
                        if (self->animate)
                            zsys_debug ("zs_parser:             $ read next character");
                        read_next_character (self->parent);
                    }
                    if (!self->exception)
                        self->state = reading_number_state;
                }
                else
                if (self->event == digit_event) {
                    if (!self->exception) {
                        //  start new token
                        if (self->animate)
                            zsys_debug ("zs_parser:             $ start new token");
                        start_new_token (self->parent);
                    }
                    if (!self->exception) {
                        //  store the character
                        if (self->animate)
                            zsys_debug ("zs_parser:             $ store the character");
                        store_the_character (self->parent);
                    }
                    if (!self->exception) {
                        //  read next character
                        if (self->animate)
                            zsys_debug ("zs_parser:             $ read next character");
                        read_next_character (self->parent);
                    }
                    if (!self->exception)
                        self->state = reading_number_state;
                }
                else
                if (self->event == open_quote_event) {
                    if (!self->exception) {
                        //  start new token
                        if (self->animate)
                            zsys_debug ("zs_parser:             $ start new token");
                        start_new_token (self->parent);
                    }
                    if (!self->exception) {
                        //  read next character
                        if (self->animate)
                            zsys_debug ("zs_parser:             $ read next character");
                        read_next_character (self->parent);
                    }
                    if (!self->exception)
                        self->state = reading_string_state;
                }
                else
                if (self->event == white_space_event) {
                    if (!self->exception) {
                        //  read next character
                        if (self->animate)
                            zsys_debug ("zs_parser:             $ read next character");
                        read_next_character (self->parent);
                    }
                }
                else
                if (self->event == punctuation_event) {
                    if (!self->exception) {
                        //  have punctuation
                        if (self->animate)
                            zsys_debug ("zs_parser:             $ have punctuation");
                        have_punctuation (self->parent);
                    }
                    if (!self->exception) {
                        //  read next character
                        if (self->animate)
                            zsys_debug ("zs_parser:             $ read next character");
                        read_next_character (self->parent);
                    }
                }
                else
                if (self->event == eol_event) {
                    if (!self->exception) {
                        //  terminate
                        if (self->animate)
                            zsys_debug ("zs_parser:             $ terminate");
                        self->terminated = true;
                    }
                }
                else
                if (self->event == separator_event) {
                    if (!self->exception) {
                        //  report unexpected input
                        if (self->animate)
                            zsys_debug ("zs_parser:             $ report unexpected input");
                        report_unexpected_input (self->parent);
                    }
                    if (!self->exception) {
                        //  terminate
                        if (self->animate)
                            zsys_debug ("zs_parser:             $ terminate");
                        self->terminated = true;
                    }
                }
                else
                if (self->event == close_quote_event) {
                    if (!self->exception) {
                        //  report unexpected input
                        if (self->animate)
                            zsys_debug ("zs_parser:             $ report unexpected input");
                        report_unexpected_input (self->parent);
                    }
                    if (!self->exception) {
                        //  terminate
                        if (self->animate)
                            zsys_debug ("zs_parser:             $ terminate");
                        self->terminated = true;
                    }
                }
                else
                if (self->event == other_event) {
                    if (!self->exception) {
                        //  report unexpected input
                        if (self->animate)
                            zsys_debug ("zs_parser:             $ report unexpected input");
                        report_unexpected_input (self->parent);
                    }
                    if (!self->exception) {
                        //  terminate
                        if (self->animate)
                            zsys_debug ("zs_parser:             $ terminate");
                        self->terminated = true;
                    }
                }
                else {
                    //  Handle unexpected internal events
                    zsys_warning ("zs_parser: unhandled event %s in %s",
                        s_event_name [self->event], s_state_name [self->state]);
                    assert (false);
                }
                break;

            case reading_function_state:
                if (self->event == letter_event) {
                    if (!self->exception) {
                        //  store the character
                        if (self->animate)
                            zsys_debug ("zs_parser:             $ store the character");
                        store_the_character (self->parent);
                    }
                    if (!self->exception) {
                        //  read next character
                        if (self->animate)
                            zsys_debug ("zs_parser:             $ read next character");
                        read_next_character (self->parent);
                    }
                }
                else
                if (self->event == sign_event) {
                    if (!self->exception) {
                        //  store the character
                        if (self->animate)
                            zsys_debug ("zs_parser:             $ store the character");
                        store_the_character (self->parent);
                    }
                    if (!self->exception) {
                        //  read next character
                        if (self->animate)
                            zsys_debug ("zs_parser:             $ read next character");
                        read_next_character (self->parent);
                    }
                }
                else
                if (self->event == separator_event) {
                    if (!self->exception) {
                        //  store the character
                        if (self->animate)
                            zsys_debug ("zs_parser:             $ store the character");
                        store_the_character (self->parent);
                    }
                    if (!self->exception) {
                        //  read next character
                        if (self->animate)
                            zsys_debug ("zs_parser:             $ read next character");
                        read_next_character (self->parent);
                    }
                }
                else
                if (self->event == white_space_event) {
                    if (!self->exception) {
                        //  have function
                        if (self->animate)
                            zsys_debug ("zs_parser:             $ have function");
                        have_function (self->parent);
                    }
                    if (!self->exception) {
                        //  read next character
                        if (self->animate)
                            zsys_debug ("zs_parser:             $ read next character");
                        read_next_character (self->parent);
                    }
                    if (!self->exception)
                        self->state = expecting_token_state;
                }
                else
                if (self->event == eol_event) {
                    if (!self->exception) {
                        //  have function
                        if (self->animate)
                            zsys_debug ("zs_parser:             $ have function");
                        have_function (self->parent);
                    }
                    if (!self->exception) {
                        //  terminate
                        if (self->animate)
                            zsys_debug ("zs_parser:             $ terminate");
                        self->terminated = true;
                    }
                }
                else
                if (self->event == digit_event) {
                    if (!self->exception) {
                        //  report unexpected input
                        if (self->animate)
                            zsys_debug ("zs_parser:             $ report unexpected input");
                        report_unexpected_input (self->parent);
                    }
                    if (!self->exception) {
                        //  terminate
                        if (self->animate)
                            zsys_debug ("zs_parser:             $ terminate");
                        self->terminated = true;
                    }
                }
                else
                if (self->event == punctuation_event) {
                    if (!self->exception) {
                        //  report unexpected input
                        if (self->animate)
                            zsys_debug ("zs_parser:             $ report unexpected input");
                        report_unexpected_input (self->parent);
                    }
                    if (!self->exception) {
                        //  terminate
                        if (self->animate)
                            zsys_debug ("zs_parser:             $ terminate");
                        self->terminated = true;
                    }
                }
                else
                if (self->event == open_quote_event) {
                    if (!self->exception) {
                        //  report unexpected input
                        if (self->animate)
                            zsys_debug ("zs_parser:             $ report unexpected input");
                        report_unexpected_input (self->parent);
                    }
                    if (!self->exception) {
                        //  terminate
                        if (self->animate)
                            zsys_debug ("zs_parser:             $ terminate");
                        self->terminated = true;
                    }
                }
                else
                if (self->event == close_quote_event) {
                    if (!self->exception) {
                        //  report unexpected input
                        if (self->animate)
                            zsys_debug ("zs_parser:             $ report unexpected input");
                        report_unexpected_input (self->parent);
                    }
                    if (!self->exception) {
                        //  terminate
                        if (self->animate)
                            zsys_debug ("zs_parser:             $ terminate");
                        self->terminated = true;
                    }
                }
                else
                if (self->event == other_event) {
                    if (!self->exception) {
                        //  report unexpected input
                        if (self->animate)
                            zsys_debug ("zs_parser:             $ report unexpected input");
                        report_unexpected_input (self->parent);
                    }
                    if (!self->exception) {
                        //  terminate
                        if (self->animate)
                            zsys_debug ("zs_parser:             $ terminate");
                        self->terminated = true;
                    }
                }
                else {
                    //  Handle unexpected internal events
                    zsys_warning ("zs_parser: unhandled event %s in %s",
                        s_event_name [self->event], s_state_name [self->state]);
                    assert (false);
                }
                break;

            case reading_number_state:
                if (self->event == digit_event) {
                    if (!self->exception) {
                        //  store the character
                        if (self->animate)
                            zsys_debug ("zs_parser:             $ store the character");
                        store_the_character (self->parent);
                    }
                    if (!self->exception) {
                        //  read next character
                        if (self->animate)
                            zsys_debug ("zs_parser:             $ read next character");
                        read_next_character (self->parent);
                    }
                }
                else
                if (self->event == white_space_event) {
                    if (!self->exception) {
                        //  have number
                        if (self->animate)
                            zsys_debug ("zs_parser:             $ have number");
                        have_number (self->parent);
                    }
                    if (!self->exception) {
                        //  read next character
                        if (self->animate)
                            zsys_debug ("zs_parser:             $ read next character");
                        read_next_character (self->parent);
                    }
                    if (!self->exception)
                        self->state = expecting_token_state;
                }
                else
                if (self->event == eol_event) {
                    if (!self->exception) {
                        //  have number
                        if (self->animate)
                            zsys_debug ("zs_parser:             $ have number");
                        have_number (self->parent);
                    }
                    if (!self->exception) {
                        //  terminate
                        if (self->animate)
                            zsys_debug ("zs_parser:             $ terminate");
                        self->terminated = true;
                    }
                }
                else
                if (self->event == letter_event) {
                    if (!self->exception) {
                        //  report unexpected input
                        if (self->animate)
                            zsys_debug ("zs_parser:             $ report unexpected input");
                        report_unexpected_input (self->parent);
                    }
                    if (!self->exception) {
                        //  terminate
                        if (self->animate)
                            zsys_debug ("zs_parser:             $ terminate");
                        self->terminated = true;
                    }
                }
                else
                if (self->event == sign_event) {
                    if (!self->exception) {
                        //  report unexpected input
                        if (self->animate)
                            zsys_debug ("zs_parser:             $ report unexpected input");
                        report_unexpected_input (self->parent);
                    }
                    if (!self->exception) {
                        //  terminate
                        if (self->animate)
                            zsys_debug ("zs_parser:             $ terminate");
                        self->terminated = true;
                    }
                }
                else
                if (self->event == separator_event) {
                    if (!self->exception) {
                        //  report unexpected input
                        if (self->animate)
                            zsys_debug ("zs_parser:             $ report unexpected input");
                        report_unexpected_input (self->parent);
                    }
                    if (!self->exception) {
                        //  terminate
                        if (self->animate)
                            zsys_debug ("zs_parser:             $ terminate");
                        self->terminated = true;
                    }
                }
                else
                if (self->event == punctuation_event) {
                    if (!self->exception) {
                        //  report unexpected input
                        if (self->animate)
                            zsys_debug ("zs_parser:             $ report unexpected input");
                        report_unexpected_input (self->parent);
                    }
                    if (!self->exception) {
                        //  terminate
                        if (self->animate)
                            zsys_debug ("zs_parser:             $ terminate");
                        self->terminated = true;
                    }
                }
                else
                if (self->event == open_quote_event) {
                    if (!self->exception) {
                        //  report unexpected input
                        if (self->animate)
                            zsys_debug ("zs_parser:             $ report unexpected input");
                        report_unexpected_input (self->parent);
                    }
                    if (!self->exception) {
                        //  terminate
                        if (self->animate)
                            zsys_debug ("zs_parser:             $ terminate");
                        self->terminated = true;
                    }
                }
                else
                if (self->event == close_quote_event) {
                    if (!self->exception) {
                        //  report unexpected input
                        if (self->animate)
                            zsys_debug ("zs_parser:             $ report unexpected input");
                        report_unexpected_input (self->parent);
                    }
                    if (!self->exception) {
                        //  terminate
                        if (self->animate)
                            zsys_debug ("zs_parser:             $ terminate");
                        self->terminated = true;
                    }
                }
                else
                if (self->event == other_event) {
                    if (!self->exception) {
                        //  report unexpected input
                        if (self->animate)
                            zsys_debug ("zs_parser:             $ report unexpected input");
                        report_unexpected_input (self->parent);
                    }
                    if (!self->exception) {
                        //  terminate
                        if (self->animate)
                            zsys_debug ("zs_parser:             $ terminate");
                        self->terminated = true;
                    }
                }
                else {
                    //  Handle unexpected internal events
                    zsys_warning ("zs_parser: unhandled event %s in %s",
                        s_event_name [self->event], s_state_name [self->state]);
                    assert (false);
                }
                break;

            case reading_string_state:
                if (self->event == close_quote_event) {
                    if (!self->exception) {
                        //  have string
                        if (self->animate)
                            zsys_debug ("zs_parser:             $ have string");
                        have_string (self->parent);
                    }
                    if (!self->exception) {
                        //  read next character
                        if (self->animate)
                            zsys_debug ("zs_parser:             $ read next character");
                        read_next_character (self->parent);
                    }
                    if (!self->exception)
                        self->state = expecting_token_state;
                }
                else
                if (self->event == eol_event) {
                    if (!self->exception) {
                        //  report unfinished string
                        if (self->animate)
                            zsys_debug ("zs_parser:             $ report unfinished string");
                        report_unfinished_string (self->parent);
                    }
                    if (!self->exception) {
                        //  terminate
                        if (self->animate)
                            zsys_debug ("zs_parser:             $ terminate");
                        self->terminated = true;
                    }
                }
                else {
                    //  Handle all other events
                    if (!self->exception) {
                        //  store the character
                        if (self->animate)
                            zsys_debug ("zs_parser:             $ store the character");
                        store_the_character (self->parent);
                    }
                    if (!self->exception) {
                        //  read next character
                        if (self->animate)
                            zsys_debug ("zs_parser:             $ read next character");
                        read_next_character (self->parent);
                    }
                }
                break;

            case defaults_state:
                if (self->event == letter_event) {
                    if (!self->exception) {
                        //  report unexpected input
                        if (self->animate)
                            zsys_debug ("zs_parser:             $ report unexpected input");
                        report_unexpected_input (self->parent);
                    }
                    if (!self->exception) {
                        //  terminate
                        if (self->animate)
                            zsys_debug ("zs_parser:             $ terminate");
                        self->terminated = true;
                    }
                }
                else
                if (self->event == digit_event) {
                    if (!self->exception) {
                        //  report unexpected input
                        if (self->animate)
                            zsys_debug ("zs_parser:             $ report unexpected input");
                        report_unexpected_input (self->parent);
                    }
                    if (!self->exception) {
                        //  terminate
                        if (self->animate)
                            zsys_debug ("zs_parser:             $ terminate");
                        self->terminated = true;
                    }
                }
                else
                if (self->event == sign_event) {
                    if (!self->exception) {
                        //  report unexpected input
                        if (self->animate)
                            zsys_debug ("zs_parser:             $ report unexpected input");
                        report_unexpected_input (self->parent);
                    }
                    if (!self->exception) {
                        //  terminate
                        if (self->animate)
                            zsys_debug ("zs_parser:             $ terminate");
                        self->terminated = true;
                    }
                }
                else
                if (self->event == separator_event) {
                    if (!self->exception) {
                        //  report unexpected input
                        if (self->animate)
                            zsys_debug ("zs_parser:             $ report unexpected input");
                        report_unexpected_input (self->parent);
                    }
                    if (!self->exception) {
                        //  terminate
                        if (self->animate)
                            zsys_debug ("zs_parser:             $ terminate");
                        self->terminated = true;
                    }
                }
                else
                if (self->event == punctuation_event) {
                    if (!self->exception) {
                        //  report unexpected input
                        if (self->animate)
                            zsys_debug ("zs_parser:             $ report unexpected input");
                        report_unexpected_input (self->parent);
                    }
                    if (!self->exception) {
                        //  terminate
                        if (self->animate)
                            zsys_debug ("zs_parser:             $ terminate");
                        self->terminated = true;
                    }
                }
                else
                if (self->event == open_quote_event) {
                    if (!self->exception) {
                        //  report unexpected input
                        if (self->animate)
                            zsys_debug ("zs_parser:             $ report unexpected input");
                        report_unexpected_input (self->parent);
                    }
                    if (!self->exception) {
                        //  terminate
                        if (self->animate)
                            zsys_debug ("zs_parser:             $ terminate");
                        self->terminated = true;
                    }
                }
                else
                if (self->event == close_quote_event) {
                    if (!self->exception) {
                        //  report unexpected input
                        if (self->animate)
                            zsys_debug ("zs_parser:             $ report unexpected input");
                        report_unexpected_input (self->parent);
                    }
                    if (!self->exception) {
                        //  terminate
                        if (self->animate)
                            zsys_debug ("zs_parser:             $ terminate");
                        self->terminated = true;
                    }
                }
                else
                if (self->event == eol_event) {
                    if (!self->exception) {
                        //  report unexpected input
                        if (self->animate)
                            zsys_debug ("zs_parser:             $ report unexpected input");
                        report_unexpected_input (self->parent);
                    }
                    if (!self->exception) {
                        //  terminate
                        if (self->animate)
                            zsys_debug ("zs_parser:             $ terminate");
                        self->terminated = true;
                    }
                }
                else
                if (self->event == white_space_event) {
                    if (!self->exception) {
                        //  report unexpected input
                        if (self->animate)
                            zsys_debug ("zs_parser:             $ report unexpected input");
                        report_unexpected_input (self->parent);
                    }
                    if (!self->exception) {
                        //  terminate
                        if (self->animate)
                            zsys_debug ("zs_parser:             $ terminate");
                        self->terminated = true;
                    }
                }
                else
                if (self->event == other_event) {
                    if (!self->exception) {
                        //  report unexpected input
                        if (self->animate)
                            zsys_debug ("zs_parser:             $ report unexpected input");
                        report_unexpected_input (self->parent);
                    }
                    if (!self->exception) {
                        //  terminate
                        if (self->animate)
                            zsys_debug ("zs_parser:             $ terminate");
                        self->terminated = true;
                    }
                }
                else {
                    //  Handle unexpected internal events
                    zsys_warning ("zs_parser: unhandled event %s in %s",
                        s_event_name [self->event], s_state_name [self->state]);
                    assert (false);
                }
                break;
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
