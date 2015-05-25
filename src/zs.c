//  This is a ZeroScript shell

#include "zs_classes.h"
#include <editline.h>

#define HISTORY "/tmp/.zs_history"

//  We make this a static global, so the readline callbacks can access it.
static zs_repl_t *repl;

//  ------------------------------------------------------------------------
//  Readline API; these functions are explained in the FSF readline docs.
//  This is a rather crappy, complex and thread unsafe API yet it's what
//  we have to work with. We're using editline, a smaller clone of FSF
//  readline, with the same API.

static char *
s_complete_func (char *token, int *unique)
{
    size_t nbr_matches = 0;
    const char *name = zs_repl_first (repl);
    const char *completion = NULL;
    while (name) {
        if (strncmp (name, token, strlen (token)) == 0) {
            completion = name + strlen (token);
            nbr_matches++;
        }
        name = zs_repl_next (repl);
    }
    //  If we had a unique match return missing part of name, with a
    //  trailing space in all cases.
    if (nbr_matches == 1) {
        *unique = 1;
        return zsys_sprintf ("%s ", completion);
    }
    return NULL;
}

static int
s_list_possib_func (char *token, char ***names_p)
{
    //  Count current number of known names
    size_t count = 0;
    const char *name = zs_repl_first (repl);
    while (name) {
        if (strncmp (name, token, strlen (token)) == 0)
            count++;
        name = zs_repl_next (repl);
    }
    //  Allocate array of char * for these names
    char **names = (char **) malloc (count * sizeof (char *));
    count = 0;
    name = zs_repl_first (repl);
    while (name) {
        if (strncmp (name, token, strlen (token)) == 0)
            names [count++] = strdup (name);
        name = zs_repl_next (repl);
    }
    *names_p = names;
    return count;
}

static el_status_t
s_list_possible (void)
{
    char **names;
    char *word = el_find_word ();
    int ac = rl_list_possib (word, &names);
    if (word)
        free (word);
    if (ac) {
        el_print_columns (ac, names);
        while (--ac >= 0)
            free (names [ac]);
        free (names);
        return CSmove;
    }
    return CSdone;
}


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
    repl = zs_repl_new ();
    zs_repl_verbose (repl, verbose);

    //  Set-up the command line
    read_history (HISTORY);
    rl_set_complete_func (&s_complete_func);
    rl_set_list_possib_func (&s_list_possib_func);
    el_bind_key ('?', s_list_possible);

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
            char *input = readline (prompt);
            if (!input || streq (input, "quit"))
                break;

            add_history (input);
            if (zs_repl_execute (repl, input)) {
                printf ("  %*c\n", zs_repl_offset (repl), '^');
                puts ("Syntax error");
            }
            else
            if (zs_repl_completed (repl)) {
                const char *results = zs_repl_results (repl);
                if (*results)
                    puts (results);
            }
            free (input);
        }
    }
    write_history (HISTORY);
    zs_repl_destroy (&repl);
    return 0;
}
