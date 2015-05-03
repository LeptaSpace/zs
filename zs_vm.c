#include "zs_classes.h"

//  -- open, close scopes
//  -- add stop function


//  Primitive function type
typedef void (call_t) (zs_exec_t *self);

static void
s_sum (zs_exec_t *self)
{
    int64_t sum = 0;
    while (zs_pipe_size (zs_exec_input (self)) > 0)
        sum += zs_pipe_get_number (zs_exec_input (self));
    zs_pipe_put_number (zs_exec_output (self), sum);
}

static void
s_count (zs_exec_t *self)
{
    zs_pipe_put_number (zs_exec_output (self), zs_pipe_size (zs_exec_input (self)));
}

static
void s_echo (zs_exec_t *self)
{
    char *results = zs_pipe_contents (zs_exec_input (self));
    printf ("%s\n", results);
    zstr_free (&results);
}

//  Bytecodes
//  - up to 240 class 0 primitives
//  - 255 + 0 - stop
//  - 255 + 16 bits = extensions; class (1..n) + primitive
//

//  These are built-in opcodes which are allowed to modify the IP
//  So we can keep the IP in a register variable here and perhaps
//  save some CPU cycles. Lol. \o/ Put these in order of frequency.
#define OPCODE_CALL         254
#define OPCODE_RETURN       253
#define OPCODE_NUMBER       252
#define OPCODE_STRING       251
#define OPCODE_SHIFT        250
#define OPCODE_PUSH         249
#define OPCODE_POP          248
#define OPCODE_STOP         241
//  ... down to 241 at most


//  Encode a number; for now we always store as an 8-byte signed int
#define encode_number(n) \
    number = n; \
    code [code_size++] = OPCODE_NUMBER; \
    memcpy (code + code_size, &number, 8); \
    code_size += 8

//  Encode a string
#define encode_string(s) \
    code [code_size++] = OPCODE_STRING; \
    memcpy (code + code_size, (byte *) s, strlen (s) + 1); \
    code_size += strlen (s) + 1


int main (void)
{
    zs_exec_t *exec = zs_exec_new ();

    //  Map VM instruction to 240 class 0 primitives
    //  Extension classes each need their own lookup
    //  We register extensions separately
    call_t **class0 = malloc (240 * sizeof (call_t *));
    size_t class0_size = 0;

    //  We use a bytecode; this should be resized if we run out
    //  of space... we assume no limit to the compiled code size
    size_t code_size = 0;
    size_t code_max = 1000;
    byte *code = (byte *) malloc (code_max);

    //  Call/return stack, holds positions into code; we can realloc
    //  the stack if we overflow it
    size_t stack_ptr = 0;
    size_t stack_max = 100;
    size_t *stack = malloc (stack_max * sizeof (size_t));

    //  Stuff we'll work with
    int64_t number;

    //  Register class 0 primitives
    size_t exec_sum = class0_size;
    class0 [class0_size++] = s_sum;

    size_t exec_count = class0_size;
    class0 [class0_size++] = s_count;

    size_t exec_echo = class0_size;
    class0 [class0_size++] = s_echo;

    //  --------------------------------------------------------------------
    //  main: (
    //  --------------------------------------------------------------------
    size_t exec_main = code_size;      //  "main" -> exec_main

    //  --------------------------------------------------------------------
    //  123 1234567890 sum echo
    //  --------------------------------------------------------------------
    encode_number (123);
    encode_number (1234567890);
    code [code_size++] = OPCODE_SHIFT;
    code [code_size++] = exec_sum;
    code [code_size++] = OPCODE_SHIFT;
    code [code_size++] = exec_echo;

    //  --------------------------------------------------------------------
    //  <Hello,> <World> echo
    //  --------------------------------------------------------------------
    //  Compile string constant
    //  [string][0]
    encode_string ("Hello,");
    encode_string ("World");
    code [code_size++] = OPCODE_SHIFT;
    code [code_size++] = exec_echo;

    //  --------------------------------------------------------------------
    //  sum ( 123 456 ) echo
    code [code_size++] = OPCODE_PUSH;
    encode_number (123);
    encode_number (456);
    code [code_size++] = OPCODE_POP;
    code [code_size++] = exec_sum;
    code [code_size++] = OPCODE_SHIFT;
    code [code_size++] = exec_echo;

    //  --------------------------------------------------------------------
    //  sum ( 123 count (1 2 3) ) echo
    code [code_size++] = OPCODE_PUSH;
    encode_number (123);
    code [code_size++] = OPCODE_PUSH;
    encode_number (1);
    encode_number (2);
    encode_number (3);
    code [code_size++] = OPCODE_POP;
    code [code_size++] = exec_count;
    code [code_size++] = OPCODE_POP;
    code [code_size++] = exec_sum;
    code [code_size++] = OPCODE_SHIFT;
    code [code_size++] = exec_echo;

    //  --------------------------------------------------------------------
    //  )
    code [code_size++] = OPCODE_RETURN;

    //  --------------------------------------------------------------------
    //  sub: ( <OK> <Guys> )

    size_t exec_sub = code_size;      //  "OK" -> exec_sub
    encode_string ("OK");
    encode_string ("Guys");
    code [code_size++] = OPCODE_RETURN;

    //  main sub
    size_t exec_start = code_size;
    code [code_size++] = OPCODE_CALL;
    code [code_size++] = (byte) exec_sub;   //  fix to long address
    code [code_size++] = OPCODE_CALL;
    code [code_size++] = (byte) exec_sub;
    code [code_size++] = OPCODE_SHIFT;
    code [code_size++] = exec_echo;
    code [code_size++] = OPCODE_CALL;
    code [code_size++] = (byte) exec_main;

    //  --------------------------------------------------------------------
    //  End program
    code [code_size++] = OPCODE_STOP;

    //  Reset execution
    size_t program = exec_start;

    //  Execute until finished

    while (true) {
        byte opcode = code [program];
//         printf ("program=%d opcode=%d\n", (int) program, opcode);
        program++;

        if (opcode < 241)
            (class0 [opcode]) (exec);
        else
        if (opcode == OPCODE_CALL) {
            stack [stack_ptr++] = program + 1;
            program = code [program];
        }
        else
        if (opcode == OPCODE_RETURN) {
            program = stack [--stack_ptr];
        }
        else
        if (opcode == OPCODE_NUMBER) {
            int64_t number;
            memcpy (&number, code + program, 8);
            program += 8;
            zs_pipe_put_number (zs_exec_output (exec), number);
        }
        else
        if (opcode == OPCODE_STRING) {
            zs_pipe_put_string (zs_exec_output (exec), (char *) code + program);
            program += strlen ((char *) code + program) + 1;
        }
        else
        if (opcode == OPCODE_SHIFT)
            zs_exec_shift (exec);
        else
        if (opcode == OPCODE_PUSH)
            zs_exec_push (exec);
        else
        if (opcode == OPCODE_POP)
            zs_exec_pop (exec);
        else
        if (opcode == OPCODE_STOP)
            break;
    }
    char *results = zs_pipe_contents (zs_exec_output (exec));
    puts (results);
    free (results);
    free (code);
    zs_exec_destroy (&exec);
    return 0;
}
