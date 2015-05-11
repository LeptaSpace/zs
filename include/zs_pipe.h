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
#ifndef ZS_PIPE_T_DEFINED
typedef struct _zs_pipe_t zs_pipe_t;
#endif

//  @interface
//  Create a new zs_pipe, return the reference if successful, or NULL
//  if construction failed due to lack of available memory.
zs_pipe_t *
    zs_pipe_new (void);

//  Destroy the zs_pipe and free all memory used by the object.
void
    zs_pipe_destroy (zs_pipe_t **self_p);

//  Add numeric value to end of pipe, after any existing values
void
    zs_pipe_queue_number (zs_pipe_t *self, int64_t number);

//  Add numeric value to start of pipe, before any existing values. Use this
//  if you want to modify and push back a numeric value.
void
    zs_pipe_push_number (zs_pipe_t *self, int64_t number);

//  Add string value to end of pipe, after any existing values
void
    zs_pipe_queue_string (zs_pipe_t *self, const char *string);

//  Add string value to start of pipe, before any existing values. Use this
//  if you want to modify and push back a numeric value.
void
    zs_pipe_push_string (zs_pipe_t *self, const char *string);

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
    zs_pipe_dequeue_number (zs_pipe_t *self);

//  Return next value off pipe as string (converting if needed)
//  Caller should not modify value; this is managed by pipe class.
const char *
    zs_pipe_dequeue_string (zs_pipe_t *self);

//  Return last value off pipe as number; if the value is a string it's
//  converted to a number, quite brutally.
int64_t
    zs_pipe_pop_number (zs_pipe_t *self);

//  Return next value off pipe as string (converting if needed). Caller
//  should not modify value; this is managed by pipe class.
const char *
    zs_pipe_pop_string (zs_pipe_t *self);

//  Return pipe contents, as string. Caller must free it when done. Values are
//  separated by spaces.
char *
    zs_pipe_contents (zs_pipe_t *self);

//  Empty the pipe of any values it might contain.
void
    zs_pipe_purge (zs_pipe_t *self);

//  Self test of this class
void
    zs_pipe_test (bool animate);
//  @end

#ifdef __cplusplus
}
#endif

#endif
