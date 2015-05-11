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
        int64_t whole = zs_pipe_whole_pull (zs_vm_output (self));
        if (whole != ZS_NULL_WHOLE)
            zs_pipe_whole_send (zs_vm_output (self), whole * 1024);
    }
    else {
        //  List function
        while (zs_pipe_size (zs_vm_input (self))) {
            int64_t whole = zs_pipe_whole_recv (zs_vm_input (self));
            if (whole != ZS_NULL_WHOLE)
                zs_pipe_whole_send (zs_vm_output (self), whole * 1024);
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
        int64_t whole = zs_pipe_whole_pull (zs_vm_output (self));
        if (whole != ZS_NULL_WHOLE)
            zs_pipe_whole_send (zs_vm_output (self), whole * 1024 * 1024);
    }
    else {
        //  List function
        while (zs_pipe_size (zs_vm_input (self))) {
            int64_t whole = zs_pipe_whole_recv (zs_vm_input (self));
            if (whole != ZS_NULL_WHOLE)
                zs_pipe_whole_send (zs_vm_output (self), whole * 1024 * 1024);
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
        int64_t whole = zs_pipe_whole_pull (zs_vm_output (self));
        if (whole != ZS_NULL_WHOLE)
            zs_pipe_whole_send (zs_vm_output (self), whole * 1024 * 1024 * 1024);
    }
    else {
        //  List function
        while (zs_pipe_size (zs_vm_input (self))) {
            int64_t whole = zs_pipe_whole_recv (zs_vm_input (self));
            if (whole != ZS_NULL_WHOLE)
                zs_pipe_whole_send (zs_vm_output (self), whole * 1024 * 1024 * 1024);
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
        int64_t whole = zs_pipe_whole_pull (zs_vm_output (self));
        if (whole != ZS_NULL_WHOLE)
            zs_pipe_whole_send (zs_vm_output (self), whole * 1024 * 1024 * 1024 * 1024);
    }
    else {
        //  List function
        while (zs_pipe_size (zs_vm_input (self))) {
            int64_t whole = zs_pipe_whole_recv (zs_vm_input (self));
            if (whole != ZS_NULL_WHOLE)
                zs_pipe_whole_send (zs_vm_output (self), whole * 1024 * 1024 * 1024 * 1024);
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
        int64_t whole = zs_pipe_whole_pull (zs_vm_output (self));
        if (whole != ZS_NULL_WHOLE)
            zs_pipe_whole_send (zs_vm_output (self), whole * 1024 * 1024 * 1024 * 1024 * 1024);
    }
    else {
        //  List function
        while (zs_pipe_size (zs_vm_input (self))) {
            int64_t whole = zs_pipe_whole_recv (zs_vm_input (self));
            if (whole != ZS_NULL_WHOLE)
                zs_pipe_whole_send (zs_vm_output (self), whole * 1024 * 1024 * 1024 * 1024 * 1024);
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
        int64_t whole = zs_pipe_whole_pull (zs_vm_output (self));
        if (whole != ZS_NULL_WHOLE)
            zs_pipe_whole_send (zs_vm_output (self), whole * 1024 * 1024 * 1024 * 1024 * 1024 * 1024);
    }
    else {
        //  List function
        while (zs_pipe_size (zs_vm_input (self))) {
            int64_t whole = zs_pipe_whole_recv (zs_vm_input (self));
            if (whole != ZS_NULL_WHOLE)
                zs_pipe_whole_send (zs_vm_output (self), whole * 1024 * 1024 * 1024 * 1024 * 1024 * 1024);
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
        int64_t whole = zs_pipe_whole_pull (zs_vm_output (self));
        if (whole != ZS_NULL_WHOLE)
            zs_pipe_whole_send (zs_vm_output (self), whole * 10);
    }
    else {
        //  List function
        while (zs_pipe_size (zs_vm_input (self))) {
            int64_t whole = zs_pipe_whole_recv (zs_vm_input (self));
            if (whole != ZS_NULL_WHOLE)
                zs_pipe_whole_send (zs_vm_output (self), whole * 10);
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
        int64_t whole = zs_pipe_whole_pull (zs_vm_output (self));
        if (whole != ZS_NULL_WHOLE)
            zs_pipe_whole_send (zs_vm_output (self), whole * 100);
    }
    else {
        //  List function
        while (zs_pipe_size (zs_vm_input (self))) {
            int64_t whole = zs_pipe_whole_recv (zs_vm_input (self));
            if (whole != ZS_NULL_WHOLE)
                zs_pipe_whole_send (zs_vm_output (self), whole * 100);
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
        int64_t whole = zs_pipe_whole_pull (zs_vm_output (self));
        if (whole != ZS_NULL_WHOLE)
            zs_pipe_whole_send (zs_vm_output (self), whole * 1000);
    }
    else {
        //  List function
        while (zs_pipe_size (zs_vm_input (self))) {
            int64_t whole = zs_pipe_whole_recv (zs_vm_input (self));
            if (whole != ZS_NULL_WHOLE)
                zs_pipe_whole_send (zs_vm_output (self), whole * 1000);
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
        int64_t whole = zs_pipe_whole_pull (zs_vm_output (self));
        if (whole != ZS_NULL_WHOLE)
            zs_pipe_whole_send (zs_vm_output (self), whole * 1000000);
    }
    else {
        //  List function
        while (zs_pipe_size (zs_vm_input (self))) {
            int64_t whole = zs_pipe_whole_recv (zs_vm_input (self));
            if (whole != ZS_NULL_WHOLE)
                zs_pipe_whole_send (zs_vm_output (self), whole * 1000000);
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
        int64_t whole = zs_pipe_whole_pull (zs_vm_output (self));
        if (whole != ZS_NULL_WHOLE)
            zs_pipe_whole_send (zs_vm_output (self), whole * 1e9);
    }
    else {
        //  List function
        while (zs_pipe_size (zs_vm_input (self))) {
            int64_t whole = zs_pipe_whole_recv (zs_vm_input (self));
            if (whole != ZS_NULL_WHOLE)
                zs_pipe_whole_send (zs_vm_output (self), whole * 1e9);
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
        int64_t whole = zs_pipe_whole_pull (zs_vm_output (self));
        if (whole != ZS_NULL_WHOLE)
            zs_pipe_whole_send (zs_vm_output (self), whole * 1e12);
    }
    else {
        //  List function
        while (zs_pipe_size (zs_vm_input (self))) {
            int64_t whole = zs_pipe_whole_recv (zs_vm_input (self));
            if (whole != ZS_NULL_WHOLE)
                zs_pipe_whole_send (zs_vm_output (self), whole * 1e12);
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
        int64_t whole = zs_pipe_whole_pull (zs_vm_output (self));
        if (whole != ZS_NULL_WHOLE)
            zs_pipe_whole_send (zs_vm_output (self), whole * 1e15);
    }
    else {
        //  List function
        while (zs_pipe_size (zs_vm_input (self))) {
            int64_t whole = zs_pipe_whole_recv (zs_vm_input (self));
            if (whole != ZS_NULL_WHOLE)
                zs_pipe_whole_send (zs_vm_output (self), whole * 1e15);
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
        int64_t whole = zs_pipe_whole_pull (zs_vm_output (self));
        if (whole != ZS_NULL_WHOLE)
            zs_pipe_whole_send (zs_vm_output (self), whole * 1e18);
    }
    else {
        //  List function
        while (zs_pipe_size (zs_vm_input (self))) {
            int64_t whole = zs_pipe_whole_recv (zs_vm_input (self));
            if (whole != ZS_NULL_WHOLE)
                zs_pipe_whole_send (zs_vm_output (self), whole * 1e18);
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
        int64_t whole = zs_pipe_whole_pull (zs_vm_output (self));
        if (whole != ZS_NULL_WHOLE)
            zs_pipe_whole_send (zs_vm_output (self), whole * 1e21);
    }
    else {
        //  List function
        while (zs_pipe_size (zs_vm_input (self))) {
            int64_t whole = zs_pipe_whole_recv (zs_vm_input (self));
            if (whole != ZS_NULL_WHOLE)
                zs_pipe_whole_send (zs_vm_output (self), whole * 1e21);
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
        int64_t whole = zs_pipe_whole_pull (zs_vm_output (self));
        if (whole != ZS_NULL_WHOLE)
            zs_pipe_whole_send (zs_vm_output (self), whole * 1e24);
    }
    else {
        //  List function
        while (zs_pipe_size (zs_vm_input (self))) {
            int64_t whole = zs_pipe_whole_recv (zs_vm_input (self));
            if (whole != ZS_NULL_WHOLE)
                zs_pipe_whole_send (zs_vm_output (self), whole * 1e24);
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
