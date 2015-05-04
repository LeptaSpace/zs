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
    bytecode virtual machine. Actual parsing is done by zs_core and zs_lex.
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
        - decoding costs are significant
        - handled by if/switch in core interpreter
        - able to modify instruction pointer (needle)
    - 0..239 are class 0 primitives
        - no class name
        - assumed to be most commonly used
        - core runtime for ZeroScript machines
        - easy to extend by modifying codebase
        - decoding costs are very low
    - 255 + n are higher class primitives
        - naming proposal is class.function
        - classes are numbered by VM 1..n
        - assumed to change externally
        - designed to be added dynamically
        - decoding costs are insignificant

    TODO:
        - drop last function for interactive use
        - allow extension classes
        - way to signal "thread exception" to caller
@end
*/

//  Bytecodes
//  - up to 240 class 0 dictionary
//  - 255 + 16 bits = extensions; class (1..n) + primitive

//  These are built-in opcodes which are allowed to modify the needle, so we
//  can keep it in a register variable here and perhaps save some CPU cycles.
//  Lol. \o/  Put these in order of frequency.
#define VM_CALL         254     //  Call a user function
#define VM_RETURN       253     //  Return to previous needle
#define VM_NUMBER       252     //  Issue a number constant
#define VM_STRING       251     //  Issue a string constant
#define VM_JOIN         250     //  Join input to output pipe
#define VM_OPEN         249     //  Open new output pipe
#define VM_CLOSE        248     //  Return to previous output pipe
#define VM_GUARD        241     //  Assert if we ever reach this
#define VM_STOP         240     //  Last built-in

#include "zs_classes.h"

//  Work with primitives

typedef struct {
    zs_vm_fn_t *function;           //  Native C function
    char *name;                     //  Primitive name
    char *hint;                     //  Hint to user
} s_primitive_t;

static s_primitive_t *
s_primitive_new (const char *name, const char *hint, zs_vm_fn_t *function)
{
    s_primitive_t *self = (s_primitive_t *) zmalloc (sizeof (s_primitive_t));
    assert (self);
    self->name = strdup (name);
    self->hint = strdup (hint);
    self->function = function;
    return self;
}

static void
s_primitive_destroy (s_primitive_t **self_p)
{
    s_primitive_t *self = *self_p;
    if (self) {
        free (self->name);
        free (self->hint);
        free (self);
    }
}

//  Structure of our class

struct _zs_vm_t {
    s_primitive_t *class0 [240];    //  Class 0 primitives
    size_t class0_size;             //  Number defined so far
    zs_vm_fn_t *probing;            //  Primitive during registration

    byte *code;                     //  Compiled bytecode (zchunk?)
    size_t code_max;                //  Allocated memory
    size_t code_size;               //  Actual amount used
    size_t last_guard;              //  Last defined function guard
    size_t main;                    //  VM main function, if any
    bool defining;                  //  Are we defining a function?

    zs_pipe_t *pipe_stack [256];    //  Pipe stack, arbitrary size
    size_t pipe_stack_ptr;          //  Size of pipe stack
    zs_pipe_t *input;               //  Current input pipe
    zs_pipe_t *output;              //  Current output pipe

    size_t call_stack [256];        //  Function call stack
    size_t call_stack_ptr;          //  Size of pipe stack

    bool verbose;                   //  Trace execution progress
};


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
        self->code_max = 32000;
        self->code = malloc (self->code_max);

        //  When main function returns, VM will stop
        self->call_stack [self->call_stack_ptr++] = 0;
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
        //  Destroy current input/output pipes
        zs_pipe_destroy (&self->input);
        zs_pipe_destroy (&self->output);
        //  Destroy all defined primitives
        while (self->class0_size)
            s_primitive_destroy (&self->class0 [--self->class0_size]);
        //  Destroy VM code block
        free (self->code);
        free (self);
        *self_p = NULL;
    }
}


//  ---------------------------------------------------------------------------
//  Probe primitive to ask it to register itself; we use a self-registration
//  system where all information about a primitive is encapsulated in its
//  source code, rather than spread throughout the codebase. It's valid to
//  probe dictionary at any time.

void
zs_vm_probe (zs_vm_t *self, zs_vm_fn_t *primitive)
{
    self->probing = primitive;
    (self->probing) (self);
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
//  failed due to an internal error.

int
zs_vm_register (zs_vm_t *self, const char *name, const char *hint)
{
    assert (self->probing);
    self->class0 [self->class0_size++] = s_primitive_new (name, hint, self->probing);
    return 0;
}


//  ---------------------------------------------------------------------------
//  Compile a number constant into the virtual machine.
//  Numbers are stored thus:
//      [VM_NUMBER][8 bytes in host format]

void
zs_vm_compile_number (zs_vm_t *self, int64_t number)
{
    self->code [self->code_size++] = VM_NUMBER;
    memcpy (self->code + self->code_size, &number, 8);
    self->code_size += 8;
}


//  ---------------------------------------------------------------------------
//  Compile a string constant into the virtual machine.
//  Strings are stored thus:
//      [VM_STRING][null-terminated string]
//  TODO: add 2-byte length to eliminate strlen calculation

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
//      [VM_GUARD]                  <-- self->last_guard
//      [offset]                    Offset to previous, hi/lo 2 bytes
//      [name, null-terminated]
//      [ ... ]                     code
//      [VM_RETURN]

void
zs_vm_compile_define (zs_vm_t *self, const char *name)
{
    assert (!self->defining);
    self->defining = true;
    uint16_t offset = self->code_size - self->last_guard;
    self->last_guard = self->code_size;
    self->code [self->code_size++] = VM_GUARD;
    self->code [self->code_size++] = (byte) (offset >> 8);
    self->code [self->code_size++] = (byte) (offset & 0xFF);
    strcpy ((char *) self->code + self->code_size, name);
    self->code_size += strlen (name) + 1;
}


//  ---------------------------------------------------------------------------
//  Close the current function definition.

void
zs_vm_compile_commit (zs_vm_t *self)
{
    //  Check that we're in a function definition
    assert (self->defining);
    self->code [self->code_size++] = VM_RETURN;
    self->defining = false;
    //  This should be simplified
    self->main = self->last_guard + 3;
    self->main += strlen ((char *) self->code + self->main) + 1;
}


//  ---------------------------------------------------------------------------
//  Compile a function call; returns 0 if OK, -1 if name is not known.

int
zs_vm_compile_invoke (zs_vm_t *self, const char *name)
{
    if (streq (name, "stop")) {
        self->code [self->code_size++] = VM_STOP;
        return 0;
    }
    //  1. Check if name is a user-defined function
    size_t guard = self->last_guard;
    while (guard) {
        assert (self->code [guard] == VM_GUARD);
        if (streq (name, (char *) self->code + guard + 3)) {
            self->code [self->code_size++] = VM_CALL;
            self->code [self->code_size++] = (byte) (guard >> 8);
            self->code [self->code_size++] = (byte) (guard & 0xFF);
            return 0;
        }
        size_t offset = (self->code [guard + 1] << 8) + self->code [guard + 2];
        assert (guard >= offset);
        guard -= offset;
    }
    //  2. Check if name is a primitive
    size_t index;
    for (index = 0; index < self->class0_size; index++) {
        if (streq ((self->class0 [index])->name, name)) {
            self->code [self->code_size++] = index;
            return 0;
        }
    }
    return -1;                  //  Not known
}


//  ---------------------------------------------------------------------------
//  Compile a join scope operation;

void
zs_vm_compile_join (zs_vm_t *self)
{
    self->code [self->code_size++] = VM_JOIN;
}


//  ---------------------------------------------------------------------------
//  Compile an open scope operation;

void
zs_vm_compile_open (zs_vm_t *self)
{
    self->code [self->code_size++] = VM_OPEN;
}


//  ---------------------------------------------------------------------------
//  Compile a close scope operation;

void
zs_vm_compile_close (zs_vm_t *self)
{
    self->code [self->code_size++] = VM_CLOSE;
}


//  ---------------------------------------------------------------------------
//  Return input pipe for the execution context

zs_pipe_t *
zs_vm_input (zs_vm_t *self)
{
    return self->input;
}


//  ---------------------------------------------------------------------------
//  Return output pipe for the execution context

zs_pipe_t *
zs_vm_output (zs_vm_t *self)
{
    return self->output;
}


//  ---------------------------------------------------------------------------
//  Dump VM contents (state and code)

void
zs_vm_dump (zs_vm_t *self)
{
    printf ("Primitives: %zd\n", self->class0_size);
    size_t index;
    for (index = 0; index < self->class0_size; index++)
        printf (" - %s: %s\n", self->class0 [index]->name, self->class0 [index]->hint);
    printf ("Compiled size: %zd\n", self->code_size);
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
//  stopped due to some error.

int
zs_vm_run (zs_vm_t *self)
{
    assert (!self->defining);

    //  Run virtual machine until stopped
    size_t needle = self->main;
    while (true) {
        byte opcode = self->code [needle];
        needle++;
        if (opcode < 240) {
            if (self->verbose)
                printf ("D [%04zd]: primitive=%s\n", needle, self->class0 [opcode]->name);
            if ((self->class0 [opcode]->function) (self))
                break;
        }
        else
        if (opcode == VM_CALL) {
            self->call_stack [self->call_stack_ptr++] = needle + 2;
            size_t guard = (self->code [needle] << 8) + self->code [needle + 1];
            assert (self->code [guard] == VM_GUARD);
            char *name = (char *) self->code + guard + 3;
            if (self->verbose)
                printf ("D [%04zd]: call function=%s stack=%zd\n", needle,
                        name, self->call_stack_ptr);
            needle = guard + 4 + strlen (name);
        }
        else
        if (opcode == VM_RETURN) {
            if (self->verbose)
                printf ("D [%04zd]: return stack=%zd\n", needle, self->call_stack_ptr);
            needle = self->call_stack [--self->call_stack_ptr];
        }
        else
        if (opcode == VM_NUMBER) {
            int64_t number;
            memcpy (&number, self->code + needle, 8);
            if (self->verbose)
                printf ("D [%04zd]: number value=%" PRId64 "\n", needle, number);
            needle += 8;
            zs_pipe_put_number (self->output, number);
        }
        else
        if (opcode == VM_STRING) {
            char *string = (char *) self->code + needle;
            zs_pipe_put_string (self->output, string);
            if (self->verbose)
                printf ("D [%04zd]: string value=%s\n", needle, string);
            needle += strlen (string) + 1;
        }
        else
        if (opcode == VM_JOIN) {
            if (self->verbose)
                printf ("D [%04zd]: join\n", needle);
            zs_pipe_destroy (&self->input);
            self->input = self->output;
            self->output = zs_pipe_new ();
        }
        else
        if (opcode == VM_OPEN) {
            if (self->verbose)
                printf ("D [%04zd]: open stack=%zd\n", needle, self->pipe_stack_ptr);
            self->pipe_stack [self->pipe_stack_ptr] = self->output;
            self->output = zs_pipe_new ();
            zs_pipe_purge (self->input);
            self->pipe_stack_ptr++;
        }
        else
        if (opcode == VM_CLOSE) {
            if (self->verbose)
                printf ("D [%04zd]: close stack=%zd\n", needle, self->pipe_stack_ptr);
            assert (self->pipe_stack_ptr > 0);
            self->pipe_stack_ptr--;
            zs_pipe_destroy (&self->input);
            self->input = self->output;
            self->output = self->pipe_stack [self->pipe_stack_ptr];
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
//  Selftest

//  These are the primitives we use in the selftest application
static int
s_sum (zs_vm_t *self)
{
    if (zs_vm_probing (self))
        zs_vm_register (self, "sum", "Add all the values");
    else {
        int64_t sum = 0;
        while (zs_pipe_size (zs_vm_input (self)) > 0)
            sum += zs_pipe_get_number (zs_vm_input (self));
        zs_pipe_put_number (zs_vm_output (self), sum);
    }
    return 0;
}

static int
s_count (zs_vm_t *self)
{
    if (zs_vm_probing (self))
        zs_vm_register (self, "count", "Count how many values there are");
    else
        zs_pipe_put_number (zs_vm_output (self), zs_pipe_size (zs_vm_input (self)));
    return 0;
}

static int
s_clear (zs_exec_t *self)
{
    if (zs_exec_probing (self))
        zs_exec_register (self, "clear", "Clear output values");
    else
        ;   //  This is a no-op
    return 0;
}

static int
s_assert (zs_vm_t *self)
{
    if (zs_vm_probing (self))
        zs_vm_register (self, "assert", "Assert first two values are the same");
    else {
        int64_t first = zs_pipe_get_number (zs_vm_input (self));
        int64_t second = zs_pipe_get_number (zs_vm_input (self));
        if (first != second) {
            printf ("E: assertion failed, %" PRId64 " != %" PRId64 "\n", first, second);
            return -1;          //  Destroy the thread
        }
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

    //  --------------------------------------------------------------------
    //  sub: (<OK> <Guys> count 2 assert)

    zs_vm_compile_define (vm, "sub");
    zs_vm_compile_string (vm, "OK");
    zs_vm_compile_string (vm, "Guys");
    zs_vm_compile_join (vm);
    zs_vm_compile_invoke (vm, "count");
    zs_vm_compile_number (vm, 2);
    zs_vm_compile_join (vm);
    zs_vm_compile_invoke (vm, "assert");
    zs_vm_compile_commit (vm);

    //  --------------------------------------------------------------------
    //  main: (
    //      123 1000000000 sum 1000000123 assert
    //      <Hello,> <World> count 2 assert
    //      sum (123 456) 579 assert
    //      sum (123 count (1 2 3)) 126 assert
    //  )
    zs_vm_compile_define (vm, "main");

    zs_vm_compile_number (vm, 123);
    zs_vm_compile_number (vm, 1000000000);
    zs_vm_compile_join (vm);
    zs_vm_compile_invoke (vm, "sum");
    zs_vm_compile_number (vm, 1000000123);
    zs_vm_compile_join (vm);
    zs_vm_compile_invoke (vm, "assert");

    zs_vm_compile_string (vm, "Hello,");
    zs_vm_compile_string (vm, "World");
    zs_vm_compile_join (vm);
    zs_vm_compile_invoke (vm, "count");
    zs_vm_compile_number (vm, 2);
    zs_vm_compile_join (vm);
    zs_vm_compile_invoke (vm, "assert");

    zs_vm_compile_open (vm);
    zs_vm_compile_number (vm, 123);
    zs_vm_compile_number (vm, 456);
    zs_vm_compile_close (vm);
    zs_vm_compile_invoke (vm, "sum");
    zs_vm_compile_number (vm, 579);
    zs_vm_compile_join (vm);
    zs_vm_compile_invoke (vm, "assert");

    zs_vm_compile_open (vm);
    zs_vm_compile_number (vm, 123);
    zs_vm_compile_open (vm);
    zs_vm_compile_number (vm, 1);
    zs_vm_compile_number (vm, 2);
    zs_vm_compile_number (vm, 3);
    zs_vm_compile_close (vm);
    zs_vm_compile_invoke (vm, "count");
    zs_vm_compile_close (vm);
    zs_vm_compile_invoke (vm, "sum");
    zs_vm_compile_number (vm, 126);
    zs_vm_compile_join (vm);
    zs_vm_compile_invoke (vm, "assert");
    zs_vm_compile_commit (vm);

    //  --------------------------------------------------------------------
    //  sub sub main
    zs_vm_compile_define (vm, "go");
    zs_vm_compile_join (vm);
    zs_vm_compile_invoke (vm, "sub");
    zs_vm_compile_join (vm);
    zs_vm_compile_invoke (vm, "sub");
    zs_vm_compile_join (vm);
    zs_vm_compile_invoke (vm, "main");
    zs_vm_compile_commit (vm);
    if (verbose)
        zs_vm_dump (vm);
    zs_vm_run (vm);

    zs_vm_destroy (&vm);
    //  @end
    printf ("OK\n");
}
