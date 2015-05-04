/*  =========================================================================
    zs_exec - ZeroScript execution context

    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of the ZeroScript language, http://zeroscript.org.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

/*
@header
    The execution context provides input and output pipes to functions.
@discuss
@end
*/

#include "zs_classes.h"

//  Structure of our class

struct _zs_exec_t {
    zs_pipe_t *input;
    zs_pipe_t *output;
    zs_call_t *primitive;
    bool probing;
    zhash_t *primitives;            //  Cheap and cheerful dictionary
    uint stack_ptr;                 //  Current size of stack
    zs_call_t *call_stack [256];    //  Call stack
    zs_pipe_t *pipe_stack [256];    //  Pipe stack
};


//  ---------------------------------------------------------------------------
//  Create a new zs_exec, return the reference if successful, or NULL
//  if construction failed due to lack of available memory.

zs_exec_t *
zs_exec_new (void)
{
    zs_exec_t *self = (zs_exec_t *) zmalloc (sizeof (zs_exec_t));
    if (self) {
        self->input = zs_pipe_new ();
        self->output = zs_pipe_new ();
        self->primitives = zhash_new ();
    }
    return self;
}


//  ---------------------------------------------------------------------------
//  Destroy the zs_exec and free all memory used by the object.

void
zs_exec_destroy (zs_exec_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        zs_exec_t *self = *self_p;
        zs_pipe_destroy (&self->input);
        zs_pipe_destroy (&self->output);
        zhash_destroy (&self->primitives);
        free (self);
        *self_p = NULL;
    }
}


//  ---------------------------------------------------------------------------
//  Return true if we're probing primitives; this tells primitives to
//  register rather than to run.

bool
zs_exec_probing (zs_exec_t *self)
{
    return self->probing;
}


//  ---------------------------------------------------------------------------
//  Probe primitive to ask it to register itself; we use a self-registration
//  system where all information about a primitive is encapsulated in its
//  source code, rather than spread throughout the codebase. It's valid to
//  probe primitives at any time.

void
zs_exec_probe (zs_exec_t *self, zs_call_t *primitive)
{
    self->probing = true;
    self->primitive = primitive;
    (primitive) (self);
    self->probing = false;
    self->primitive = NULL;
}


//  ---------------------------------------------------------------------------
//  Primitive registers itself with the execution context. This is only valid
//  if zs_exec_probing () is true. Returns 0 if registration worked, -1 if it
//  failed due to an internal error.

int
zs_exec_register (zs_exec_t *self, const char *name, const char *hint)
{
    assert (self->probing);
    assert (self->primitive);
    if (zhash_lookup (self->primitives, name))
        return -1;              //  Duplicate name
    zhash_insert (self->primitives, name, self->primitive);
    return 0;
}


//  ---------------------------------------------------------------------------
//  Return input pipe for the execution context

zs_pipe_t *
zs_exec_input (zs_exec_t *self)
{
    return self->input;
}


//  ---------------------------------------------------------------------------
//  Return output pipe for the execution context

zs_pipe_t *
zs_exec_output (zs_exec_t *self)
{
    return self->output;
}


//  ---------------------------------------------------------------------------
//  Reset the pipes, so whatever follows has empty input and output pipes.

void
zs_exec_reset (zs_exec_t *self)
{
    zs_pipe_purge (self->input);
    zs_pipe_purge (self->output);
}


//  ---------------------------------------------------------------------------
//  Resolve a function by name, return function address, or NULL

zs_call_t *
zs_exec_resolve (zs_exec_t *self, const char *name)
{
    return (zs_call_t *) zhash_lookup (self->primitives, name);
}


//  ---------------------------------------------------------------------------
//  Execute an inline function without value list, passing current output
//  pipe to the function as input, and offering a new output pipe.

void
zs_exec_inline (zs_exec_t *self, zs_call_t *function)
{
    zs_pipe_destroy (&self->input);
    self->input = self->output;
    self->output = zs_pipe_new ();
    (function) (self);
}


//  ---------------------------------------------------------------------------
//  Open new execution scope for a complex function (with a value list)

void
zs_exec_open (zs_exec_t *self, zs_call_t *function)
{
    //  Save output pipe and create new empty output and input pipes
    self->pipe_stack [self->stack_ptr] = self->output;
    self->output = zs_pipe_new ();
    zs_pipe_purge (self->input);
    self->call_stack [self->stack_ptr] = function;
    self->stack_ptr++;
}


//  ---------------------------------------------------------------------------
//  Close execution scope for complex function, and execute the function,
//  passing the result of the value list as input pipe. Returns 0 if OK,
//  -1 on error (stack underflow).

int
zs_exec_close (zs_exec_t *self)
{
    if (self->stack_ptr > 0) {
        self->stack_ptr--;
        //  Current output becomes new input and saved pipe is new output
        zs_pipe_destroy (&self->input);
        self->input = self->output;
        self->output = self->pipe_stack [self->stack_ptr];
        (self->call_stack [self->stack_ptr]) (self);
        return 0;
    }
    else
        return -1;
}



//  ---------------------------------------------------------------------------

void
zs_exec_shift (zs_exec_t *self)
{
    zs_pipe_destroy (&self->input);
    self->input = self->output;
    self->output = zs_pipe_new ();
}


//  ---------------------------------------------------------------------------

void
zs_exec_nest (zs_exec_t *self)
{
    self->pipe_stack [self->stack_ptr] = self->output;
    self->output = zs_pipe_new ();
    zs_pipe_purge (self->input);
    self->stack_ptr++;
}


//  ---------------------------------------------------------------------------

void
zs_exec_unnest (zs_exec_t *self)
{
    assert (self->stack_ptr > 0);
    self->stack_ptr--;
    zs_pipe_destroy (&self->input);
    self->input = self->output;
    self->output = self->pipe_stack [self->stack_ptr];
}


//  ---------------------------------------------------------------------------
//  Selftest

void
zs_exec_test (bool verbose)
{
    printf (" * zs_exec: ");
    if (verbose)
        printf ("\n");

    //  @selftest
    zs_exec_t *exec = zs_exec_new ();
    //  TODO
    zs_exec_destroy (&exec);
    //  @end
    printf ("OK\n");
}
