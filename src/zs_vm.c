/*  =========================================================================
    zs_vm - ZeroScript virtual machine

    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of the ZeroScript language, http://zeroscript.org.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

/*
@header
    This class encodes and executes a ZeroScript application using a threaded
    bytecode virtual machine. Actual parsing is done by zs_repl and zs_lex.
@discuss
    Notes about input/output:
    - each function receives an input pipe and an output pipe
    - pipes are connected automatically as in Unix command line |
    - constants are added to current output pipe

    Notes about the virtual machine:
    - token threaded bytecode interpreter
    - machine uses bytecodes with parameters following each opcode
    - 240-254 are built-in opcodes
        - essential to machine operation
        - decoding costs must be minimized
        - handled by if/switch in core interpreter
        - can modify instruction pointer (needle)
    - 0..239 are class 0 atomics
        - no class name (short obvious names)
        - assumed to be most commonly used
        - core runtime for ZeroScript machines
        - easy to extend by modifying codebase
        - decoding costs are very low
    - 255 + n are higher class atomics
        - naming proposal is class.function
        - classes are numbered by VM 1..n
        - assumed to change externally
        - designed to be added dynamically
        - decoding costs are insignificant
@end
*/

//  Bytecodes
//  - up to 240 class 0 dictionary
//  - 255 + 16 bits = extensions; class (1..n) + function numbe

//  These are built-in opcodes which are allowed to modify the needle, so we
//  can keep it in a register variable here and perhaps save some CPU cycles.
//  Lol. \o/  Put these in order of frequency.
#define VM_CALL         254     //  Call a user function
#define VM_RETURN       253     //  Return to previous needle
#define VM_WHOLE        252     //  Issue a whole number constant
#define VM_REAL         251     //  Issue a real number constant
#define VM_STRING       250     //  Issue a string constant

#define VM_STRICT       249     //  Prepare strict function call
#define VM_MODEST       248     //  Prepare modest function call
#define VM_GREEDY       247     //  Prepare greedy function call
#define VM_NESTED       246     //  Open new pipe scope
#define VM_UNNEST       245     //  End pipe scope, pop pipes
#define VM_PHRASE       244     //  End phrase
#define VM_SENTENCE     243     //  End sentence

#define VM_UNUSED       242     //  Unused
#define VM_GUARD        241     //  Assert if we ever reach this
#define VM_STOP         240     //  Last built-in


#include "zs_classes.h"

//  Work with atomics

typedef struct {
    zs_vm_fn_t *function;           //  Native C function
    char *name;                     //  Primitive name
    char *hint;                     //  Hint to user
    zs_type_t type;                 //  Function type
} s_atomic_t;

static s_atomic_t *
s_atomic_new (zs_vm_fn_t *function, const char *name, zs_type_t type, const char *hint)
{
    s_atomic_t *self = (s_atomic_t *) zmalloc (sizeof (s_atomic_t));
    assert (self);
    self->function = function;
    self->name = strdup (name);
    self->hint = strdup (hint);
    self->type = type;
    return self;
}

static void
s_atomic_destroy (s_atomic_t **self_p)
{
    s_atomic_t *self = *self_p;
    if (self) {
        free (self->name);
        free (self->hint);
        free (self);
    }
}

//  Structure of our class

struct _zs_vm_t {
    s_atomic_t *atomics [240];      //  Class 0 atomics
    size_t nbr_atomics;             //  Nbr of atomics defined so far
    zs_vm_fn_t *probing;            //  Primitive during registration

    byte *code;                     //  Compiled bytecode (zchunk?)
    size_t code_max;                //  Allocated memory
    size_t code_size;               //  Actual amount used
    size_t code_head;               //  Last defined function
    size_t checkpoint;              //  When defining a function

    char *scope_stack [256];        //  Scope stack, arbitrary size
    size_t scope_stack_ptr;         //  Size of scope stack

    zs_pipe_t *output_stack [256];  //  Output stack, arbitrary size
    size_t output_stack_ptr;        //  Size of output stack
    zs_pipe_t *input;               //  Input to next function
    zs_pipe_t *output;              //  Current output sentence
    char *results;                  //  Sentence results, if any

    size_t call_stack [256];        //  Function call stack
    size_t call_stack_ptr;          //  Size of call stack

    bool verbose;                   //  Trace execution progress
    size_t iterator;                //  For listing function names
};

//  Map function guard to code body
static size_t
s_function_body (zs_vm_t *self, size_t guard)
{
    if (guard) {
        size_t body = guard + 3;
        body += strlen ((char *) self->code + body) + 1;
        return body;
    }
    else
        return 0;
}

//  Map function guard to printable name
static const char *
s_function_name (zs_vm_t *self, size_t guard)
{
    if (guard)
        return (const char *) self->code + guard + 3;
    else
        return "";
}

//  Return builtin opcode for name, or -1 if not known
static int
s_try_builtin (zs_vm_t *self, const char *name)
{
    if (streq (name, "stop"))
        return VM_STOP;
    return -1;
}

//  Return atomic opcode for name, or -1 if not known
static int
s_try_atomic (zs_vm_t *self, const char *name)
{
    size_t index;
    for (index = 0; index < self->nbr_atomics; index++)
        if (streq ((self->atomics [index])->name, name))
            return index;
    return -1;
}

//  Return function guard for name, or -1 if not known
static int
s_try_function (zs_vm_t *self, const char *name)
{
    size_t guard = self->code_head;
    while (guard) {
        assert (self->code [guard] == VM_GUARD);
        if (streq (name, s_function_name (self, guard)))
            return guard;
        size_t offset = (self->code [guard + 1] << 8) + self->code [guard + 2];
        assert (guard >= offset);
        guard -= offset;
    }
    return -1;
}

//  Compile call to function, atomic, or builtin (in that order)
static int
s_compile_call (zs_vm_t *self, byte opcode, const char *name)
{
    int found;
    if ((found = s_try_function (self, name)) != -1) {
        if (opcode)
            self->code [self->code_size++] = opcode;
        self->code [self->code_size++] = VM_CALL;
        self->code [self->code_size++] = (byte) (found >> 8);
        self->code [self->code_size++] = (byte) (found & 0xFF);
        return 0;
    }
    if ((found = s_try_atomic (self, name)) != -1) {
        if (opcode)
            self->code [self->code_size++] = opcode;
        self->code [self->code_size++] = (byte) found;
        return 0;
    }
    if ((found = s_try_builtin (self, name)) != -1) {
        if (opcode)
            self->code [self->code_size++] = opcode;
        self->code [self->code_size++] = (byte) found;
        return 0;
    }
    return -1;
}


//  ---------------------------------------------------------------------------
//  Create a new empty virtual machine. Returns the reference if successful,
//  or NULL if construction failed due to lack of available memory.

zs_vm_t *
zs_vm_new (void)
{
    zs_vm_t *self = (zs_vm_t *) zmalloc (sizeof (zs_vm_t));
    if (self) {
        self->input = zs_pipe_new ();
        self->output = zs_pipe_new ();
        self->code_max = 32000;         //  Arbitrary; TODO: extensible
        self->code = (byte *) malloc (self->code_max);
        self->code [self->code_size++] = VM_STOP;
    }
    return self;
}


//  ---------------------------------------------------------------------------
//  Destroy the virtual machine and free all memory used by it.

void
zs_vm_destroy (zs_vm_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        zs_vm_t *self = *self_p;
        zstr_free (&self->results);
        zs_pipe_destroy (&self->input);
        zs_pipe_destroy (&self->output);
        while (self->nbr_atomics)
            s_atomic_destroy (&self->atomics [--self->nbr_atomics]);
        free (self->code);
        free (self);
        *self_p = NULL;
    }
}


//  ---------------------------------------------------------------------------
//  Probe atomic to ask it to register itself; we use a self-registration
//  system where all information about an atomic is encapsulated in its
//  source code, rather than spread throughout the codebase. It's valid to
//  probe dictionary at any time.

void
zs_vm_probe (zs_vm_t *self, zs_vm_fn_t *atomic)
{
    self->probing = atomic;
    (self->probing) (self, NULL, NULL);
    self->probing = NULL;
}


//  ---------------------------------------------------------------------------
//  Return true if we're probing dictionary; this tells dictionary to
//  register rather than to run.

bool
zs_vm_probing (zs_vm_t *self)
{
    return self->probing != NULL;
}


//  ---------------------------------------------------------------------------
//  Primitive registers itself with the execution context. This is only valid
//  if zs_vm_probing () is true. Returns 0 if registration worked, -1 if it
//  failed due to an internal error. If hint is NULL, uses same hint as last
//  registered method (this is for aliases).

int
zs_vm_register (zs_vm_t *self, const char *name, zs_type_t type, const char *hint)
{
    assert (self->probing);
    assert (hint || self->nbr_atomics);
    if (hint == NULL)
        hint = self->atomics [self->nbr_atomics - 1]->hint;
    self->atomics [self->nbr_atomics++] = s_atomic_new (self->probing, name, type, hint);
    return 0;
}


//  ---------------------------------------------------------------------------
//  Resolve a function name, return type of function or zs_type_unknown if
//  not defined. Resolves user-defined functions, then atomics, then builtins.

zs_type_t
zs_vm_function_type (zs_vm_t *self, const char *name)
{
    int found;
    if (s_try_function (self, name) != -1)
        return zs_type_greedy;      //  User functions are always greedy
    else
    if ((found = s_try_atomic (self, name)) != -1)
        return self->atomics [found]->type;
    else
    if (s_try_builtin (self, name) != -1)
        return zs_type_strict;      //  Builtins are always strict
    return zs_type_unknown;
}


//  ---------------------------------------------------------------------------
//  Compile a whole number constant into the virtual machine.
//  Whole numbers are stored thus:
//      [VM_WHOLE][8 bytes in host format]

void
zs_vm_compile_whole (zs_vm_t *self, int64_t whole)
{
    self->code [self->code_size++] = VM_WHOLE;
    memcpy (self->code + self->code_size, &whole, sizeof (whole));
    self->code_size += sizeof (whole);
}


//  ---------------------------------------------------------------------------
//  Compile a real number constant into the virtual machine.
//  Whole numbers are stored thus:
//      [VM_REAL][8 bytes in host format]

void
zs_vm_compile_real (zs_vm_t *self, double real)
{
    self->code [self->code_size++] = VM_REAL;
    memcpy (self->code + self->code_size, &real, sizeof (real));
    self->code_size += sizeof (real);
}


//  ---------------------------------------------------------------------------
//  Compile a string constant into the virtual machine.
//  Strings are stored thus:
//      [VM_STRING][null-terminated string]

void
zs_vm_compile_string (zs_vm_t *self, const char *string)
{
    self->code [self->code_size++] = VM_STRING;
    strcpy ((char *) self->code + self->code_size, string);
    self->code_size += strlen (string) + 1;
}


//  ---------------------------------------------------------------------------
//  Compile a new function definition; end with a commit.
//  Functions are stored thus:
//      [VM_GUARD]                  <-- self->code_head
//      [offset]                    Offset to previous, hi/lo 2 bytes
//      [name, null-terminated]
//      [ ... ]                     code
//      [VM_RETURN]

void
zs_vm_compile_define (zs_vm_t *self, const char *name)
{
    assert (!self->checkpoint);
    //  This is provisional on a successful commit
    self->checkpoint = self->code_size;
    //  Store offset to previous function guard, if any
    uint16_t offset = self->code_size - self->code_head;
    self->code [self->code_size++] = VM_GUARD;
    self->code [self->code_size++] = (byte) (offset >> 8);
    self->code [self->code_size++] = (byte) (offset & 0xFF);
    //  Store function name and bump code size
    strcpy ((char *) self->code + self->code_size, name);
    self->code_size += strlen (name) + 1;
}


//  ---------------------------------------------------------------------------
//  Close the current function definition.

void
zs_vm_compile_commit (zs_vm_t *self)
{
    //  We must have an open function definition
    assert (self->checkpoint);
    //  End function with a RETURN operation
    self->code [self->code_size++] = VM_RETURN;
    //  The function is now successfully compiled in the bytecode
    self->code_head = self->checkpoint;
    self->checkpoint = 0;
}


//  ---------------------------------------------------------------------------
//  Cancel the current or last function definition and reset the virtual
//  machine to the state before the previous _define. You can call this
//  repeatedly to delete function definitions until the machine is empty.
//  Returns 0 if OK, -1 if there was no function to rollback (the machine
//  is then empty).

int
zs_vm_compile_rollback (zs_vm_t *self)
{
    int rc = 0;
    if (self->checkpoint) {
        self->code_size = self->checkpoint;
        self->checkpoint = 0;
    }
    else
    if (self->code_head > 0) {
        size_t guard = self->code_head;
        assert (self->code [guard] == VM_GUARD);
        size_t offset = (self->code [guard + 1] << 8) + self->code [guard + 2];
        assert (guard >= offset);
        self->code_head = guard - offset;
        self->code_size = guard;
    }
    else
        rc = -1;

    return rc;
}


//  ---------------------------------------------------------------------------
//  Compile a strict function call; the function gets no input. Returns 0
//  if OK or -1 if the function was not defined.

int
zs_vm_compile_strict (zs_vm_t *self, const char *name)
{
    return s_compile_call (self, VM_STRICT, name);
}


//  ---------------------------------------------------------------------------
//  Compile a modest function call; the function gets a single input value
//  which is the last value produced by the phrase. Returns 0 if OK or -1
//  if the function was not defined.

int
zs_vm_compile_modest (zs_vm_t *self, const char *name)
{
    return s_compile_call (self, VM_MODEST, name);
}


//  ---------------------------------------------------------------------------
//  Compile a greedy function call. The function gets all output produced
//  by the phrase. Returns 0 if OK, or -1 if the function was not defined.

int
zs_vm_compile_greedy (zs_vm_t *self, const char *name)
{
    return s_compile_call (self, VM_GREEDY, name);
}


//  ---------------------------------------------------------------------------
//  Compile a nested function call. The current sentence is stacked and we
//  start a new sentence. The actual function call is executed when we hit
//  the matching unnest. Returns 0 if OK or -1 if the function was not
//  defined.

int
zs_vm_compile_nested (zs_vm_t *self, const char *name)
{
    if (s_try_function (self, name) != -1
    ||  s_try_atomic (self, name) != -1
    ||  s_try_builtin (self, name) != -1) {
        //  We use a scope stack during compilation so it's less work for the
        //  caller, who has the function name now, rather than at closing time.
        self->code [self->code_size++] = VM_NESTED;
        self->scope_stack [self->scope_stack_ptr++] = strdup (name);
        return 0;
    }
    else
        return -1;              //  Not a defined function
}

//  ---------------------------------------------------------------------------
//  Compile an unnest operation; this executes the nested function on the
//  current phrase or sentence.

void
zs_vm_compile_unnest (zs_vm_t *self)
{
    assert (self->scope_stack_ptr);
    char *name = self->scope_stack [--self->scope_stack_ptr];
    s_compile_call (self, VM_UNNEST, name);
    free (name);
}


//  ---------------------------------------------------------------------------
//  Compile end of phrase. This appends the phrase output to the current
//  sentence output, and starts a new phrase.

void
zs_vm_compile_phrase (zs_vm_t *self)
{
    self->code [self->code_size++] = VM_PHRASE;
}


//  ---------------------------------------------------------------------------
//  Compile end of sentence. This prints the sentence output and starts a
//  new sentence.

void
zs_vm_compile_sentence (zs_vm_t *self)
{
    self->code [self->code_size++] = VM_SENTENCE;
}


//  ---------------------------------------------------------------------------
//  Dump VM contents (state and code)

void
zs_vm_dump (zs_vm_t *self)
{
    printf ("Primitives: %zd\n", self->nbr_atomics);
    size_t index;
    for (index = 0; index < self->nbr_atomics; index++)
        printf (" - %s: %s\n", self->atomics [index]->name, self->atomics [index]->hint);
    printf ("Compiled size: %zd\n", self->code_size);
}


//  ---------------------------------------------------------------------------
//  Return latest function by name; use with _prev to iterate through
//  functions. Returns function name or NULL if there are none defined.

const char *
zs_vm_function_first (zs_vm_t *self)
{
    self->iterator = self->code_head;
    return zs_vm_function_next (self);
}


//  ---------------------------------------------------------------------------
//  Return previous function by name; use after a _last to iterate through
//  functions. Returns function name or NULL if there are no more.

const char *
zs_vm_function_next (zs_vm_t *self)
{
    if (self->iterator) {
        assert (self->code [self->iterator] == VM_GUARD);
        const char *name = s_function_name (self, self->iterator);
        size_t offset = (self->code [self->iterator + 1] << 8)
                      +  self->code [self->iterator + 2];
        assert (self->iterator >= offset);
        self->iterator -= offset;
        return name;
    }
    else
        return NULL;
}


//  ---------------------------------------------------------------------------
//  Return first atomic by name; use with _next to iterate through atomics.
//  Returns atomic name or NULL if there are none defined.

const char *
zs_vm_atomic_first (zs_vm_t *self)
{
    self->iterator = 0;
    return zs_vm_atomic_next (self);
}


//  ---------------------------------------------------------------------------
//  Return next atomic by name; use with _first to iterate through atomics.
//  Returns atomic name or NULL if there are no more defined.

const char *
zs_vm_atomic_next (zs_vm_t *self)
{
    if (self->iterator < self->nbr_atomics) {
        const char *name = self->atomics [self->iterator]->name;
        self->iterator++;
        return name;
    }
    else
        return NULL;
}


//  ---------------------------------------------------------------------------
//  Enable tracing of VM compilation and execution.

void
zs_vm_set_verbose (zs_vm_t *self, bool verbose)
{
    self->verbose = verbose;
}


//  ---------------------------------------------------------------------------
//  Run last defined function, if any, in the VM. This continues forever or
//  until the function ends. Returns 0 if stopped successfully, or -1 if
//  stopped due to some error. Each run of the VM starts with clean pipes.

int
zs_vm_run (zs_vm_t *self)
{
    assert (!self->checkpoint);

    //  We call the last function that was defined, which is at code_head.
    //  When this function returns, the VM ends at needle = 0, and stops.
    assert (self->code [0] == VM_STOP);
    size_t needle = s_function_body (self, self->code_head);
    self->call_stack [0] = 0;
    self->call_stack_ptr = 1;
    if (self->verbose)
        printf ("D [%04zd]: run '%s'\n", needle, s_function_name (self, self->code_head));

    //  Clean pipes before each run
    zs_pipe_purge (self->input);
    zs_pipe_purge (self->output);

    //  Run virtual machine until stopped
    while (true) {
        byte opcode = self->code [needle];
        needle++;
        if (opcode < 240) {
            if (self->verbose)
                printf ("D [%04zd]: atomic=%s\n", needle, self->atomics [opcode]->name);
            if ((self->atomics [opcode]->function) (self, self->input, self->output))
                break;
        }
        else
        if (opcode == VM_CALL) {
            self->call_stack [self->call_stack_ptr++] = needle + 2;
            size_t guard = (self->code [needle] << 8) + self->code [needle + 1];
            assert (self->code [guard] == VM_GUARD);
            if (self->verbose)
                printf ("D [%04zd]: call function=%s stack=%zd\n", needle,
                        s_function_name (self, guard), self->call_stack_ptr);
            needle = s_function_body (self, guard);
        }
        else
        if (opcode == VM_RETURN) {
            if (self->verbose)
                printf ("D [%04zd]: return stack=%zd\n", needle, self->call_stack_ptr);
            needle = self->call_stack [--self->call_stack_ptr];
        }
        else
        if (opcode == VM_WHOLE) {
            int64_t whole;
            memcpy (&whole, self->code + needle, sizeof (whole));
            zs_pipe_send_whole (self->output, whole);
            if (self->verbose)
                printf ("D [%04zd]: whole value=%" PRId64 "\n", needle, whole);
            needle += sizeof (whole);
        }
        else
        if (opcode == VM_REAL) {
            double real;
            memcpy (&real, self->code + needle, sizeof (real));
            zs_pipe_send_real (self->output, real);
            if (self->verbose)
                printf ("D [%04zd]: real value=%g\n", needle, real);
            needle += sizeof (real);
        }
        else
        if (opcode == VM_STRING) {
            char *string = (char *) self->code + needle;
            zs_pipe_send_string (self->output, string);
            if (self->verbose)
                printf ("D [%04zd]: string value=%s\n", needle, string);
            needle += strlen (string) + 1;
        }
        else
        if (opcode == VM_STRICT) {
            if (self->verbose)
                printf ("D [%04zd]: strict\n", needle);
            //  Function gets no input; we could pass null, though for
            //  now we pass an empty input pipe
            zs_pipe_purge (self->input);
        }
        else
        if (opcode == VM_MODEST) {
            if (self->verbose)
                printf ("D [%04zd]: modest\n", needle);
            zs_pipe_pull (self->input, self->output, false);
        }
        else
        if (opcode == VM_GREEDY) {
            if (self->verbose)
                printf ("D [%04zd]: greedy\n", needle);
            zs_pipe_pull (self->input, self->output, true);
        }
        else
        if (opcode == VM_NESTED) {
            if (self->verbose)
                printf ("D [%04zd]: nested stack=%zd\n", needle, self->output_stack_ptr);
            self->output_stack [self->output_stack_ptr] = self->output;
            self->output_stack_ptr++;
            self->output = zs_pipe_new ();
            zs_pipe_purge (self->input);
        }
        else
        if (opcode == VM_UNNEST) {
            if (self->verbose)
                printf ("D [%04zd]: unnest stack=%zd\n", needle, self->output_stack_ptr);
            assert (self->output_stack_ptr > 0);
            self->output_stack_ptr--;
            zs_pipe_destroy (&self->input);
            self->input = self->output;
            self->output = self->output_stack [self->output_stack_ptr];
        }
        else
        if (opcode == VM_PHRASE) {
            if (self->verbose)
                printf ("D [%04zd]: phrase\n", needle);
            zs_pipe_mark (self->output);
        }
        else
        if (opcode == VM_SENTENCE) {
            if (self->verbose)
                printf ("D [%04zd]: sentence\n", needle);
            //  TODO: send results to console/actor pipe
            puts (zs_vm_results (self));
        }
        else
        if (opcode == VM_GUARD) {
            if (self->verbose)
                printf ("D [%04zd]: guard\n", needle);
            printf ("E: corrupt VM, aborting\n");
            assert (false);
        }
        else
        if (opcode == VM_STOP) {
            if (self->verbose)
                printf ("D [%04zd]: stop\n", needle);
            break;
        }
    }
    return 0;
}


//  ---------------------------------------------------------------------------
//  Return results as string, after successful execution. Caller must not
//  modify returned value.

const char *
zs_vm_results (zs_vm_t *self)
{
    zstr_free (&self->results);
    self->results = zs_pipe_paste (self->output);
    return self->results;
}


//  ---------------------------------------------------------------------------
//  Selftest

//  These are the atomics we use in the selftest application
static int
s_add (zs_vm_t *self, zs_pipe_t *input, zs_pipe_t *output)
{
    if (zs_vm_probing (self))
        zs_vm_register (self, "add", zs_type_greedy, "Add all the values");
    else {
        int64_t sum = 0;
        while (!zs_pipe_recv (input))
            sum += zs_pipe_whole (input);
        zs_pipe_send_whole (output, sum);
    }
    return 0;
}

static int
s_count (zs_vm_t *self, zs_pipe_t *input, zs_pipe_t *output)
{
    if (zs_vm_probing (self))
        zs_vm_register (self, "count", zs_type_greedy, "Eat and count all the values");
    else {
        int64_t count = 0;
        while (!zs_pipe_recv (input))
            count++;
        zs_pipe_send_whole (output, count);
    }
    return 0;
}

static int
s_assert (zs_vm_t *self, zs_pipe_t *input, zs_pipe_t *output)
{
    if (zs_vm_probing (self))
        zs_vm_register (self, "assert", zs_type_greedy, "Assert first two values are the same");
    else {
        int64_t first = zs_pipe_recv_whole (input);
        int64_t second = zs_pipe_recv_whole (input);
        if (first != second) {
            printf ("E: assertion failed, %" PRId64 " != %" PRId64 "\n", first, second);
            return -1;          //  Destroy the thread
        }
    }
    return 0;
}

static int
s_year (zs_vm_t *self, zs_pipe_t *input, zs_pipe_t *output)
{
    if (zs_vm_probing (self))
        zs_vm_register (self, "year", zs_type_strict, "Tell us what year it is");
    else
        zs_pipe_send_whole (output, 2015);
    return 0;
}


void
zs_vm_test (bool verbose)
{
    printf (" * zs_vm: ");
    if (verbose)
        printf ("\n");

    //  @selftest
    int rc;
    zs_vm_t *vm = zs_vm_new ();
    zs_vm_set_verbose (vm, verbose);

    zs_vm_probe (vm, s_add);
    zs_vm_probe (vm, s_count);
    zs_vm_probe (vm, s_assert);
    zs_vm_probe (vm, s_year);

    //  --------------------------------------------------------------------
    //  sub: (<OK> <Guys> count 2 assert)

    zs_vm_compile_define (vm, "sub");
    zs_vm_compile_string (vm, "OK");
    zs_vm_compile_string (vm, "Guys");
    zs_vm_compile_greedy (vm, "count");
    zs_vm_compile_whole  (vm, 2);
    zs_vm_compile_greedy (vm, "assert");
    zs_vm_compile_commit (vm);

    //  --------------------------------------------------------------------
    //  main: (
    //      123 1000000000 sum 1000000123 assert,
    //      <Hello,> <World> count 2 assert,
    //      add (123 456) 579 assert,
    //      add (123 count (1 2 3)) 126 assert,
    //      year year count 2 assert
    //  )
    zs_vm_compile_define (vm, "main");

    zs_vm_compile_whole  (vm, 123);
    zs_vm_compile_whole  (vm, 1000000000);
    zs_vm_compile_greedy (vm, "add");
    zs_vm_compile_whole  (vm, 1000000123);
    zs_vm_compile_greedy (vm, "assert");
    zs_vm_compile_phrase (vm);

    zs_vm_compile_string (vm, "Hello,");
    zs_vm_compile_string (vm, "World");
    zs_vm_compile_greedy (vm, "count");
    zs_vm_compile_whole  (vm, 2);
    zs_vm_compile_greedy (vm, "assert");
    zs_vm_compile_phrase (vm);

    zs_vm_compile_nested (vm, "add");
    zs_vm_compile_whole  (vm, 123);
    zs_vm_compile_whole  (vm, 456);
    zs_vm_compile_unnest (vm);
    zs_vm_compile_whole  (vm, 579);
    zs_vm_compile_greedy (vm, "assert");
    zs_vm_compile_phrase (vm);

    zs_vm_compile_nested (vm, "add");
    zs_vm_compile_whole  (vm, 123);
    zs_vm_compile_nested (vm, "count");
    zs_vm_compile_whole  (vm, 1);
    zs_vm_compile_whole  (vm, 2);
    zs_vm_compile_whole  (vm, 3);
    zs_vm_compile_unnest (vm);
    zs_vm_compile_unnest (vm);
    zs_vm_compile_whole  (vm, 126);
    zs_vm_compile_greedy (vm, "assert");
    zs_vm_compile_phrase (vm);

    zs_vm_compile_strict (vm, "year");
    zs_vm_compile_strict (vm, "year");
    zs_vm_compile_greedy (vm, "count");
    zs_vm_compile_whole  (vm, 2);
    zs_vm_compile_greedy (vm, "assert");

    zs_vm_compile_commit (vm);

    //  --------------------------------------------------------------------
    //  sub sub main
    zs_vm_compile_define (vm, "go");
    zs_vm_compile_greedy (vm, "sub");
    zs_vm_compile_greedy (vm, "sub");
    zs_vm_compile_greedy (vm, "main");
    zs_vm_compile_commit (vm);
    if (verbose)
        zs_vm_dump (vm);

    zs_vm_run (vm);

    rc = zs_vm_compile_rollback (vm);
    assert (rc == 0);
    zs_vm_run (vm);

    rc = zs_vm_compile_rollback (vm);
    assert (rc == 0);
    zs_vm_run (vm);

    rc = zs_vm_compile_rollback (vm);
    assert (rc == 0);
    zs_vm_run (vm);

    rc = zs_vm_compile_rollback (vm);
    assert (rc == -1);
    zs_vm_run (vm);

    zs_vm_destroy (&vm);
    //  @end
    printf ("OK\n");
}
