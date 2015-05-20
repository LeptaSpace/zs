/*  =========================================================================
    zs_atomics - ZeroScript atomics

    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of the ZeroScript language, http://zeroscript.org.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

#ifndef ZS_ATOMICS_H_INCLUDED
#define ZS_ATOMICS_H_INCLUDED

//  ---------------------------------------------------------------------------
//  Core language atomics

static int
s_add (zs_vm_t *self, zs_pipe_t *input, zs_pipe_t *output)
{
    if (zs_vm_probing (self)) {
        zs_vm_register (self, "add", zs_type_greedy, "Add all the values");
        zs_vm_register (self, "+", zs_type_greedy, NULL);
    }
    else {
        int64_t value = 0;
        while (!zs_pipe_recv (input))
            value += zs_pipe_whole (input);
        zs_pipe_send_whole (output, value);
    }
    return 0;
}

static int
s_subtract (zs_vm_t *self, zs_pipe_t *input, zs_pipe_t *output)
{
    if (zs_vm_probing (self)) {
        zs_vm_register (self, "subtract", zs_type_greedy, "Subtract all values from first");
        zs_vm_register (self, "-", zs_type_greedy, NULL);
    }
    else
    if (!zs_pipe_recv (input)) {
        int64_t value = zs_pipe_whole (input);
        while (!zs_pipe_recv (input) > 0)
            value -= zs_pipe_whole (input);
        zs_pipe_send_whole (output, value);
    }
    return 0;
}

static int
s_multiply (zs_vm_t *self, zs_pipe_t *input, zs_pipe_t *output)
{
    if (zs_vm_probing (self)) {
        zs_vm_register (self, "multiply", zs_type_greedy, "Multiply all the values");
        zs_vm_register (self, "*", zs_type_greedy, NULL);
        zs_vm_register (self, "x", zs_type_greedy, NULL);
    }
    else
    if (!zs_pipe_recv (input)) {
        int64_t value = zs_pipe_whole (input);
        while (!zs_pipe_recv (input) > 0)
            value *= zs_pipe_whole (input);
        zs_pipe_send_whole (output, value);
    }
    return 0;
}

static int
s_divide (zs_vm_t *self, zs_pipe_t *input, zs_pipe_t *output)
{
    if (zs_vm_probing (self)) {
        zs_vm_register (self, "divide", zs_type_greedy, "Divide all values into first");
        zs_vm_register (self, "/", zs_type_greedy, NULL);
        zs_vm_register (self, ":", zs_type_greedy, NULL);
    }
    else
    if (!zs_pipe_recv (input)) {
        int64_t value = zs_pipe_whole (input);
        while (!zs_pipe_recv (input) > 0)
            value /= zs_pipe_whole (input);
        zs_pipe_send_whole (output, value);
    }
    else {
        printf ("E: syntax: 1 2 3, / or / (1 2 3)");
        return -1;
    }
    return 0;
}

static int
s_count (zs_vm_t *self, zs_pipe_t *input, zs_pipe_t *output)
{
    if (zs_vm_probing (self))
        zs_vm_register (self, "count", zs_type_greedy, "Count number of values");
    else {
        int64_t count = 0;
        while (!zs_pipe_recv (input))
            count++;
        zs_pipe_send_whole (output, count);
    }
    return 0;
}

static int
s_check (zs_vm_t *self, zs_pipe_t *input, zs_pipe_t *output)
{
    if (zs_vm_probing (self))
        zs_vm_register (self, "check", zs_type_strict, "Run internal checks");
    else {
        int verbose = (zs_pipe_recv_whole (input) != 0);
        zs_lex_test (verbose);
        zs_pipe_test (verbose);
        zs_vm_test (verbose);
        zs_repl_test (verbose);
        zs_pipe_send_string (output, "Checks passed successfully");
    }
    return 0;
}

static int
s_assert (zs_vm_t *self, zs_pipe_t *input, zs_pipe_t *output)
{
    if (zs_vm_probing (self))
        zs_vm_register (self, "assert", zs_type_greedy, "Assert first two values are the same");
    else {
        int64_t first = zs_pipe_recv_whole (input);
        int64_t second = zs_pipe_recv_whole (input);
        if (first != second) {
            printf ("E: assertion failed, %" PRId64 " != %" PRId64 "\n", first, second);
            return -1;          //  Destroy the thread
        }
    }
    return 0;
}

static int
s_whole (zs_vm_t *self, zs_pipe_t *input, zs_pipe_t *output)
{
    if (zs_vm_probing (self))
        zs_vm_register (self, "whole", zs_type_greedy, "Coerce values to whole numbers");
    else {
        while (!zs_pipe_recv (input) == 0) {
            zs_pipe_set_whole (output, zs_pipe_whole (input));
            zs_pipe_send (output);
        }
    }
    return 0;
}

static int
s_help (zs_vm_t *self, zs_pipe_t *input, zs_pipe_t *output)
{
    if (zs_vm_probing (self))
        zs_vm_register (self, "help", zs_type_strict, "List all known functions");
    else {
        const char *name = zs_vm_function_first (self);
        while (name) {
            if (strneq (name, "$shell$"))
                printf ("%s ", name);
            name = zs_vm_function_next (self);
        }
        name = zs_vm_atomic_first (self);
        while (name) {
            printf ("%s ", name);
            name = zs_vm_atomic_next (self);
        }
    }
    return 0;
}


static void
s_register_atomics (zs_vm_t *self)
{
    zs_vm_probe (self, s_add);
    zs_vm_probe (self, s_subtract);
    zs_vm_probe (self, s_multiply);
    zs_vm_probe (self, s_divide);

    zs_vm_probe (self, s_count);
    zs_vm_probe (self, s_check);
    zs_vm_probe (self, s_assert);
    zs_vm_probe (self, s_whole);
    zs_vm_probe (self, s_help);
}
#endif