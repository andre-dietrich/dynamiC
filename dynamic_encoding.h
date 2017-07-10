#ifndef ENCODING_C_H
#define ENCODING_C_H

#include "dynamic.h"


#define ENC_NONE    0
#define ENC_TRUE    1
#define ENC_FALSE   2
#define ENC_INT1    3
#define ENC_INT2    4
#define ENC_INT4    5
#define ENC_FLOAT   6
#define ENC_LIST    7
#define ENC_SET     8
#define ENC_PROC    9

#define ENC_STRING 10
#define ENC_DICT   11

#define ENC_HALT   12


dyn_ushort dyn_encoding_length(const dyn_c *dyn);

dyn_char*  dyn_encode     (dyn_char *to, const dyn_c *from);

dyn_char*  dyn_decode     (dyn_char code, dyn_char *from, dyn_c *to);

dyn_char*  dyn_decode_all (dyn_char *from, dyn_c *to);


#endif
