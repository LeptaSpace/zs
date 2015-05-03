/*  =========================================================================
    zs_exec - ZeroScript execution context

    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of the ZeroScript language, http://zeroscript.org.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

#ifndef ZS_EXEC_H_INCLUDED
#define ZS_EXEC_H_INCLUDED

#include <czmq.h>

#ifdef __cplusplus
extern "C" {
#endif

//  Opaque class structure
typedef struct _zs_exec_t zs_exec_t;

//  Primitive function type
typedef void (zs_call_t) (zs_exec_t *self);

//  @interface
//  Create a new zs_exec, return the reference if successful, or NULL
//  if construction failed due to lack of available memory.
zs_exec_t *
    zs_exec_new (void);

//  Destroy the zs_exec and free all memory used by the object.
void
    zs_exec_destroy (zs_exec_t **self_p);

//  Return true if we're probing primitives; this tells primitives to
//  register rather than to run.
bool
    zs_exec_probing (zs_exec_t *self);

//  Probe primitive to ask it to register itself; we use a self-registration
//  system where all information about a primitive is encapsulated in its
//  source code, rather than spread throughout the codebase. It's valid to
//  probe primitives at any time.
void
    zs_exec_probe (zs_exec_t *self, zs_call_t *primitive);

//  Primitive registers itself with the execution context. This is only valid
//  if zs_exec_probing () is true. Returns 0 if registration worked, -1 if it
//  failed due to an internal error.
int
    zs_exec_register (zs_exec_t *self, const char *name, const char *hint);

//  Reset the pipes, so whatever follows has empty input and output pipes.
void
    zs_exec_reset (zs_exec_t *self);

//  Return input pipe for the execution context
zs_pipe_t *
    zs_exec_input (zs_exec_t *self);

//  Return output pipe for the execution context
zs_pipe_t *
    zs_exec_output (zs_exec_t *self);

//  Resolve a function by name, return function address, or NULL
zs_call_t *
    zs_exec_resolve (zs_exec_t *self, const char *name);

//  Execute an inline function without value list, passing current output
//  pipe to the function as input, and offering a new output pipe.
void
    zs_exec_inline (zs_exec_t *self, zs_call_t *function);

//  Open new execution scope for a complex function (with a value list)
void
    zs_exec_open (zs_exec_t *self, zs_call_t *function);

//  Close execution scope for complex function, and execute the function,
//  passing the result of the value list as input pipe. Returns 0 if OK,
//  -1 on error (stack underflow).
int
    zs_exec_close (zs_exec_t *self);

void
    zs_exec_shift (zs_exec_t *self);
void
    zs_exec_nest (zs_exec_t *self);
int
    zs_exec_unnest (zs_exec_t *self);

//  Self test of this class
void
    zs_exec_test (bool animate);
//  @end

#ifdef __cplusplus
}
#endif

#endif
