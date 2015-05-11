/*  =========================================================================
    zs_repl_lib - ZeroScript atomics

    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of the ZeroScript language, http://zeroscript.org.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

#ifndef ZS_REPL_LIB_H_INCLUDED
#define ZS_REPL_LIB_H_INCLUDED

//  ---------------------------------------------------------------------------
//  Some repl atomics

static int
s_sum (zs_vm_t *self)
{
    if (zs_vm_probing (self))
        zs_vm_register (self, "sum", "Add all the values");
    else {
        int64_t sum = 0;
        while (zs_pipe_size (zs_vm_input (self)) > 0)
            sum += zs_pipe_whole_recv (zs_vm_input (self));
        zs_pipe_whole_send (zs_vm_output (self), sum);
    }
    return 0;
}

static int
s_prod (zs_vm_t *self)
{
    if (zs_vm_probing (self))
        zs_vm_register (self, "prod", "Multiply all the values");
    else {
        int64_t prod = 0;
        while (zs_pipe_size (zs_vm_input (self)) > 0)
            prod *= zs_pipe_whole_recv (zs_vm_input (self));
        zs_pipe_whole_send (zs_vm_output (self), prod);
    }
    return 0;
}

static int
s_count (zs_vm_t *self)
{
    if (zs_vm_probing (self))
        zs_vm_register (self, "count", "Count how many values there are");
    else
        zs_pipe_whole_send (zs_vm_output (self), zs_pipe_size (zs_vm_input (self)));
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
s_python (zs_vm_t *self)
{
    if (zs_vm_probing (self))
        zs_vm_register (self, "python", "Why not just use Python?");
    else
        return -1;
    return 0;
}

static int
s_check (zs_vm_t *self)
{
    if (zs_vm_probing (self))
        zs_vm_register (self, "check", "Run internal checks");
    else {
        int verbose = (zs_pipe_whole_recv (zs_vm_input (self)) != 0);
        zs_lex_test (verbose);
        zs_pipe_test (verbose);
        zs_vm_test (verbose);
        zs_repl_test (verbose);
        zs_pipe_string_send (zs_vm_output (self), "Checks passed successfully");
    }
    return 0;
}

static int
s_assert (zs_vm_t *self)
{
    if (zs_vm_probing (self))
        zs_vm_register (self, "assert", "Assert first two values are the same");
    else {
        int64_t first = zs_pipe_whole_recv (zs_vm_input (self));
        int64_t second = zs_pipe_whole_recv (zs_vm_input (self));
        if (first != second) {
            printf ("E: assertion failed, %" PRId64 " != %" PRId64 "\n", first, second);
            return -1;          //  Destroy the thread
        }
    }
    return 0;
}

static void
s_register_atomics (zs_vm_t *self)
{
    zs_vm_probe (self, s_sum);
    zs_vm_probe (self, s_prod);
    zs_vm_probe (self, s_count);
    zs_vm_probe (self, s_echo);
    zs_vm_probe (self, s_clr);
    zs_vm_probe (self, s_python);
    zs_vm_probe (self, s_check);
    zs_vm_probe (self, s_assert);
}
#endif