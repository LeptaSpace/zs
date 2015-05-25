/*  =========================================================================
    zs_repl_fsm - No title state machine engine

    ** WARNING *************************************************************
    THIS SOURCE FILE IS 100% GENERATED. If you edit this file, you will lose
    your changes at the next build cycle. This is great for temporary printf
    statements. DO NOT MAKE ANY CHANGES YOU WISH TO KEEP. The correct places
    for commits are:

     * The XML model used for this code generation: zs_repl.xml, or
     * The code generation script that built this file: fsm_c
    ************************************************************************
    =========================================================================
*/


//  ---------------------------------------------------------------------------
//  State machine constants

typedef enum {
    starting_state = 1,
    building_shell_state = 2,
    building_function_state = 3,
    defaults_state = 4
} state_t;

typedef enum {
    NULL_event = 0,
    number_event = 1,
    string_event = 2,
    fn_inline_event = 3,
    fn_nested_event = 4,
    fn_define_event = 5,
    fn_close_event = 6,
    completed_event = 7,
    committed_event = 8,
    phrase_event = 9,
    sentence_event = 10,
    finished_event = 11,
    invalid_event = 12
} event_t;

//  Names for state machine logging and error reporting
static char *
s_state_name [] = {
    "(NONE)",
    "starting",
    "building_shell",
    "building_function",
    "defaults"
};

static char *
s_event_name [] = {
    "(NONE)",
    "number",
    "string",
    "fn_inline",
    "fn_nested",
    "fn_define",
    "fn_close",
    "completed",
    "committed",
    "phrase",
    "sentence",
    "finished",
    "invalid"
};

//  Action prototypes
static void compile_define_shell (zs_repl_t *self);
static void compile_number (zs_repl_t *self);
static void get_next_token (zs_repl_t *self);
static void compile_string (zs_repl_t *self);
static void compile_inline_call (zs_repl_t *self);
static void compile_nested_call (zs_repl_t *self);
static void compile_define (zs_repl_t *self);
static void compile_unnest (zs_repl_t *self);
static void compile_end_of_sentence (zs_repl_t *self);
static void compile_commit_shell (zs_repl_t *self);
static void run_virtual_machine (zs_repl_t *self);
static void rollback_the_function (zs_repl_t *self);
static void compile_unnest_or_commit (zs_repl_t *self);
static void compile_end_of_phrase (zs_repl_t *self);
static void check_if_completed (zs_repl_t *self);
static void signal_syntax_error (zs_repl_t *self);

//  This is the context block for a FSM thread; use the setter
//  methods to set the FSM properties.

typedef struct {
    zs_repl_t *parent;          //  Parent class
    bool animate;               //  Animate state machine
    state_t state;              //  Current state
    event_t event;              //  Current event
    event_t next_event;         //  The next event
    event_t exception;          //  Exception event, if any
    uint64_t cycles;            //  Track the work done
} fsm_t;

static fsm_t *
fsm_new (zs_repl_t *parent)
{
    fsm_t *self = (fsm_t *) zmalloc (sizeof (fsm_t));
    if (self) {
        self->state = starting_state;
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
zs_repl_not_used (fsm_t *self)
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
            zsys_debug ("zs_repl: %s:", s_state_name [self->state]);
            zsys_debug ("zs_repl:           %s", s_event_name [self->event]);
        }
        if (self->state == starting_state) {
            if (self->event == number_event) {
                if (!self->exception) {
                    //  compile_define_shell
                    if (self->animate)
                        zsys_debug ("zs_repl:               $ compile_define_shell");
                    compile_define_shell (self->parent);
                }
                if (!self->exception) {
                    //  compile_number
                    if (self->animate)
                        zsys_debug ("zs_repl:               $ compile_number");
                    compile_number (self->parent);
                }
                if (!self->exception) {
                    //  get_next_token
                    if (self->animate)
                        zsys_debug ("zs_repl:               $ get_next_token");
                    get_next_token (self->parent);
                }
                if (!self->exception)
                    self->state = building_shell_state;
            }
            else
            if (self->event == string_event) {
                if (!self->exception) {
                    //  compile_define_shell
                    if (self->animate)
                        zsys_debug ("zs_repl:               $ compile_define_shell");
                    compile_define_shell (self->parent);
                }
                if (!self->exception) {
                    //  compile_string
                    if (self->animate)
                        zsys_debug ("zs_repl:               $ compile_string");
                    compile_string (self->parent);
                }
                if (!self->exception) {
                    //  get_next_token
                    if (self->animate)
                        zsys_debug ("zs_repl:               $ get_next_token");
                    get_next_token (self->parent);
                }
                if (!self->exception)
                    self->state = building_shell_state;
            }
            else
            if (self->event == fn_inline_event) {
                if (!self->exception) {
                    //  compile_define_shell
                    if (self->animate)
                        zsys_debug ("zs_repl:               $ compile_define_shell");
                    compile_define_shell (self->parent);
                }
                if (!self->exception) {
                    //  compile_inline_call
                    if (self->animate)
                        zsys_debug ("zs_repl:               $ compile_inline_call");
                    compile_inline_call (self->parent);
                }
                if (!self->exception) {
                    //  get_next_token
                    if (self->animate)
                        zsys_debug ("zs_repl:               $ get_next_token");
                    get_next_token (self->parent);
                }
                if (!self->exception)
                    self->state = building_shell_state;
            }
            else
            if (self->event == fn_nested_event) {
                if (!self->exception) {
                    //  compile_define_shell
                    if (self->animate)
                        zsys_debug ("zs_repl:               $ compile_define_shell");
                    compile_define_shell (self->parent);
                }
                if (!self->exception) {
                    //  compile_nested_call
                    if (self->animate)
                        zsys_debug ("zs_repl:               $ compile_nested_call");
                    compile_nested_call (self->parent);
                }
                if (!self->exception) {
                    //  get_next_token
                    if (self->animate)
                        zsys_debug ("zs_repl:               $ get_next_token");
                    get_next_token (self->parent);
                }
                if (!self->exception)
                    self->state = building_shell_state;
            }
            else
            if (self->event == fn_define_event) {
                if (!self->exception) {
                    //  compile_define
                    if (self->animate)
                        zsys_debug ("zs_repl:               $ compile_define");
                    compile_define (self->parent);
                }
                if (!self->exception) {
                    //  get_next_token
                    if (self->animate)
                        zsys_debug ("zs_repl:               $ get_next_token");
                    get_next_token (self->parent);
                }
                if (!self->exception)
                    self->state = building_function_state;
            }
            else
            if (self->event == finished_event) {
                if (!self->exception) {
                    //  check_if_completed
                    if (self->animate)
                        zsys_debug ("zs_repl:               $ check_if_completed");
                    check_if_completed (self->parent);
                }
            }
            else
            if (self->event == completed_event) {
                //  No action - just logging
                if (self->animate)
                    zsys_debug ("zs_repl:               $ completed");
                if (!self->exception)
                    self->state = starting_state;
            }
            else
            if (self->event == phrase_event) {
                if (!self->exception) {
                    //  rollback_the_function
                    if (self->animate)
                        zsys_debug ("zs_repl:               $ rollback_the_function");
                    rollback_the_function (self->parent);
                }
                if (!self->exception) {
                    //  signal_syntax_error
                    if (self->animate)
                        zsys_debug ("zs_repl:               $ signal_syntax_error");
                    signal_syntax_error (self->parent);
                }
                if (!self->exception)
                    self->state = starting_state;
            }
            else
            if (self->event == sentence_event) {
                if (!self->exception) {
                    //  rollback_the_function
                    if (self->animate)
                        zsys_debug ("zs_repl:               $ rollback_the_function");
                    rollback_the_function (self->parent);
                }
                if (!self->exception) {
                    //  signal_syntax_error
                    if (self->animate)
                        zsys_debug ("zs_repl:               $ signal_syntax_error");
                    signal_syntax_error (self->parent);
                }
                if (!self->exception)
                    self->state = starting_state;
            }
            else
            if (self->event == fn_close_event) {
                if (!self->exception) {
                    //  rollback_the_function
                    if (self->animate)
                        zsys_debug ("zs_repl:               $ rollback_the_function");
                    rollback_the_function (self->parent);
                }
                if (!self->exception) {
                    //  signal_syntax_error
                    if (self->animate)
                        zsys_debug ("zs_repl:               $ signal_syntax_error");
                    signal_syntax_error (self->parent);
                }
                if (!self->exception)
                    self->state = starting_state;
            }
            else
            if (self->event == invalid_event) {
                if (!self->exception) {
                    //  rollback_the_function
                    if (self->animate)
                        zsys_debug ("zs_repl:               $ rollback_the_function");
                    rollback_the_function (self->parent);
                }
                if (!self->exception) {
                    //  signal_syntax_error
                    if (self->animate)
                        zsys_debug ("zs_repl:               $ signal_syntax_error");
                    signal_syntax_error (self->parent);
                }
                if (!self->exception)
                    self->state = starting_state;
            }
            else {
                //  Handle unexpected internal events
                zsys_warning ("zs_repl: unhandled event %s in %s",
                    s_event_name [self->event], s_state_name [self->state]);
                exit (-1);
            }
        }
        else
        if (self->state == building_shell_state) {
            if (self->event == fn_close_event) {
                if (!self->exception) {
                    //  compile_unnest
                    if (self->animate)
                        zsys_debug ("zs_repl:               $ compile_unnest");
                    compile_unnest (self->parent);
                }
                if (!self->exception) {
                    //  get_next_token
                    if (self->animate)
                        zsys_debug ("zs_repl:               $ get_next_token");
                    get_next_token (self->parent);
                }
            }
            else
            if (self->event == completed_event) {
                if (!self->exception) {
                    //  compile_end_of_sentence
                    if (self->animate)
                        zsys_debug ("zs_repl:               $ compile_end_of_sentence");
                    compile_end_of_sentence (self->parent);
                }
                if (!self->exception) {
                    //  compile_commit_shell
                    if (self->animate)
                        zsys_debug ("zs_repl:               $ compile_commit_shell");
                    compile_commit_shell (self->parent);
                }
                if (!self->exception) {
                    //  run_virtual_machine
                    if (self->animate)
                        zsys_debug ("zs_repl:               $ run_virtual_machine");
                    run_virtual_machine (self->parent);
                }
                if (!self->exception) {
                    //  rollback_the_function
                    if (self->animate)
                        zsys_debug ("zs_repl:               $ rollback_the_function");
                    rollback_the_function (self->parent);
                }
                if (!self->exception)
                    self->state = starting_state;
            }
            else
            if (self->event == number_event) {
                if (!self->exception) {
                    //  compile_number
                    if (self->animate)
                        zsys_debug ("zs_repl:               $ compile_number");
                    compile_number (self->parent);
                }
                if (!self->exception) {
                    //  get_next_token
                    if (self->animate)
                        zsys_debug ("zs_repl:               $ get_next_token");
                    get_next_token (self->parent);
                }
            }
            else
            if (self->event == string_event) {
                if (!self->exception) {
                    //  compile_string
                    if (self->animate)
                        zsys_debug ("zs_repl:               $ compile_string");
                    compile_string (self->parent);
                }
                if (!self->exception) {
                    //  get_next_token
                    if (self->animate)
                        zsys_debug ("zs_repl:               $ get_next_token");
                    get_next_token (self->parent);
                }
            }
            else
            if (self->event == fn_inline_event) {
                if (!self->exception) {
                    //  compile_inline_call
                    if (self->animate)
                        zsys_debug ("zs_repl:               $ compile_inline_call");
                    compile_inline_call (self->parent);
                }
                if (!self->exception) {
                    //  get_next_token
                    if (self->animate)
                        zsys_debug ("zs_repl:               $ get_next_token");
                    get_next_token (self->parent);
                }
            }
            else
            if (self->event == fn_nested_event) {
                if (!self->exception) {
                    //  compile_nested_call
                    if (self->animate)
                        zsys_debug ("zs_repl:               $ compile_nested_call");
                    compile_nested_call (self->parent);
                }
                if (!self->exception) {
                    //  get_next_token
                    if (self->animate)
                        zsys_debug ("zs_repl:               $ get_next_token");
                    get_next_token (self->parent);
                }
            }
            else
            if (self->event == committed_event) {
                if (!self->exception) {
                    //  get_next_token
                    if (self->animate)
                        zsys_debug ("zs_repl:               $ get_next_token");
                    get_next_token (self->parent);
                }
                if (!self->exception)
                    self->state = starting_state;
            }
            else
            if (self->event == phrase_event) {
                if (!self->exception) {
                    //  compile_end_of_phrase
                    if (self->animate)
                        zsys_debug ("zs_repl:               $ compile_end_of_phrase");
                    compile_end_of_phrase (self->parent);
                }
                if (!self->exception) {
                    //  get_next_token
                    if (self->animate)
                        zsys_debug ("zs_repl:               $ get_next_token");
                    get_next_token (self->parent);
                }
            }
            else
            if (self->event == sentence_event) {
                if (!self->exception) {
                    //  compile_end_of_sentence
                    if (self->animate)
                        zsys_debug ("zs_repl:               $ compile_end_of_sentence");
                    compile_end_of_sentence (self->parent);
                }
                if (!self->exception) {
                    //  get_next_token
                    if (self->animate)
                        zsys_debug ("zs_repl:               $ get_next_token");
                    get_next_token (self->parent);
                }
            }
            else
            if (self->event == finished_event) {
                if (!self->exception) {
                    //  check_if_completed
                    if (self->animate)
                        zsys_debug ("zs_repl:               $ check_if_completed");
                    check_if_completed (self->parent);
                }
            }
            else
            if (self->event == fn_define_event) {
                if (!self->exception) {
                    //  rollback_the_function
                    if (self->animate)
                        zsys_debug ("zs_repl:               $ rollback_the_function");
                    rollback_the_function (self->parent);
                }
                if (!self->exception) {
                    //  signal_syntax_error
                    if (self->animate)
                        zsys_debug ("zs_repl:               $ signal_syntax_error");
                    signal_syntax_error (self->parent);
                }
                if (!self->exception)
                    self->state = starting_state;
            }
            else
            if (self->event == invalid_event) {
                if (!self->exception) {
                    //  rollback_the_function
                    if (self->animate)
                        zsys_debug ("zs_repl:               $ rollback_the_function");
                    rollback_the_function (self->parent);
                }
                if (!self->exception) {
                    //  signal_syntax_error
                    if (self->animate)
                        zsys_debug ("zs_repl:               $ signal_syntax_error");
                    signal_syntax_error (self->parent);
                }
                if (!self->exception)
                    self->state = starting_state;
            }
            else {
                //  Handle unexpected internal events
                zsys_warning ("zs_repl: unhandled event %s in %s",
                    s_event_name [self->event], s_state_name [self->state]);
                exit (-1);
            }
        }
        else
        if (self->state == building_function_state) {
            if (self->event == number_event) {
                if (!self->exception) {
                    //  compile_number
                    if (self->animate)
                        zsys_debug ("zs_repl:               $ compile_number");
                    compile_number (self->parent);
                }
                if (!self->exception) {
                    //  get_next_token
                    if (self->animate)
                        zsys_debug ("zs_repl:               $ get_next_token");
                    get_next_token (self->parent);
                }
            }
            else
            if (self->event == string_event) {
                if (!self->exception) {
                    //  compile_string
                    if (self->animate)
                        zsys_debug ("zs_repl:               $ compile_string");
                    compile_string (self->parent);
                }
                if (!self->exception) {
                    //  get_next_token
                    if (self->animate)
                        zsys_debug ("zs_repl:               $ get_next_token");
                    get_next_token (self->parent);
                }
            }
            else
            if (self->event == fn_inline_event) {
                if (!self->exception) {
                    //  compile_inline_call
                    if (self->animate)
                        zsys_debug ("zs_repl:               $ compile_inline_call");
                    compile_inline_call (self->parent);
                }
                if (!self->exception) {
                    //  get_next_token
                    if (self->animate)
                        zsys_debug ("zs_repl:               $ get_next_token");
                    get_next_token (self->parent);
                }
            }
            else
            if (self->event == fn_nested_event) {
                if (!self->exception) {
                    //  compile_nested_call
                    if (self->animate)
                        zsys_debug ("zs_repl:               $ compile_nested_call");
                    compile_nested_call (self->parent);
                }
                if (!self->exception) {
                    //  get_next_token
                    if (self->animate)
                        zsys_debug ("zs_repl:               $ get_next_token");
                    get_next_token (self->parent);
                }
            }
            else
            if (self->event == fn_close_event) {
                if (!self->exception) {
                    //  compile_unnest_or_commit
                    if (self->animate)
                        zsys_debug ("zs_repl:               $ compile_unnest_or_commit");
                    compile_unnest_or_commit (self->parent);
                }
                if (!self->exception) {
                    //  get_next_token
                    if (self->animate)
                        zsys_debug ("zs_repl:               $ get_next_token");
                    get_next_token (self->parent);
                }
            }
            else
            if (self->event == committed_event) {
                if (!self->exception) {
                    //  get_next_token
                    if (self->animate)
                        zsys_debug ("zs_repl:               $ get_next_token");
                    get_next_token (self->parent);
                }
                if (!self->exception)
                    self->state = starting_state;
            }
            else
            if (self->event == phrase_event) {
                if (!self->exception) {
                    //  compile_end_of_phrase
                    if (self->animate)
                        zsys_debug ("zs_repl:               $ compile_end_of_phrase");
                    compile_end_of_phrase (self->parent);
                }
                if (!self->exception) {
                    //  get_next_token
                    if (self->animate)
                        zsys_debug ("zs_repl:               $ get_next_token");
                    get_next_token (self->parent);
                }
            }
            else
            if (self->event == sentence_event) {
                if (!self->exception) {
                    //  compile_end_of_sentence
                    if (self->animate)
                        zsys_debug ("zs_repl:               $ compile_end_of_sentence");
                    compile_end_of_sentence (self->parent);
                }
                if (!self->exception) {
                    //  get_next_token
                    if (self->animate)
                        zsys_debug ("zs_repl:               $ get_next_token");
                    get_next_token (self->parent);
                }
            }
            else
            if (self->event == finished_event) {
                if (!self->exception) {
                    //  check_if_completed
                    if (self->animate)
                        zsys_debug ("zs_repl:               $ check_if_completed");
                    check_if_completed (self->parent);
                }
            }
            else
            if (self->event == completed_event) {
                //  No action - just logging
                if (self->animate)
                    zsys_debug ("zs_repl:               $ completed");
                if (!self->exception)
                    self->state = starting_state;
            }
            else
            if (self->event == fn_define_event) {
                if (!self->exception) {
                    //  rollback_the_function
                    if (self->animate)
                        zsys_debug ("zs_repl:               $ rollback_the_function");
                    rollback_the_function (self->parent);
                }
                if (!self->exception) {
                    //  signal_syntax_error
                    if (self->animate)
                        zsys_debug ("zs_repl:               $ signal_syntax_error");
                    signal_syntax_error (self->parent);
                }
                if (!self->exception)
                    self->state = starting_state;
            }
            else
            if (self->event == invalid_event) {
                if (!self->exception) {
                    //  rollback_the_function
                    if (self->animate)
                        zsys_debug ("zs_repl:               $ rollback_the_function");
                    rollback_the_function (self->parent);
                }
                if (!self->exception) {
                    //  signal_syntax_error
                    if (self->animate)
                        zsys_debug ("zs_repl:               $ signal_syntax_error");
                    signal_syntax_error (self->parent);
                }
                if (!self->exception)
                    self->state = starting_state;
            }
            else {
                //  Handle unexpected internal events
                zsys_warning ("zs_repl: unhandled event %s in %s",
                    s_event_name [self->event], s_state_name [self->state]);
                exit (-1);
            }
        }
        else
        if (self->state == defaults_state) {
            if (self->event == finished_event) {
                if (!self->exception) {
                    //  check_if_completed
                    if (self->animate)
                        zsys_debug ("zs_repl:               $ check_if_completed");
                    check_if_completed (self->parent);
                }
            }
            else
            if (self->event == completed_event) {
                //  No action - just logging
                if (self->animate)
                    zsys_debug ("zs_repl:               $ completed");
                if (!self->exception)
                    self->state = starting_state;
            }
            else
            if (self->event == number_event) {
                if (!self->exception) {
                    //  rollback_the_function
                    if (self->animate)
                        zsys_debug ("zs_repl:               $ rollback_the_function");
                    rollback_the_function (self->parent);
                }
                if (!self->exception) {
                    //  signal_syntax_error
                    if (self->animate)
                        zsys_debug ("zs_repl:               $ signal_syntax_error");
                    signal_syntax_error (self->parent);
                }
                if (!self->exception)
                    self->state = starting_state;
            }
            else
            if (self->event == string_event) {
                if (!self->exception) {
                    //  rollback_the_function
                    if (self->animate)
                        zsys_debug ("zs_repl:               $ rollback_the_function");
                    rollback_the_function (self->parent);
                }
                if (!self->exception) {
                    //  signal_syntax_error
                    if (self->animate)
                        zsys_debug ("zs_repl:               $ signal_syntax_error");
                    signal_syntax_error (self->parent);
                }
                if (!self->exception)
                    self->state = starting_state;
            }
            else
            if (self->event == fn_inline_event) {
                if (!self->exception) {
                    //  rollback_the_function
                    if (self->animate)
                        zsys_debug ("zs_repl:               $ rollback_the_function");
                    rollback_the_function (self->parent);
                }
                if (!self->exception) {
                    //  signal_syntax_error
                    if (self->animate)
                        zsys_debug ("zs_repl:               $ signal_syntax_error");
                    signal_syntax_error (self->parent);
                }
                if (!self->exception)
                    self->state = starting_state;
            }
            else
            if (self->event == fn_nested_event) {
                if (!self->exception) {
                    //  rollback_the_function
                    if (self->animate)
                        zsys_debug ("zs_repl:               $ rollback_the_function");
                    rollback_the_function (self->parent);
                }
                if (!self->exception) {
                    //  signal_syntax_error
                    if (self->animate)
                        zsys_debug ("zs_repl:               $ signal_syntax_error");
                    signal_syntax_error (self->parent);
                }
                if (!self->exception)
                    self->state = starting_state;
            }
            else
            if (self->event == fn_define_event) {
                if (!self->exception) {
                    //  rollback_the_function
                    if (self->animate)
                        zsys_debug ("zs_repl:               $ rollback_the_function");
                    rollback_the_function (self->parent);
                }
                if (!self->exception) {
                    //  signal_syntax_error
                    if (self->animate)
                        zsys_debug ("zs_repl:               $ signal_syntax_error");
                    signal_syntax_error (self->parent);
                }
                if (!self->exception)
                    self->state = starting_state;
            }
            else
            if (self->event == phrase_event) {
                if (!self->exception) {
                    //  rollback_the_function
                    if (self->animate)
                        zsys_debug ("zs_repl:               $ rollback_the_function");
                    rollback_the_function (self->parent);
                }
                if (!self->exception) {
                    //  signal_syntax_error
                    if (self->animate)
                        zsys_debug ("zs_repl:               $ signal_syntax_error");
                    signal_syntax_error (self->parent);
                }
                if (!self->exception)
                    self->state = starting_state;
            }
            else
            if (self->event == sentence_event) {
                if (!self->exception) {
                    //  rollback_the_function
                    if (self->animate)
                        zsys_debug ("zs_repl:               $ rollback_the_function");
                    rollback_the_function (self->parent);
                }
                if (!self->exception) {
                    //  signal_syntax_error
                    if (self->animate)
                        zsys_debug ("zs_repl:               $ signal_syntax_error");
                    signal_syntax_error (self->parent);
                }
                if (!self->exception)
                    self->state = starting_state;
            }
            else
            if (self->event == fn_close_event) {
                if (!self->exception) {
                    //  rollback_the_function
                    if (self->animate)
                        zsys_debug ("zs_repl:               $ rollback_the_function");
                    rollback_the_function (self->parent);
                }
                if (!self->exception) {
                    //  signal_syntax_error
                    if (self->animate)
                        zsys_debug ("zs_repl:               $ signal_syntax_error");
                    signal_syntax_error (self->parent);
                }
                if (!self->exception)
                    self->state = starting_state;
            }
            else
            if (self->event == invalid_event) {
                if (!self->exception) {
                    //  rollback_the_function
                    if (self->animate)
                        zsys_debug ("zs_repl:               $ rollback_the_function");
                    rollback_the_function (self->parent);
                }
                if (!self->exception) {
                    //  signal_syntax_error
                    if (self->animate)
                        zsys_debug ("zs_repl:               $ signal_syntax_error");
                    signal_syntax_error (self->parent);
                }
                if (!self->exception)
                    self->state = starting_state;
            }
            else {
                //  Handle unexpected internal events
                zsys_warning ("zs_repl: unhandled event %s in %s",
                    s_event_name [self->event], s_state_name [self->state]);
                exit (-1);
            }
        }
        //  If we had an exception event, interrupt normal programming
        if (self->exception) {
            if (self->animate)
                zsys_debug ("zs_repl:               ! %s", s_event_name [self->exception]);
            self->next_event = self->exception;
        }
        else
        if (self->animate)
            zsys_debug ("zs_repl:               > %s", s_state_name [self->state]);
    }
}
