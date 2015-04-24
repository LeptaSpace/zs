/*  =========================================================================
    zs_parser - No title

    =========================================================================
*/

#ifndef ZS_PARSER_H_INCLUDED
#define ZS_PARSER_H_INCLUDED

#include <czmq.h>

#ifdef __cplusplus
extern "C" {
#endif

//  Opaque class structure
typedef struct _zs_parser_t zs_parser_t;

//  @interface
//  Create a new zs_parser, return the reference if successful, or NULL
//  if construction failed due to lack of available memory.
zs_parser_t *
    zs_parser_new (void);

//  Destroy the zs_parser and free all memory used by the object.
void
    zs_parser_destroy (zs_parser_t **self_p);

//  Enable verbose tracing of parser
void
    zs_parser_verbose (zs_parser_t *self, bool verbose);

//  Parse and execute a buffer of input text.
void
    zs_parser_execute (zs_parser_t *self, const char *input);

//  Self test of this class
void
    zs_parser_test (bool animate);
//  @end

#ifdef __cplusplus
}
#endif

#endif
