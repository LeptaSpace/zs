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

//  Send whole number to pipe; this wipes the current pipe register.
void
    zs_pipe_send_whole (zs_pipe_t *self, int64_t whole);

//  Send real number to pipe; this wipes the current pipe register.
void
    zs_pipe_send_real (zs_pipe_t *self, double real);

//  Send string to pipe; this wipes the current pipe register.
void
    zs_pipe_send_string (zs_pipe_t *self, const char *string);

//  Returns true if the pipe contains at least one real number. Returns false
//  otherwise.
bool
    zs_pipe_realish (zs_pipe_t *self);

//  Receives the next value off the pipe, into the register. Any previous
//  value in the register is lost. Returns true if a value was successfully
//  received. If no values were received, returns false. This method does
//  not block.
bool
    zs_pipe_recv (zs_pipe_t *self);

//  Returns the type of the register, 'w' for whole, 'r' for real, or 's' for
//  string. Returns -1 if the register is empty.
char
    zs_pipe_type (zs_pipe_t *self);

//  Returns the value of the register, coerced to a whole number. This can
//  cause loss of precision. If no conversion was possible, or the register
//  is empty, returns zero.
int64_t
    zs_pipe_whole (zs_pipe_t *self);

//  Returns the value of the register, coerced to a real number. This can
//  cause loss of precision. If no conversion was possible, or the register
//  is empty, returns zero.
double
    zs_pipe_real (zs_pipe_t *self);

//  Returns the value of the register, coerced to a string if needed. If the
//  register is empty, returns an empty string "". The caller must not modify
//  or free the string.
char *
    zs_pipe_string (zs_pipe_t *self);

//  Receives the next value off the pipe, into the register, and coerces it
//  to a whole if needed. If there is no value to receive, returns 0.
int64_t
    zs_pipe_recv_whole (zs_pipe_t *self);

//  Receives the next value off the pipe, into the register, and coerces it
//  to a real if needed. If there is no value to receive, returns 0.
double
    zs_pipe_recv_real (zs_pipe_t *self);

//  Receives the next value off the pipe, into the register, and coerces it
//  to a string if needed. If there is no value to receive, returns "". The
//  The caller must not modify or free the string.
char *
    zs_pipe_recv_string (zs_pipe_t *self);

//  Marks an end of phrase in the pipe. This is used to delimit the pipe
//  as input for later function calls. Marks are ignored when receiving
//  values off a pipe.
void
    zs_pipe_mark (zs_pipe_t *self);

//  Pulls a list of values from the source pipe into the pipe. This function
//  does a "modest" pull: in a phrase, pulls the last single value. After a
//  phrase, pulls the preceding phrase.
void
    zs_pipe_pull_modest (zs_pipe_t *self, zs_pipe_t *source);

//  Pulls a list of values from the source pipe into the pipe. This function
//  does a "greedy" pull: in a phrase, pulls the current phrase. After a
//  phrase, pulls the preceding entire sentence.
void
    zs_pipe_pull_greedy (zs_pipe_t *self, zs_pipe_t *source);

//  Pulls a list of values from the source pipe into the pipe. This function
//  does an array pull: (a) move last value to input, then (b) move either rest
//  of current phrase, or entire previous phrase, to input.
void
    zs_pipe_pull_array (zs_pipe_t *self, zs_pipe_t *source);

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
