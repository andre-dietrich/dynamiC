#include "ss_string.h"

#define FLOAT_DIGITS 100000.

/**
 *  Iteratates through an character array to sum up its length, the end is
 *  defined by the character '\0'.
 *
 *  @param str Pointer to an character array
 *
 *  @return string length
 */
dyn_ushort ss_strlen(dyn_str str)
{
    dyn_ushort len = 0;

    while (*str++)
        ++len;

    return len;
}

/**
 *  Appends a copy of the source string to the destination string. The
 *  terminating null character in destination is overwritten by the first
 *  character of source, and a null-character is included at the end of the new
 *  string formed by the concatenation of both in destination.
 *
 *  destination and source shall not overlap and the length of destination must
 *  be sufficient for concatenation, otherwise use ss_strcat2.
 *
 *  @see ss_strcat2
 *
 *  @param destination  Pointer to the destination array, which should contain
 *                      a C string, and be large enough to contain the
 *                      concatenated resulting string.
 *  @param source       C string to be appended. This should not overlap
 *                      destination.
 */
void ss_strcat(dyn_str destination, dyn_str source)
{
    ss_strcpy(&destination[ss_strlen(destination)], source);
}

/**
 *  Appends a copy of the source string to the destination string and adds
 *  automatically required memory. The terminating null character in destination
 +  is overwritten by the first character of source, and a null-character is
 *  included at the end of the new string formed by the concatenation of both in
 *  destination.
 *
 *  destination and source shall not overlap.
 *
 *  @see ss_strcat
 *
 *  @param destination  Pointer to the destination array, which should contain
 *                      a C string, and be large enough to contain the
 *                      concatenated resulting string.
 *  @param source       C string to be appended. This should not overlap
 *                      destination.
 */
void ss_strcat2(dyn_str destination, dyn_str source)
{
    destination = (dyn_str) realloc(destination, ss_strlen(destination)+ss_strlen(source)+1);
    ss_strcat(destination, source);
}

/**
 *  Copies the C string pointed by source into the array pointed by destination,
 *  including the terminating null character (and stopping at that point).
 *
 *  To avoid overflows, the size of the array pointed by destination shall be
 *  long enough to contain the same C string as source (including the
 *  terminating null character), and should not overlap in memory with source.
 *
 *  @param [out] destination Pointer to the destination array where the content
 *                           is to be copied.
 *  @param [in]  source      C string to be copied.
 */
void ss_strcpy (dyn_str destination, dyn_str source)
{
    while(*source)
        *destination++=*source++;

    *destination = '\0';
}

/**
 *  Examples:
 *  @code
 *  ss_itoa_len(0)    == 1
 *  ss_itoa_len(1)    == 1
 *  ss_itoa_len(999)  == 3
 *  ss_itoa_len(-999) == 4
 *  @endcode
 *
 *  @param i  integer value to convert
 *  @returns  length of decimal string
 */
dyn_ushort ss_itoa_len (dyn_int i)
{
    if (!i) return 1;

    dyn_ushort len = 0;

    if (i < 0) {
        i *= -1;
        ++len;
    }

    while (i)
        i/=10, ++len;

    return len;
}

/**
 *  The length of the character array has to have a sufficient length, it can be
 *  calculated previously with function ss_itoa_len "(" str ")"
 *
 *  @see ss_ftoa
 *
 *  @param [out] str character array with ASCII representation of i
 *  @param [in]  i   integer value to convert
 */
void ss_itoa (dyn_str str, dyn_int i)
{
    char const digit[] = "0123456789";

    if (i<0) {
        *str++ = '-';
        i *= -1;
    }

    str += ss_itoa_len(i);

    *str = '\0';
    do {
        *--str = digit[i%10];
        i /= 10;
    } while(i);
}

/**
 *  @param f  float value to check
 *  @returns  string length
 */
dyn_ushort ss_ftoa_len (dyn_float f)
{
    dyn_ushort len = 1;

    dyn_int a = (dyn_int) f;
    dyn_int b = (dyn_int) ((f - a) * FLOAT_DIGITS);

    len += ss_itoa_len(a);
    len += ss_itoa_len(b);

    return len;
}

/**
 *  The length of the character-array has to be sufficient, it can be
 *  calculated previously with function ss_ftoa_len.
 *
 *  @see ss_ftoa_len
 *  @see ss_itoa
 *
 *  @param [out] str character array with with new ASCII representation of f
 *  @param [in]  f   float value to convert
 */
void ss_ftoa (dyn_str str, dyn_float f)
{
    dyn_int a = (dyn_int) f;
    dyn_int b = (dyn_int) ((f - a) * FLOAT_DIGITS);

    ss_itoa(str, a);

    dyn_ushort len = ss_strlen(str);

    str[len] = '.';
    ss_itoa(&str[len+1], b < 0 ? -b : b);
}

/**
 *  This function starts comparing the first character of each string. If they
 *  are equal to each other, it continues with the following pairs until the
 *  characters differ or until a terminating '\0' is reached.
 *
 *  @param a  char array to be compared
 *  @param b  char array to be compared
 *
 *  @retval <0	the first character that does not match has a lower value in a
 *              than in b
 *  @retval 0	  the contents of both strings are equal
 *  @retval >0	the first character that does not match has a greater value in a
 *              than in b
 */
dyn_char ss_strcmp(dyn_str a, dyn_str b)
{
    while (*a == *b++) {
        if (*a++ == 0)
            return 0;
    }
    return (*a - *(b - 1));
}
