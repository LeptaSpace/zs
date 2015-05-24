/*  =========================================================================
    zs_units_misc - scaling atomics

    ** WARNING *************************************************************
    THIS SOURCE FILE IS 100% GENERATED. If you edit this file, you will lose
    your changes at the next build cycle. This is great for temporary printf
    statements. DO NOT MAKE ANY CHANGES YOU WISH TO KEEP. The correct places
    for commits are:

     * The XML model used for this code generation: zs_units_misc.xml, or
     * The code generation script that built this file: zs_scaling
    ************************************************************************
    =========================================================================
*/

#ifndef ZS_UNITS_MISC_H_INCLUDED
#define ZS_UNITS_MISC_H_INCLUDED

#ifndef S_APPLY_SCALE_DEFINED
#define S_APPLY_SCALE_DEFINED
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
#endif

static int
s_minutes (zs_vm_t *self, zs_pipe_t *input, zs_pipe_t *output)
{
    if (zs_vm_probing (self)) {
        zs_vm_register (self, "minutes", zs_type_modest, "Seconds in a minute");
        zs_vm_register (self, "minute", zs_type_modest, NULL);
    }
    else {
        //  Process all values on input pipe
        size_t set_size = 0;
        while (zs_pipe_recv (input)) {
            s_apply_scale (input, output, (int64_t) (60LL));
            zs_pipe_send (output);
            set_size++;
        }
        //  If input set was empty, operate as constant
        if (set_size == 0)
            zs_pipe_send_whole (output, (int64_t) 60LL);
    }
    return 0;
}

static int
s_hours (zs_vm_t *self, zs_pipe_t *input, zs_pipe_t *output)
{
    if (zs_vm_probing (self)) {
        zs_vm_register (self, "hours", zs_type_modest, "Seconds in a hour");
        zs_vm_register (self, "hour", zs_type_modest, NULL);
    }
    else {
        //  Process all values on input pipe
        size_t set_size = 0;
        while (zs_pipe_recv (input)) {
            s_apply_scale (input, output, (int64_t) (60LL * 60LL));
            zs_pipe_send (output);
            set_size++;
        }
        //  If input set was empty, operate as constant
        if (set_size == 0)
            zs_pipe_send_whole (output, (int64_t) 60LL * 60LL);
    }
    return 0;
}

static int
s_days (zs_vm_t *self, zs_pipe_t *input, zs_pipe_t *output)
{
    if (zs_vm_probing (self)) {
        zs_vm_register (self, "days", zs_type_modest, "Seconds in a day");
        zs_vm_register (self, "day", zs_type_modest, NULL);
    }
    else {
        //  Process all values on input pipe
        size_t set_size = 0;
        while (zs_pipe_recv (input)) {
            s_apply_scale (input, output, (int64_t) (60LL * 60LL * 24LL));
            zs_pipe_send (output);
            set_size++;
        }
        //  If input set was empty, operate as constant
        if (set_size == 0)
            zs_pipe_send_whole (output, (int64_t) 60LL * 60LL * 24LL);
    }
    return 0;
}

static int
s_weeks (zs_vm_t *self, zs_pipe_t *input, zs_pipe_t *output)
{
    if (zs_vm_probing (self)) {
        zs_vm_register (self, "weeks", zs_type_modest, "Seconds in a week");
        zs_vm_register (self, "week", zs_type_modest, NULL);
    }
    else {
        //  Process all values on input pipe
        size_t set_size = 0;
        while (zs_pipe_recv (input)) {
            s_apply_scale (input, output, (int64_t) (60LL * 60LL * 24LL * 7LL));
            zs_pipe_send (output);
            set_size++;
        }
        //  If input set was empty, operate as constant
        if (set_size == 0)
            zs_pipe_send_whole (output, (int64_t) 60LL * 60LL * 24LL * 7LL);
    }
    return 0;
}

static int
s_years (zs_vm_t *self, zs_pipe_t *input, zs_pipe_t *output)
{
    if (zs_vm_probing (self)) {
        zs_vm_register (self, "years", zs_type_modest, "Seconds in a non-leap year");
        zs_vm_register (self, "year", zs_type_modest, NULL);
    }
    else {
        //  Process all values on input pipe
        size_t set_size = 0;
        while (zs_pipe_recv (input)) {
            s_apply_scale (input, output, (int64_t) (60LL * 60LL * 24LL * 365LL));
            zs_pipe_send (output);
            set_size++;
        }
        //  If input set was empty, operate as constant
        if (set_size == 0)
            zs_pipe_send_whole (output, (int64_t) 60LL * 60LL * 24LL * 365LL);
    }
    return 0;
}

static int
s_msecs (zs_vm_t *self, zs_pipe_t *input, zs_pipe_t *output)
{
    if (zs_vm_probing (self)) {
        zs_vm_register (self, "msecs", zs_type_modest, "Divide by 1000 (real)");
        zs_vm_register (self, "msec", zs_type_modest, NULL);
    }
    else {
        //  Process all values on input pipe
        size_t set_size = 0;
        while (zs_pipe_recv (input)) {
            //  Always coerce to a real value
            zs_pipe_set_real (output, zs_pipe_real (input) * 0.001);
            zs_pipe_send (output);
            set_size++;
        }
        //  If input set was empty, operate as constant
        if (set_size == 0)
            zs_pipe_send_real (output, 0.001);
    }
    return 0;
}

static void
s_register_zs_units_misc (zs_vm_t *self)
{
    zs_vm_probe (self, s_minutes);
    zs_vm_probe (self, s_hours);
    zs_vm_probe (self, s_days);
    zs_vm_probe (self, s_weeks);
    zs_vm_probe (self, s_years);
    zs_vm_probe (self, s_msecs);
}
#endif
