/*  =========================================================================
    zs_vm - ZeroScript virtual machine

    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of the ZeroScript language, http://zeroscript.org.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

#ifndef ZS_VM_H_INCLUDED
#define ZS_VM_H_INCLUDED

#include <czmq.h>

#ifdef __cplusplus
extern "C" {
#endif

//  Opaque class structure
typedef struct _zs_vm_t zs_vm_t;

//  Virtual machine atomic function type
typedef int (zs_vm_fn_t) (zs_vm_t *self);

//  @interface
//  Create a new empty virtual machine. Returns the reference if successful,
//  or NULL if construction failed due to lack of available memory.
zs_vm_t *
    zs_vm_new (void);

//  Destroy the virtual machine and free all memory used by it.
void
    zs_vm_destroy (zs_vm_t **self_p);

//  Probe atomic to ask it to register itself; we use a self-registration
//  system where all information about an atomic is encapsulated in its
//  source code, rather than spread throughout the codebase. It's valid to
//  probe dictionary at any time.
void
    zs_vm_probe (zs_vm_t *self, zs_vm_fn_t *atomic);

//  Return true if we're probing dictionary; this tells dictionary to
//  register rather than to run.
bool
    zs_vm_probing (zs_vm_t *self);

//  Primitive registers itself with the execution context. This is only valid
//  if zs_vm_probing () is true. Returns 0 if registration worked, -1 if it
//  failed due to an internal error.
int
    zs_vm_register (zs_vm_t *self, const char *name, const char *hint);

//  Compile a number constant into the virtual machine.
//  Numbers are stored thus:
void
    zs_vm_compile_number (zs_vm_t *self, int64_t number);

//  Compile a string constant into the virtual machine.
void
    zs_vm_compile_string (zs_vm_t *self, const char *string);

//  Compile a new function definition; end with a commit.
void
    zs_vm_compile_define (zs_vm_t *self, const char *name);

//  Close the current function definition.
void
    zs_vm_compile_commit (zs_vm_t *self);

//  Cancel the current or last function definition and reset the virtual
//  machine to the state before the previous _define. You can call this
//  repeatedly to delete function definitions until the machine is empty.
//  Returns 0 if OK, -1 if there was no function to rollback (the machine
//  is then empty).
int
    zs_vm_compile_rollback (zs_vm_t *self);

//  Compile an open scope operation; you must match this with a close.
//  Returns 0 if OK or -1 if the function was not defined.
int
    zs_vm_compile_open (zs_vm_t *self, const char *name);

//  Compile a close scope + execute function. The function gets the current
//  output pipe as input, and sends output to the parent output pipe.
void
    zs_vm_compile_close (zs_vm_t *self);

//  Compile a chain scope + execute function. The function gets the current
//  output pipe as input, and sends its output to a new pipe. Returns 0 if OK
//  or -1 if the function was not defined.
int
    zs_vm_compile_chain (zs_vm_t *self, const char *name);

//  Return input pipe for the execution context
zs_pipe_t *
    zs_vm_input (zs_vm_t *self);

//  Return output pipe for the execution context
zs_pipe_t *
    zs_vm_output (zs_vm_t *self);

//  Dump VM contents (state and code)
void
    zs_vm_dump (zs_vm_t *self);

//  Enable tracing of VM compilation and execution.
void
    zs_vm_set_verbose (zs_vm_t *self, bool verbose);

//  Run last defined function, if any, in the VM. This continues forever or
//  until the function ends. Returns 0 if stopped successfully, or -1 if
//  stopped due to some error. Each run of the VM starts with clean pipes.
int
    zs_vm_run (zs_vm_t *self);

//  Self test of this class
void
    zs_vm_test (bool animate);
//  @end

#ifdef __cplusplus
}
#endif

#endif
