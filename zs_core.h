/*  =========================================================================
    zs_core - the ZeroScript core engine

    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of the ZeroScript language, http://zeroscript.org.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

#ifndef ZS_CORE_H_INCLUDED
#define ZS_CORE_H_INCLUDED

#include <czmq.h>

#ifdef __cplusplus
extern "C" {
#endif

//  Opaque class structure
typedef struct _zs_core_t zs_core_t;

//  @interface
//  Create a new core engine, return the reference if successful, or NULL
//  if construction failed due to lack of available memory.
zs_core_t *
    zs_core_new (void);

//  Destroy the zs_core and free all memory used by the object
void
    zs_core_destroy (zs_core_t **self_p);

//  Enable verbose tracing of engine
void
    zs_core_verbose (zs_core_t *self, bool verbose);

//  Execute a buffer of code; to reset the engine you destroy it and create a
//  new one. Returns 0 if OK, -1 on syntax errors or cataclysmic implosions 
//  of the Sun (can be determined from context).
int
    zs_core_execute (zs_core_t *self, const char *input);

//  Return pipe results as string, after successful execution. Caller must
//  free results when finished.
char *
    zs_core_results (zs_core_t *self);

//  After a syntax error, return position of syntax error in text.
uint
    zs_core_offset (zs_core_t *self);

//  Self test of this class
void
    zs_core_test (bool animate);
//  @end

#ifdef __cplusplus
}
#endif

#endif
