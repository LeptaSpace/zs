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
    zs_primitive_t *primitive;
    bool probing;

    zhash_t *primitives;            //  cheap and cheerful for now
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
zs_exec_probe (zs_exec_t *self, zs_primitive_t *primitive)
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
//  Call a function by name, return 0 if OK, -1 if function does not exist.

int
zs_exec_call (zs_exec_t *self, const char *name)
{
    zs_primitive_t *primitive = (zs_primitive_t *) zhash_lookup (self->primitives, name);
    if (primitive) {
        (primitive) (self);
        return 0;
    }
    else
        return -1;              //  Duplicate name
}


//  ---------------------------------------------------------------------------
//  Cycle output pipe to input, create new output pipe

void
zs_exec_cycle (zs_exec_t *self)
{
    zs_pipe_destroy (&self->input);
    self->input = self->output;
    self->output = zs_pipe_new ();
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
    zs_exec_destroy (&exec);
    //  @end
    printf ("OK\n");
}
