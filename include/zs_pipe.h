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
#define ZS_NULL_WHOLE   INT64_MIN
#define ZS_NULL_REAL    DBL_MIN

//  @interface
//  Create a new zs_pipe, return the reference if successful, or NULL
//  if construction failed due to lack of available memory.
zs_pipe_t *
    zs_pipe_new (void);

//  Destroy the zs_pipe and free all memory used by the object.
void
    zs_pipe_destroy (zs_pipe_t **self_p);

//  Send a whole number to the pipe; values are sent and received in
//  FIFO order, except for the "pull" method (see below), which pulls
//  back the last sent value.
void
    zs_pipe_whole_send (zs_pipe_t *self, int64_t whole);

//  Receive the next whole number from the pipe. If the pipe is empty or
//  next value cannot be treated as a whole number, returns ZS_NULL_WHOLE.
//  This call never blocks.
int64_t
    zs_pipe_whole_recv (zs_pipe_t *self);

//  Pull the last sent whole number from the pipe. Returns the value of the
//  whole number or ZS_NULL_WHOLE if the last value could not be converted,
//  or the pipe was empty.
int64_t
    zs_pipe_whole_pull (zs_pipe_t *self);

//  Send a real number to the pipe; values are sent and received in
//  FIFO order, except for the "pull" method (see below), which pulls
//  back the last sent value.
void
    zs_pipe_real_send (zs_pipe_t *self, double real);

//  Receive the next real number from the pipe. If the pipe is empty or
//  next value cannot be treated as a real number, returns ZS_NULL_REAL.
//  This call never blocks.
double
    zs_pipe_real_recv (zs_pipe_t *self);

//  Pull the last sent real number from the pipe. Returns the value of the
//  real number or ZS_NULL_REAL if the last value could not be converted,
//  or the pipe was empty.
double
    zs_pipe_real_pull (zs_pipe_t *self);

//  Send a string to the pipe; values are sent and received in FIFO order,
//  except for the "pull" method (see below), which pulls back the last sent
//  value.
void
    zs_pipe_string_send (zs_pipe_t *self, const char *string);

//  Receive the next string from the pipe. If the pipe is empty, returns NULL.
//  This call never blocks. Caller should not modify value; this is managed by
//  the pipe class.
char *
    zs_pipe_string_recv (zs_pipe_t *self);

//  Pull the last sent string from the pipe. Returns the value of the string
//  or NULL if the pipe was empty. Caller should not modify value; this is
//  managed by the pipe class.
char *
    zs_pipe_string_pull (zs_pipe_t *self);

//  Return number of values in pipe (0 or more)
size_t
    zs_pipe_size (zs_pipe_t *self);

//  Return pipe contents, as string. Caller must free it when done. Values are
//  separated by spaces. This empties the pipe.
char *
    zs_pipe_paste (zs_pipe_t *self);

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
