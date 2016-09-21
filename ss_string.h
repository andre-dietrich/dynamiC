#ifndef STRING_C_H
#define STRING_C_H

#include "dynamic_types.h"

#include <stdlib.h>

ss_ushort ss_strlen   (ss_str str);

void      ss_strcat   (ss_str a, ss_str b);
void      ss_strcat2  (ss_str a, ss_str b);
void      ss_strcpy   (ss_str a, ss_str b);

void      ss_itoa     (ss_str str, ss_int i);
ss_ushort ss_itoa_len (ss_int i);

void      ss_ftoa     (ss_str str, ss_float f);
ss_ushort ss_ftoa_len (ss_float f);

ss_char   ss_strcmp   (ss_str a, ss_str b);

#endif
