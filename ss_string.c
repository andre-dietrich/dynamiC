#include "ss_string.h"

ss_ushort ss_strlen(ss_str str)
{
    ss_ushort len = 0;

    while (*str++)
        ++len;

    return len;
}

void ss_strcat(ss_str a, ss_str b)
{
    ss_strcpy(&a[ss_strlen(a)], b);
}

void ss_strcat2(ss_str a, ss_str b)
{
    a = (ss_str) realloc(a, ss_strlen(a)+ss_strlen(b)+1);
    ss_strcat(a, b);
}


void ss_strcpy (ss_str a, ss_str b)
{
    while(*b)
        *a++=*b++;

    *a = '\0';
}

ss_ushort ss_itoa_len (ss_int i)
{
    if (!i) return 1;

    ss_ushort len = 0;

    if (i < 0) {
        i *= -1;
        ++len;
    }

    while (i)
        i/=10, ++len;

    return len;
}

void ss_itoa (ss_str str, ss_int i)
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

ss_ushort ss_ftoa_len (ss_float f)
{
    ss_ushort len = 1;

    ss_int a = (ss_int) f;
    ss_int b = (ss_int) ((f - a) * 100000.);

    len += ss_itoa_len(a);
    len += ss_itoa_len(b);

    return len;
}


void ss_ftoa (ss_str str, ss_float f)
{
    ss_int a = (ss_int) f;
    ss_int b = (ss_int) ((f - a) * 100000.);

    ss_itoa(str, a);

    ss_ushort len = ss_strlen(str);

    str[len] = '.';
    ss_itoa(&str[len+1], b < 0 ? -b : b);
}


ss_char ss_strcmp(ss_str a, ss_str b)
{
    while (*a == *b++) {
        if (*a++ == 0)
            return 0;
    }
    return (*a - *(b - 1));
}
