/*  =========================================================================
    zs_units_si - scaling atomics

    ** WARNING *************************************************************
    THIS SOURCE FILE IS 100% GENERATED. If you edit this file, you will lose
    your changes at the next build cycle. This is great for temporary printf
    statements. DO NOT MAKE ANY CHANGES YOU WISH TO KEEP. The correct places
    for commits are:

     * The XML model used for this code generation: zs_units_si.xml, or
     * The code generation script that built this file: zs_scaling
    ************************************************************************
    =========================================================================
*/

#ifndef ZS_UNITS_SI_H_INCLUDED
#define ZS_UNITS_SI_H_INCLUDED

#ifndef S_APPLY_SCALE_DEFINED
#define S_APPLY_SCALE_DEFINED
//  Applies a scale up to a pipe register
static void
s_scale_up (zs_pipe_t *input, zs_pipe_t *output, int64_t scale)
{
    if (zs_pipe_type (input) == 'w')
        zs_pipe_set_whole (output, zs_pipe_whole (input) * scale);
    else
    if (zs_pipe_type (input) == 'r')
        zs_pipe_set_real (output, zs_pipe_real (input) * (double) scale);
    else {
        //  String could be real or whole
        if ((double) zs_pipe_whole (input) == zs_pipe_real (input))
            zs_pipe_set_whole (output, zs_pipe_whole (input) * scale);
        else
            zs_pipe_set_real (output, zs_pipe_real (input) * scale);
    }
}
#endif

static int
s_Ki_up (zs_vm_t *self, zs_pipe_t *input, zs_pipe_t *output)
{
    if (zs_vm_probing (self)) {
        zs_vm_register (self, "Ki", zs_type_modest, "Scale up by 2^10");
    }
    else {
        //  Process all values on input pipe
        while (zs_pipe_recv (input)) {
            s_scale_up (input, output, (int64_t) (1024LL));
            zs_pipe_send (output);
        }
    }
    return 0;
}

static int
s_Ki_down (zs_vm_t *self, zs_pipe_t *input, zs_pipe_t *output)
{
    if (zs_vm_probing (self)) {
        zs_vm_register (self, "/Ki", zs_type_modest, "Scale down by 2^10");
    }
    else {
        //  Process all values on input pipe, always coerce to real
        while (zs_pipe_recv (input))
            zs_pipe_send_real (output, zs_pipe_real (input) / (double) (1024LL));
    }
    return 0;
}

static int
s_Mi_up (zs_vm_t *self, zs_pipe_t *input, zs_pipe_t *output)
{
    if (zs_vm_probing (self)) {
        zs_vm_register (self, "Mi", zs_type_modest, "Scale up by 2^20");
    }
    else {
        //  Process all values on input pipe
        while (zs_pipe_recv (input)) {
            s_scale_up (input, output, (int64_t) (1024LL * 1024LL));
            zs_pipe_send (output);
        }
    }
    return 0;
}

static int
s_Mi_down (zs_vm_t *self, zs_pipe_t *input, zs_pipe_t *output)
{
    if (zs_vm_probing (self)) {
        zs_vm_register (self, "/Mi", zs_type_modest, "Scale down by 2^20");
    }
    else {
        //  Process all values on input pipe, always coerce to real
        while (zs_pipe_recv (input))
            zs_pipe_send_real (output, zs_pipe_real (input) / (double) (1024LL * 1024LL));
    }
    return 0;
}

static int
s_Gi_up (zs_vm_t *self, zs_pipe_t *input, zs_pipe_t *output)
{
    if (zs_vm_probing (self)) {
        zs_vm_register (self, "Gi", zs_type_modest, "Scale up by 2^30");
    }
    else {
        //  Process all values on input pipe
        while (zs_pipe_recv (input)) {
            s_scale_up (input, output, (int64_t) (1024LL * 1024LL * 1024LL));
            zs_pipe_send (output);
        }
    }
    return 0;
}

static int
s_Gi_down (zs_vm_t *self, zs_pipe_t *input, zs_pipe_t *output)
{
    if (zs_vm_probing (self)) {
        zs_vm_register (self, "/Gi", zs_type_modest, "Scale down by 2^30");
    }
    else {
        //  Process all values on input pipe, always coerce to real
        while (zs_pipe_recv (input))
            zs_pipe_send_real (output, zs_pipe_real (input) / (double) (1024LL * 1024LL * 1024LL));
    }
    return 0;
}

static int
s_Ti_up (zs_vm_t *self, zs_pipe_t *input, zs_pipe_t *output)
{
    if (zs_vm_probing (self)) {
        zs_vm_register (self, "Ti", zs_type_modest, "Scale up by 2^40");
    }
    else {
        //  Process all values on input pipe
        while (zs_pipe_recv (input)) {
            s_scale_up (input, output, (int64_t) (1024LL * 1024LL * 1024LL * 1024LL));
            zs_pipe_send (output);
        }
    }
    return 0;
}

static int
s_Ti_down (zs_vm_t *self, zs_pipe_t *input, zs_pipe_t *output)
{
    if (zs_vm_probing (self)) {
        zs_vm_register (self, "/Ti", zs_type_modest, "Scale down by 2^40");
    }
    else {
        //  Process all values on input pipe, always coerce to real
        while (zs_pipe_recv (input))
            zs_pipe_send_real (output, zs_pipe_real (input) / (double) (1024LL * 1024LL * 1024LL * 1024LL));
    }
    return 0;
}

static int
s_Pi_up (zs_vm_t *self, zs_pipe_t *input, zs_pipe_t *output)
{
    if (zs_vm_probing (self)) {
        zs_vm_register (self, "Pi", zs_type_modest, "Scale up by 2^50");
    }
    else {
        //  Process all values on input pipe
        while (zs_pipe_recv (input)) {
            s_scale_up (input, output, (int64_t) (1024LL * 1024LL * 1024LL * 1024LL * 1024LL));
            zs_pipe_send (output);
        }
    }
    return 0;
}

static int
s_Pi_down (zs_vm_t *self, zs_pipe_t *input, zs_pipe_t *output)
{
    if (zs_vm_probing (self)) {
        zs_vm_register (self, "/Pi", zs_type_modest, "Scale down by 2^50");
    }
    else {
        //  Process all values on input pipe, always coerce to real
        while (zs_pipe_recv (input))
            zs_pipe_send_real (output, zs_pipe_real (input) / (double) (1024LL * 1024LL * 1024LL * 1024LL * 1024LL));
    }
    return 0;
}

static int
s_Ei_up (zs_vm_t *self, zs_pipe_t *input, zs_pipe_t *output)
{
    if (zs_vm_probing (self)) {
        zs_vm_register (self, "Ei", zs_type_modest, "Scale up by 2^60");
    }
    else {
        //  Process all values on input pipe
        while (zs_pipe_recv (input)) {
            s_scale_up (input, output, (int64_t) (1024LL * 1024LL * 1024LL * 1024LL * 1024LL * 1024LL));
            zs_pipe_send (output);
        }
    }
    return 0;
}

static int
s_Ei_down (zs_vm_t *self, zs_pipe_t *input, zs_pipe_t *output)
{
    if (zs_vm_probing (self)) {
        zs_vm_register (self, "/Ei", zs_type_modest, "Scale down by 2^60");
    }
    else {
        //  Process all values on input pipe, always coerce to real
        while (zs_pipe_recv (input))
            zs_pipe_send_real (output, zs_pipe_real (input) / (double) (1024LL * 1024LL * 1024LL * 1024LL * 1024LL * 1024LL));
    }
    return 0;
}

static int
s_da_up (zs_vm_t *self, zs_pipe_t *input, zs_pipe_t *output)
{
    if (zs_vm_probing (self)) {
        zs_vm_register (self, "da", zs_type_modest, "Scale up by 10");
    }
    else {
        //  Process all values on input pipe
        while (zs_pipe_recv (input)) {
            s_scale_up (input, output, (int64_t) (10));
            zs_pipe_send (output);
        }
    }
    return 0;
}

static int
s_da_down (zs_vm_t *self, zs_pipe_t *input, zs_pipe_t *output)
{
    if (zs_vm_probing (self)) {
        zs_vm_register (self, "/da", zs_type_modest, "Scale down by 10");
    }
    else {
        //  Process all values on input pipe, always coerce to real
        while (zs_pipe_recv (input))
            zs_pipe_send_real (output, zs_pipe_real (input) / (double) (10));
    }
    return 0;
}

static int
s_h_up (zs_vm_t *self, zs_pipe_t *input, zs_pipe_t *output)
{
    if (zs_vm_probing (self)) {
        zs_vm_register (self, "h", zs_type_modest, "Scale up by 100");
    }
    else {
        //  Process all values on input pipe
        while (zs_pipe_recv (input)) {
            s_scale_up (input, output, (int64_t) (100));
            zs_pipe_send (output);
        }
    }
    return 0;
}

static int
s_h_down (zs_vm_t *self, zs_pipe_t *input, zs_pipe_t *output)
{
    if (zs_vm_probing (self)) {
        zs_vm_register (self, "/h", zs_type_modest, "Scale down by 100");
    }
    else {
        //  Process all values on input pipe, always coerce to real
        while (zs_pipe_recv (input))
            zs_pipe_send_real (output, zs_pipe_real (input) / (double) (100));
    }
    return 0;
}

static int
s_k_up (zs_vm_t *self, zs_pipe_t *input, zs_pipe_t *output)
{
    if (zs_vm_probing (self)) {
        zs_vm_register (self, "k", zs_type_modest, "Scale up by 1000");
    }
    else {
        //  Process all values on input pipe
        while (zs_pipe_recv (input)) {
            s_scale_up (input, output, (int64_t) (1000));
            zs_pipe_send (output);
        }
    }
    return 0;
}

static int
s_k_down (zs_vm_t *self, zs_pipe_t *input, zs_pipe_t *output)
{
    if (zs_vm_probing (self)) {
        zs_vm_register (self, "/k", zs_type_modest, "Scale down by 1000");
    }
    else {
        //  Process all values on input pipe, always coerce to real
        while (zs_pipe_recv (input))
            zs_pipe_send_real (output, zs_pipe_real (input) / (double) (1000));
    }
    return 0;
}

static int
s_M_up (zs_vm_t *self, zs_pipe_t *input, zs_pipe_t *output)
{
    if (zs_vm_probing (self)) {
        zs_vm_register (self, "M", zs_type_modest, "Scale up by 10^6");
    }
    else {
        //  Process all values on input pipe
        while (zs_pipe_recv (input)) {
            s_scale_up (input, output, (int64_t) (1000000));
            zs_pipe_send (output);
        }
    }
    return 0;
}

static int
s_M_down (zs_vm_t *self, zs_pipe_t *input, zs_pipe_t *output)
{
    if (zs_vm_probing (self)) {
        zs_vm_register (self, "/M", zs_type_modest, "Scale down by 10^6");
    }
    else {
        //  Process all values on input pipe, always coerce to real
        while (zs_pipe_recv (input))
            zs_pipe_send_real (output, zs_pipe_real (input) / (double) (1000000));
    }
    return 0;
}

static int
s_G_up (zs_vm_t *self, zs_pipe_t *input, zs_pipe_t *output)
{
    if (zs_vm_probing (self)) {
        zs_vm_register (self, "G", zs_type_modest, "Scale up by 10^9");
    }
    else {
        //  Process all values on input pipe
        while (zs_pipe_recv (input)) {
            s_scale_up (input, output, (int64_t) (1E9));
            zs_pipe_send (output);
        }
    }
    return 0;
}

static int
s_G_down (zs_vm_t *self, zs_pipe_t *input, zs_pipe_t *output)
{
    if (zs_vm_probing (self)) {
        zs_vm_register (self, "/G", zs_type_modest, "Scale down by 10^9");
    }
    else {
        //  Process all values on input pipe, always coerce to real
        while (zs_pipe_recv (input))
            zs_pipe_send_real (output, zs_pipe_real (input) / (double) (1E9));
    }
    return 0;
}

static int
s_T_up (zs_vm_t *self, zs_pipe_t *input, zs_pipe_t *output)
{
    if (zs_vm_probing (self)) {
        zs_vm_register (self, "T", zs_type_modest, "Scale up by 10^12");
    }
    else {
        //  Process all values on input pipe
        while (zs_pipe_recv (input)) {
            s_scale_up (input, output, (int64_t) (1E12));
            zs_pipe_send (output);
        }
    }
    return 0;
}

static int
s_T_down (zs_vm_t *self, zs_pipe_t *input, zs_pipe_t *output)
{
    if (zs_vm_probing (self)) {
        zs_vm_register (self, "/T", zs_type_modest, "Scale down by 10^12");
    }
    else {
        //  Process all values on input pipe, always coerce to real
        while (zs_pipe_recv (input))
            zs_pipe_send_real (output, zs_pipe_real (input) / (double) (1E12));
    }
    return 0;
}

static int
s_P_up (zs_vm_t *self, zs_pipe_t *input, zs_pipe_t *output)
{
    if (zs_vm_probing (self)) {
        zs_vm_register (self, "P", zs_type_modest, "Scale up by 10^15");
    }
    else {
        //  Process all values on input pipe
        while (zs_pipe_recv (input)) {
            s_scale_up (input, output, (int64_t) (1E15));
            zs_pipe_send (output);
        }
    }
    return 0;
}

static int
s_P_down (zs_vm_t *self, zs_pipe_t *input, zs_pipe_t *output)
{
    if (zs_vm_probing (self)) {
        zs_vm_register (self, "/P", zs_type_modest, "Scale down by 10^15");
    }
    else {
        //  Process all values on input pipe, always coerce to real
        while (zs_pipe_recv (input))
            zs_pipe_send_real (output, zs_pipe_real (input) / (double) (1E15));
    }
    return 0;
}

static int
s_E_up (zs_vm_t *self, zs_pipe_t *input, zs_pipe_t *output)
{
    if (zs_vm_probing (self)) {
        zs_vm_register (self, "E", zs_type_modest, "Scale up by 10^18");
    }
    else {
        //  Process all values on input pipe
        while (zs_pipe_recv (input)) {
            s_scale_up (input, output, (int64_t) (1E18));
            zs_pipe_send (output);
        }
    }
    return 0;
}

static int
s_E_down (zs_vm_t *self, zs_pipe_t *input, zs_pipe_t *output)
{
    if (zs_vm_probing (self)) {
        zs_vm_register (self, "/E", zs_type_modest, "Scale down by 10^18");
    }
    else {
        //  Process all values on input pipe, always coerce to real
        while (zs_pipe_recv (input))
            zs_pipe_send_real (output, zs_pipe_real (input) / (double) (1E18));
    }
    return 0;
}

static int
s_Z_up (zs_vm_t *self, zs_pipe_t *input, zs_pipe_t *output)
{
    if (zs_vm_probing (self)) {
        zs_vm_register (self, "Z", zs_type_modest, "Scale up by 10^21");
    }
    else {
        //  Process all values on input pipe
        while (zs_pipe_recv (input)) {
            //  Always coerce to a real value
            zs_pipe_set_real (output, zs_pipe_real (input) * 1E21);
            zs_pipe_send (output);
        }
    }
    return 0;
}

static int
s_Z_down (zs_vm_t *self, zs_pipe_t *input, zs_pipe_t *output)
{
    if (zs_vm_probing (self)) {
        zs_vm_register (self, "/Z", zs_type_modest, "Scale down by 10^21");
    }
    else {
        //  Process all values on input pipe, always coerce to real
        while (zs_pipe_recv (input))
            zs_pipe_send_real (output, zs_pipe_real (input) / (double) (1E21));
    }
    return 0;
}

static int
s_Y_up (zs_vm_t *self, zs_pipe_t *input, zs_pipe_t *output)
{
    if (zs_vm_probing (self)) {
        zs_vm_register (self, "Y", zs_type_modest, "Scale up by 10^24");
    }
    else {
        //  Process all values on input pipe
        while (zs_pipe_recv (input)) {
            //  Always coerce to a real value
            zs_pipe_set_real (output, zs_pipe_real (input) * 1E24);
            zs_pipe_send (output);
        }
    }
    return 0;
}

static int
s_Y_down (zs_vm_t *self, zs_pipe_t *input, zs_pipe_t *output)
{
    if (zs_vm_probing (self)) {
        zs_vm_register (self, "/Y", zs_type_modest, "Scale down by 10^24");
    }
    else {
        //  Process all values on input pipe, always coerce to real
        while (zs_pipe_recv (input))
            zs_pipe_send_real (output, zs_pipe_real (input) / (double) (1E24));
    }
    return 0;
}

static int
s_d_up (zs_vm_t *self, zs_pipe_t *input, zs_pipe_t *output)
{
    if (zs_vm_probing (self)) {
        zs_vm_register (self, "d", zs_type_modest, "Scale up by 1/10");
    }
    else {
        //  Process all values on input pipe
        while (zs_pipe_recv (input)) {
            //  Always coerce to a real value
            zs_pipe_set_real (output, zs_pipe_real (input) * 0.1);
            zs_pipe_send (output);
        }
    }
    return 0;
}

static int
s_d_down (zs_vm_t *self, zs_pipe_t *input, zs_pipe_t *output)
{
    if (zs_vm_probing (self)) {
        zs_vm_register (self, "/d", zs_type_modest, "Scale down by 1/10");
    }
    else {
        //  Process all values on input pipe, always coerce to real
        while (zs_pipe_recv (input))
            zs_pipe_send_real (output, zs_pipe_real (input) / (double) (0.1));
    }
    return 0;
}

static int
s_c_up (zs_vm_t *self, zs_pipe_t *input, zs_pipe_t *output)
{
    if (zs_vm_probing (self)) {
        zs_vm_register (self, "c", zs_type_modest, "Scale up by 1/100");
    }
    else {
        //  Process all values on input pipe
        while (zs_pipe_recv (input)) {
            //  Always coerce to a real value
            zs_pipe_set_real (output, zs_pipe_real (input) * 0.01);
            zs_pipe_send (output);
        }
    }
    return 0;
}

static int
s_c_down (zs_vm_t *self, zs_pipe_t *input, zs_pipe_t *output)
{
    if (zs_vm_probing (self)) {
        zs_vm_register (self, "/c", zs_type_modest, "Scale down by 1/100");
    }
    else {
        //  Process all values on input pipe, always coerce to real
        while (zs_pipe_recv (input))
            zs_pipe_send_real (output, zs_pipe_real (input) / (double) (0.01));
    }
    return 0;
}

static int
s_m_up (zs_vm_t *self, zs_pipe_t *input, zs_pipe_t *output)
{
    if (zs_vm_probing (self)) {
        zs_vm_register (self, "m", zs_type_modest, "Scale up by 1/1000");
    }
    else {
        //  Process all values on input pipe
        while (zs_pipe_recv (input)) {
            //  Always coerce to a real value
            zs_pipe_set_real (output, zs_pipe_real (input) * 1E-3);
            zs_pipe_send (output);
        }
    }
    return 0;
}

static int
s_m_down (zs_vm_t *self, zs_pipe_t *input, zs_pipe_t *output)
{
    if (zs_vm_probing (self)) {
        zs_vm_register (self, "/m", zs_type_modest, "Scale down by 1/1000");
    }
    else {
        //  Process all values on input pipe, always coerce to real
        while (zs_pipe_recv (input))
            zs_pipe_send_real (output, zs_pipe_real (input) / (double) (1E-3));
    }
    return 0;
}

static int
s_u_up (zs_vm_t *self, zs_pipe_t *input, zs_pipe_t *output)
{
    if (zs_vm_probing (self)) {
        zs_vm_register (self, "u", zs_type_modest, "Scale up by 1/10^6");
    }
    else {
        //  Process all values on input pipe
        while (zs_pipe_recv (input)) {
            //  Always coerce to a real value
            zs_pipe_set_real (output, zs_pipe_real (input) * 1E-6);
            zs_pipe_send (output);
        }
    }
    return 0;
}

static int
s_u_down (zs_vm_t *self, zs_pipe_t *input, zs_pipe_t *output)
{
    if (zs_vm_probing (self)) {
        zs_vm_register (self, "/u", zs_type_modest, "Scale down by 1/10^6");
    }
    else {
        //  Process all values on input pipe, always coerce to real
        while (zs_pipe_recv (input))
            zs_pipe_send_real (output, zs_pipe_real (input) / (double) (1E-6));
    }
    return 0;
}

static int
s_n_up (zs_vm_t *self, zs_pipe_t *input, zs_pipe_t *output)
{
    if (zs_vm_probing (self)) {
        zs_vm_register (self, "n", zs_type_modest, "Scale up by 1/10^9");
    }
    else {
        //  Process all values on input pipe
        while (zs_pipe_recv (input)) {
            //  Always coerce to a real value
            zs_pipe_set_real (output, zs_pipe_real (input) * 1E-9);
            zs_pipe_send (output);
        }
    }
    return 0;
}

static int
s_n_down (zs_vm_t *self, zs_pipe_t *input, zs_pipe_t *output)
{
    if (zs_vm_probing (self)) {
        zs_vm_register (self, "/n", zs_type_modest, "Scale down by 1/10^9");
    }
    else {
        //  Process all values on input pipe, always coerce to real
        while (zs_pipe_recv (input))
            zs_pipe_send_real (output, zs_pipe_real (input) / (double) (1E-9));
    }
    return 0;
}

static int
s_p_up (zs_vm_t *self, zs_pipe_t *input, zs_pipe_t *output)
{
    if (zs_vm_probing (self)) {
        zs_vm_register (self, "p", zs_type_modest, "Scale up by 1/10^12");
    }
    else {
        //  Process all values on input pipe
        while (zs_pipe_recv (input)) {
            //  Always coerce to a real value
            zs_pipe_set_real (output, zs_pipe_real (input) * 1E-12);
            zs_pipe_send (output);
        }
    }
    return 0;
}

static int
s_p_down (zs_vm_t *self, zs_pipe_t *input, zs_pipe_t *output)
{
    if (zs_vm_probing (self)) {
        zs_vm_register (self, "/p", zs_type_modest, "Scale down by 1/10^12");
    }
    else {
        //  Process all values on input pipe, always coerce to real
        while (zs_pipe_recv (input))
            zs_pipe_send_real (output, zs_pipe_real (input) / (double) (1E-12));
    }
    return 0;
}

static int
s_f_up (zs_vm_t *self, zs_pipe_t *input, zs_pipe_t *output)
{
    if (zs_vm_probing (self)) {
        zs_vm_register (self, "f", zs_type_modest, "Scale up by 1/10^15");
    }
    else {
        //  Process all values on input pipe
        while (zs_pipe_recv (input)) {
            //  Always coerce to a real value
            zs_pipe_set_real (output, zs_pipe_real (input) * 1E-15);
            zs_pipe_send (output);
        }
    }
    return 0;
}

static int
s_f_down (zs_vm_t *self, zs_pipe_t *input, zs_pipe_t *output)
{
    if (zs_vm_probing (self)) {
        zs_vm_register (self, "/f", zs_type_modest, "Scale down by 1/10^15");
    }
    else {
        //  Process all values on input pipe, always coerce to real
        while (zs_pipe_recv (input))
            zs_pipe_send_real (output, zs_pipe_real (input) / (double) (1E-15));
    }
    return 0;
}

static int
s_a_up (zs_vm_t *self, zs_pipe_t *input, zs_pipe_t *output)
{
    if (zs_vm_probing (self)) {
        zs_vm_register (self, "a", zs_type_modest, "Scale up by 1/10^18");
    }
    else {
        //  Process all values on input pipe
        while (zs_pipe_recv (input)) {
            //  Always coerce to a real value
            zs_pipe_set_real (output, zs_pipe_real (input) * 1E-18);
            zs_pipe_send (output);
        }
    }
    return 0;
}

static int
s_a_down (zs_vm_t *self, zs_pipe_t *input, zs_pipe_t *output)
{
    if (zs_vm_probing (self)) {
        zs_vm_register (self, "/a", zs_type_modest, "Scale down by 1/10^18");
    }
    else {
        //  Process all values on input pipe, always coerce to real
        while (zs_pipe_recv (input))
            zs_pipe_send_real (output, zs_pipe_real (input) / (double) (1E-18));
    }
    return 0;
}

static int
s_z_up (zs_vm_t *self, zs_pipe_t *input, zs_pipe_t *output)
{
    if (zs_vm_probing (self)) {
        zs_vm_register (self, "z", zs_type_modest, "Scale up by 1/10^21");
    }
    else {
        //  Process all values on input pipe
        while (zs_pipe_recv (input)) {
            //  Always coerce to a real value
            zs_pipe_set_real (output, zs_pipe_real (input) * 1E-21);
            zs_pipe_send (output);
        }
    }
    return 0;
}

static int
s_z_down (zs_vm_t *self, zs_pipe_t *input, zs_pipe_t *output)
{
    if (zs_vm_probing (self)) {
        zs_vm_register (self, "/z", zs_type_modest, "Scale down by 1/10^21");
    }
    else {
        //  Process all values on input pipe, always coerce to real
        while (zs_pipe_recv (input))
            zs_pipe_send_real (output, zs_pipe_real (input) / (double) (1E-21));
    }
    return 0;
}

static int
s_y_up (zs_vm_t *self, zs_pipe_t *input, zs_pipe_t *output)
{
    if (zs_vm_probing (self)) {
        zs_vm_register (self, "y", zs_type_modest, "Scale up by 1/10^24");
    }
    else {
        //  Process all values on input pipe
        while (zs_pipe_recv (input)) {
            //  Always coerce to a real value
            zs_pipe_set_real (output, zs_pipe_real (input) * 1E-24);
            zs_pipe_send (output);
        }
    }
    return 0;
}

static int
s_y_down (zs_vm_t *self, zs_pipe_t *input, zs_pipe_t *output)
{
    if (zs_vm_probing (self)) {
        zs_vm_register (self, "/y", zs_type_modest, "Scale down by 1/10^24");
    }
    else {
        //  Process all values on input pipe, always coerce to real
        while (zs_pipe_recv (input))
            zs_pipe_send_real (output, zs_pipe_real (input) / (double) (1E-24));
    }
    return 0;
}

static void
s_register_zs_units_si (zs_vm_t *self)
{
    zs_vm_probe (self, s_Ki_up);
    zs_vm_probe (self, s_Ki_down);
    zs_vm_probe (self, s_Mi_up);
    zs_vm_probe (self, s_Mi_down);
    zs_vm_probe (self, s_Gi_up);
    zs_vm_probe (self, s_Gi_down);
    zs_vm_probe (self, s_Ti_up);
    zs_vm_probe (self, s_Ti_down);
    zs_vm_probe (self, s_Pi_up);
    zs_vm_probe (self, s_Pi_down);
    zs_vm_probe (self, s_Ei_up);
    zs_vm_probe (self, s_Ei_down);
    zs_vm_probe (self, s_da_up);
    zs_vm_probe (self, s_da_down);
    zs_vm_probe (self, s_h_up);
    zs_vm_probe (self, s_h_down);
    zs_vm_probe (self, s_k_up);
    zs_vm_probe (self, s_k_down);
    zs_vm_probe (self, s_M_up);
    zs_vm_probe (self, s_M_down);
    zs_vm_probe (self, s_G_up);
    zs_vm_probe (self, s_G_down);
    zs_vm_probe (self, s_T_up);
    zs_vm_probe (self, s_T_down);
    zs_vm_probe (self, s_P_up);
    zs_vm_probe (self, s_P_down);
    zs_vm_probe (self, s_E_up);
    zs_vm_probe (self, s_E_down);
    zs_vm_probe (self, s_Z_up);
    zs_vm_probe (self, s_Z_down);
    zs_vm_probe (self, s_Y_up);
    zs_vm_probe (self, s_Y_down);
    zs_vm_probe (self, s_d_up);
    zs_vm_probe (self, s_d_down);
    zs_vm_probe (self, s_c_up);
    zs_vm_probe (self, s_c_down);
    zs_vm_probe (self, s_m_up);
    zs_vm_probe (self, s_m_down);
    zs_vm_probe (self, s_u_up);
    zs_vm_probe (self, s_u_down);
    zs_vm_probe (self, s_n_up);
    zs_vm_probe (self, s_n_down);
    zs_vm_probe (self, s_p_up);
    zs_vm_probe (self, s_p_down);
    zs_vm_probe (self, s_f_up);
    zs_vm_probe (self, s_f_down);
    zs_vm_probe (self, s_a_up);
    zs_vm_probe (self, s_a_down);
    zs_vm_probe (self, s_z_up);
    zs_vm_probe (self, s_z_down);
    zs_vm_probe (self, s_y_up);
    zs_vm_probe (self, s_y_down);
}
#endif
