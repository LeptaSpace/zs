//  This is a ZeroScript machine

#include "zs_classes.h"
#ifdef USE_READLINE
#   include <readline/readline.h>
#endif

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

#ifdef USE_READLINE
    //  Disable default completion on filenames
    rl_bind_key ('\t', rl_insert);
#endif

    //  If run with arguments, treat as script to execute
    if (argn < argc) {
        char input [1024 + 2] = "";
        while (argn < argc) {
            strncat (input, argv [argn++], 1024);
            strncat (input, " ", 1024);
        }
        if (zs_repl_execute (repl, input))
            puts ("E: syntax error");
    }
    else {
        //  If run without arguments, drop into REPL shell
        while (!zctx_interrupted) {
            char *prompt = zs_repl_completed (repl)? "> ": ": ";
#ifdef USE_READLINE
            char *input = readline (prompt);
            if (!input || streq (input, "quit"))
                break;
            add_history (input);
#else
            printf ("%s", prompt);
            char input [1024 + 2];          //  1024 chars + LF + null
            if (!fgets (input, 1026, stdin) || streq (input, "quit\n"))
                break;
#endif
            if (zs_repl_execute (repl, input)) {
                printf ("  %*c\n", zs_repl_offset (repl), '^');
                puts ("Syntax error");
            }
#ifdef USE_READLINE
            free (input);
#endif
        }
    }
    zs_repl_destroy (&repl);
    return 0;
}
