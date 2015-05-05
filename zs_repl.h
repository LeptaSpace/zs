/*  =========================================================================
    zs_repl - the ZeroScript read-evaluate-print loop

    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of the ZeroScript language, http://zeroscript.org.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

#ifndef ZS_REPL_H_INCLUDED
#define ZS_REPL_H_INCLUDED

#include <czmq.h>

#ifdef __cplusplus
extern "C" {
#endif

//  Opaque class structure
typedef struct _zs_repl_t zs_repl_t;

//  @interface
//  Create a new core engine, return the reference if successful, or NULL
//  if construction failed due to lack of available memory.
zs_repl_t *
    zs_repl_new (void);

//  Destroy the zs_repl and free all memory used by the object
void
    zs_repl_destroy (zs_repl_t **self_p);

//  Enable verbose tracing of engine
void
    zs_repl_verbose (zs_repl_t *self, bool verbose);

//  Execute a buffer of code; to reset the engine you destroy it and create a
//  new one. Returns 0 if OK, -1 on syntax errors or cataclysmic implosions
//  of the Sun (can be determined from context).
int
    zs_repl_execute (zs_repl_t *self, const char *input);

//  Return true if the input formed a complete phrase that was successfully
//  evaulated. If not, the core expects more input.
bool
    zs_repl_completed (zs_repl_t *self);

//  Return pipe results as string, after successful execution. Caller must
//  free results when finished.
char *
    zs_repl_results (zs_repl_t *self);

//  After a syntax error, return position of syntax error in text.
uint
    zs_repl_offset (zs_repl_t *self);

//  Self test of this class
void
    zs_repl_test (bool animate);
//  @end

#ifdef __cplusplus
}
#endif

#endif
