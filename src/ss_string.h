/**
 *  @file ss_string.h
 *  @author André Dietrich
 *  @date 14 December 2016
 *
 *  @copyright Copyright 2016 André Dietrich. All rights reserved.
 *
 *  @license This project is released under the BSD-License.
 *
 *  @brief Definition of C string manipulation functions.
 *
 *
 */

#ifndef STRING_C_H
#define STRING_C_H

#include "dynamic_types.h"

#include <stdlib.h>

/** @brief Returns the length of an string.                                   */
ss_ushort ss_strlen   (ss_str str);
/** @brief Concatenate strings                                                */
void      ss_strcat   (ss_str destination, ss_str source);
/** @brief Concatenate strings, required memory is automatically allocated.   */
void      ss_strcat2  (ss_str destination, ss_str source);
/** @brief Copy string.                                                       */
void      ss_strcpy   (ss_str destination, ss_str source);
/** @brief Integer to ASCII-string conversion.                                */
void      ss_itoa     (ss_str str, ss_int i);

/** @brief Calculates the number of required characters for integer to string
 *         (decimal) conversion, minus increases the value by one.            */
ss_ushort ss_itoa_len (ss_int i);

/** @brief Float to ASCII-string conversion (decimal).                        */
void      ss_ftoa     (ss_str str, ss_float f);

/** @brief Calculates the number of required characters for float to string
 *         (decimal) conversion, minus increases the value by one.            */
ss_ushort ss_ftoa_len (ss_float f);

/** @brief Compares the string a to the string b.                             */
ss_char   ss_strcmp   (ss_str a, ss_str b);

#endif