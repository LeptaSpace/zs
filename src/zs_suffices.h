/*  =========================================================================
    zs_suffices - atomics

    ** WARNING *************************************************************
    THIS SOURCE FILE IS 100% GENERATED. If you edit this file, you will lose
    your changes at the next build cycle. This is great for temporary printf
    statements. DO NOT MAKE ANY CHANGES YOU WISH TO KEEP. The correct places
    for commits are:

     * The XML model used for this code generation: zs_suffices.xml, or
     * The code generation script that built this file: zs_suffices
    ************************************************************************
    =========================================================================
*/

#ifndef ZS_SUFFICES_H_INCLUDED
#define ZS_SUFFICES_H_INCLUDED

static int
s_Ki (zs_vm_t *self)
{
    
    if (zs_vm_probing (self))
        zs_vm_register (self, "Ki", "Multiply by 2^10");
    else
    if (zs_pipe_size (zs_vm_output (self))) {
        //  Unary function
        int64_t number = zs_pipe_pop_number (zs_vm_output (self)) * 1024;
        zs_pipe_push_number (zs_vm_output (self), number);
    }
    else {
        //  List function
        while (zs_pipe_size (zs_vm_input (self))) {
            int64_t number = zs_pipe_pop_number (zs_vm_input (self)) * 1024;
            zs_pipe_push_number (zs_vm_output (self), number);
        }
    }
    return 0;
}

static int
s_Mi (zs_vm_t *self)
{
    
    if (zs_vm_probing (self))
        zs_vm_register (self, "Mi", "Multiply by 2^20");
    else
    if (zs_pipe_size (zs_vm_output (self))) {
        //  Unary function
        int64_t number = zs_pipe_pop_number (zs_vm_output (self)) * 1024 * 1024;
        zs_pipe_push_number (zs_vm_output (self), number);
    }
    else {
        //  List function
        while (zs_pipe_size (zs_vm_input (self))) {
            int64_t number = zs_pipe_pop_number (zs_vm_input (self)) * 1024 * 1024;
            zs_pipe_push_number (zs_vm_output (self), number);
        }
    }
    return 0;
}

static int
s_Gi (zs_vm_t *self)
{
    
    if (zs_vm_probing (self))
        zs_vm_register (self, "Gi", "Multiply by 2^30");
    else
    if (zs_pipe_size (zs_vm_output (self))) {
        //  Unary function
        int64_t number = zs_pipe_pop_number (zs_vm_output (self)) * 1024 * 1024 * 1024;
        zs_pipe_push_number (zs_vm_output (self), number);
    }
    else {
        //  List function
        while (zs_pipe_size (zs_vm_input (self))) {
            int64_t number = zs_pipe_pop_number (zs_vm_input (self)) * 1024 * 1024 * 1024;
            zs_pipe_push_number (zs_vm_output (self), number);
        }
    }
    return 0;
}

static int
s_Ti (zs_vm_t *self)
{
    
    if (zs_vm_probing (self))
        zs_vm_register (self, "Ti", "Multiply by 2^40");
    else
    if (zs_pipe_size (zs_vm_output (self))) {
        //  Unary function
        int64_t number = zs_pipe_pop_number (zs_vm_output (self)) * 1024 * 1024 * 1024 * 1024;
        zs_pipe_push_number (zs_vm_output (self), number);
    }
    else {
        //  List function
        while (zs_pipe_size (zs_vm_input (self))) {
            int64_t number = zs_pipe_pop_number (zs_vm_input (self)) * 1024 * 1024 * 1024 * 1024;
            zs_pipe_push_number (zs_vm_output (self), number);
        }
    }
    return 0;
}

static int
s_Pi (zs_vm_t *self)
{
    
    if (zs_vm_probing (self))
        zs_vm_register (self, "Pi", "Multiply by 2^50");
    else
    if (zs_pipe_size (zs_vm_output (self))) {
        //  Unary function
        int64_t number = zs_pipe_pop_number (zs_vm_output (self)) * 1024 * 1024 * 1024 * 1024 * 1024;
        zs_pipe_push_number (zs_vm_output (self), number);
    }
    else {
        //  List function
        while (zs_pipe_size (zs_vm_input (self))) {
            int64_t number = zs_pipe_pop_number (zs_vm_input (self)) * 1024 * 1024 * 1024 * 1024 * 1024;
            zs_pipe_push_number (zs_vm_output (self), number);
        }
    }
    return 0;
}

static int
s_Ei (zs_vm_t *self)
{
    
    if (zs_vm_probing (self))
        zs_vm_register (self, "Ei", "Multiply by 2^60");
    else
    if (zs_pipe_size (zs_vm_output (self))) {
        //  Unary function
        int64_t number = zs_pipe_pop_number (zs_vm_output (self)) * 1024 * 1024 * 1024 * 1024 * 1024 * 1024;
        zs_pipe_push_number (zs_vm_output (self), number);
    }
    else {
        //  List function
        while (zs_pipe_size (zs_vm_input (self))) {
            int64_t number = zs_pipe_pop_number (zs_vm_input (self)) * 1024 * 1024 * 1024 * 1024 * 1024 * 1024;
            zs_pipe_push_number (zs_vm_output (self), number);
        }
    }
    return 0;
}

static int
s_da (zs_vm_t *self)
{
    
    if (zs_vm_probing (self))
        zs_vm_register (self, "da", "Multiply by 10");
    else
    if (zs_pipe_size (zs_vm_output (self))) {
        //  Unary function
        int64_t number = zs_pipe_pop_number (zs_vm_output (self)) * 10;
        zs_pipe_push_number (zs_vm_output (self), number);
    }
    else {
        //  List function
        while (zs_pipe_size (zs_vm_input (self))) {
            int64_t number = zs_pipe_pop_number (zs_vm_input (self)) * 10;
            zs_pipe_push_number (zs_vm_output (self), number);
        }
    }
    return 0;
}

static int
s_h (zs_vm_t *self)
{
    
    if (zs_vm_probing (self))
        zs_vm_register (self, "h", "Multiply by 100");
    else
    if (zs_pipe_size (zs_vm_output (self))) {
        //  Unary function
        int64_t number = zs_pipe_pop_number (zs_vm_output (self)) * 100;
        zs_pipe_push_number (zs_vm_output (self), number);
    }
    else {
        //  List function
        while (zs_pipe_size (zs_vm_input (self))) {
            int64_t number = zs_pipe_pop_number (zs_vm_input (self)) * 100;
            zs_pipe_push_number (zs_vm_output (self), number);
        }
    }
    return 0;
}

static int
s_k (zs_vm_t *self)
{
    
    if (zs_vm_probing (self))
        zs_vm_register (self, "k", "Multiply by 1000");
    else
    if (zs_pipe_size (zs_vm_output (self))) {
        //  Unary function
        int64_t number = zs_pipe_pop_number (zs_vm_output (self)) * 1000;
        zs_pipe_push_number (zs_vm_output (self), number);
    }
    else {
        //  List function
        while (zs_pipe_size (zs_vm_input (self))) {
            int64_t number = zs_pipe_pop_number (zs_vm_input (self)) * 1000;
            zs_pipe_push_number (zs_vm_output (self), number);
        }
    }
    return 0;
}

static int
s_M (zs_vm_t *self)
{
    
    if (zs_vm_probing (self))
        zs_vm_register (self, "M", "Multiply by 1E6");
    else
    if (zs_pipe_size (zs_vm_output (self))) {
        //  Unary function
        int64_t number = zs_pipe_pop_number (zs_vm_output (self)) * 1000000;
        zs_pipe_push_number (zs_vm_output (self), number);
    }
    else {
        //  List function
        while (zs_pipe_size (zs_vm_input (self))) {
            int64_t number = zs_pipe_pop_number (zs_vm_input (self)) * 1000000;
            zs_pipe_push_number (zs_vm_output (self), number);
        }
    }
    return 0;
}

static int
s_G (zs_vm_t *self)
{
    
    if (zs_vm_probing (self))
        zs_vm_register (self, "G", "Multiply by 1E9");
    else
    if (zs_pipe_size (zs_vm_output (self))) {
        //  Unary function
        int64_t number = zs_pipe_pop_number (zs_vm_output (self)) * 1e9;
        zs_pipe_push_number (zs_vm_output (self), number);
    }
    else {
        //  List function
        while (zs_pipe_size (zs_vm_input (self))) {
            int64_t number = zs_pipe_pop_number (zs_vm_input (self)) * 1e9;
            zs_pipe_push_number (zs_vm_output (self), number);
        }
    }
    return 0;
}

static int
s_T (zs_vm_t *self)
{
    
    if (zs_vm_probing (self))
        zs_vm_register (self, "T", "Multiply by 1E12");
    else
    if (zs_pipe_size (zs_vm_output (self))) {
        //  Unary function
        int64_t number = zs_pipe_pop_number (zs_vm_output (self)) * 1e12;
        zs_pipe_push_number (zs_vm_output (self), number);
    }
    else {
        //  List function
        while (zs_pipe_size (zs_vm_input (self))) {
            int64_t number = zs_pipe_pop_number (zs_vm_input (self)) * 1e12;
            zs_pipe_push_number (zs_vm_output (self), number);
        }
    }
    return 0;
}

static int
s_P (zs_vm_t *self)
{
    
    if (zs_vm_probing (self))
        zs_vm_register (self, "P", "Multiply by 1E15");
    else
    if (zs_pipe_size (zs_vm_output (self))) {
        //  Unary function
        int64_t number = zs_pipe_pop_number (zs_vm_output (self)) * 1e15;
        zs_pipe_push_number (zs_vm_output (self), number);
    }
    else {
        //  List function
        while (zs_pipe_size (zs_vm_input (self))) {
            int64_t number = zs_pipe_pop_number (zs_vm_input (self)) * 1e15;
            zs_pipe_push_number (zs_vm_output (self), number);
        }
    }
    return 0;
}

static int
s_E (zs_vm_t *self)
{
    
    if (zs_vm_probing (self))
        zs_vm_register (self, "E", "Multiply by 1E18");
    else
    if (zs_pipe_size (zs_vm_output (self))) {
        //  Unary function
        int64_t number = zs_pipe_pop_number (zs_vm_output (self)) * 1e18;
        zs_pipe_push_number (zs_vm_output (self), number);
    }
    else {
        //  List function
        while (zs_pipe_size (zs_vm_input (self))) {
            int64_t number = zs_pipe_pop_number (zs_vm_input (self)) * 1e18;
            zs_pipe_push_number (zs_vm_output (self), number);
        }
    }
    return 0;
}

static int
s_Z (zs_vm_t *self)
{
    
    if (zs_vm_probing (self))
        zs_vm_register (self, "Z", "Multiply by 1E21");
    else
    if (zs_pipe_size (zs_vm_output (self))) {
        //  Unary function
        int64_t number = zs_pipe_pop_number (zs_vm_output (self)) * 1e21;
        zs_pipe_push_number (zs_vm_output (self), number);
    }
    else {
        //  List function
        while (zs_pipe_size (zs_vm_input (self))) {
            int64_t number = zs_pipe_pop_number (zs_vm_input (self)) * 1e21;
            zs_pipe_push_number (zs_vm_output (self), number);
        }
    }
    return 0;
}

static int
s_Y (zs_vm_t *self)
{
    
    if (zs_vm_probing (self))
        zs_vm_register (self, "Y", "Multiply by 1E24");
    else
    if (zs_pipe_size (zs_vm_output (self))) {
        //  Unary function
        int64_t number = zs_pipe_pop_number (zs_vm_output (self)) * 1e24;
        zs_pipe_push_number (zs_vm_output (self), number);
    }
    else {
        //  List function
        while (zs_pipe_size (zs_vm_input (self))) {
            int64_t number = zs_pipe_pop_number (zs_vm_input (self)) * 1e24;
            zs_pipe_push_number (zs_vm_output (self), number);
        }
    }
    return 0;
}



static void
s_register_zs_suffices (zs_vm_t *self)
{
    zs_vm_probe (self, s_Ki);
    zs_vm_probe (self, s_Mi);
    zs_vm_probe (self, s_Gi);
    zs_vm_probe (self, s_Ti);
    zs_vm_probe (self, s_Pi);
    zs_vm_probe (self, s_Ei);
    zs_vm_probe (self, s_da);
    zs_vm_probe (self, s_h);
    zs_vm_probe (self, s_k);
    zs_vm_probe (self, s_M);
    zs_vm_probe (self, s_G);
    zs_vm_probe (self, s_T);
    zs_vm_probe (self, s_P);
    zs_vm_probe (self, s_E);
    zs_vm_probe (self, s_Z);
    zs_vm_probe (self, s_Y);
}
#endif
