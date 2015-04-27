//  This is a ZeroScript machine

#include "zs_classes.h"

int main (void)
{
    zs_lex_test (false);
    zs_core_test (false);
//     //  Main thread is read/parse/execute input text
//     zsys_init ();
//     uint line_nbr = 0;
//     
//     while (!zctx_interrupted) {
//         char input [1024 + 2];          //  1024 chars + LF + null
//         if (!fgets (input, 1026, stdin))
//             break;
//         line_nbr++;
//     }
    return 0;
}
    