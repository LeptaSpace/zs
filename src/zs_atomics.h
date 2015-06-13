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

//  ---------------------------------------------------------------------------
//  Modest functions

static int
s_debug (zs_vm_t *self, zs_pipe_t *input, zs_pipe_t *output)
{
    if (zs_vm_probing (self))
        zs_vm_register (self, "debug", zs_type_modest, "Trace pipe state in detail");
    else
        zs_vm_trace_pipes (self, (zs_pipe_recv_whole (input) > 0));
    return 0;
}


//  This is the times {} loop function
static int
s_times (zs_vm_t *self, zs_pipe_t *input, zs_pipe_t *output)
{
    if (zs_vm_probing (self))
        zs_vm_register (self, "times", zs_type_modest, "Loop N times");
    else {
        int64_t cycles = zs_pipe_recv_whole (input);
        zs_pipe_mark (output);

        if (cycles > 0) {
            //  Send loop event 1 = continue loop
            zs_pipe_send_whole (output, 1);
            //  Send loop state: remaining cycles
            zs_pipe_send_whole (output, cycles - 1);
        }
        else
            //  Send loop event 0 = end loop
            zs_pipe_send_whole (output, 0);
    }
    return 0;
}

//  This is the count {} loop function
static int
s_count (zs_vm_t *self, zs_pipe_t *input, zs_pipe_t *output)
{
    if (zs_vm_probing (self))
        zs_vm_register (self, "count", zs_type_modest, "Loop N times, counting");
    else {
        int64_t cycles = zs_pipe_recv_whole (input);
        //  Get optional index start and delta from input
        //  These both default to 1 (so default is 1, 2, 3,...)
        int64_t index = zs_pipe_recv (input)? zs_pipe_whole (input): 1;
        int64_t delta = zs_pipe_recv (input)? zs_pipe_whole (input): 1;

        if (cycles > 0) {
            //  Send index and split off state into next phrase
            zs_pipe_send_whole (output, index);
            zs_pipe_mark (output);
            //  Send loop event 1 = continue loop
            zs_pipe_send_whole (output, 1);
            //  Send loop state: remaining cycles, index, delta
            zs_pipe_send_whole (output, cycles - 1);
            zs_pipe_send_whole (output, index + delta);
            zs_pipe_send_whole (output, delta);
        }
        else {
            //  Send loop event 0 = end loop
            zs_pipe_mark (output);
            zs_pipe_send_whole (output, 0);
        }
    }
    return 0;
}


//  This is the countdown {} loop function
static int
s_countdown (zs_vm_t *self, zs_pipe_t *input, zs_pipe_t *output)
{
    if (zs_vm_probing (self))
        zs_vm_register (self, "countdown", zs_type_modest, "Loop N times, counting dow");
    else {
        int64_t cycles = zs_pipe_recv_whole (input);
        if (cycles > 0) {
            //  Send index and split off state into next phrase
            zs_pipe_send_whole (output, cycles);
            zs_pipe_mark (output);
            //  Send loop event 1 = continue loop
            zs_pipe_send_whole (output, 1);
            //  Send loop state: remaining cycles
            zs_pipe_send_whole (output, cycles - 1);
        }
        else {
            //  Send loop event 0 = end loop
            zs_pipe_mark (output);
            zs_pipe_send_whole (output, 0);
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
    if (zs_pipe_realish (input)) {
        double sum = 0;
        while (zs_pipe_recv (input))
            sum += zs_pipe_real (input);
        zs_pipe_send_real (output, sum);
    }
    else {
        int64_t sum = 0;
        while (zs_pipe_recv (input))
            sum += zs_pipe_whole (input);
        zs_pipe_send_whole (output, sum);
    }
    return 0;
}

static int
s_product (zs_vm_t *self, zs_pipe_t *input, zs_pipe_t *output)
{
    if (zs_vm_probing (self))
        zs_vm_register (self, "product", zs_type_greedy, "Product of the values");
    else
    if (zs_pipe_realish (input)) {
        double product = 1;
        while (zs_pipe_recv (input))
            product += zs_pipe_real (input);
        zs_pipe_send_real (output, product);
    }
    else {
        int64_t product = 1;
        while (zs_pipe_recv (input))
            product += zs_pipe_whole (input);
        zs_pipe_send_whole (output, product);
    }
    return 0;
}

static int
s_tally (zs_vm_t *self, zs_pipe_t *input, zs_pipe_t *output)
{
    if (zs_vm_probing (self))
        zs_vm_register (self, "tally", zs_type_greedy, "Number of values");
    else {
        int64_t tally = 0;
        while (zs_pipe_recv (input))
            tally++;
        zs_pipe_send_whole (output, tally);
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
        double tally = 0;
        while (zs_pipe_recv (input)) {
            total += zs_pipe_real (input);
            tally++;
        }
        zs_pipe_send_real (output, total / tally);
    }
    return 0;
}

static int
s_min (zs_vm_t *self, zs_pipe_t *input, zs_pipe_t *output)
{
    if (zs_vm_probing (self))
        zs_vm_register (self, "min", zs_type_greedy, "Minimum of the values");
    else
    if (zs_pipe_realish (input)) {
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
    if (zs_pipe_realish (input)) {
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
    else
    if (zs_pipe_realish (input)) {
        double first = zs_pipe_recv_real (input);
        double second = zs_pipe_recv_real (input);
        if (first != second) {
            printf ("E: assertion failed, %g != %g\n", first, second);
            return -1;          //  Destroy the thread
        }
    }
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
        while (zs_pipe_recv (input))
            zs_pipe_send_whole (output, zs_pipe_whole (input));
    }
    return 0;
}


//  ---------------------------------------------------------------------------
//  Array functions

static int
s_add (zs_vm_t *self, zs_pipe_t *input, zs_pipe_t *output)
{
    if (zs_vm_probing (self)) {
        zs_vm_register (self, "+", zs_type_array, "Add value to all");
        zs_vm_register (self, "add", zs_type_array, NULL);
    }
    else
    if (zs_pipe_realish (input)) {
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
s_subtract (zs_vm_t *self, zs_pipe_t *input, zs_pipe_t *output)
{
    if (zs_vm_probing (self)) {
        zs_vm_register (self, "-", zs_type_array, "Subtract value from all");
        zs_vm_register (self, "subtract", zs_type_array, NULL);
    }
    else
    if (zs_pipe_realish (input)) {
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
s_multiply (zs_vm_t *self, zs_pipe_t *input, zs_pipe_t *output)
{
    if (zs_vm_probing (self)) {
        zs_vm_register (self, "*", zs_type_array, "Multiply value by all");
        zs_vm_register (self, "x", zs_type_array, NULL);
        zs_vm_register (self, "multiply", zs_type_array, NULL);
    }
    else
    if (zs_pipe_realish (input)) {
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
        zs_vm_register (self, "/", zs_type_array, "Divide value into all");
        zs_vm_register (self, "divide", zs_type_array, NULL);
    }
    else {
        double operand = zs_pipe_recv_real (input);
        while (zs_pipe_recv (input))
            zs_pipe_send_real (output, zs_pipe_real (input) / operand);
    }
    return 0;
}


static void
s_register_atomics (zs_vm_t *self)
{
    zs_vm_probe (self, s_check);

    zs_vm_probe (self, s_debug);
    zs_vm_probe (self, s_times);
    zs_vm_probe (self, s_count);
    zs_vm_probe (self, s_countdown);

    zs_vm_probe (self, s_sum);
    zs_vm_probe (self, s_product);
    zs_vm_probe (self, s_tally);
    zs_vm_probe (self, s_mean);
    zs_vm_probe (self, s_min);
    zs_vm_probe (self, s_max);

    zs_vm_probe (self, s_assert);
    zs_vm_probe (self, s_whole);
    zs_vm_probe (self, s_add);
    zs_vm_probe (self, s_subtract);
    zs_vm_probe (self, s_multiply);
    zs_vm_probe (self, s_divide);
}
#endif