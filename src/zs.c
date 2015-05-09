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
    zs_repl_t *repl = zs_repl_new ();
    zs_repl_verbose (repl, verbose);

    while (!zctx_interrupted) {
        printf ("> ");                  //  Our normal prompt
        char input [1024 + 2];          //  1024 chars + LF + null
        if (!fgets (input, 1026, stdin))
            break;

        if (zs_repl_execute (repl, input) == 0) {
            if (zs_repl_completed (repl)) {
                char *results = zs_repl_results (repl);
                puts (results);
                zstr_free (&results);
            }
            else
                printf ("...");
        }
        else {
            printf ("  %*c\n", zs_repl_offset (repl), '^');
            puts ("Syntax error");
        }
    }
    zs_repl_destroy (&repl);
    return 0;
}
