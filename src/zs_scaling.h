/*  =========================================================================
    zs_scaling - scaling atomics

    ** WARNING *************************************************************
    THIS SOURCE FILE IS 100% GENERATED. If you edit this file, you will lose
    your changes at the next build cycle. This is great for temporary printf
    statements. DO NOT MAKE ANY CHANGES YOU WISH TO KEEP. The correct places
    for commits are:

     * The XML model used for this code generation: zs_scaling.xml, or
     * The code generation script that built this file: zs_scaling
    ************************************************************************
    =========================================================================
*/

#ifndef ZS_SCALING_H_INCLUDED
#define ZS_SCALING_H_INCLUDED

//  Applies a whole scale to a pipe register
static void
s_apply_scale (zs_pipe_t *input, zs_pipe_t *output, int64_t scale)
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


static int
s_Ki (zs_vm_t *self)
{
    zs_pipe_t *output = zs_vm_output (self);
    if (zs_vm_probing (self))
        zs_vm_register (self, "Ki", "Multiply by 2^10");
    else
    if (zs_pipe_size (zs_vm_input (self))) {
        zs_pipe_t *input = zs_vm_input (self);
        //  List function; recv and process all values
        while (zs_pipe_recv (input) == 0) {
            s_apply_scale (input, output, (int64_t) (1024LL));
            zs_pipe_send (output);
        }
    }
    else
    if (zs_pipe_size (output)) {
        //  Unary function; pull last value off pipe
        zs_pipe_pull (output);
        s_apply_scale (output, output, (int64_t) (1024LL));
        zs_pipe_send (output);
    }
    else
        //  Constant; send multipler as value
        zs_pipe_send_whole (output, (int64_t) (1024LL));
    return 0;
}

static int
s_Mi (zs_vm_t *self)
{
    zs_pipe_t *output = zs_vm_output (self);
    if (zs_vm_probing (self))
        zs_vm_register (self, "Mi", "Multiply by 2^20");
    else
    if (zs_pipe_size (zs_vm_input (self))) {
        zs_pipe_t *input = zs_vm_input (self);
        //  List function; recv and process all values
        while (zs_pipe_recv (input) == 0) {
            s_apply_scale (input, output, (int64_t) (1024LL * 1024LL));
            zs_pipe_send (output);
        }
    }
    else
    if (zs_pipe_size (output)) {
        //  Unary function; pull last value off pipe
        zs_pipe_pull (output);
        s_apply_scale (output, output, (int64_t) (1024LL * 1024LL));
        zs_pipe_send (output);
    }
    else
        //  Constant; send multipler as value
        zs_pipe_send_whole (output, (int64_t) (1024LL * 1024LL));
    return 0;
}

static int
s_Gi (zs_vm_t *self)
{
    zs_pipe_t *output = zs_vm_output (self);
    if (zs_vm_probing (self))
        zs_vm_register (self, "Gi", "Multiply by 2^30");
    else
    if (zs_pipe_size (zs_vm_input (self))) {
        zs_pipe_t *input = zs_vm_input (self);
        //  List function; recv and process all values
        while (zs_pipe_recv (input) == 0) {
            s_apply_scale (input, output, (int64_t) (1024LL * 1024LL * 1024LL));
            zs_pipe_send (output);
        }
    }
    else
    if (zs_pipe_size (output)) {
        //  Unary function; pull last value off pipe
        zs_pipe_pull (output);
        s_apply_scale (output, output, (int64_t) (1024LL * 1024LL * 1024LL));
        zs_pipe_send (output);
    }
    else
        //  Constant; send multipler as value
        zs_pipe_send_whole (output, (int64_t) (1024LL * 1024LL * 1024LL));
    return 0;
}

static int
s_Ti (zs_vm_t *self)
{
    zs_pipe_t *output = zs_vm_output (self);
    if (zs_vm_probing (self))
        zs_vm_register (self, "Ti", "Multiply by 2^40");
    else
    if (zs_pipe_size (zs_vm_input (self))) {
        zs_pipe_t *input = zs_vm_input (self);
        //  List function; recv and process all values
        while (zs_pipe_recv (input) == 0) {
            s_apply_scale (input, output, (int64_t) (1024LL * 1024LL * 1024LL * 1024LL));
            zs_pipe_send (output);
        }
    }
    else
    if (zs_pipe_size (output)) {
        //  Unary function; pull last value off pipe
        zs_pipe_pull (output);
        s_apply_scale (output, output, (int64_t) (1024LL * 1024LL * 1024LL * 1024LL));
        zs_pipe_send (output);
    }
    else
        //  Constant; send multipler as value
        zs_pipe_send_whole (output, (int64_t) (1024LL * 1024LL * 1024LL * 1024LL));
    return 0;
}

static int
s_Pi (zs_vm_t *self)
{
    zs_pipe_t *output = zs_vm_output (self);
    if (zs_vm_probing (self))
        zs_vm_register (self, "Pi", "Multiply by 2^50");
    else
    if (zs_pipe_size (zs_vm_input (self))) {
        zs_pipe_t *input = zs_vm_input (self);
        //  List function; recv and process all values
        while (zs_pipe_recv (input) == 0) {
            s_apply_scale (input, output, (int64_t) (1024LL * 1024LL * 1024LL * 1024LL * 1024LL));
            zs_pipe_send (output);
        }
    }
    else
    if (zs_pipe_size (output)) {
        //  Unary function; pull last value off pipe
        zs_pipe_pull (output);
        s_apply_scale (output, output, (int64_t) (1024LL * 1024LL * 1024LL * 1024LL * 1024LL));
        zs_pipe_send (output);
    }
    else
        //  Constant; send multipler as value
        zs_pipe_send_whole (output, (int64_t) (1024LL * 1024LL * 1024LL * 1024LL * 1024LL));
    return 0;
}

static int
s_Ei (zs_vm_t *self)
{
    zs_pipe_t *output = zs_vm_output (self);
    if (zs_vm_probing (self))
        zs_vm_register (self, "Ei", "Multiply by 2^60");
    else
    if (zs_pipe_size (zs_vm_input (self))) {
        zs_pipe_t *input = zs_vm_input (self);
        //  List function; recv and process all values
        while (zs_pipe_recv (input) == 0) {
            s_apply_scale (input, output, (int64_t) (1024LL * 1024LL * 1024LL * 1024LL * 1024LL * 1024LL));
            zs_pipe_send (output);
        }
    }
    else
    if (zs_pipe_size (output)) {
        //  Unary function; pull last value off pipe
        zs_pipe_pull (output);
        s_apply_scale (output, output, (int64_t) (1024LL * 1024LL * 1024LL * 1024LL * 1024LL * 1024LL));
        zs_pipe_send (output);
    }
    else
        //  Constant; send multipler as value
        zs_pipe_send_whole (output, (int64_t) (1024LL * 1024LL * 1024LL * 1024LL * 1024LL * 1024LL));
    return 0;
}

static int
s_da (zs_vm_t *self)
{
    zs_pipe_t *output = zs_vm_output (self);
    if (zs_vm_probing (self))
        zs_vm_register (self, "da", "Multiply by 10");
    else
    if (zs_pipe_size (zs_vm_input (self))) {
        zs_pipe_t *input = zs_vm_input (self);
        //  List function; recv and process all values
        while (zs_pipe_recv (input) == 0) {
            s_apply_scale (input, output, (int64_t) (10));
            zs_pipe_send (output);
        }
    }
    else
    if (zs_pipe_size (output)) {
        //  Unary function; pull last value off pipe
        zs_pipe_pull (output);
        s_apply_scale (output, output, (int64_t) (10));
        zs_pipe_send (output);
    }
    else
        //  Constant; send multipler as value
        zs_pipe_send_whole (output, (int64_t) (10));
    return 0;
}

static int
s_h (zs_vm_t *self)
{
    zs_pipe_t *output = zs_vm_output (self);
    if (zs_vm_probing (self))
        zs_vm_register (self, "h", "Multiply by 100");
    else
    if (zs_pipe_size (zs_vm_input (self))) {
        zs_pipe_t *input = zs_vm_input (self);
        //  List function; recv and process all values
        while (zs_pipe_recv (input) == 0) {
            s_apply_scale (input, output, (int64_t) (100));
            zs_pipe_send (output);
        }
    }
    else
    if (zs_pipe_size (output)) {
        //  Unary function; pull last value off pipe
        zs_pipe_pull (output);
        s_apply_scale (output, output, (int64_t) (100));
        zs_pipe_send (output);
    }
    else
        //  Constant; send multipler as value
        zs_pipe_send_whole (output, (int64_t) (100));
    return 0;
}

static int
s_k (zs_vm_t *self)
{
    zs_pipe_t *output = zs_vm_output (self);
    if (zs_vm_probing (self))
        zs_vm_register (self, "k", "Multiply by 1000");
    else
    if (zs_pipe_size (zs_vm_input (self))) {
        zs_pipe_t *input = zs_vm_input (self);
        //  List function; recv and process all values
        while (zs_pipe_recv (input) == 0) {
            s_apply_scale (input, output, (int64_t) (1000));
            zs_pipe_send (output);
        }
    }
    else
    if (zs_pipe_size (output)) {
        //  Unary function; pull last value off pipe
        zs_pipe_pull (output);
        s_apply_scale (output, output, (int64_t) (1000));
        zs_pipe_send (output);
    }
    else
        //  Constant; send multipler as value
        zs_pipe_send_whole (output, (int64_t) (1000));
    return 0;
}

static int
s_M (zs_vm_t *self)
{
    zs_pipe_t *output = zs_vm_output (self);
    if (zs_vm_probing (self))
        zs_vm_register (self, "M", "Multiply by 10^6");
    else
    if (zs_pipe_size (zs_vm_input (self))) {
        zs_pipe_t *input = zs_vm_input (self);
        //  List function; recv and process all values
        while (zs_pipe_recv (input) == 0) {
            s_apply_scale (input, output, (int64_t) (1000000));
            zs_pipe_send (output);
        }
    }
    else
    if (zs_pipe_size (output)) {
        //  Unary function; pull last value off pipe
        zs_pipe_pull (output);
        s_apply_scale (output, output, (int64_t) (1000000));
        zs_pipe_send (output);
    }
    else
        //  Constant; send multipler as value
        zs_pipe_send_whole (output, (int64_t) (1000000));
    return 0;
}

static int
s_G (zs_vm_t *self)
{
    zs_pipe_t *output = zs_vm_output (self);
    if (zs_vm_probing (self))
        zs_vm_register (self, "G", "Multiply by 10^9");
    else
    if (zs_pipe_size (zs_vm_input (self))) {
        zs_pipe_t *input = zs_vm_input (self);
        //  List function; recv and process all values
        while (zs_pipe_recv (input) == 0) {
            s_apply_scale (input, output, (int64_t) (1E9));
            zs_pipe_send (output);
        }
    }
    else
    if (zs_pipe_size (output)) {
        //  Unary function; pull last value off pipe
        zs_pipe_pull (output);
        s_apply_scale (output, output, (int64_t) (1E9));
        zs_pipe_send (output);
    }
    else
        //  Constant; send multipler as value
        zs_pipe_send_whole (output, (int64_t) (1E9));
    return 0;
}

static int
s_T (zs_vm_t *self)
{
    zs_pipe_t *output = zs_vm_output (self);
    if (zs_vm_probing (self))
        zs_vm_register (self, "T", "Multiply by 10^12");
    else
    if (zs_pipe_size (zs_vm_input (self))) {
        zs_pipe_t *input = zs_vm_input (self);
        //  List function; recv and process all values
        while (zs_pipe_recv (input) == 0) {
            s_apply_scale (input, output, (int64_t) (1E12));
            zs_pipe_send (output);
        }
    }
    else
    if (zs_pipe_size (output)) {
        //  Unary function; pull last value off pipe
        zs_pipe_pull (output);
        s_apply_scale (output, output, (int64_t) (1E12));
        zs_pipe_send (output);
    }
    else
        //  Constant; send multipler as value
        zs_pipe_send_whole (output, (int64_t) (1E12));
    return 0;
}

static int
s_P (zs_vm_t *self)
{
    zs_pipe_t *output = zs_vm_output (self);
    if (zs_vm_probing (self))
        zs_vm_register (self, "P", "Multiply by 10^15");
    else
    if (zs_pipe_size (zs_vm_input (self))) {
        zs_pipe_t *input = zs_vm_input (self);
        //  List function; recv and process all values
        while (zs_pipe_recv (input) == 0) {
            s_apply_scale (input, output, (int64_t) (1E15));
            zs_pipe_send (output);
        }
    }
    else
    if (zs_pipe_size (output)) {
        //  Unary function; pull last value off pipe
        zs_pipe_pull (output);
        s_apply_scale (output, output, (int64_t) (1E15));
        zs_pipe_send (output);
    }
    else
        //  Constant; send multipler as value
        zs_pipe_send_whole (output, (int64_t) (1E15));
    return 0;
}

static int
s_E (zs_vm_t *self)
{
    zs_pipe_t *output = zs_vm_output (self);
    if (zs_vm_probing (self))
        zs_vm_register (self, "E", "Multiply by 10^18");
    else
    if (zs_pipe_size (zs_vm_input (self))) {
        zs_pipe_t *input = zs_vm_input (self);
        //  List function; recv and process all values
        while (zs_pipe_recv (input) == 0) {
            s_apply_scale (input, output, (int64_t) (1E18));
            zs_pipe_send (output);
        }
    }
    else
    if (zs_pipe_size (output)) {
        //  Unary function; pull last value off pipe
        zs_pipe_pull (output);
        s_apply_scale (output, output, (int64_t) (1E18));
        zs_pipe_send (output);
    }
    else
        //  Constant; send multipler as value
        zs_pipe_send_whole (output, (int64_t) (1E18));
    return 0;
}

static int
s_Z (zs_vm_t *self)
{
    zs_pipe_t *output = zs_vm_output (self);
    if (zs_vm_probing (self))
        zs_vm_register (self, "Z", "Multiply by 10^21");
    else
    if (zs_pipe_size (zs_vm_input (self))) {
        zs_pipe_t *input = zs_vm_input (self);
        //  List function; recv and process all values
        while (zs_pipe_recv (input) == 0) {
            s_apply_scale (input, output, (int64_t) (1E21));
            zs_pipe_send (output);
        }
    }
    else
    if (zs_pipe_size (output)) {
        //  Unary function; pull last value off pipe
        zs_pipe_pull (output);
        s_apply_scale (output, output, (int64_t) (1E21));
        zs_pipe_send (output);
    }
    else
        //  Constant; send multipler as value
        zs_pipe_send_whole (output, (int64_t) (1E21));
    return 0;
}

static int
s_Y (zs_vm_t *self)
{
    zs_pipe_t *output = zs_vm_output (self);
    if (zs_vm_probing (self))
        zs_vm_register (self, "Y", "Multiply by 10^24");
    else
    if (zs_pipe_size (zs_vm_input (self))) {
        zs_pipe_t *input = zs_vm_input (self);
        //  List function; recv and process all values
        while (zs_pipe_recv (input) == 0) {
            s_apply_scale (input, output, (int64_t) (1E24));
            zs_pipe_send (output);
        }
    }
    else
    if (zs_pipe_size (output)) {
        //  Unary function; pull last value off pipe
        zs_pipe_pull (output);
        s_apply_scale (output, output, (int64_t) (1E24));
        zs_pipe_send (output);
    }
    else
        //  Constant; send multipler as value
        zs_pipe_send_whole (output, (int64_t) (1E24));
    return 0;
}

static int
s_d (zs_vm_t *self)
{
    zs_pipe_t *output = zs_vm_output (self);
    if (zs_vm_probing (self))
        zs_vm_register (self, "d", "Divide by 10 (real)");
    else
    if (zs_pipe_size (zs_vm_input (self))) {
        zs_pipe_t *input = zs_vm_input (self);
        //  List function; recv and process all values
        while (zs_pipe_recv (input) == 0) {
            //  Always coerce to a real value
            zs_pipe_set_real (output, zs_pipe_real (input) * 0.1);
            zs_pipe_send (output);
        }
    }
    else
    if (zs_pipe_size (output)) {
        //  Unary function; pull last value off pipe
        zs_pipe_pull (output);
        //  Always coerce to a real value
        zs_pipe_set_real (output, zs_pipe_real (output) * 0.1);
        zs_pipe_send (output);
    }
    else
        //  Constant; send multipler as value
        zs_pipe_send_real (output, 0.1);
    return 0;
}

static int
s_c (zs_vm_t *self)
{
    zs_pipe_t *output = zs_vm_output (self);
    if (zs_vm_probing (self))
        zs_vm_register (self, "c", "Divide by 100 (real)");
    else
    if (zs_pipe_size (zs_vm_input (self))) {
        zs_pipe_t *input = zs_vm_input (self);
        //  List function; recv and process all values
        while (zs_pipe_recv (input) == 0) {
            //  Always coerce to a real value
            zs_pipe_set_real (output, zs_pipe_real (input) * 0.01);
            zs_pipe_send (output);
        }
    }
    else
    if (zs_pipe_size (output)) {
        //  Unary function; pull last value off pipe
        zs_pipe_pull (output);
        //  Always coerce to a real value
        zs_pipe_set_real (output, zs_pipe_real (output) * 0.01);
        zs_pipe_send (output);
    }
    else
        //  Constant; send multipler as value
        zs_pipe_send_real (output, 0.01);
    return 0;
}

static int
s_m (zs_vm_t *self)
{
    zs_pipe_t *output = zs_vm_output (self);
    if (zs_vm_probing (self))
        zs_vm_register (self, "m", "Divide by 1000 (real)");
    else
    if (zs_pipe_size (zs_vm_input (self))) {
        zs_pipe_t *input = zs_vm_input (self);
        //  List function; recv and process all values
        while (zs_pipe_recv (input) == 0) {
            //  Always coerce to a real value
            zs_pipe_set_real (output, zs_pipe_real (input) * 1E-3);
            zs_pipe_send (output);
        }
    }
    else
    if (zs_pipe_size (output)) {
        //  Unary function; pull last value off pipe
        zs_pipe_pull (output);
        //  Always coerce to a real value
        zs_pipe_set_real (output, zs_pipe_real (output) * 1E-3);
        zs_pipe_send (output);
    }
    else
        //  Constant; send multipler as value
        zs_pipe_send_real (output, 1E-3);
    return 0;
}

static int
s_u (zs_vm_t *self)
{
    zs_pipe_t *output = zs_vm_output (self);
    if (zs_vm_probing (self))
        zs_vm_register (self, "u", "Divide by 10^6 (real)");
    else
    if (zs_pipe_size (zs_vm_input (self))) {
        zs_pipe_t *input = zs_vm_input (self);
        //  List function; recv and process all values
        while (zs_pipe_recv (input) == 0) {
            //  Always coerce to a real value
            zs_pipe_set_real (output, zs_pipe_real (input) * 1E-6);
            zs_pipe_send (output);
        }
    }
    else
    if (zs_pipe_size (output)) {
        //  Unary function; pull last value off pipe
        zs_pipe_pull (output);
        //  Always coerce to a real value
        zs_pipe_set_real (output, zs_pipe_real (output) * 1E-6);
        zs_pipe_send (output);
    }
    else
        //  Constant; send multipler as value
        zs_pipe_send_real (output, 1E-6);
    return 0;
}

static int
s_n (zs_vm_t *self)
{
    zs_pipe_t *output = zs_vm_output (self);
    if (zs_vm_probing (self))
        zs_vm_register (self, "n", "Divide by 10^9 (real)");
    else
    if (zs_pipe_size (zs_vm_input (self))) {
        zs_pipe_t *input = zs_vm_input (self);
        //  List function; recv and process all values
        while (zs_pipe_recv (input) == 0) {
            //  Always coerce to a real value
            zs_pipe_set_real (output, zs_pipe_real (input) * 1E-9);
            zs_pipe_send (output);
        }
    }
    else
    if (zs_pipe_size (output)) {
        //  Unary function; pull last value off pipe
        zs_pipe_pull (output);
        //  Always coerce to a real value
        zs_pipe_set_real (output, zs_pipe_real (output) * 1E-9);
        zs_pipe_send (output);
    }
    else
        //  Constant; send multipler as value
        zs_pipe_send_real (output, 1E-9);
    return 0;
}

static int
s_p (zs_vm_t *self)
{
    zs_pipe_t *output = zs_vm_output (self);
    if (zs_vm_probing (self))
        zs_vm_register (self, "p", "Divide by 10^12 (real)");
    else
    if (zs_pipe_size (zs_vm_input (self))) {
        zs_pipe_t *input = zs_vm_input (self);
        //  List function; recv and process all values
        while (zs_pipe_recv (input) == 0) {
            //  Always coerce to a real value
            zs_pipe_set_real (output, zs_pipe_real (input) * 1E-12);
            zs_pipe_send (output);
        }
    }
    else
    if (zs_pipe_size (output)) {
        //  Unary function; pull last value off pipe
        zs_pipe_pull (output);
        //  Always coerce to a real value
        zs_pipe_set_real (output, zs_pipe_real (output) * 1E-12);
        zs_pipe_send (output);
    }
    else
        //  Constant; send multipler as value
        zs_pipe_send_real (output, 1E-12);
    return 0;
}

static int
s_f (zs_vm_t *self)
{
    zs_pipe_t *output = zs_vm_output (self);
    if (zs_vm_probing (self))
        zs_vm_register (self, "f", "Divide by 10^15 (real)");
    else
    if (zs_pipe_size (zs_vm_input (self))) {
        zs_pipe_t *input = zs_vm_input (self);
        //  List function; recv and process all values
        while (zs_pipe_recv (input) == 0) {
            //  Always coerce to a real value
            zs_pipe_set_real (output, zs_pipe_real (input) * 1E-15);
            zs_pipe_send (output);
        }
    }
    else
    if (zs_pipe_size (output)) {
        //  Unary function; pull last value off pipe
        zs_pipe_pull (output);
        //  Always coerce to a real value
        zs_pipe_set_real (output, zs_pipe_real (output) * 1E-15);
        zs_pipe_send (output);
    }
    else
        //  Constant; send multipler as value
        zs_pipe_send_real (output, 1E-15);
    return 0;
}

static int
s_a (zs_vm_t *self)
{
    zs_pipe_t *output = zs_vm_output (self);
    if (zs_vm_probing (self))
        zs_vm_register (self, "a", "Divide by 10^18 (real)");
    else
    if (zs_pipe_size (zs_vm_input (self))) {
        zs_pipe_t *input = zs_vm_input (self);
        //  List function; recv and process all values
        while (zs_pipe_recv (input) == 0) {
            //  Always coerce to a real value
            zs_pipe_set_real (output, zs_pipe_real (input) * 1E-18);
            zs_pipe_send (output);
        }
    }
    else
    if (zs_pipe_size (output)) {
        //  Unary function; pull last value off pipe
        zs_pipe_pull (output);
        //  Always coerce to a real value
        zs_pipe_set_real (output, zs_pipe_real (output) * 1E-18);
        zs_pipe_send (output);
    }
    else
        //  Constant; send multipler as value
        zs_pipe_send_real (output, 1E-18);
    return 0;
}

static int
s_z (zs_vm_t *self)
{
    zs_pipe_t *output = zs_vm_output (self);
    if (zs_vm_probing (self))
        zs_vm_register (self, "z", "Divide by 10^21 (real)");
    else
    if (zs_pipe_size (zs_vm_input (self))) {
        zs_pipe_t *input = zs_vm_input (self);
        //  List function; recv and process all values
        while (zs_pipe_recv (input) == 0) {
            //  Always coerce to a real value
            zs_pipe_set_real (output, zs_pipe_real (input) * 1E-21);
            zs_pipe_send (output);
        }
    }
    else
    if (zs_pipe_size (output)) {
        //  Unary function; pull last value off pipe
        zs_pipe_pull (output);
        //  Always coerce to a real value
        zs_pipe_set_real (output, zs_pipe_real (output) * 1E-21);
        zs_pipe_send (output);
    }
    else
        //  Constant; send multipler as value
        zs_pipe_send_real (output, 1E-21);
    return 0;
}

static int
s_y (zs_vm_t *self)
{
    zs_pipe_t *output = zs_vm_output (self);
    if (zs_vm_probing (self))
        zs_vm_register (self, "y", "Divide by 10^24 (real)");
    else
    if (zs_pipe_size (zs_vm_input (self))) {
        zs_pipe_t *input = zs_vm_input (self);
        //  List function; recv and process all values
        while (zs_pipe_recv (input) == 0) {
            //  Always coerce to a real value
            zs_pipe_set_real (output, zs_pipe_real (input) * 1E-24);
            zs_pipe_send (output);
        }
    }
    else
    if (zs_pipe_size (output)) {
        //  Unary function; pull last value off pipe
        zs_pipe_pull (output);
        //  Always coerce to a real value
        zs_pipe_set_real (output, zs_pipe_real (output) * 1E-24);
        zs_pipe_send (output);
    }
    else
        //  Constant; send multipler as value
        zs_pipe_send_real (output, 1E-24);
    return 0;
}

static void
s_register_zs_scaling (zs_vm_t *self)
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
    zs_vm_probe (self, s_d);
    zs_vm_probe (self, s_c);
    zs_vm_probe (self, s_m);
    zs_vm_probe (self, s_u);
    zs_vm_probe (self, s_n);
    zs_vm_probe (self, s_p);
    zs_vm_probe (self, s_f);
    zs_vm_probe (self, s_a);
    zs_vm_probe (self, s_z);
    zs_vm_probe (self, s_y);
}
#endif
