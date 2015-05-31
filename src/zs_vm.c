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

    Current limitations:
        - max VM code size is 2^24 (3-byte addresses)
        - max size of a single function is 64k (2-byte offsets)
        - max nesting depth is 256 (asserted)
        - max output stack nesting is 256 (asserted)
        - max function call depth is 256 (asserted)
@end
*/

#define MAX_SCOPE   256     //  Nesting limit at compile time
#define MAX_NEST    256     //  Maximum nest () depth
#define MAX_LOOP    256     //  Maximum loop {} depth
#define MAX_CALLS   256     //  Maximum function call depth

//  Bytecodes
//  - up to 240 class 0 dictionary
//  - 255 + 16 bits = extensions; class (1..n) + function numbe

//  These are built-in opcodes which are allowed to modify the needle, so we
//  can keep it in a register variable here and perhaps save some CPU cycles.
//  Lol. \o/  Put these in order of frequency.
#define VM_CALL         254     //  Call a user function
#define VM_RETURN       253     //  Return to previous needle
#define VM_LOOP         252     //  Open loop
#define VM_XLOOP        251     //  Close loop
#define VM_JUMP         250     //  Jump unconditionally
#define VM_JUMPEX       249     //  Jump if current value not positive
#define VM_WHOLE        248     //  Issue a whole number constant
#define VM_REAL         247     //  Issue a real number constant
#define VM_STRING       246     //  Issue a string constant
#define VM_PIPE         245     //  Execute pipe operation
#define VM_SENTENCE     244     //  End sentence
#define VM_GUARD        241     //  Assert if we ever reach this
#define VM_STOP         240     //  Last built-in

//  These are the pipe operations, managing output and input pipes so that
//  functions what they need. The pipe operation is always compiled after a
//  VM_PIPE opcode.
#define VM_PIPE_NEST    1       //  Prepare nested argument list
#define VM_PIPE_UNNEST  2       //  Prepare nested function call
#define VM_PIPE_SINGLE  3       //  Pull single value from pipe
#define VM_PIPE_MODEST  4       //  Prepare modest function call
#define VM_PIPE_GREEDY  5       //  Prepare greedy function call
#define VM_PIPE_ARRAY   6       //  Prepare array function call
#define VM_PIPE_UNLOOP  7       //  Prepare to call loop function
#define VM_PIPE_MARK    8       //  End phrase

static const char *
pipe_op_name [] = {
    "?", "NEST", "UNNEST", "SINGLE", "MODEST", "GREEDY", "ARRAY", "UNLOOP", "MARK"
};

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

    //  We use this during compile time to match start/end scopes
    size_t scope_stack [MAX_SCOPE]; //  Scope stack, arbitrary size
    size_t scope_stack_ptr;         //  Size of scope stack

    //  The nest stack holds output pipes during nested calls
    zs_pipe_t *nest_stack [MAX_NEST];
    size_t nest_stack_ptr;

    //  The loop stack holds input pipes during loop cycles
    zs_pipe_t *loop_stack [MAX_LOOP];
    size_t loop_stack_ptr;

    //  The call stack is used for actual function calls
    size_t call_stack [MAX_CALLS];
    size_t call_stack_ptr;

    zs_pipe_t *stdin;               //  Input to next function
    zs_pipe_t *stdout;              //  Current phrase output
    zs_pipe_t *loopin;              //  Input to next loop function
    zs_pipe_t *loopout;             //  Output of next loop function
    char *results;                  //  Sentence results, if any
    bool loop_fn;                   //  Call as loop function

    bool verbose;                   //  Trace execution progress
    size_t iterator;                //  For listing functions & atomics
    bool userspace;                 //  True when iterating functions
};

//  Map function address to code body
static size_t
s_function_body (zs_vm_t *self, size_t address)
{
    if (address) {
        size_t body = address + 3;
        body += strlen ((char *) self->code + body) + 1;
        return body;
    }
    else
        return 0;
}

//  Map function address to printable name
static const char *
s_function_name (zs_vm_t *self, size_t address)
{
    if (address)
        return (const char *) self->code + address + 3;
    else
        return "";
}

//  Resolve function name to address, which is:
//  1-253           - built in atomic, compiled as one byte
//  254 + 3 bytes   - VM_CALL + 24-bit function address
//  255 + 3 bytes   - extended atomic address (TBD)
//
//  Resolves to most recent instance of any given function name.
//  Returns 0 if the function name is not defined (0 is not a valid address).

static size_t
s_resolve (zs_vm_t *self, const char *name)
{
    //  Look for a user-defined function from newest to oldest
    size_t address = self->code_head;
    while (address) {
        assert (self->code [address] == VM_GUARD);
        if (streq (name, s_function_name (self, address)))
            return (VM_CALL << 24) + address;
        size_t offset = (self->code [address + 1] << 8) + self->code [address + 2];
        assert (address >= offset);
        address -= offset;
    }
    //  Look for a class zero atomic
    for (address = 0; address < self->nbr_atomics; address++)
        if (streq ((self->atomics [address])->name, name))
            return address;

    //  Look for a built-in
    if (streq (name, "stop"))
        return VM_STOP;

    return 0;
}


//  Compile call to function, atomic, or built-in

static void
s_compile_call (zs_vm_t *self, size_t address, byte pipe_op)
{
    //  A non-zero pipe_op means we muck with the plumbing
    if (pipe_op) {
        self->code [self->code_size++] = VM_PIPE;
        self->code [self->code_size++] = pipe_op;
    }
    if (address < 256)
        self->code [self->code_size++] = (byte) address;
    else {
        //  Store 4 bytes from high to low
        self->code [self->code_size++] = (byte) (address >> 24);
        self->code [self->code_size++] = (byte) (address >> 16);
        self->code [self->code_size++] = (byte) (address >> 8);
        self->code [self->code_size++] = (byte) (address);
        assert (self->code [self->code_size - 4] == VM_CALL);
    }
}

//  Registered as atomic zero, so if we ever try to execute an opcode zero, we
//  come here and kill the machine.

static int
s_halt_error (zs_vm_t *self, zs_pipe_t *input, zs_pipe_t *output)
{
    if (zs_vm_probing (self))
        zs_vm_register (self, "$halt$", zs_type_nullary, "Halt on error");
    else {
        printf ("E: tried to execute zero opcode, halting\n");
        return -1;
    }
    return 0;
}


//  ---------------------------------------------------------------------------
//  Create a new empty virtual machine. Returns the reference if successful,
//  or NULL if construction failed due to lack of available memory.

zs_vm_t *
zs_vm_new (void)
{
    zs_vm_t *self = (zs_vm_t *) zmalloc (sizeof (zs_vm_t));
    if (self) {
        self->stdin = zs_pipe_new ();
        self->stdout = zs_pipe_new ();
        self->loopin = zs_pipe_new ();
        self->loopout = zs_pipe_new ();
        self->code_max = 32000;         //  Arbitrary; TODO: extensible
        self->code = (byte *) malloc (self->code_max);
        self->code [self->code_size++] = VM_STOP;
        zs_vm_probe (self, s_halt_error);
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
        zs_pipe_destroy (&self->stdin);
        zs_pipe_destroy (&self->stdout);
        zs_pipe_destroy (&self->loopin);
        zs_pipe_destroy (&self->loopout);
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
    assert (self->nbr_atomics < 240);
    self->atomics [self->nbr_atomics++] = s_atomic_new (self->probing, name, type, hint);
    return 0;
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
zs_vm_commit (zs_vm_t *self)
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
zs_vm_rollback (zs_vm_t *self)
{
    int rc = 0;
    if (self->checkpoint) {
        self->code_size = self->checkpoint;
        self->checkpoint = 0;
    }
    else
    if (self->code_head > 0) {
        size_t address = self->code_head;
        assert (self->code [address] == VM_GUARD);
        size_t offset = (self->code [address + 1] << 8) + self->code [address + 2];
        assert (address >= offset);
        self->code_head = address - offset;
        self->code_size = address;
    }
    else
        rc = -1;

    return rc;
}


//  ---------------------------------------------------------------------------
//  Compile an inline function call; the actual pipe semantics depend on the
//  type of the function. Returns 0 if successful, -1 if the function is not
//  defined.

int
zs_vm_compile_inline (zs_vm_t *self, const char *name)
{
    //  User-defined functions do not touch the pipes; they're effectively
    //  macros that inline their contents where they are invoked
    size_t address = s_resolve (self, name);
    if (!address)
        return -1;              //  Undefined function, forget it

    byte pipe_op = 0;
    if (address < 256) {
        //  Atomics can need extra work to prepare an input pipe
        if (self->atomics [address]->type == zs_type_modest)
            pipe_op = VM_PIPE_MODEST;
        else
        if (self->atomics [address]->type == zs_type_greedy)
            pipe_op = VM_PIPE_GREEDY;
        else
        if (self->atomics [address]->type == zs_type_array)
            pipe_op = VM_PIPE_ARRAY;
    }
    s_compile_call (self, address, pipe_op);
    return 0;
}


//  ---------------------------------------------------------------------------
//  Open a nested function call. This stacks the current VM output and starts
//  a new output pipe. Returns 0 if OK or -1 if the function is not defined.

int
zs_vm_compile_nest (zs_vm_t *self, const char *name)
{
    size_t address = s_resolve (self, name);
    if (!address)
        return -1;              //  Undefined function, forget it

    self->code [self->code_size++] = VM_PIPE;
    self->code [self->code_size++] = VM_PIPE_NEST;
    assert (self->scope_stack_ptr < MAX_SCOPE);
    self->scope_stack [self->scope_stack_ptr++] = address;
    return 0;
}


//  ---------------------------------------------------------------------------
//  Close a nested function call. This moves the current output to input, and
//  unstacks the saved VM output, then executes the function.

void
zs_vm_compile_xnest (zs_vm_t *self)
{
    assert (self->scope_stack_ptr);
    size_t address = self->scope_stack [--self->scope_stack_ptr];
    s_compile_call (self, address, VM_PIPE_UNNEST);
}


//  ---------------------------------------------------------------------------
//  TBD

void
zs_vm_compile_menu (zs_vm_t *self)
{
    //  VM: pull test value from loop function
    self->code [self->code_size++] = VM_PIPE;
    self->code [self->code_size++] = VM_PIPE_SINGLE;

    //  Stack address of jump address
    //  Leave 24 bits for the jump address, fill with magic
    assert (self->scope_stack_ptr < MAX_SCOPE);
    self->scope_stack [self->scope_stack_ptr++] = self->code_size + 1;
    self->code [self->code_size++] = VM_JUMPEX;
    self->code [self->code_size++] = 0xA5;
    self->code [self->code_size++] = 0xA5;
    self->code [self->code_size++] = 0xA5;
}


//  ---------------------------------------------------------------------------
//  TBD

void
zs_vm_compile_xmenu (zs_vm_t *self)
{
    //  Pop location of jump address
    size_t address = self->scope_stack [--self->scope_stack_ptr];
    assert (self->code [address + 0] == 0xA5);
    assert (self->code [address + 1] == 0xA5);
    assert (self->code [address + 2] == 0xA5);

    //  Store current code_size into jump address
    self->code [address++] = (byte) (self->code_size >> 16);
    self->code [address++] = (byte) (self->code_size >> 8);
    self->code [address++] = (byte) (self->code_size);
}


//  ---------------------------------------------------------------------------
//  Compiles a loop. Caller must provide name of function, which has just run
//  and left its output on stdout: loop state, and loop event.

int
zs_vm_compile_loop (zs_vm_t *self, const char *name)
{
    size_t fn_address = s_resolve (self, name);
    if (!fn_address)
        return -1;              //  Undefined function, forget it

    //  The loop starts here
    //  VM:
    //  - stack loopout
    //  - new loopout, empty loopin
    //  - pipe op GREEDY (stdout -> loopin)
    //  - recv whole event from loopin
    //  - jump to address if event <= 0
    //
    //   - push loop_address for xloop so it can fill in the blanks
    //   - use a magic value A5A5A5 to double-check this code
    assert (self->scope_stack_ptr < MAX_SCOPE);
    self->scope_stack [self->scope_stack_ptr++] = self->code_size + 1;
    self->code [self->code_size++] = VM_LOOP;
    self->code [self->code_size++] = 0xA5;
    self->code [self->code_size++] = 0xA5;
    self->code [self->code_size++] = 0xA5;

    //  Push function address to scope stack for xloop
    assert (self->scope_stack_ptr < MAX_SCOPE);
    self->scope_stack [self->scope_stack_ptr++] = fn_address;

    //  Push body address to scope stack for xloop
    assert (self->scope_stack_ptr < MAX_SCOPE);
    self->scope_stack [self->scope_stack_ptr++] = self->code_size;

    return 0;
}


//  ---------------------------------------------------------------------------
//  TBD
//  We compile xloop into this code:
//  VM:
//  - call loop function (loopin -> loopout)
//  - pipe op GREEDY (loopout -> loopin)
//  - recv whole event from loopin
//  - jump to address if event > 0
//    ... continue

void
zs_vm_compile_xloop (zs_vm_t *self)
{
    //  Pop stacked body address (jump here if loop continues)
    assert (self->scope_stack_ptr);
    size_t body_address = self->scope_stack [--self->scope_stack_ptr];

    //  Pop loop function address (call to reevaluate loop)
    assert (self->scope_stack_ptr);
    size_t fn_address = self->scope_stack [--self->scope_stack_ptr];

    //  Pop stacked loop address (address of VM_LOOP parameter)
    assert (self->scope_stack_ptr);
    size_t loop_address = self->scope_stack [--self->scope_stack_ptr];

    //  VM: execute loop function with unloop pipe semantics
    s_compile_call (self, fn_address, VM_PIPE_UNLOOP);

    //  VM: evaluate loop event and jump to body if positive
    self->code [self->code_size++] = VM_XLOOP;
    self->code [self->code_size++] = (byte) (body_address >> 16);
    self->code [self->code_size++] = (byte) (body_address >> 8);
    self->code [self->code_size++] = (byte) (body_address);

    //  Fix VM_LOOP argument to point to current code_size
    assert (self->code [loop_address + 0] == 0xA5);
    assert (self->code [loop_address + 1] == 0xA5);
    assert (self->code [loop_address + 2] == 0xA5);
    self->code [loop_address++] = (byte) (self->code_size >> 16);
    self->code [loop_address++] = (byte) (self->code_size >> 8);
    self->code [loop_address++] = (byte) (self->code_size);
}


//  ---------------------------------------------------------------------------
//  Compile end of phrase. This appends the phrase output to the current
//  sentence output, and starts a new phrase.

void
zs_vm_compile_phrase (zs_vm_t *self)
{
    self->code [self->code_size++] = VM_PIPE;
    self->code [self->code_size++] = VM_PIPE_MARK;
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
//  Return latest function by name; use with next to iterate through
//  functions. Returns function name or NULL if there are none defined.
//  TODO: hide/mask functions that get redefined.

const char *
zs_vm_function_first (zs_vm_t *self)
{
    self->iterator = self->code_head;
    self->userspace = true;
    return zs_vm_function_next (self);
}


//  ---------------------------------------------------------------------------
//  Return previous function by name; use after first to iterate through
//  functions. Returns function name or NULL if there are no more.

const char *
zs_vm_function_next (zs_vm_t *self)
{
    if (self->userspace) {
        if (self->iterator) {
            assert (self->code [self->iterator] == VM_GUARD);
            const char *name = s_function_name (self, self->iterator);
            size_t offset = (self->code [self->iterator + 1] << 8)
                           + self->code [self->iterator + 2];
            assert (self->iterator >= offset);
            self->iterator -= offset;
            return name;
        }
        else
            self->userspace = false;
    }
    if (self->iterator < self->nbr_atomics) {
        const char *name = self->atomics [self->iterator]->name;
        self->iterator++;
        //  Don't report system functions starting with $
        if (name [0] == '$')
            return zs_vm_function_next (self);
        else
            return name;
    }
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

static size_t
s_decode_address (byte *code)
{
    return (size_t) (code [0] << 16) + (size_t) (code [1] << 8) + (size_t) (code [2]);
}


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
    zs_pipe_purge (self->stdin);
    zs_pipe_purge (self->stdout);

    //  Run virtual machine until stopped or interrupted
    while (!zctx_interrupted) {
        if (self->verbose) {
//          Enable this only when debugging pipes; it creates a lot of output
//             zs_pipe_print (self->stdin, "Stdin:   ");
//             zs_pipe_print (self->stdout, "Stdout:  ");
//             zs_pipe_print (self->loopin, "Loopin:  ");
//             zs_pipe_print (self->loopout, "Loopout: ");
            printf ("D [%04zd]: ", needle);
        }
        byte opcode = self->code [needle++];
        if (opcode < 240) {
            if (self->verbose)
                printf ("atomic=%s\n", self->atomics [opcode]->name);
            if ((self->atomics [opcode]->function) (self,
                self->loop_fn? self->loopin: self->stdin,
                self->loop_fn? self->loopout: self->stdout))
                break;
            self->loop_fn = false;
        }
        else
        if (opcode == VM_CALL) {
            //  Address is in next 3 bytes
            size_t address = s_decode_address (self->code + needle);
            needle += 3;
            assert (self->code [address] == VM_GUARD);
            if (self->verbose)
                printf ("CALL function=%s address=%zd stack=%zd\n",
                        s_function_name (self, address), address, self->call_stack_ptr);
            assert (self->call_stack_ptr < MAX_CALLS);
            self->call_stack [self->call_stack_ptr++] = needle;
            needle = s_function_body (self, address);
        }
        else
        if (opcode == VM_RETURN) {
            if (self->verbose)
                printf ("RETURN stack=%zd\n", self->call_stack_ptr);
            needle = self->call_stack [--self->call_stack_ptr];
        }
        else
        if (opcode == VM_LOOP) {
            //  - stack loopin, create new loopin
            //  - pipe op GREEDY (stdout -> loopin)
            //  - recv event from loopin (state remains on loopin)
            //  - jump to address if event <= 0
            self->loop_stack [self->loop_stack_ptr++] = self->loopin;
            self->loopin = zs_pipe_new ();
            //  Get last phrase into loopin pipe
            zs_pipe_pull_greedy (self->loopin, self->stdout);
            //  Get event and jump if false
            int64_t event = zs_pipe_recv_whole (self->loopin);
            if (self->verbose)
                printf ("LOOP event=%" PRId64 "\n", event);
            if (event > 0)
                needle += 3;        //  Skip jump address
            else
                needle = s_decode_address (self->code + needle);
        }
        else
        if (opcode == VM_XLOOP) {
            //  - pipe op GREEDY (loopout -> loopin)
            //  - recv event from loopin
            //  - jump to address if event > 0
            //  - destroy loopin and pop saved loopin
            //  Get last phrase into loopin pipe
            zs_pipe_pull_greedy (self->loopin, self->loopout);
            //  Get event and jump if true
            int64_t event = zs_pipe_recv_whole (self->loopin);
            if (self->verbose)
                printf ("XLOOP event=%" PRId64 "\n", event);
            if (event > 0)
                needle = s_decode_address (self->code + needle);
            else {
                needle += 3;        //  Skip jump address
                //  Restore previous loopin pipe
                assert (self->loop_stack_ptr > 0);
                zs_pipe_destroy (&self->loopin);
                self->loopin = self->loop_stack [--self->loop_stack_ptr];
            }
        }
        else
        if (opcode == VM_JUMP) {
            //  Jump unconditionally
            needle = s_decode_address (self->code + needle);
            if (self->verbose)
                printf ("JUMP address=%zd\n", needle);
        }
        else
        if (opcode == VM_JUMPEX) {
            //  We expect test value on input pipe
            int64_t event = zs_pipe_recv_whole (self->stdin);
            if (self->verbose)
                printf ("JUMPEX event=%" PRId64 "\n", event);
            //  Jump if next input value is zero or negative
            if (event > 0)
                needle += 3;        //  Skip jump address
            else
                needle = s_decode_address (self->code + needle);
        }
        else
        if (opcode == VM_WHOLE) {
            int64_t whole;
            memcpy (&whole, self->code + needle, sizeof (whole));
            zs_pipe_send_whole (self->stdout, whole);
            if (self->verbose)
                printf ("WHOLE value=%" PRId64 "\n", whole);
            needle += sizeof (whole);
        }
        else
        if (opcode == VM_REAL) {
            double real;
            memcpy (&real, self->code + needle, sizeof (real));
            zs_pipe_send_real (self->stdout, real);
            if (self->verbose)
                printf ("REAL value=%g\n", real);
            needle += sizeof (real);
        }
        else
        if (opcode == VM_STRING) {
            char *string = (char *) self->code + needle;
            zs_pipe_send_string (self->stdout, string);
            if (self->verbose)
                printf ("STRING value=%s\n", string);
            needle += strlen (string) + 1;
        }
        else
        if (opcode == VM_PIPE) {
            //  Later we'll rewrite the pipe API to use fixed allocations inside
            //  the VM. The current design makes it easy to develop the language.
            byte pipe_op = self->code [needle++];
            if (self->verbose)
                printf ("PIPE op=%s\n", pipe_op_name [pipe_op]);

            switch (pipe_op) {
                case VM_PIPE_NEST:
                    assert (self->nest_stack_ptr < MAX_NEST);
                    self->nest_stack [self->nest_stack_ptr++] = self->stdout;
                    self->stdout = zs_pipe_new ();
                    break;
                case VM_PIPE_UNNEST:
                    assert (self->nest_stack_ptr > 0);
                    zs_pipe_destroy (&self->stdin);
                    self->stdin = self->stdout;
                    self->stdout = self->nest_stack [--self->nest_stack_ptr];
                    break;
                case VM_PIPE_SINGLE:
                    zs_pipe_pull_single (self->stdin, self->stdout);
                    break;
                case VM_PIPE_MODEST:
                    zs_pipe_pull_modest (self->stdin, self->stdout);
                    break;
                case VM_PIPE_GREEDY:
                    zs_pipe_pull_greedy (self->stdin, self->stdout);
                    break;
                case VM_PIPE_ARRAY:
                    zs_pipe_pull_array (self->stdin, self->stdout);
                    break;
                case VM_PIPE_UNLOOP:
                    zs_pipe_purge (self->loopout);
                    self->loop_fn = true;
                    break;
                case VM_PIPE_MARK:
                    zs_pipe_mark (self->stdout);
                    break;
            }
        }
        else
        if (opcode == VM_SENTENCE) {
            if (self->verbose)
                printf ("SENTENCE\n");
            //  TODO: send results to console/actor pipe
            //  For now zs_repl grabs results via the zs_vm_results call
        }
        else
        if (opcode == VM_GUARD) {
            if (self->verbose)
                printf ("GUARD\n");
            printf ("E: corrupt VM, aborting\n");
            assert (false);
        }
        else
        if (opcode == VM_STOP) {
            if (self->verbose)
                printf ("STOP\n");
            break;
        }
        else {
            printf ("ERROR opcode=%d\n", opcode);
            break;
        }
    }
    return 0;
}


//  ---------------------------------------------------------------------------
//  Return results as string, after successful execution. Caller must not
//  modify returned value.
//  TODO: deprected, to be replaced by sending to pipe on sentence

const char *
zs_vm_results (zs_vm_t *self)
{
    zstr_free (&self->results);
    self->results = zs_pipe_paste (self->stdout);
    return self->results;
}


//  ---------------------------------------------------------------------------
//  Selftest

//  These are the atomics we use in the selftest application
static int
s_sum (zs_vm_t *self, zs_pipe_t *input, zs_pipe_t *output)
{
    if (zs_vm_probing (self))
        zs_vm_register (self, "sum", zs_type_greedy, "Add up all the values");
    else {
        int64_t sum = 0;
        while (zs_pipe_recv (input))
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
        while (zs_pipe_recv (input))
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
            assert (false);
        }
    }
    return 0;
}

static int
s_year (zs_vm_t *self, zs_pipe_t *input, zs_pipe_t *output)
{
    if (zs_vm_probing (self))
        zs_vm_register (self, "year", zs_type_nullary, "Tell us what year it is");
    else
        zs_pipe_send_whole (output, 2015);
    return 0;
}

static int
s_times (zs_vm_t *self, zs_pipe_t *input, zs_pipe_t *output)
{
    if (zs_vm_probing (self))
        zs_vm_register (self, "times", zs_type_modest, "Loop N times");
    else {
        zs_pipe_mark (output);
        int64_t value = zs_pipe_recv_whole (input);
        if (value > 0) {
            //  Send loop event 1 = continue loop
            zs_pipe_send_whole (output, 1);
            //  Send loop state = our next counter
            zs_pipe_send_whole (output, value - 1);
        }
        else
            //  Send loop event 0 = end loop
            zs_pipe_send_whole (output, 0);
    }
    return 0;
}


void
zs_vm_test (bool verbose)
{
    printf (" * zs_vm: ");
    if (verbose)
        printf ("\n");

    //  @selftest
    zs_vm_t *vm = zs_vm_new ();
    zs_vm_set_verbose (vm, verbose);

    zs_vm_probe (vm, s_sum);
    zs_vm_probe (vm, s_count);
    zs_vm_probe (vm, s_assert);
    zs_vm_probe (vm, s_year);
    zs_vm_probe (vm, s_times);

    //  --------------------------------------------------------------------
    //  sub: (<OK> <Guys> count 2 assert)

    zs_vm_compile_define (vm, "sub");
    zs_vm_compile_string (vm, "OK");
    zs_vm_compile_string (vm, "Guys");
    zs_vm_compile_inline (vm, "count");
    zs_vm_compile_whole  (vm, 2);
    zs_vm_compile_inline (vm, "assert");
    zs_vm_commit (vm);

    //  --------------------------------------------------------------------
    //  main: (
    //      123 1000000000 sum 1000000123 assert,
    //      <Hello,> <World> count 2 assert,
    //      sum (123 456) 579 assert,
    //      sum (123 count (1 2 3)) 126 assert,
    //      year year count 2 assert
    //  )

    zs_vm_compile_define (vm, "main");

    zs_vm_compile_whole  (vm, 123);
    zs_vm_compile_whole  (vm, 1000000000);
    zs_vm_compile_inline (vm, "sum");
    zs_vm_compile_whole  (vm, 1000000123);
    zs_vm_compile_inline (vm, "assert");
    zs_vm_compile_phrase (vm);

    zs_vm_compile_string (vm, "Hello,");
    zs_vm_compile_string (vm, "World");
    zs_vm_compile_inline (vm, "count");
    zs_vm_compile_whole  (vm, 2);
    zs_vm_compile_inline (vm, "assert");
    zs_vm_compile_phrase (vm);

    zs_vm_compile_nest   (vm, "sum");
    zs_vm_compile_whole  (vm, 123);
    zs_vm_compile_whole  (vm, 456);
    zs_vm_compile_xnest  (vm);
    zs_vm_compile_whole  (vm, 579);
    zs_vm_compile_inline (vm, "assert");
    zs_vm_compile_phrase (vm);

    zs_vm_compile_nest   (vm, "sum");
    zs_vm_compile_whole  (vm, 123);
    zs_vm_compile_nest   (vm, "count");
    zs_vm_compile_whole  (vm, 1);
    zs_vm_compile_whole  (vm, 2);
    zs_vm_compile_whole  (vm, 3);
    zs_vm_compile_xnest  (vm);
    zs_vm_compile_xnest  (vm);
    zs_vm_compile_whole  (vm, 126);
    zs_vm_compile_inline (vm, "assert");
    zs_vm_compile_phrase (vm);

    zs_vm_compile_inline (vm, "year");
    zs_vm_compile_inline (vm, "year");
    zs_vm_compile_inline (vm, "count");
    zs_vm_compile_whole  (vm, 2);
    zs_vm_compile_inline (vm, "assert");

    zs_vm_commit (vm);

    //  --------------------------------------------------------------------
    //  menu: (0 [ <hello> ] -1 [ <world> ] count 1 assert)

    zs_vm_compile_define (vm, "menu");
    zs_vm_compile_whole  (vm, 1);
    zs_vm_compile_menu   (vm);
    zs_vm_compile_string (vm, "hello");
    zs_vm_compile_xmenu  (vm);
    zs_vm_compile_whole  (vm, 0);
    zs_vm_compile_menu   (vm);
    zs_vm_compile_string (vm, "world");
    zs_vm_compile_xmenu  (vm);
    zs_vm_compile_inline (vm, "count");
    zs_vm_compile_whole  (vm, 1);
    zs_vm_compile_inline (vm, "assert");
    zs_vm_commit (vm);

    //  --------------------------------------------------------------------
    //  loop: (3 times { <hello> } count 3 assert )

    zs_vm_compile_define (vm, "loop");
    zs_vm_compile_whole  (vm, 3);
    zs_vm_compile_inline (vm, "times");
    zs_vm_compile_loop   (vm, "times");
    zs_vm_compile_string (vm, "hello");
    zs_vm_compile_xloop  (vm);
    zs_vm_compile_inline (vm, "count");
    zs_vm_compile_whole  (vm, 3);
    zs_vm_compile_inline (vm, "assert");
    zs_vm_commit (vm);

    //  --------------------------------------------------------------------
    //  go: (sub main menu)
    zs_vm_compile_define (vm, "go");
    zs_vm_compile_inline (vm, "sub");
    zs_vm_compile_inline (vm, "main");
    zs_vm_compile_inline (vm, "menu");
    zs_vm_compile_inline (vm, "loop");
    zs_vm_commit (vm);
    if (verbose)
        zs_vm_dump (vm);

    //  Unwind the VM, to check that it's sane (don't do this at home)
    size_t nbr_functions = 0;
    while (true) {
        zs_vm_run (vm);
        if (zs_vm_rollback (vm))
            break;
        nbr_functions++;
    }
    assert (nbr_functions == 5);

    zs_vm_destroy (&vm);
    //  @end
    printf ("OK\n");
}
