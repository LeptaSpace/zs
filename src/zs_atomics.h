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
s_add (zs_vm_t *self)
{
    if (zs_vm_probing (self)) {
        zs_vm_register (self, "add", "Add all the values");
        zs_vm_register (self, "+", NULL);
    }
    else {
        int64_t sum = 0;
        while (zs_pipe_size (zs_vm_input (self)) > 0)
            sum += zs_pipe_recv_whole (zs_vm_input (self));
        zs_pipe_send_whole (zs_vm_output (self), sum);
    }
    return 0;
}

static int
s_subtract (zs_vm_t *self)
{
    if (zs_vm_probing (self)) {
        zs_vm_register (self, "subtract", "Subtract all values from first");
        zs_vm_register (self, "-", NULL);
    }
    else {
        int64_t sum = 0;
        while (zs_pipe_size (zs_vm_input (self)) > 0)
            sum += zs_pipe_recv_whole (zs_vm_input (self));
        zs_pipe_send_whole (zs_vm_output (self), sum);
    }
    return 0;
}

static int
s_multiply (zs_vm_t *self)
{
    if (zs_vm_probing (self)) {
        zs_vm_register (self, "multiply", "Multiply all the values");
        zs_vm_register (self, "*", NULL);
        zs_vm_register (self, "x", NULL);
    }
    else {
        int64_t prod = 0;
        while (zs_pipe_size (zs_vm_input (self)) > 0)
            prod *= zs_pipe_recv_whole (zs_vm_input (self));
        zs_pipe_send_whole (zs_vm_output (self), prod);
    }
    return 0;
}

static int
s_divide (zs_vm_t *self)
{
    zs_pipe_t *input = zs_vm_input (self);
    zs_pipe_t *output = zs_vm_output (self);

    if (zs_vm_probing (self)) {
        zs_vm_register (self, "divide", "Divide all values into first");
        zs_vm_register (self, "/", NULL);
        zs_vm_register (self, ":", NULL);
    }
    else
    if (zs_pipe_size (input)) {
        int64_t current = zs_pipe_recv_whole (input);
        printf ("DIVIDE pipe=%d initial=%ld\n", (int) zs_pipe_size (input), (long) current);

        while (zs_pipe_recv (input) == 0) {
            int64_t value = zs_pipe_whole (input);
            current = current / value;
            printf ("DIVIDE pipe=%d value=%ld current=%ld\n", (int) zs_pipe_size (input), (long) value, (long) current);
        }
        zs_pipe_send_whole (output, current);
    }
    else {
        printf ("E: syntax: 1 2 3, / or / (1 2 3)");
        return -1;
    }
    return 0;
}

static int
s_count (zs_vm_t *self)
{
    if (zs_vm_probing (self))
        zs_vm_register (self, "count", "Count number of values");
    else
        zs_pipe_send_whole (zs_vm_output (self), zs_pipe_size (zs_vm_input (self)));
    return 0;
}

static int
s_echo (zs_vm_t *self)
{
    if (zs_vm_probing (self))
        zs_vm_register (self, "echo", "Print all values");
    else {
        char *results = zs_pipe_paste (zs_vm_input (self));
        printf ("%s\n", results);
        zstr_free (&results);
    }
    return 0;
}

static int
s_clr (zs_vm_t *self)
{
    if (zs_vm_probing (self))
        zs_vm_register (self, "clr", "Clear all values");
    else
        ;           //  Do nothing; this is a no-op
    return 0;
}

static int
s_check (zs_vm_t *self)
{
    if (zs_vm_probing (self))
        zs_vm_register (self, "check", "Run internal checks");
    else {
        int verbose = (zs_pipe_recv_whole (zs_vm_input (self)) != 0);
        zs_lex_test (verbose);
        zs_pipe_test (verbose);
        zs_vm_test (verbose);
        zs_repl_test (verbose);
        zs_pipe_send_string (zs_vm_output (self), "Checks passed successfully");
    }
    return 0;
}

static int
s_assert (zs_vm_t *self)
{
    if (zs_vm_probing (self))
        zs_vm_register (self, "assert", "Assert first two values are the same");
    else {
        int64_t first = zs_pipe_recv_whole (zs_vm_input (self));
        int64_t second = zs_pipe_recv_whole (zs_vm_input (self));
        if (first != second) {
            printf ("E: assertion failed, %" PRId64 " != %" PRId64 "\n", first, second);
            return -1;          //  Destroy the thread
        }
    }
    return 0;
}

static int
s_whole (zs_vm_t *self)
{
    if (zs_vm_probing (self))
        zs_vm_register (self, "whole", "Coerce value to whole number");
    else {
        zs_pipe_t *output = zs_vm_output (self);
        if (zs_pipe_size (output)) {
            //  Unary function; pull last value off pipe
            zs_pipe_pull (output);
            zs_pipe_set_whole (output, zs_pipe_whole (output));
            zs_pipe_send (output);
        }
        else
        if (zs_pipe_size (zs_vm_input (self))) {
            zs_pipe_t *input = zs_vm_input (self);
            //  List function; recv and process all values
            while (zs_pipe_recv (input) == 0) {
                zs_pipe_set_whole (output, zs_pipe_whole (input));
                zs_pipe_send (output);
            }
        }
    }
    return 0;
}

static int
s_help (zs_vm_t *self)
{
    if (zs_vm_probing (self))
        zs_vm_register (self, "help", "List all known functions");
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
    zs_vm_probe (self, s_echo);
    zs_vm_probe (self, s_clr);
    zs_vm_probe (self, s_check);
    zs_vm_probe (self, s_assert);
    zs_vm_probe (self, s_whole);
    zs_vm_probe (self, s_help);
}
#endif