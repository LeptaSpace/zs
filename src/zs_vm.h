/*  =========================================================================
    zs_vm - ZeroScript virtual machine

    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of the ZeroScript language, http://zeroscript.org.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL is not distributed with this
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
#ifndef ZS_VM_T_DEFINED
typedef struct _zs_vm_t zs_vm_t;
#endif

//  Atomic function types
typedef enum {
    //  A nullary function is never takes arguments; it is a constant or a
    //  measurement of the physical enviornment, e.g. time or temperature.
    zs_type_nullary,
    //  A modest function is singular (unary) by default; however we can force
    //  it to work on a list of values. An exmaple is "k".
    zs_type_modest,
    //  A greedy function operates on as many values as it can, this means
    //  the current phrase or sentence. An example is "sum".
    zs_type_greedy,
    //  An array function applies a phrase to a greedy list (phrase or
    //  sentence), producing a phrase as result. An example is "times".
    zs_type_array,
    //  If a function name is not defined, zs_vm_resolve returns this.
    zs_type_unknown
} zs_type_t;


//  Virtual machine atomic function type
typedef int (zs_vm_fn_t) (zs_vm_t *self, zs_pipe_t *input, zs_pipe_t *output);

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
//  failed due to an internal error. If hint is NULL, uses same hint as last
//  registered method (this is for aliases).
int
    zs_vm_register (zs_vm_t *self, const char *name, zs_type_t type, const char *hint);

//  Compile a whole number constant into the virtual machine.
//  Whole numbers are stored thus:
//      [VM_WHOLE][8 bytes in host format]
void
    zs_vm_compile_whole (zs_vm_t *self, int64_t whole);

//  Compile a real number constant into the virtual machine.
//  Whole numbers are stored thus:
//      [VM_REAL][8 bytes in host format]
void
    zs_vm_compile_real (zs_vm_t *self, double real);

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

//  Compile an inline function call; the actual pipe semantics depend on the
//  type of the function. Returns 0 if successful, -1 if the function is not
//  defined.
int
    zs_vm_compile_inline (zs_vm_t *self, const char *name);

//  Compile a nested function call. The current sentence is stacked and we
//  start a new sentence. The actual function call is executed when we hit
//  the matching unnest. Returns 0 if OK or -1 if the function is not
//  defined.
int
    zs_vm_compile_nested (zs_vm_t *self, const char *name);

//  Compile an unnest operation; this executes the nested function on the
//  current phrase or sentence.
void
    zs_vm_compile_unnest (zs_vm_t *self);

//  Compile end of phrase. This appends the phrase output to the current
//  sentence output, and starts a new phrase.
void
    zs_vm_compile_phrase (zs_vm_t *self);

//  Compile end of sentence. This prints the sentence output and starts a
//  new sentence.
void
    zs_vm_compile_sentence (zs_vm_t *self);

//  Dump VM contents (state and code)
void
    zs_vm_dump (zs_vm_t *self);

//  Return latest function by name; use with first to iterate through
//  functions. Returns function name or NULL if there are none defined.
const char *
    zs_vm_function_first (zs_vm_t *self);

//  Return previous function by name; use after first to iterate through
//  functions. Returns function name or NULL if there are no more.
const char *
    zs_vm_function_next (zs_vm_t *self);

//  Enable tracing of VM compilation and execution.
void
    zs_vm_set_verbose (zs_vm_t *self, bool verbose);

//  Run last defined function, if any, in the VM. This continues forever or
//  until the function ends. Returns 0 if stopped successfully, or -1 if
//  stopped due to some error. Each run of the VM starts with clean pipes.
int
    zs_vm_run (zs_vm_t *self);

//  Return results as string, after successful execution. Caller must not
//  modify returned value.
const char *
    zs_vm_results (zs_vm_t *self);

//  Self test of this class
void
    zs_vm_test (bool animate);
//  @end

#ifdef __cplusplus
}
#endif

#endif
