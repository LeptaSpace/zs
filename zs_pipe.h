/*  =========================================================================
    zs_pipe - ZeroScript data pipe

    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of the ZeroScript language, http://zeroscript.org.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

#ifndef ZS_PIPE_H_INCLUDED
#define ZS_PIPE_H_INCLUDED

#include <czmq.h>

#ifdef __cplusplus
extern "C" {
#endif

//  Opaque class structure
typedef struct _zs_pipe_t zs_pipe_t;

//  @interface
//  Create a new zs_pipe, return the reference if successful, or NULL
//  if construction failed due to lack of available memory.
zs_pipe_t *
    zs_pipe_new (void);

//  Destroy the zs_pipe and free all memory used by the object.
void
    zs_pipe_destroy (zs_pipe_t **self_p);

//  Add numeric value to pipe
void
    zs_pipe_put_number (zs_pipe_t *self, int64_t number);

//  Add string value to pipe
void
    zs_pipe_put_string (zs_pipe_t *self, const char *value);

//  Return number of values in pipe
size_t
    zs_pipe_size (zs_pipe_t *self);

//  Return true if next value off pipe is numeric
bool
    zs_pipe_isnumber (zs_pipe_t *self);

//  Return true if next value off pipe is numeric
bool
    zs_pipe_isstring (zs_pipe_t *self);

//  Return next value off pipe as number; if the value is a string it's
//  converted to a number, quite brutally.
int64_t
    zs_pipe_get_number (zs_pipe_t *self);

//  Return next value off pipe as string (converting if needed)
//  Caller should not modify value; this is managed by pipe class.
const char *
    zs_pipe_get_string (zs_pipe_t *self);

//  Print pipe contents, if any
void
    zs_pipe_print (zs_pipe_t *self);

//  Self test of this class
void
    zs_pipe_test (bool animate);
//  @end

#ifdef __cplusplus
}
#endif

#endif
