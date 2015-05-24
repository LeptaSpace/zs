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
//  Nullary functions

static int
s_check (zs_vm_t *self, zs_pipe_t *input, zs_pipe_t *output)
{
    if (zs_vm_probing (self))
        zs_vm_register (self, "check", zs_type_nullary, "Run internal checks");
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
s_help (zs_vm_t *self, zs_pipe_t *input, zs_pipe_t *output)
{
    if (zs_vm_probing (self))
        zs_vm_register (self, "help", zs_type_nullary, "List all known functions");
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


//  ---------------------------------------------------------------------------
//  Greedy functions

static int
s_sum (zs_vm_t *self, zs_pipe_t *input, zs_pipe_t *output)
{
    if (zs_vm_probing (self))
        zs_vm_register (self, "sum", zs_type_greedy, "Sum of the values");
    else
    if (zs_pipe_has_real (input)) {
        double result = 0;
        while (zs_pipe_recv (input))
            result += zs_pipe_real (input);
        zs_pipe_send_real (output, result);
    }
    else {
        int64_t result = 0;
        while (zs_pipe_recv (input))
            result += zs_pipe_whole (input);
        zs_pipe_send_whole (output, result);
    }
    return 0;
}

static int
s_product (zs_vm_t *self, zs_pipe_t *input, zs_pipe_t *output)
{
    if (zs_vm_probing (self))
        zs_vm_register (self, "product", zs_type_greedy, "Product of the values");
    else
    if (zs_pipe_has_real (input)) {
        double result = 1;
        while (zs_pipe_recv (input))
            result += zs_pipe_real (input);
        zs_pipe_send_real (output, result);
    }
    else {
        int64_t result = 1;
        while (zs_pipe_recv (input))
            result += zs_pipe_whole (input);
        zs_pipe_send_whole (output, result);
    }
    return 0;
}

static int
s_count (zs_vm_t *self, zs_pipe_t *input, zs_pipe_t *output)
{
    if (zs_vm_probing (self))
        zs_vm_register (self, "count", zs_type_greedy, "Number of values");
    else {
        int64_t result = 0;
        while (zs_pipe_recv (input))
            result++;
        zs_pipe_send_whole (output, result);
    }
    return 0;
}

static int
s_mean (zs_vm_t *self, zs_pipe_t *input, zs_pipe_t *output)
{
    if (zs_vm_probing (self))
        zs_vm_register (self, "mean", zs_type_greedy, "Mean of the values");
    else {
        double total = 0;
        double count = 0;
        while (zs_pipe_recv (input)) {
            total += zs_pipe_real (input);
            count++;
        }
        zs_pipe_send_real (output, total / count);
    }
    return 0;
}

static int
s_min (zs_vm_t *self, zs_pipe_t *input, zs_pipe_t *output)
{
    if (zs_vm_probing (self))
        zs_vm_register (self, "min", zs_type_greedy, "Minimum of the values");
    else
    if (zs_pipe_has_real (input)) {
        double result = zs_pipe_recv_whole (input);
        while (zs_pipe_recv (input)) {
            if (result > zs_pipe_real (input))
                result = zs_pipe_real (input);
        }
        zs_pipe_send_real (output, result);
    }
    else {
        int64_t result = zs_pipe_recv_whole (input);
        while (zs_pipe_recv (input)) {
            if (result > zs_pipe_whole (input))
                result = zs_pipe_whole (input);
        }
        zs_pipe_send_whole (output, result);
    }
    return 0;
}

static int
s_max (zs_vm_t *self, zs_pipe_t *input, zs_pipe_t *output)
{
    if (zs_vm_probing (self))
        zs_vm_register (self, "max", zs_type_greedy, "Maximum of the values");
    else
    if (zs_pipe_has_real (input)) {
        double result = zs_pipe_recv_whole (input);
        while (zs_pipe_recv (input)) {
            if (result < zs_pipe_real (input))
                result = zs_pipe_real (input);
        }
        zs_pipe_send_real (output, result);
    }
    else {
        int64_t result = zs_pipe_recv_whole (input);
        while (zs_pipe_recv (input)) {
            if (result < zs_pipe_whole (input))
                result = zs_pipe_whole (input);
        }
        zs_pipe_send_whole (output, result);
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
        while (zs_pipe_recv (input)) {
            zs_pipe_set_whole (output, zs_pipe_whole (input));
            zs_pipe_send (output);
        }
    }
    return 0;
}


//  ---------------------------------------------------------------------------
//  Array functions

static int
s_plus (zs_vm_t *self, zs_pipe_t *input, zs_pipe_t *output)
{
    if (zs_vm_probing (self)) {
        zs_vm_register (self, "plus", zs_type_array, "Add value to all");
        zs_vm_register (self, "+", zs_type_array, NULL);
    }
    else
    if (zs_pipe_has_real (input)) {
        double operand = zs_pipe_recv_real (input);
        while (zs_pipe_recv (input))
            zs_pipe_send_real (output, zs_pipe_real (input) + operand);
    }
    else {
        int64_t operand = zs_pipe_recv_whole (input);
        while (zs_pipe_recv (input))
            zs_pipe_send_whole (output, zs_pipe_whole (input) + operand);
    }
    return 0;
}

static int
s_minus (zs_vm_t *self, zs_pipe_t *input, zs_pipe_t *output)
{
    if (zs_vm_probing (self)) {
        zs_vm_register (self, "minus", zs_type_array, "Subtract value from all");
        zs_vm_register (self, "-", zs_type_array, NULL);
    }
    else
    if (zs_pipe_has_real (input)) {
        double operand = zs_pipe_recv_real (input);
        while (zs_pipe_recv (input))
            zs_pipe_send_real (output, zs_pipe_real (input) - operand);
    }
    else {
        int64_t operand = zs_pipe_recv_whole (input);
        while (zs_pipe_recv (input))
            zs_pipe_send_whole (output, zs_pipe_whole (input) - operand);
    }
    return 0;
}

static int
s_times (zs_vm_t *self, zs_pipe_t *input, zs_pipe_t *output)
{
    if (zs_vm_probing (self)) {
        zs_vm_register (self, "times", zs_type_array, "Multiply value by all");
        zs_vm_register (self, "*", zs_type_array, NULL);
        zs_vm_register (self, "x", zs_type_array, NULL);
    }
    else
    if (zs_pipe_has_real (input)) {
        double operand = zs_pipe_recv_real (input);
        while (zs_pipe_recv (input))
            zs_pipe_send_real (output, zs_pipe_real (input) * operand);
    }
    else {
        int64_t operand = zs_pipe_recv_whole (input);
        while (zs_pipe_recv (input))
            zs_pipe_send_whole (output, zs_pipe_whole (input) * operand);
    }
    return 0;
}

static int
s_divide (zs_vm_t *self, zs_pipe_t *input, zs_pipe_t *output)
{
    if (zs_vm_probing (self)) {
        zs_vm_register (self, "divide", zs_type_array, "Divide value into all");
        zs_vm_register (self, "/", zs_type_array, NULL);
    }
    else {
        double operand = zs_pipe_recv_real (input);
        printf ("/ OPERAND: %g\n", operand);
        while (zs_pipe_recv (input)) {
            double value = zs_pipe_real (input);
            printf ("/ INTO: %g\n", value);
            zs_pipe_send_real (output, zs_pipe_real (input) / operand);
        }
    }
    return 0;
}


static void
s_register_atomics (zs_vm_t *self)
{
    zs_vm_probe (self, s_check);
    zs_vm_probe (self, s_help);

    zs_vm_probe (self, s_sum);
    zs_vm_probe (self, s_product);
    zs_vm_probe (self, s_count);
    zs_vm_probe (self, s_mean);
    zs_vm_probe (self, s_min);
    zs_vm_probe (self, s_max);

    zs_vm_probe (self, s_assert);
    zs_vm_probe (self, s_whole);
    zs_vm_probe (self, s_plus);
    zs_vm_probe (self, s_minus);
    zs_vm_probe (self, s_times);
    zs_vm_probe (self, s_divide);
}
#endif