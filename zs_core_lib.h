/*  =========================================================================
    zs_core_lib - ZeroScript core primitives

    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of the ZeroScript language, http://zeroscript.org.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

#ifndef ZS_CORE_LIB_H_INCLUDED
#define ZS_CORE_LIB_H_INCLUDED

//  ---------------------------------------------------------------------------
//  Some core primitives

static void
s_sum (zs_exec_t *self)
{
    if (zs_exec_probing (self))
        zs_exec_register (self, "sum", "Add all the values");
    else {
        int64_t sum = 0;
        while (zs_pipe_size (zs_exec_input (self)) > 0)
            sum += zs_pipe_get_number (zs_exec_input (self));
        zs_pipe_put_number (zs_exec_output (self), sum);
    }
}

static void
s_prod (zs_exec_t *self)
{
    if (zs_exec_probing (self))
        zs_exec_register (self, "prod", "Multiply all the values");
    else {
        int64_t prod = 0;
        while (zs_pipe_size (zs_exec_input (self)) > 0)
            prod *= zs_pipe_get_number (zs_exec_input (self));
        zs_pipe_put_number (zs_exec_output (self), prod);
    }
}

static void
s_count (zs_exec_t *self)
{
    if (zs_exec_probing (self))
        zs_exec_register (self, "count", "Count how many values there are");
    else
        zs_pipe_put_number (zs_exec_output (self), zs_pipe_size (zs_exec_input (self)));
}

static void
s_echo (zs_exec_t *self)
{
    if (zs_exec_probing (self))
        zs_exec_register (self, "echo", "Print all values");
    else {
        char *results = zs_pipe_contents (zs_exec_input (self));
        printf ("%s\n", results);
        zstr_free (&results);
    }
}

static void
s_clr (zs_exec_t *self)
{
    if (zs_exec_probing (self))
        zs_exec_register (self, "clr", "Clear all values");
    else
        ;           //  Do nothing; this is a no-op
}

static void
s_python (zs_exec_t *self)
{
    if (zs_exec_probing (self))
        zs_exec_register (self, "python", "Why not just use Python?");
    else
        while (true);
}

static void
s_check (zs_exec_t *self)
{
    if (zs_exec_probing (self))
        zs_exec_register (self, "check", "Run internal checks");
    else {
        zs_lex_test (false);
        zs_pipe_test (false);
        zs_core_test (false);
        zs_exec_test (false);
        zs_pipe_put_string (zs_exec_output (self), "Checks passed successfully");
    }
}

static void
s_register_primitives (zs_exec_t *self)
{
    zs_exec_probe (self, s_sum);
    zs_exec_probe (self, s_prod);
    zs_exec_probe (self, s_count);
    zs_exec_probe (self, s_echo);
    zs_exec_probe (self, s_clr);
    zs_exec_probe (self, s_python);
    zs_exec_probe (self, s_check);
}
#endif