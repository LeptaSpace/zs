//  This is a ZeroScript machine

#include "zs_classes.h"

int main (int argc, char *argv [])
{
    int argn = 1;
    bool verbose = false;
    if (argc > argn && streq (argv [argn], "-v")) {
        verbose = true;
        argn++;
    }
    if (argc > argn && streq (argv [argn], "-h")) {
        puts ("Usage: zs [ -v ]");
        return 0;
    }
    //  Main thread is read/parse/execute input text
    zsys_init ();
    zs_core_t *core = zs_core_new ();
    zs_core_verbose (core, verbose);

    while (!zctx_interrupted) {
        char input [1024 + 2];          //  1024 chars + LF + null
        if (!fgets (input, 1026, stdin))
            break;

        if (zs_core_execute (core, input))
            puts ("Syntax error");
    }
    zs_core_destroy (&core);
    return 0;
}
    