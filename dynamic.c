/**
 *  @file dynamic.c
 *  @author André Dietrich
 *  @date 14 December 2016
 *
 *  @copyright Copyright 2016 André Dietrich. All rights reserved.
 *
 *  @license This project is released under the MIT-License.
 *
 *  @brief Implementation of dynamiC base module.
 *
 *
 */

#include "dynamic.h"
#include <stdio.h>


/**
 * Frees any kind of dynamic type and convertes it to a NONE element. For the
 * the freeing of different data types the different free functions in the
 * different modules is applied.
 *
 * @see dyn_list_free
 * @see dyn_dict_free
 * @see dyn_fct_free
 *
 * @param[in, out] dyn element to free, result is of type NONE
 */
void dyn_free (dyn_c* dyn)
{
    switch (DYN_TYPE(dyn)) {
        case STRING:    free(dyn->data.str);
                        break;
#ifdef S2_SET
        case SET:
#endif
        case LIST:      dyn_list_free(dyn);
                        break;
        case DICT:      dyn_dict_free(dyn);
                        break;
        case FUNCTION:  dyn_fct_free(dyn);
    }
    dyn->type=NONE;
}

/**
 * It is more appropriate to apply the macro DYN_TYPE instead of calling this
 * function. This function is only applied to offer an interface if a compiled
 * library is used.
 *
 * @param dyn element to check
 *
 * @returns a value defined in enumeration TYPE
 */
TYPE dyn_type (dyn_c* dyn)
{
    return DYN_TYPE(dyn);
}

/**
 * The additional initialization of data.i is used to set the data element to
 * false. This is used to associate also NONE values directly with false.
 *
 * @param[in, out] dyn element
 */
void dyn_set_none (dyn_c* dyn)
{
    dyn_free(dyn);
    dyn->data.i = 0;
}

/**
 * @param[in, out] dyn element, which is either DYN_TRUE or DYN_FALSE
 * @param[in] v boolean value
 */
void dyn_set_bool (dyn_c* dyn, dyn_char v)
{
    dyn_free(dyn);
    dyn->type = BOOL;
    dyn->data.b = v;
}

/**
 * @param[in, out] dyn element, which is set to INTEGER
 * @param[in] v integer value
 */
void dyn_set_int (dyn_c* dyn, dyn_int v)
{
    dyn_free(dyn);
    dyn->type = INTEGER;
    dyn->data.i = v;
}

/**
 * @param[in, out] dyn element, which is set to FLOAT
 * @param[in] v float value
 */
void dyn_set_float (dyn_c* dyn, dyn_float v)
{
    dyn_free(dyn);
    dyn->type = FLOAT;
    dyn->data.f = v;
}

/**
 * This function can be used to reference anything via void pointers.
 *
 * @param[in, out] dyn element, which is set to reference the void pointer
 * @param[in] v void value
 */
void dyn_set_extern (dyn_c* dyn, void* v)
{
    dyn_free(dyn);
    dyn->type = EXTERN;
    dyn->data.ex = v;
}

/**
 * An new char array is allocated to store the entire passed string v.
  *
 * @param[in, out] dyn element, which is set to STRING
 * @param[in] v C-string value
 *
 * @retval DYN_TRUE if the memory could allocated
 * @retval DYN_FALSE otherwise
 */
trilean dyn_set_string (dyn_c* dyn, char const * v)
{
    dyn_free(dyn);

    dyn->data.str = (dyn_str) malloc(dyn_strlen((dyn_str)v)+1);

    if (dyn->data.str) {
        dyn->type = STRING;
        dyn_strcpy(dyn->data.str, (dyn_str)v);
        return DYN_TRUE;
    }
    return DYN_FALSE;
}

/**
 * This function is used to store a reference to a dynamic element instead of
 * copying it.
 *
 * @param[in, out] ref element, which is used to reference the original element
 * @param[in] orig original dynamic element
 */
void dyn_set_ref (dyn_c* ref, dyn_c* orig)
{
    dyn_free(ref);

    ref->type = REFERENCE;
    ref->data.ref = DYN_IS_REFERENCE(orig) ? orig->data.ref : orig;
}

/**
 * This function is intended calculate the size of an dynamic element in bytes.
 *
 * @param[in, out] dyn element to check
 *
 * @returns size in bytes
 */
dyn_uint dyn_size (dyn_c* dyn)
{
    dyn_uint bytes = sizeof(dyn_c);

    switch (DYN_TYPE(dyn)) {
        case STRING: {
            bytes += dyn_strlen(dyn->data.str)+1;
            break;
        }
#ifdef S2_SET
        case SET:
#endif
        case LIST: {
            bytes += sizeof(dyn_list);

            dyn_ushort len = dyn->data.list->space;
            dyn_ushort i;
            for (i=0; i<len; ++i)
                bytes += dyn_size( DYN_LIST_GET_REF(dyn, i) );

            break;
        }
        case DICT: {
            bytes += sizeof(dyn_dict);
            bytes += dyn_size(&dyn->data.dict->value);

            dyn_ushort len = dyn->data.dict->value.data.list->space;
            dyn_ushort i;
            for (i=0; i<len; ++i) {
                if (dyn->data.dict->key[i])
                    bytes += dyn_strlen(dyn->data.dict->key[i]);
                bytes += 1;
            }

            break;
        }
        case FUNCTION: {
            bytes += sizeof(dyn_fct);
            bytes += dyn_strlen(dyn->data.fct->info)+1;
            break;
        }
    }

    return bytes;
}

/**
 * The conversion of dynamic values into boolean truth values is similar to the
 * one in Python. NONE, FUNCTION, etc. elements are also converted into
 * DYN_FALSE in contrast to dyn_get_bool_3, which return DYN_NONE as a third
 * truth value.
 *
 * @code
 * dyn_get_bool(NONE)      == DYN_FALSE
 * // BOOL
 * dyn_get_bool(DYN_TRUE)  == DYN_TRUE
 * dyn_get_bool(DYN_FALSE) == DYN_FALSE
 * // INTEGER
 * dyn_get_bool(0)         == DYN_FALSE
 * otherwise                  DYN_TRUE
 * // FLOAT
 * dyn_get_bool(0.0)       == DYN_FALSE
 * otherwise                  DYN_TRUE
 *
 * // STRING, LIST, SET, DICT
 * if empty                   DYN_FALSE
 * else                       DYN_TRUE
 *
 * // everything else results in ...
 *                             DYN_FALSE
 * @endcode
 *
 * @see dyn_get_bool_3
 *
 * @param dyn dynamic element to check for boolean truth value
 *
 * @return DYN_TRUE or DYN_FALSE according to the examples above
 */
trilean dyn_get_bool (dyn_c* dyn)
{
START:
    switch (DYN_TYPE(dyn)) {
        case BOOL:      return dyn->data.b ? DYN_TRUE : DYN_FALSE;
        case INTEGER:   return dyn->data.i ? DYN_TRUE : DYN_FALSE;
        case FLOAT:     return dyn->data.f ? DYN_TRUE : DYN_FALSE;

        case STRING:
#ifdef S2_SET
        case SET:
#endif
        case LIST:
        case DICT:
                        return dyn_length(dyn) ? DYN_TRUE : DYN_FALSE;
        case REFERENCE2:
        case REFERENCE: dyn=dyn->data.ref;
                        goto START;
    }

    return DYN_FALSE;
}

/**
 * This function returns the integer value of an dynamic element, boolean,
 * INTEGER, and (casted)FLOAT values can be used. For all other element 0
 * is returned, that is why, the type should be checked previously as follows:
 *
 * @code
 * if (DYN_TYPE(dyn) && DYN_TYPE(dyn) <= FLOAT) {
 *    // it can be converted to an integer
 * }
 * @codend
 *
 * @param dyn element to convert to an int value
 *
 * @returns converted integer value
 */
dyn_int dyn_get_int (dyn_c* dyn)
{
START:
    switch (DYN_TYPE(dyn)) {
        case BOOL:      return dyn->data.b ? 1 : 0;
        case INTEGER:   return dyn->data.i;
        case FLOAT:     return (dyn_int)dyn->data.f;
        case REFERENCE2:
        case REFERENCE: dyn=dyn->data.ref;
                        goto START;
    }
    return 0;
}

/**
 * This function returns the FLOAT value of an dynamic element, boolean,
 * INTEGER, and FLOAT values can be used. For all other element NaN is
 * returned, in contrast to dyn_get_int.
 *
 * @see dyn_get_int
 *
 * @params dyn element to convert to float
 *
 * @returns converted float value
 */
dyn_float dyn_get_float (dyn_c* dyn)
{
START:
    switch (DYN_TYPE(dyn)) {
        case BOOL:      return dyn->data.b ? 1 : 0;
        case INTEGER:   return (dyn_float)dyn->data.i;
        case FLOAT:     return dyn->data.f;
        case REFERENCE2:
        case REFERENCE: dyn=dyn->data.ref;
                        goto START;
    }
    return 0.0/0.0; // Not a Number
}

/**
 * @param dyn element of type EXTERN or type REFERENCE that refers to EXTERN
 *
 * @returns void pointer or NULL if a false type was passed
 */
void* dyn_get_extern (dyn_c* dyn)
{
    if (DYN_IS_REFERENCE(dyn))
        dyn = dyn->data.ref;

    if(DYN_TYPE(dyn) == EXTERN)
        return dyn->data.ex;

    return NULL;
}

/**
 * Returns a string representation, of the passed element, new memory is
 * automatically allocated and has to be freed afterwards. Also if a STRING is
 * passed, then new memory is allocated.
 *
 * @see dyn_get_int
 *
 * @params dyn element to convert to STRING
 *
 * @returns pointer to a C-string
 */
dyn_str dyn_get_string (dyn_c* dyn)
{
    if (DYN_IS_REFERENCE(dyn))
        dyn = dyn->data.ref;

    dyn_str string = (dyn_str) malloc(dyn_string_len(dyn) + 1);
    if (string) {
        string[0] = '\0';
        /*if (DYN_TYPE(dyn) == STRING)
            dyn_strcpy(string, dyn->data.str);
        else*/
        dyn_string_add(dyn, string);
    }

    return string;
}

/**
 * Basic (recursive) function to attach string representations of dynamic
 * element to a string.
 *
 * @params[in] dyn element to convert to string
 * @params[in,out] string to attach the converted element to
 *
 * @returns converted float value
 */
void dyn_string_add (dyn_c* dyn, dyn_str string)
{
START:
    switch (DYN_TYPE(dyn)) {
        case BOOL:      dyn_strcat(string, dyn->data.b ? (dyn_str)"1": (dyn_str)"0");
                        break;
        case INTEGER:   dyn_itoa(&string[dyn_strlen(string)], dyn->data.i);
                        break;
        case FLOAT:     dyn_ftoa(&string[dyn_strlen(string)], dyn->data.f);
                        break;
        case STRING:    dyn_strcat(string, dyn->data.str);
                        break;
        case EXTERN:    dyn_strcat(string, (dyn_str)"ex");
                        //dyn_itoa(&string[dyn_strlen(string)], *((dyn_int*) dyn->data.ex));
                        break;
        case FUNCTION:  dyn_strcat(string, (dyn_str)"FCT");
                        break;
        case LIST:      dyn_list_string_add(dyn, string);
                        break;
#ifdef S2_SET
        case SET: {     dyn_uint i = dyn_strlen(string);
                        dyn_list_string_add(dyn, string);
                        string[i] = '{';
                        string[dyn_strlen(string)-1] = '}';
                        break;
        }
#endif
        case DICT:      dyn_dict_string_add (dyn, string);
                        break;
        case REFERENCE2:
        case REFERENCE: dyn=dyn->data.ref;
                        goto START;
        case MISCELLANEOUS: dyn_strcat(string, (dyn_str)"$");
    }
}

/**
 * @params dyn any kind of element
 *
 * @returns length of string representation
 */
dyn_ushort dyn_string_len (dyn_c* dyn)
{
    dyn_ushort len = 0;

START:

    switch (DYN_TYPE(dyn)) {
        case MISCELLANEOUS:
        case BOOL:      len = 1;                        break;
        case INTEGER:   len = dyn_itoa_len(dyn->data.i); break;
        case FLOAT:     len = dyn_ftoa_len(dyn->data.f); break;
        case EXTERN:    len = 2; // + dyn_itoa_len(*((dyn_int*) dyn->data.ex));
                        break;
        case FUNCTION:  len = 3;                        break;
        case STRING:    len = dyn_strlen(dyn->data.str); break;
#ifdef S2_SET
        case SET:
#endif
        case LIST:      len = dyn_list_string_len(dyn); break;
        case DICT:      len = dyn_dict_string_len(dyn); break;
        case REFERENCE2:
        case REFERENCE: dyn=dyn->data.ref;
                        goto START;
    }
    return len;
}

/**
 * Basic (recursive) copy function for creating deep copies of dynamic elements.
 *
 * @params[in] dyn original element
 * @params[in,out] copy newly created element
 *
 * @retval DYN_TRUE if element could be copied
 * @retval DYN_FALSE otherwise
 */
trilean dyn_copy (dyn_c* dyn, dyn_c* copy)
{
    switch (DYN_TYPE(dyn)) {
        case STRING:    return dyn_set_string( copy, dyn->data.str );
        case LIST:      return dyn_list_copy ( dyn, copy );
#ifdef S2_SET
        case SET:       if ( !dyn_list_copy(dyn, copy) )
                            return 0;
                        copy->type = SET;
                        break;
#endif
        case DICT:      return dyn_dict_copy( dyn, copy );
        case FUNCTION:  return dyn_fct_copy  ( dyn, copy );
        case REFERENCE: return dyn_copy ( dyn->data.ref, copy );
        default: *copy = *dyn;
    }

    return DYN_TRUE;
}

/**
 * Basic move function which moves the element from one dynamic element to
 * another, without copying. This function can be used for moving heavy data
 * types such as lists, sets, strings, etx. The from element is afterwards set
 * to NONE.
 *
 * @params[in] from original element
 * @params[in,out] to new element
 */
void dyn_move (dyn_c* from, dyn_c* to)
{
    dyn_free(to);

    if (DYN_TYPE(from) == REFERENCE)
        dyn_copy(from->data.ref, to);
    else
        *to = *from;

    if (DYN_TYPE(to) == DICT)
        dyn_dict_set_loc(to);

    DYN_INIT(from);
}

/**
 * This function is used to calculate the length of diffent elements. This
 * function is later used by dyn_get_bool to calculate the boolean value of
 * of an element.
 *
 * @param dyn element of any type
 *
 * @returns calculated length
 */
dyn_ushort dyn_length (dyn_c* dyn)
{
START:
    switch (DYN_TYPE(dyn)) {
        case STRING:    return dyn_strlen(dyn->data.str);
#ifdef S2_SET
        case SET:
#endif
        case LIST:      return DYN_LIST_LEN(dyn);
        case DICT:      return DYN_DICT_LEN(dyn);
        case REFERENCE2:
        case REFERENCE: dyn=dyn->data.ref;
                        goto START;
    }

    return 0;
}
