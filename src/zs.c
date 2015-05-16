//  This is a ZeroScript machine

#include "zs_classes.h"

int main (int argc, char *argv [])
{
    int argn = 1;
    bool verbose = false;
    if (argn < argc && streq (argv [argn], "-v")) {
        verbose = true;
        argn++;
    }
    if (argn < argc && streq (argv [argn], "-h")) {
        puts ("Usage: zs [ -v ]");
        return 0;
    }
    //  Main thread is read/parse/execute input text
    zsys_init ();
    zs_repl_t *repl = zs_repl_new ();
    zs_repl_verbose (repl, verbose);

    //  If run with arguments, treat as script to execute
    if (argn < argc) {
        char input [1024 + 2] = "";
        while (argn < argc) {
            strncat (input, argv [argn++], 1024);
            strncat (input, " ", 1024);
        }
        if (zs_repl_execute (repl, input) == 0) {
            if (zs_repl_completed (repl))
                puts (zs_repl_results (repl));
            else
                puts ("E: incomplete");
        }
        else
            puts ("E: syntax error");
    }
    else {
        //  If run without arguments, drop into REPL shell
        while (!zctx_interrupted) {
            printf ("> ");                  //  Our normal prompt
            char input [1024 + 2];          //  1024 chars + LF + null
            if (!fgets (input, 1026, stdin))
                break;

            if (zs_repl_execute (repl, input) == 0) {
                if (zs_repl_completed (repl))
                    puts (zs_repl_results (repl));
                else
                    printf ("...");
            }
            else {
                printf ("  %*c\n", zs_repl_offset (repl), '^');
                puts ("Syntax error");
            }
        }
    }
    zs_repl_destroy (&repl);
    return 0;
}
