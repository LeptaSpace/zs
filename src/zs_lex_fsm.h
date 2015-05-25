/*  =========================================================================
    zs_lex_fsm - No title state machine engine

    ** WARNING *************************************************************
    THIS SOURCE FILE IS 100% GENERATED. If you edit this file, you will lose
    your changes at the next build cycle. This is great for temporary printf
    statements. DO NOT MAKE ANY CHANGES YOU WISH TO KEEP. The correct places
    for commits are:

     * The XML model used for this code generation: zs_lex.xml, or
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
    after_function_colon_state = 4,
    after_unary_sign_state = 5,
    after_period_state = 6,
    reading_number_state = 7,
    after_number_comma_state = 8,
    after_number_period_state = 9,
    reading_string_state = 10,
    reading_comment_state = 11,
    defaults_state = 12
} state_t;

typedef enum {
    NULL_event = 0,
    letter_event = 1,
    hyphen_event = 2,
    plus_event = 3,
    digit_event = 4,
    usable_event = 5,
    open_quote_event = 6,
    close_list_event = 7,
    comma_event = 8,
    period_event = 9,
    finished_event = 10,
    whitespace_event = 11,
    newline_event = 12,
    colon_event = 13,
    open_list_event = 14,
    percent_event = 15,
    close_quote_event = 16,
    other_event = 17,
    comment_event = 18
} event_t;

//  Names for state machine logging and error reporting
static char *
s_state_name [] = {
    "(NONE)",
    "expecting_token",
    "reading_function",
    "after_function",
    "after_function_colon",
    "after_unary_sign",
    "after_period",
    "reading_number",
    "after_number_comma",
    "after_number_period",
    "reading_string",
    "reading_comment",
    "defaults"
};

static char *
s_event_name [] = {
    "(NONE)",
    "letter",
    "hyphen",
    "plus",
    "digit",
    "usable",
    "open_quote",
    "close_list",
    "comma",
    "period",
    "finished",
    "whitespace",
    "newline",
    "colon",
    "open_list",
    "percent",
    "close_quote",
    "other",
    "comment"
};

//  Action prototypes
static void start_new_token (zs_lex_t *self);
static void store_the_character (zs_lex_t *self);
static void parse_next_character (zs_lex_t *self);
static void have_close_list_token (zs_lex_t *self);
static void have_phrase_token (zs_lex_t *self);
static void have_null_token (zs_lex_t *self);
static void have_fn_nested_token (zs_lex_t *self);
static void have_fn_inline_token (zs_lex_t *self);
static void push_back_to_previous (zs_lex_t *self);
static void have_fn_define_token (zs_lex_t *self);
static void have_number_token (zs_lex_t *self);
static void have_sentence_token (zs_lex_t *self);
static void store_comma_character (zs_lex_t *self);
static void store_period_character (zs_lex_t *self);
static void have_string_token (zs_lex_t *self);
static void store_newline_character (zs_lex_t *self);
static void have_invalid_token (zs_lex_t *self);

//  This is the context block for a FSM thread; use the setter
//  methods to set the FSM properties.

typedef struct {
    zs_lex_t *parent;           //  Parent class
    bool animate;               //  Animate state machine
    state_t state;              //  Current state
    event_t event;              //  Current event
    event_t next_event;         //  The next event
    event_t exception;          //  Exception event, if any
    uint64_t cycles;            //  Track the work done
} fsm_t;

static fsm_t *
fsm_new (zs_lex_t *parent)
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
    if (self)
        self->next_event = next_event;
}

static void
fsm_set_exception (fsm_t *self, event_t exception)
{
    if (self)
        self->exception = exception;
}

static void
fsm_set_animate (fsm_t *self, bool animate)
{
    if (self)
        self->animate = animate;
}

static uint64_t
fsm_cycles (fsm_t *self)
{
    if (self)
        return self->cycles;
    else
        return 0;
}

//  Stops annoying compiler warnings on unused functions
void
zs_lex_not_used (fsm_t *self)
{
    fsm_set_next_event (NULL, NULL_event);
    fsm_set_exception (NULL, NULL_event);
    fsm_set_animate (NULL, 0);
    fsm_cycles (NULL);
}


//  Execute state machine until it has no next event. Before calling this
//  you must have set the next event using fsm_set_next_event(). Ends when
//  there is no next event set.

static void
fsm_execute (fsm_t *self)
{
    while (self->next_event != NULL_event) {
        self->cycles++;
        self->event = self->next_event;
        self->next_event = NULL_event;
        self->exception = NULL_event;
        if (self->animate) {
            zsys_debug ("zs_lex: %s:", s_state_name [self->state]);
            zsys_debug ("zs_lex:            %s", s_event_name [self->event]);
        }
        if (self->state == expecting_token_state) {
            if (self->event == letter_event) {
                if (!self->exception) {
                    //  start_new_token
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ start_new_token");
                    start_new_token (self->parent);
                }
                if (!self->exception) {
                    //  store_the_character
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ store_the_character");
                    store_the_character (self->parent);
                }
                if (!self->exception) {
                    //  parse_next_character
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ parse_next_character");
                    parse_next_character (self->parent);
                }
                if (!self->exception)
                    self->state = reading_function_state;
            }
            else
            if (self->event == hyphen_event) {
                if (!self->exception) {
                    //  start_new_token
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ start_new_token");
                    start_new_token (self->parent);
                }
                if (!self->exception) {
                    //  store_the_character
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ store_the_character");
                    store_the_character (self->parent);
                }
                if (!self->exception) {
                    //  parse_next_character
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ parse_next_character");
                    parse_next_character (self->parent);
                }
                if (!self->exception)
                    self->state = after_unary_sign_state;
            }
            else
            if (self->event == plus_event) {
                if (!self->exception) {
                    //  start_new_token
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ start_new_token");
                    start_new_token (self->parent);
                }
                if (!self->exception) {
                    //  store_the_character
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ store_the_character");
                    store_the_character (self->parent);
                }
                if (!self->exception) {
                    //  parse_next_character
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ parse_next_character");
                    parse_next_character (self->parent);
                }
                if (!self->exception)
                    self->state = after_unary_sign_state;
            }
            else
            if (self->event == digit_event) {
                if (!self->exception) {
                    //  start_new_token
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ start_new_token");
                    start_new_token (self->parent);
                }
                if (!self->exception) {
                    //  store_the_character
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ store_the_character");
                    store_the_character (self->parent);
                }
                if (!self->exception) {
                    //  parse_next_character
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ parse_next_character");
                    parse_next_character (self->parent);
                }
                if (!self->exception)
                    self->state = reading_number_state;
            }
            else
            if (self->event == usable_event) {
                if (!self->exception) {
                    //  start_new_token
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ start_new_token");
                    start_new_token (self->parent);
                }
                if (!self->exception) {
                    //  store_the_character
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ store_the_character");
                    store_the_character (self->parent);
                }
                if (!self->exception) {
                    //  parse_next_character
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ parse_next_character");
                    parse_next_character (self->parent);
                }
                if (!self->exception)
                    self->state = reading_function_state;
            }
            else
            if (self->event == open_quote_event) {
                if (!self->exception) {
                    //  start_new_token
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ start_new_token");
                    start_new_token (self->parent);
                }
                if (!self->exception) {
                    //  parse_next_character
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ parse_next_character");
                    parse_next_character (self->parent);
                }
                if (!self->exception)
                    self->state = reading_string_state;
            }
            else
            if (self->event == close_list_event) {
                if (!self->exception) {
                    //  have_close_list_token
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ have_close_list_token");
                    have_close_list_token (self->parent);
                }
            }
            else
            if (self->event == comma_event) {
                if (!self->exception) {
                    //  have_phrase_token
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ have_phrase_token");
                    have_phrase_token (self->parent);
                }
            }
            else
            if (self->event == period_event) {
                if (!self->exception) {
                    //  start_new_token
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ start_new_token");
                    start_new_token (self->parent);
                }
                if (!self->exception) {
                    //  store_the_character
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ store_the_character");
                    store_the_character (self->parent);
                }
                if (!self->exception) {
                    //  parse_next_character
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ parse_next_character");
                    parse_next_character (self->parent);
                }
                if (!self->exception)
                    self->state = after_period_state;
            }
            else
            if (self->event == finished_event) {
                if (!self->exception) {
                    //  have_null_token
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ have_null_token");
                    have_null_token (self->parent);
                }
            }
            else
            if (self->event == other_event) {
                if (!self->exception) {
                    //  have_invalid_token
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ have_invalid_token");
                    have_invalid_token (self->parent);
                }
                if (!self->exception)
                    self->state = expecting_token_state;
            }
            else
            if (self->event == whitespace_event) {
                if (!self->exception) {
                    //  parse_next_character
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ parse_next_character");
                    parse_next_character (self->parent);
                }
            }
            else
            if (self->event == comment_event) {
                if (!self->exception) {
                    //  parse_next_character
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ parse_next_character");
                    parse_next_character (self->parent);
                }
                if (!self->exception)
                    self->state = reading_comment_state;
            }
            else
            if (self->event == newline_event) {
                if (!self->exception) {
                    //  parse_next_character
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ parse_next_character");
                    parse_next_character (self->parent);
                }
            }
            else {
                //  Handle all other events
                if (!self->exception) {
                    //  have_invalid_token
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ have_invalid_token");
                    have_invalid_token (self->parent);
                }
                if (!self->exception)
                    self->state = expecting_token_state;
            }
        }
        else
        if (self->state == reading_function_state) {
            if (self->event == letter_event) {
                if (!self->exception) {
                    //  store_the_character
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ store_the_character");
                    store_the_character (self->parent);
                }
                if (!self->exception) {
                    //  parse_next_character
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ parse_next_character");
                    parse_next_character (self->parent);
                }
            }
            else
            if (self->event == digit_event) {
                if (!self->exception) {
                    //  store_the_character
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ store_the_character");
                    store_the_character (self->parent);
                }
                if (!self->exception) {
                    //  parse_next_character
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ parse_next_character");
                    parse_next_character (self->parent);
                }
            }
            else
            if (self->event == hyphen_event) {
                if (!self->exception) {
                    //  store_the_character
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ store_the_character");
                    store_the_character (self->parent);
                }
                if (!self->exception) {
                    //  parse_next_character
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ parse_next_character");
                    parse_next_character (self->parent);
                }
            }
            else
            if (self->event == usable_event) {
                if (!self->exception) {
                    //  store_the_character
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ store_the_character");
                    store_the_character (self->parent);
                }
                if (!self->exception) {
                    //  parse_next_character
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ parse_next_character");
                    parse_next_character (self->parent);
                }
            }
            else
            if (self->event == whitespace_event) {
                if (!self->exception) {
                    //  parse_next_character
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ parse_next_character");
                    parse_next_character (self->parent);
                }
                if (!self->exception)
                    self->state = after_function_state;
            }
            else
            if (self->event == newline_event) {
                if (!self->exception) {
                    //  parse_next_character
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ parse_next_character");
                    parse_next_character (self->parent);
                }
                if (!self->exception)
                    self->state = after_function_state;
            }
            else
            if (self->event == colon_event) {
                if (!self->exception) {
                    //  parse_next_character
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ parse_next_character");
                    parse_next_character (self->parent);
                }
                if (!self->exception)
                    self->state = after_function_colon_state;
            }
            else
            if (self->event == open_list_event) {
                if (!self->exception) {
                    //  have_fn_nested_token
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ have_fn_nested_token");
                    have_fn_nested_token (self->parent);
                }
                if (!self->exception)
                    self->state = expecting_token_state;
            }
            else
            if (self->event == close_list_event) {
                if (!self->exception) {
                    //  have_fn_inline_token
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ have_fn_inline_token");
                    have_fn_inline_token (self->parent);
                }
                if (!self->exception) {
                    //  push_back_to_previous
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ push_back_to_previous");
                    push_back_to_previous (self->parent);
                }
                if (!self->exception)
                    self->state = expecting_token_state;
            }
            else
            if (self->event == comma_event) {
                if (!self->exception) {
                    //  have_fn_inline_token
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ have_fn_inline_token");
                    have_fn_inline_token (self->parent);
                }
                if (!self->exception) {
                    //  push_back_to_previous
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ push_back_to_previous");
                    push_back_to_previous (self->parent);
                }
                if (!self->exception)
                    self->state = expecting_token_state;
            }
            else
            if (self->event == period_event) {
                if (!self->exception) {
                    //  have_fn_inline_token
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ have_fn_inline_token");
                    have_fn_inline_token (self->parent);
                }
                if (!self->exception) {
                    //  push_back_to_previous
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ push_back_to_previous");
                    push_back_to_previous (self->parent);
                }
                if (!self->exception)
                    self->state = expecting_token_state;
            }
            else
            if (self->event == finished_event) {
                if (!self->exception) {
                    //  have_fn_inline_token
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ have_fn_inline_token");
                    have_fn_inline_token (self->parent);
                }
                if (!self->exception)
                    self->state = expecting_token_state;
            }
            else
            if (self->event == other_event) {
                if (!self->exception) {
                    //  have_invalid_token
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ have_invalid_token");
                    have_invalid_token (self->parent);
                }
                if (!self->exception)
                    self->state = expecting_token_state;
            }
            else
            if (self->event == comment_event) {
                if (!self->exception) {
                    //  parse_next_character
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ parse_next_character");
                    parse_next_character (self->parent);
                }
                if (!self->exception)
                    self->state = reading_comment_state;
            }
            else {
                //  Handle all other events
                if (!self->exception) {
                    //  have_invalid_token
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ have_invalid_token");
                    have_invalid_token (self->parent);
                }
                if (!self->exception)
                    self->state = expecting_token_state;
            }
        }
        else
        if (self->state == after_function_state) {
            if (self->event == finished_event) {
                if (!self->exception) {
                    //  have_fn_inline_token
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ have_fn_inline_token");
                    have_fn_inline_token (self->parent);
                }
                if (!self->exception)
                    self->state = expecting_token_state;
            }
            else
            if (self->event == open_list_event) {
                if (!self->exception) {
                    //  have_fn_nested_token
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ have_fn_nested_token");
                    have_fn_nested_token (self->parent);
                }
                if (!self->exception)
                    self->state = expecting_token_state;
            }
            else
            if (self->event == colon_event) {
                if (!self->exception) {
                    //  parse_next_character
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ parse_next_character");
                    parse_next_character (self->parent);
                }
                if (!self->exception)
                    self->state = after_function_colon_state;
            }
            else {
                //  Handle all other events
                if (!self->exception) {
                    //  have_fn_inline_token
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ have_fn_inline_token");
                    have_fn_inline_token (self->parent);
                }
                if (!self->exception) {
                    //  push_back_to_previous
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ push_back_to_previous");
                    push_back_to_previous (self->parent);
                }
                if (!self->exception)
                    self->state = expecting_token_state;
            }
        }
        else
        if (self->state == after_function_colon_state) {
            if (self->event == open_list_event) {
                if (!self->exception) {
                    //  have_fn_define_token
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ have_fn_define_token");
                    have_fn_define_token (self->parent);
                }
                if (!self->exception)
                    self->state = expecting_token_state;
            }
            else
            if (self->event == other_event) {
                if (!self->exception) {
                    //  have_invalid_token
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ have_invalid_token");
                    have_invalid_token (self->parent);
                }
                if (!self->exception)
                    self->state = expecting_token_state;
            }
            else
            if (self->event == whitespace_event) {
                if (!self->exception) {
                    //  parse_next_character
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ parse_next_character");
                    parse_next_character (self->parent);
                }
            }
            else
            if (self->event == comment_event) {
                if (!self->exception) {
                    //  parse_next_character
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ parse_next_character");
                    parse_next_character (self->parent);
                }
                if (!self->exception)
                    self->state = reading_comment_state;
            }
            else
            if (self->event == newline_event) {
                if (!self->exception) {
                    //  parse_next_character
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ parse_next_character");
                    parse_next_character (self->parent);
                }
            }
            else {
                //  Handle all other events
                if (!self->exception) {
                    //  have_invalid_token
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ have_invalid_token");
                    have_invalid_token (self->parent);
                }
                if (!self->exception)
                    self->state = expecting_token_state;
            }
        }
        else
        if (self->state == after_unary_sign_state) {
            if (self->event == digit_event) {
                if (!self->exception) {
                    //  store_the_character
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ store_the_character");
                    store_the_character (self->parent);
                }
                if (!self->exception) {
                    //  parse_next_character
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ parse_next_character");
                    parse_next_character (self->parent);
                }
                if (!self->exception)
                    self->state = reading_number_state;
            }
            else
            if (self->event == letter_event) {
                if (!self->exception) {
                    //  store_the_character
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ store_the_character");
                    store_the_character (self->parent);
                }
                if (!self->exception) {
                    //  parse_next_character
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ parse_next_character");
                    parse_next_character (self->parent);
                }
                if (!self->exception)
                    self->state = reading_function_state;
            }
            else
            if (self->event == hyphen_event) {
                if (!self->exception) {
                    //  store_the_character
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ store_the_character");
                    store_the_character (self->parent);
                }
                if (!self->exception) {
                    //  parse_next_character
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ parse_next_character");
                    parse_next_character (self->parent);
                }
                if (!self->exception)
                    self->state = reading_function_state;
            }
            else
            if (self->event == plus_event) {
                if (!self->exception) {
                    //  store_the_character
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ store_the_character");
                    store_the_character (self->parent);
                }
                if (!self->exception) {
                    //  parse_next_character
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ parse_next_character");
                    parse_next_character (self->parent);
                }
                if (!self->exception)
                    self->state = reading_function_state;
            }
            else
            if (self->event == percent_event) {
                if (!self->exception) {
                    //  store_the_character
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ store_the_character");
                    store_the_character (self->parent);
                }
                if (!self->exception) {
                    //  have_number_token
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ have_number_token");
                    have_number_token (self->parent);
                }
                if (!self->exception)
                    self->state = expecting_token_state;
            }
            else
            if (self->event == usable_event) {
                if (!self->exception) {
                    //  store_the_character
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ store_the_character");
                    store_the_character (self->parent);
                }
                if (!self->exception) {
                    //  parse_next_character
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ parse_next_character");
                    parse_next_character (self->parent);
                }
            }
            else
            if (self->event == whitespace_event) {
                if (!self->exception) {
                    //  parse_next_character
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ parse_next_character");
                    parse_next_character (self->parent);
                }
                if (!self->exception)
                    self->state = after_function_state;
            }
            else
            if (self->event == newline_event) {
                if (!self->exception) {
                    //  parse_next_character
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ parse_next_character");
                    parse_next_character (self->parent);
                }
                if (!self->exception)
                    self->state = after_function_state;
            }
            else
            if (self->event == colon_event) {
                if (!self->exception) {
                    //  parse_next_character
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ parse_next_character");
                    parse_next_character (self->parent);
                }
                if (!self->exception)
                    self->state = after_function_colon_state;
            }
            else
            if (self->event == open_list_event) {
                if (!self->exception) {
                    //  have_fn_nested_token
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ have_fn_nested_token");
                    have_fn_nested_token (self->parent);
                }
                if (!self->exception)
                    self->state = expecting_token_state;
            }
            else
            if (self->event == close_list_event) {
                if (!self->exception) {
                    //  have_fn_inline_token
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ have_fn_inline_token");
                    have_fn_inline_token (self->parent);
                }
                if (!self->exception) {
                    //  push_back_to_previous
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ push_back_to_previous");
                    push_back_to_previous (self->parent);
                }
                if (!self->exception)
                    self->state = expecting_token_state;
            }
            else
            if (self->event == comma_event) {
                if (!self->exception) {
                    //  have_fn_inline_token
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ have_fn_inline_token");
                    have_fn_inline_token (self->parent);
                }
                if (!self->exception) {
                    //  push_back_to_previous
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ push_back_to_previous");
                    push_back_to_previous (self->parent);
                }
                if (!self->exception)
                    self->state = expecting_token_state;
            }
            else
            if (self->event == period_event) {
                if (!self->exception) {
                    //  have_fn_inline_token
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ have_fn_inline_token");
                    have_fn_inline_token (self->parent);
                }
                if (!self->exception) {
                    //  push_back_to_previous
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ push_back_to_previous");
                    push_back_to_previous (self->parent);
                }
                if (!self->exception)
                    self->state = expecting_token_state;
            }
            else
            if (self->event == finished_event) {
                if (!self->exception) {
                    //  have_fn_inline_token
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ have_fn_inline_token");
                    have_fn_inline_token (self->parent);
                }
                if (!self->exception)
                    self->state = expecting_token_state;
            }
            else
            if (self->event == other_event) {
                if (!self->exception) {
                    //  have_invalid_token
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ have_invalid_token");
                    have_invalid_token (self->parent);
                }
                if (!self->exception)
                    self->state = expecting_token_state;
            }
            else
            if (self->event == comment_event) {
                if (!self->exception) {
                    //  parse_next_character
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ parse_next_character");
                    parse_next_character (self->parent);
                }
                if (!self->exception)
                    self->state = reading_comment_state;
            }
            else {
                //  Handle all other events
                if (!self->exception) {
                    //  have_invalid_token
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ have_invalid_token");
                    have_invalid_token (self->parent);
                }
                if (!self->exception)
                    self->state = expecting_token_state;
            }
        }
        else
        if (self->state == after_period_state) {
            if (self->event == digit_event) {
                if (!self->exception) {
                    //  store_the_character
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ store_the_character");
                    store_the_character (self->parent);
                }
                if (!self->exception) {
                    //  parse_next_character
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ parse_next_character");
                    parse_next_character (self->parent);
                }
                if (!self->exception)
                    self->state = reading_number_state;
            }
            else {
                //  Handle all other events
                if (!self->exception) {
                    //  have_sentence_token
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ have_sentence_token");
                    have_sentence_token (self->parent);
                }
                if (!self->exception) {
                    //  push_back_to_previous
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ push_back_to_previous");
                    push_back_to_previous (self->parent);
                }
                if (!self->exception)
                    self->state = expecting_token_state;
            }
        }
        else
        if (self->state == reading_number_state) {
            if (self->event == digit_event) {
                if (!self->exception) {
                    //  store_the_character
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ store_the_character");
                    store_the_character (self->parent);
                }
                if (!self->exception) {
                    //  parse_next_character
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ parse_next_character");
                    parse_next_character (self->parent);
                }
            }
            else
            if (self->event == letter_event) {
                if (!self->exception) {
                    //  store_the_character
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ store_the_character");
                    store_the_character (self->parent);
                }
                if (!self->exception) {
                    //  parse_next_character
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ parse_next_character");
                    parse_next_character (self->parent);
                }
            }
            else
            if (self->event == hyphen_event) {
                if (!self->exception) {
                    //  store_the_character
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ store_the_character");
                    store_the_character (self->parent);
                }
                if (!self->exception) {
                    //  parse_next_character
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ parse_next_character");
                    parse_next_character (self->parent);
                }
            }
            else
            if (self->event == plus_event) {
                if (!self->exception) {
                    //  store_the_character
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ store_the_character");
                    store_the_character (self->parent);
                }
                if (!self->exception) {
                    //  parse_next_character
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ parse_next_character");
                    parse_next_character (self->parent);
                }
            }
            else
            if (self->event == percent_event) {
                if (!self->exception) {
                    //  store_the_character
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ store_the_character");
                    store_the_character (self->parent);
                }
                if (!self->exception) {
                    //  have_number_token
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ have_number_token");
                    have_number_token (self->parent);
                }
                if (!self->exception)
                    self->state = expecting_token_state;
            }
            else
            if (self->event == close_list_event) {
                if (!self->exception) {
                    //  have_number_token
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ have_number_token");
                    have_number_token (self->parent);
                }
                if (!self->exception) {
                    //  push_back_to_previous
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ push_back_to_previous");
                    push_back_to_previous (self->parent);
                }
                if (!self->exception)
                    self->state = expecting_token_state;
            }
            else
            if (self->event == comma_event) {
                if (!self->exception) {
                    //  parse_next_character
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ parse_next_character");
                    parse_next_character (self->parent);
                }
                if (!self->exception)
                    self->state = after_number_comma_state;
            }
            else
            if (self->event == period_event) {
                if (!self->exception) {
                    //  parse_next_character
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ parse_next_character");
                    parse_next_character (self->parent);
                }
                if (!self->exception)
                    self->state = after_number_period_state;
            }
            else
            if (self->event == whitespace_event) {
                if (!self->exception) {
                    //  have_number_token
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ have_number_token");
                    have_number_token (self->parent);
                }
                if (!self->exception)
                    self->state = expecting_token_state;
            }
            else
            if (self->event == newline_event) {
                if (!self->exception) {
                    //  have_number_token
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ have_number_token");
                    have_number_token (self->parent);
                }
                if (!self->exception)
                    self->state = expecting_token_state;
            }
            else
            if (self->event == finished_event) {
                if (!self->exception) {
                    //  have_number_token
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ have_number_token");
                    have_number_token (self->parent);
                }
                if (!self->exception)
                    self->state = expecting_token_state;
            }
            else
            if (self->event == other_event) {
                if (!self->exception) {
                    //  have_invalid_token
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ have_invalid_token");
                    have_invalid_token (self->parent);
                }
                if (!self->exception)
                    self->state = expecting_token_state;
            }
            else
            if (self->event == comment_event) {
                if (!self->exception) {
                    //  parse_next_character
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ parse_next_character");
                    parse_next_character (self->parent);
                }
                if (!self->exception)
                    self->state = reading_comment_state;
            }
            else {
                //  Handle all other events
                if (!self->exception) {
                    //  have_invalid_token
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ have_invalid_token");
                    have_invalid_token (self->parent);
                }
                if (!self->exception)
                    self->state = expecting_token_state;
            }
        }
        else
        if (self->state == after_number_comma_state) {
            if (self->event == digit_event) {
                if (!self->exception) {
                    //  store_comma_character
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ store_comma_character");
                    store_comma_character (self->parent);
                }
                if (!self->exception) {
                    //  store_the_character
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ store_the_character");
                    store_the_character (self->parent);
                }
                if (!self->exception) {
                    //  parse_next_character
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ parse_next_character");
                    parse_next_character (self->parent);
                }
                if (!self->exception)
                    self->state = reading_number_state;
            }
            else {
                //  Handle all other events
                if (!self->exception) {
                    //  have_number_token
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ have_number_token");
                    have_number_token (self->parent);
                }
                if (!self->exception) {
                    //  push_back_to_previous
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ push_back_to_previous");
                    push_back_to_previous (self->parent);
                }
                if (!self->exception) {
                    //  push_back_to_previous
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ push_back_to_previous");
                    push_back_to_previous (self->parent);
                }
                if (!self->exception)
                    self->state = expecting_token_state;
            }
        }
        else
        if (self->state == after_number_period_state) {
            if (self->event == digit_event) {
                if (!self->exception) {
                    //  store_period_character
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ store_period_character");
                    store_period_character (self->parent);
                }
                if (!self->exception) {
                    //  store_the_character
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ store_the_character");
                    store_the_character (self->parent);
                }
                if (!self->exception) {
                    //  parse_next_character
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ parse_next_character");
                    parse_next_character (self->parent);
                }
                if (!self->exception)
                    self->state = reading_number_state;
            }
            else {
                //  Handle all other events
                if (!self->exception) {
                    //  have_number_token
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ have_number_token");
                    have_number_token (self->parent);
                }
                if (!self->exception) {
                    //  push_back_to_previous
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ push_back_to_previous");
                    push_back_to_previous (self->parent);
                }
                if (!self->exception) {
                    //  push_back_to_previous
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ push_back_to_previous");
                    push_back_to_previous (self->parent);
                }
                if (!self->exception)
                    self->state = expecting_token_state;
            }
        }
        else
        if (self->state == reading_string_state) {
            if (self->event == close_quote_event) {
                if (!self->exception) {
                    //  have_string_token
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ have_string_token");
                    have_string_token (self->parent);
                }
                if (!self->exception)
                    self->state = expecting_token_state;
            }
            else
            if (self->event == finished_event) {
                if (!self->exception) {
                    //  store_newline_character
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ store_newline_character");
                    store_newline_character (self->parent);
                }
            }
            else {
                //  Handle all other events
                if (!self->exception) {
                    //  store_the_character
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ store_the_character");
                    store_the_character (self->parent);
                }
                if (!self->exception) {
                    //  parse_next_character
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ parse_next_character");
                    parse_next_character (self->parent);
                }
            }
        }
        else
        if (self->state == reading_comment_state) {
            if (self->event == newline_event) {
                if (!self->exception) {
                    //  parse_next_character
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ parse_next_character");
                    parse_next_character (self->parent);
                }
                if (!self->exception)
                    self->state = expecting_token_state;
            }
            else
            if (self->event == finished_event) {
                if (!self->exception) {
                    //  have_null_token
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ have_null_token");
                    have_null_token (self->parent);
                }
                if (!self->exception)
                    self->state = expecting_token_state;
            }
            else {
                //  Handle all other events
                if (!self->exception) {
                    //  parse_next_character
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ parse_next_character");
                    parse_next_character (self->parent);
                }
            }
        }
        else
        if (self->state == defaults_state) {
            if (self->event == other_event) {
                if (!self->exception) {
                    //  have_invalid_token
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ have_invalid_token");
                    have_invalid_token (self->parent);
                }
                if (!self->exception)
                    self->state = expecting_token_state;
            }
            else
            if (self->event == whitespace_event) {
                if (!self->exception) {
                    //  parse_next_character
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ parse_next_character");
                    parse_next_character (self->parent);
                }
            }
            else
            if (self->event == comment_event) {
                if (!self->exception) {
                    //  parse_next_character
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ parse_next_character");
                    parse_next_character (self->parent);
                }
                if (!self->exception)
                    self->state = reading_comment_state;
            }
            else
            if (self->event == newline_event) {
                if (!self->exception) {
                    //  parse_next_character
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ parse_next_character");
                    parse_next_character (self->parent);
                }
            }
            else {
                //  Handle all other events
                if (!self->exception) {
                    //  have_invalid_token
                    if (self->animate)
                        zsys_debug ("zs_lex:                $ have_invalid_token");
                    have_invalid_token (self->parent);
                }
                if (!self->exception)
                    self->state = expecting_token_state;
            }
        }
        //  If we had an exception event, interrupt normal programming
        if (self->exception) {
            if (self->animate)
                zsys_debug ("zs_lex:                ! %s", s_event_name [self->exception]);
            self->next_event = self->exception;
        }
        else
        if (self->animate)
            zsys_debug ("zs_lex:                > %s", s_state_name [self->state]);
    }
}
