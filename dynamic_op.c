/**
 *  @file dynamic_op.c
 *  @author André Dietrich
 *  @date 14 December 2016
 *
 *  @copyright Copyright 2016 André Dietrich. All rights reserved.
 *
 *  @license This project is released under the MIT-License.
 *
 *  @brief Implementation of dynamiC operations module.
 *
 *
 */

#include "dynamic.h"

#define max_type(A, B) (DYN_TYPE(A) > DYN_TYPE(B)) ? DYN_TYPE(A) : DYN_TYPE(B)

#define CHECK_COPY_REFERENCE(X1)        \
    if (DYN_TYPE(X1) == REFERENCE2)     \
        DYN_TYPE(X1) = REFERENCE;       \
    if(DYN_TYPE(X1) == REFERENCE)       \
        dyn_copy(X1->data.ref, X1);     \

#define CHECK_NOCOPY_REFERENCE(X2)      \
    if(DYN_IS_REFERENCE(X2))            \
        X2=X2->data.ref;                \


#define CHECK_REFERENCE(X1, X2)         \
    CHECK_COPY_REFERENCE(X1)            \
    CHECK_NOCOPY_REFERENCE(X2)


static dyn_ushort search (dyn_c *container, dyn_c *element)
{
    dyn_ushort i = 0;

    switch (DYN_TYPE(container)) {
        case DICT: {
            dyn_str key = dyn_get_string(element);
            i = dyn_dict_has_key(container, key);
            free(key);
            return i;
        }
        case SET:
        case LIST: {
            dyn_c tmp;
            DYN_INIT(&tmp);
            for (; i<DYN_LIST_LEN(container); ++i) {
                dyn_set_ref(&tmp, DYN_LIST_GET_REF(container, i));
                dyn_op_eq(&tmp, element);

                if (dyn_get_bool(&tmp))
                    return ++i;
            }
        }
    }

    return 0;
}

/**
 * The negation operation is only be applied onto numeric or boolean/trilean
 * data types. For all other types the result of dyn is set to type NONE.
 *
 * @param dyn in- and output dynamic element
 *
 * @retval DYN_TRUE   if operation could be applied onto the input data type
 * @retval DYN_FALSE  otherwise
 */
trilean dyn_op_neg (dyn_c* dyn)
{
    CHECK_COPY_REFERENCE(dyn)

    switch (DYN_TYPE(dyn)) {
        case NONE:    goto LABEL_OK;
        case BOOL:    dyn->data.b = !dyn->data.b;
                      goto LABEL_OK;
        case INTEGER: dyn->data.i = -dyn->data.i;
                      goto LABEL_OK;
        case FLOAT:   dyn->data.f = -dyn->data.f;
                      goto LABEL_OK;
    }

    dyn_free(dyn);
    return DYN_FALSE;

LABEL_OK:
    return DYN_TRUE;
}

/**
 * The add operation shows generates different results according to the input
 * data types. The resulting type is defined by the highest type, see the
 * following list for allowed operations:
 *
 * dyn_1     | dyn_2     | result type
 * ----------|-----------|------------
 * BOOL      | BOOL      | BOOL
 * INTEGER   | BOOL      | INTEGER
 * BOOL      | INTEGER   | INTEGER
 * INTEGER   | INTEGER   | INTEGER
 * FLOAT     | BOOL      | FLOAT
 * BOOL      | FLOAT     | FLOAT
 * FLOAT     | INTEGER   | FLOAT
 * INTEGER   | FLOAT     | FLOAT
 * FLOAT     | FLOAT     | FLOAT
 * STRING    | (NUMERIC) | STRING (concatenaited) "abc123"
 * (NUMERIC) | STRING    | STRING (concatenaited) "123abc"
 * STRING    | STRING    | STRING (concatenaited) "abcabc"
 * LIST      | ...       | LIST [, ...]
 * ...       | LIST      | LIST [... ,]
 * SET       | ...       | SET  {, ...} (unique elements only)
 * ...       | SET       | SET  {, ...} (unique elements only)
 * DICT      | DICT      | DICT {with all elements}
 *
 * @param[in, out] dyn1 in- and output parameter
 * @param[in]      dyn2 input parameter
 *
 * @retval DYN_TRUE   if operation could be applied onto the input data type
 * @retval DYN_FALSE  otherwise
 */
trilean dyn_op_add (dyn_c* dyn1, dyn_c* dyn2)
{
    CHECK_REFERENCE(dyn1, dyn2)

    dyn_c tmp;
    DYN_INIT(&tmp);

    if (DYN_TYPE(dyn1) && DYN_TYPE(dyn2)) {
        switch (max_type(dyn1, dyn2)) {
            case BOOL:

            case INTEGER: dyn_set_int(dyn1, dyn_get_int(dyn1) + dyn_get_int(dyn2));
                          goto LABEL_OK;
            case FLOAT:   dyn_set_float(dyn1, dyn_get_float(dyn1) + dyn_get_float(dyn2));
                          goto LABEL_OK;
            case STRING:  {
                if (DYN_TYPE(dyn1) == STRING) {
                    dyn1->data.str = (dyn_str) realloc(dyn1->data.str, dyn_strlen(dyn1->data.str) +
                                                                      dyn_string_len(dyn2) + 1 );
                    dyn_string_add(dyn2, dyn1->data.str);
                }
                else {
                    tmp.type = STRING;
                    tmp.data.str = (dyn_str) malloc(dyn_string_len(dyn1) + dyn_string_len(dyn2) + 1);
                    tmp.data.str[0]='\0';
                    dyn_string_add(dyn1, tmp.data.str);
                    dyn_string_add(dyn2, tmp.data.str);
                    dyn_move(&tmp, dyn1);
                }
                goto LABEL_OK;
            }
            case LIST: {
                if (DYN_TYPE(dyn1) == LIST) {
                    dyn_list_push(dyn1, dyn2);
                } else {
                    dyn_move(dyn1, &tmp);
                    dyn_copy(dyn2, dyn1);
                    dyn_list_insert(dyn1, &tmp, 0);
                }
                goto LABEL_OK;

            }
            case SET: {
                if (DYN_TYPE(dyn1) == DYN_TYPE(dyn2)) {
                    dyn_ushort i;
                    for (i=0; i<DYN_LIST_LEN(dyn2); ++i)
                        dyn_set_insert(dyn1, DYN_LIST_GET_REF(dyn2, i));
                } else if (DYN_TYPE(dyn1) == SET) {
                    dyn_set_insert(dyn1, dyn2);
                } else {
                    dyn_move(dyn1, &tmp);
                    dyn_copy(dyn2, dyn1);
                    dyn_set_insert(dyn1, &tmp);
                }
                goto LABEL_OK;
            }
            case DICT: {
                if (DYN_TYPE(dyn1) == DYN_TYPE(dyn2)) {
                    dyn_ushort i;
                    for (i=0; i<DYN_DICT_LEN(dyn2); ++i)
                        dyn_dict_insert(dyn1,
                                        DYN_DICT_GET_I_KEY(dyn2, i),
                                        DYN_DICT_GET_I_REF(dyn2, i));
                    goto LABEL_OK;
                }
            }
        }
    }

    dyn_free(dyn1);
    return DYN_FALSE;
LABEL_OK:
    return DYN_TRUE;
}

/**
 * The subtract operation shows generates different results according to the
 * input data types. Subtraction is allowed on numeric values and onto sets,
 * where the second parameter defines the element/set that shoult be subtracted
 * from the other set.
 *
 * @param[in, out] dyn1 in- and output parameter
 * @param[in]      dyn2 input parameter
 *
 * @retval DYN_TRUE   if operation could be applied onto the input data types
 * @retval DYN_FALSE  otherwise
 */
trilean dyn_op_sub (dyn_c* dyn1, dyn_c* dyn2)
{
    CHECK_REFERENCE(dyn1, dyn2)

    if ( DYN_TYPE(dyn1) && DYN_TYPE(dyn2) ) {
        switch (max_type(dyn1, dyn2)) {
            case BOOL:
            case INTEGER: dyn_set_int(dyn1, dyn_get_int(dyn1) - dyn_get_int(dyn2));
                          goto LABEL_OK;
            case FLOAT:   dyn_set_float(dyn1, dyn_get_float(dyn1) - dyn_get_float(dyn2));
                          goto LABEL_OK;
            case SET: {
                dyn_ushort pos;
                if (DYN_TYPE(dyn1) == DYN_TYPE(dyn2)) {
                    dyn_ushort i;
                    for (i=0; i<DYN_LIST_LEN(dyn2); ++i) {
                        pos = search(dyn1, DYN_LIST_GET_REF(dyn2, i));
                        if (pos)
                            dyn_list_remove(dyn1, pos-1);
                    }
                } else if (DYN_TYPE(dyn1) == SET) {
                    pos = search(dyn1, dyn2);
                    if (pos)
                        dyn_list_remove(dyn1, pos-1);
                } else {
                    dyn_c tmp;
                    DYN_INIT(&tmp);
                    dyn_move(dyn1, &tmp);
                    dyn_copy(dyn2, dyn1);
                    pos = search(dyn1, &tmp);
                    if (pos)
                        dyn_list_remove(dyn1, pos-1);
                }
                goto LABEL_OK;
            }
        }
    }

    dyn_free(dyn1);
    return DYN_FALSE;

LABEL_OK:
    return DYN_TRUE;
}

/**
 * Multiplication generates different results according to the input data types.
 * For NUMERIC and NUMERIC data types the common arithmetic multiplication gets
 * applied and for the combination of:
 * (LIST|STRING) * NUMERIC or NUMERIC * (LIST|STRING)
 * results in a repeated string or repeated list, multiplication with ZERO
 * generates an empty LIST or STRING, negative values are not allowed.
 *
 * @param[in, out] dyn1 in- and output parameter
 * @param[in]      dyn2 input parameter
 *
 * @retval DYN_TRUE   if operation could be applied onto the input data types
 *                    or values
 * @retval DYN_FALSE  otherwise
 */
trilean dyn_op_mul (dyn_c* dyn1, dyn_c* dyn2)
{
    CHECK_REFERENCE(dyn1, dyn2)

    if (DYN_TYPE(dyn1) && DYN_TYPE(dyn2)) {
        switch (max_type(dyn1, dyn2)) {
            case BOOL:
            case INTEGER: dyn_set_int(dyn1, dyn_get_int(dyn1) * dyn_get_int(dyn2));
                          goto LABEL_OK;
            case FLOAT:   dyn_set_float(dyn1, dyn_get_float(dyn1) * dyn_get_float(dyn2));
                          goto LABEL_OK;
            case STRING:  {
                dyn_ushort i;
                if (DYN_TYPE(dyn1) == INTEGER && DYN_TYPE(dyn2) == STRING) {
                    i = dyn_get_int(dyn1);
                    dyn_copy(dyn2, dyn1);
                } else if (DYN_TYPE(dyn1) == STRING && DYN_TYPE(dyn2) == INTEGER)
                    i = dyn_get_int(dyn2);
                else
                    break;
                switch (i) {
                    case 0: dyn_set_string(dyn1, "");
                    case 1: break;
                    default: {
                        dyn_ushort len = dyn_strlen(dyn1->data.str);
                        dyn1->data.str = (dyn_str) realloc(dyn1->data.str, len * i + 1);

                        dyn_str c = &dyn1->data.str[len];
                        dyn_ushort j;
                        while(--i) {
                            for(j=0; j<len; ++j) {
                                *c++ = dyn1->data.str[j];
                            }
                        }
                        *c = '\0';
                    }
                }
                goto LABEL_OK;
            }
            case LIST: {
                dyn_ushort i;
                if (DYN_TYPE(dyn1) == INTEGER && DYN_TYPE(dyn2) == LIST) {
                    i = dyn_get_int(dyn1);
                    dyn_copy(dyn2, dyn1);
                } else if (DYN_TYPE(dyn1) == LIST && DYN_TYPE(dyn2) == INTEGER)
                    i = dyn_get_int(dyn2);
                else
                    break;

                dyn_ushort len = DYN_LIST_LEN(dyn1);
                if (!i) {
                    dyn_set_list_len(dyn1, 1);
                    goto LABEL_OK;
                }
                if (i > 0) {
                    if (dyn_list_resize(dyn1, DYN_LIST_LEN(dyn1)*i ) ) {
                        dyn_ushort m, n;
                        for (m=1; m<i; ++m) {
                            for (n=0; n<len; n++)
                                dyn_list_push(dyn1, DYN_LIST_GET_REF(dyn1 ,n));
                        }
                        goto LABEL_OK;
                    }
                }
            }
        }
    }

    dyn_free(dyn1);
    return DYN_FALSE;

LABEL_OK:
    return DYN_TRUE;
}

/**
 * Dividing is only performed onto NUMERIC values, for all other values
 * DYN_FALSE gets returned.
 *
 * @param[in, out] dyn1 in- and output parameter
 * @param[in]      dyn2 input parameter
 *
 * @retval DYN_TRUE   if operation could be applied onto the input data types
 * @retval DYN_FALSE  otherwise
 */
trilean dyn_op_div (dyn_c* dyn1, dyn_c* dyn2)
{
    CHECK_REFERENCE(dyn1, dyn2)

    if ( DYN_TYPE(dyn1) && DYN_TYPE(dyn2) ) {
        switch (max_type(dyn1, dyn2)) {
            case BOOL:
            case INTEGER:   dyn_set_int(dyn1, dyn_get_int(dyn1) / dyn_get_int(dyn2));
                            goto LABEL_OK;
            case FLOAT:     dyn_set_float(dyn1, dyn_get_float(dyn1) / dyn_get_float(dyn2));
                            goto LABEL_OK;
        }
    }

    dyn_free(dyn1);
    return DYN_FALSE;

LABEL_OK:
    return DYN_TRUE;
}

/**
 * Modulo can only be performed onto NUMERIC values, if these are of type FLOAT
 * then they are casted to INTEGER, such that the result is always of type
 * INTEGER.
 *
 * @param[in, out] dyn1 in- and output(INTEGER) parameter
 * @param[in]      dyn2 input parameter
 *
 * @retval DYN_TRUE   if operation could be applied onto the input data types
 * @retval DYN_FALSE  otherwise
 */
trilean dyn_op_mod (dyn_c* dyn1, dyn_c* dyn2)
{
    CHECK_REFERENCE(dyn1, dyn2)

    dyn_char t1 = DYN_TYPE(dyn1);
    dyn_char t2 = DYN_TYPE(dyn2);

    if ( (t1 && t2) && t1 <= FLOAT && t2 <= FLOAT ) {
        dyn_set_int(dyn1, dyn_get_int(dyn1) % dyn_get_int(dyn2));
        return DYN_TRUE;
    }

    dyn_free(dyn1);
    return DYN_FALSE;
}

/**
 * @brief Power function approximation x^y
 *
 * This implementation is based on the fastapprox library of Paul Minero.
 *
 * @see https://code.google.com/archive/p/fastapprox/
 * @see http://www.machinedlearnings.com/2011/06/fast-approximate-logarithm-exponential.html
 *
 * @param x base value
 * @param y exponential value
 *
 * @returs approx. float value
 */
float fast_approx_pow (float x, float p)
{
    union { float f;
            unsigned int i;
    } vx = { x };

    union { unsigned int i;
            float f;
    } mx = { (vx.i & 0x007FFFFF) | 0x3f000000 };

    x = vx.i * 1.1920928955078125e-7f
        - 124.22551499f
        - 1.498030302f * mx.f
        - 1.72587999f / (0.3520887068f + mx.f);

    x *= p;

    float offset = (x < 0) ? 1.0f : 0.0f;
    float clipp = (x < -126) ? -126.0f : x;
    float z = clipp - (int) clipp + offset;
    mx.i =  (unsigned int) ( (1 << 23) * (clipp + 121.2740575f + 27.7280233f / (4.84252568f - z) - 1.49012907f * z) );

    return mx.f;
}

/**
 * Power function is only applied onto NUMERIC values less than FLOAT, in case
 * of a FLOAT value the system pow function gets applied or in case of the
 * usage of an embedded system, fast_approx_pow algorithm is applied.
 *
 * @param[in, out] dyn1 base value and result value
 * @param[in]      dyn2 exponent value
 *
 * @retval DYN_TRUE   if operation could be applied onto the input data types
 * @retval DYN_FALSE  otherwise
 */
trilean dyn_op_pow (dyn_c* dyn1, dyn_c* dyn2)
{
    CHECK_REFERENCE(dyn1, dyn2)

    if (DYN_IS_NONE(dyn1) || DYN_IS_NONE(dyn2)) {
        dyn_free(dyn1);
        return DYN_FALSE;
    }

    if (DYN_TYPE(dyn1) <= FLOAT && DYN_TYPE(dyn2) == FLOAT) {
        dyn_set_float(dyn1, fast_approx_pow(dyn_get_float(dyn1),
                                            dyn_get_float(dyn2)));
        return DYN_TRUE;
    }

    if (DYN_TYPE(dyn2) == INTEGER) {
        dyn_int exponent = dyn_get_int(dyn2);

        if (DYN_TYPE(dyn1) == INTEGER) {
            dyn_int base = dyn_get_int(dyn1);
            if (exponent > 0) {
                while (--exponent)
                    dyn1->data.i *= base;
            } else {
                dyn_set_float(dyn1, base);
                --exponent;
                while (exponent++)
                    dyn1->data.f /= base;
            }
            return DYN_TRUE;
        } else if (DYN_TYPE(dyn1) == FLOAT) {
            dyn_float base = dyn_get_float(dyn1);
            if (exponent > 0) {
                while (--exponent)
                    dyn1->data.f *= base;
            } else {
                --exponent;
                while (exponent++)
                    dyn1->data.f /= base;
            }
            return DYN_TRUE;
        }
    }

    dyn_free(dyn1);
    return DYN_FALSE;
}

/**
 * In case of a NONE value or a FUNCTION the unknown truth value DYN_NONE gets
 * returned otherwise the boolean truth value from dyn_get_bool.
 *
 * @param dyn input dynamic element
 *
 * @returns trilean truth value
 */
trilean dyn_get_bool_3 (dyn_c* dyn)
{
    if(DYN_IS_REFERENCE(dyn))
        dyn=dyn->data.ref;

    return (DYN_IS_NONE(dyn) || DYN_TYPE(dyn) == FUNCTION) ? DYN_NONE : dyn_get_bool(dyn);
}

/**
 * As depicted in the truth table below, this function performes the standard
 * boolean AND operation, the extention with the None boolean type does not
 * affect this operation. The truth value of every dynamic element is generated
 * with the help of function dyn_get_bool_3.
 *
 * AND          | True  | False | None
 * -------------|-------|-------|-------
 * <b>True </b> | True  | False | None
 * <b>False</b> | False | False | False
 * <b>None </b> | None  | False | None
 *
 * The result of this operation is stored within the first parameter dyn1, if
 * this value is a reference, then a new value is created, otherwise the
 * original value gets overwritten.
 *
 * @see dyn_get_bool_3
 * @see dyn_get_bool
 *
 * @param[out] dyn1 in- and output dynamic element
 * @param[in]  dyn2 second operand
 *
 * @return DYN_TRUE, operation can be applied on every combination of dynamic
 *         data types
 */
trilean dyn_op_and (dyn_c* dyn1, dyn_c* dyn2)
{
    dyn_char o1 = dyn_get_bool_3(dyn1);
    dyn_char o2 = dyn_get_bool_3(dyn2);

    if (o1 != DYN_NONE && o2 != DYN_NONE)
        dyn_set_bool(dyn1, o1 && o2);
    else if (o1 == DYN_FALSE || o2 == DYN_FALSE)
        dyn_set_bool(dyn1, DYN_FALSE);
    else
        dyn_free(dyn1);

    return DYN_TRUE;
}

/**
 * As depicted in the truth table below, this function performes the standard
 * boolean OR operation, the extention with the None boolean type does not
 * affect this operation. The truth value of every dynamic element is generated
 * with the help of function dyn_get_bool_3.
 *
 * OR           | True  | False | None
 * -------------|-------|-------|-------
 * <b>True </b> | True  | True  | True
 * <b>False</b> | True  | False | None
 * <b>None </b> | True  | None  | None
 *
 * The result of this operation is stored within the first parameter dyn1, if
 * this value is a reference, then a new value is created, otherwise the
 * original value gets overwritten.
 *
 * @see dyn_get_bool_3
 * @see dyn_get_bool
 *
 * @param[out] dyn1 in- and output dynamic element
 * @param[in]  dyn2 second operand
 *
 * @return DYN_TRUE, operation can be applied on every combination of dynamic
 *         data types
 */
trilean dyn_op_or (dyn_c* dyn1, dyn_c* dyn2)
{
    dyn_char o1 = dyn_get_bool_3(dyn1);
    dyn_char o2 = dyn_get_bool_3(dyn2);

    if (o1 != DYN_NONE && o2 != DYN_NONE)
        dyn_set_bool(dyn1, o1 || o2);
    else if (o1 == DYN_TRUE || o2 == DYN_TRUE)
        dyn_set_bool(dyn1, DYN_TRUE);
    else
        dyn_free(dyn1);

    return DYN_TRUE;
}


/**
 * As depicted in the truth table below, this function performes the standard
 * boolean XOR operation, the extention with the None boolean type does not
 * affect this operation. The truth value of every dynamic element is generated
 * with the help of function dyn_get_bool_3.
 *
 * XOR          | True  | False | None
 * -------------|-------|-------|------
 * <b>True </b> | False | True  | None
 * <b>False</b> | True  | False | None
 * <b>None </b> | None  | None  | None
 *
 * The result of this operation is stored within the first parameter dyn1, if
 * this value is a reference, then a new value is created, otherwise the
 * original value gets overwritten.
 *
 * @see dyn_get_bool_3
 * @see dyn_get_bool
 *
 * @param[out] dyn1 in- and output dynamic element
 * @param[in]  dyn2 second operand
 *
 * @return DYN_TRUE, operation can be applied on every combination of dynamic
 *         data types
 */
trilean dyn_op_xor (dyn_c* dyn1, dyn_c* dyn2)
{
    dyn_char o1 = dyn_get_bool_3(dyn1);
    dyn_char o2 = dyn_get_bool_3(dyn2);

    if (o1 != DYN_NONE && o2 != DYN_NONE)
        dyn_set_bool(dyn1, o1 != o2);
    else
        dyn_free(dyn1);

    return DYN_TRUE;
}

/**
 * As depicted in the truth table below, this function performes the standard
 * boolean NOT operation, the extention with the None boolean type does not
 * affect this operation. The truth value of every dynamic element is generated
 * with the help of function dyn_get_bool_3.
 *
 * NOT  | True  | False | None
 * -----|-------|-------|-------
 * NOT  | False | True  | None
 *
 * The result of this operation is stored within the first parameter dyn1, if
 * this value is a reference, then a new value is created, otherwise the
 * original value gets overwritten.
 *
 * @see dyn_get_bool_3
 * @see dyn_get_bool
 *
 * @param[out] dyn in- and output dynamic element
 *
 * @return DYN_TRUE, operation can be applied on every combination of dynamic
 *         data types
 */
trilean dyn_op_not (dyn_c* dyn)
{
    CHECK_COPY_REFERENCE(dyn)

    if (DYN_NOT_NONE(dyn))
        dyn_set_bool(dyn, !dyn_get_bool_3(dyn));

    return DYN_TRUE;
}

/**
 * @brief Common compare function for dynamic elements
 *
 * Basic compare function for dynamic data dypes, based on the relation between
 * the input parameters, different values are returned, see the list below.
 *
 * @param dyn1 first dynamic parameter
 * @param dyn2 second dynamic parameter
 *
 * @retval 0   if dyn1 == dyn2
 * @retval 1   if dyn1 < dyn2
 * @retval 2   if dyn1 > dyn2
 * @retval 3   if dyn1 != dyn2
 * @retval 4   if not comparable due to different data types (STRING <= SET)
 */
dyn_char dyn_op_cmp (dyn_c* dyn1, dyn_c* dyn2)
{
    enum{EQ,LT,GT,NEQ,TYPE,MARK=0xff};//0,1,2,3,4

    dyn_c  *tmp = DYN_IS_REFERENCE(dyn1) ? dyn1->data.ref : dyn1;
    dyn_ushort ret;
    dyn_c tmp2;
    DYN_INIT(&tmp2);
    dyn_ushort i;

    if(DYN_IS_REFERENCE(dyn2))
        dyn2=dyn2->data.ref;

    if (DYN_IS_NONE(tmp) && DYN_IS_NONE(dyn2))
        goto GOTO_EQ;

    if (DYN_IS_NONE(tmp) && DYN_NOT_NONE(dyn2))
        goto GOTO_LT;

    if (DYN_NOT_NONE(tmp) && DYN_IS_NONE(dyn2))
        goto GOTO_GT;

    //this switch has no breaks goto is used to leave to desired return
    switch (max_type(tmp, dyn2)) {
        case BOOL:
            if (dyn_get_bool(tmp) < dyn_get_bool(dyn2))
                goto GOTO_LT;
            if (dyn_get_bool(tmp) > dyn_get_bool(dyn2))
                goto GOTO_GT;
            goto GOTO_EQ;
        case INTEGER:
            if (dyn_get_int(tmp) < dyn_get_int(dyn2))
                goto GOTO_LT;
            if (dyn_get_int(tmp) > dyn_get_int(dyn2))
                goto GOTO_GT;
            goto GOTO_EQ;
        case FLOAT:
            if (dyn_get_float(tmp) < dyn_get_float(dyn2))
                goto GOTO_LT;
            if (dyn_get_float(tmp) > dyn_get_float(dyn2))
                goto GOTO_GT;
            goto GOTO_EQ;
        case STRING: {
            if (DYN_TYPE(tmp) != DYN_TYPE(dyn2))
                goto GOTO_TYPE;
            i = dyn_strcmp(tmp->data.str, dyn2->data.str);
            if (i < 0)
                goto GOTO_LT;
            if (i > 0)
                goto GOTO_GT;
            goto GOTO_EQ;
        }
        case SET: {
            if (DYN_TYPE(tmp) != DYN_TYPE(dyn2))
                goto GOTO_TYPE;
            {
                dyn_c * lset;
                dyn_c * rset;
                if (DYN_LIST_LEN(tmp) == DYN_LIST_LEN(dyn2)){
                    // list might be EQ
                    ret = EQ;
                    lset = tmp;
                    rset = dyn2;
                } else if (DYN_LIST_LEN(tmp) < DYN_LIST_LEN(dyn2)){
                    // list might be EQ
                    ret = LT;
                    lset = tmp;
                    rset = dyn2;
                } else if (DYN_LIST_LEN(tmp) > DYN_LIST_LEN(dyn2)){
                    // list might be GT
                    ret = GT;
                    lset = dyn2;
                    rset = tmp;
                }
                //check if all elements of lset are in rset
                //break to NEQ or complete to confirm
                for (i=0; i<DYN_LIST_LEN(lset); ++i) {
                    dyn_set_ref(&tmp2, DYN_LIST_GET_REF(lset, i));
                    dyn_op_in(&tmp2, rset);
                    if (!dyn_get_bool(&tmp2)){
                        ret = NEQ;
                        break;
                    }

                }
                goto GOTO_RET;

            }
            //unreachable
        }
        case LIST: {
            if (DYN_TYPE(tmp) != DYN_TYPE(dyn2))
                goto GOTO_TYPE;
            if (DYN_LIST_LEN(tmp) < DYN_LIST_LEN(dyn2))
                goto GOTO_LT;
            if (DYN_LIST_LEN(tmp) > DYN_LIST_LEN(dyn2))
                goto GOTO_GT;
            if (DYN_LIST_LEN(tmp) == DYN_LIST_LEN(dyn2)) {
                if (DYN_LIST_LEN(tmp) == 0)
                    goto GOTO_EQ;

                //put marker into return value
                ret = MARK;
                for (i=0; i<DYN_LIST_LEN(tmp); ++i) {
                    dyn_set_ref(&tmp2, DYN_LIST_GET_REF(tmp, i));
                    ret = dyn_op_cmp(&tmp2, DYN_LIST_GET_REF(dyn2, i));
                    if (ret != EQ)
                        break;
                }
                //reached the end of the for loop
                goto GOTO_RET;
            }
            //unreachable
        }
    }
    //unreachable
    // break with diffent type (string, list and set)
    goto GOTO_TYPE;
GOTO_TYPE:
    ret = TYPE;
    goto GOTO_RET;

GOTO_EQ:
    ret = EQ;
    goto GOTO_RET;
GOTO_LT:
    ret = LT;
    goto GOTO_RET;
GOTO_GT:
    ret = GT;
    goto GOTO_RET;
/*
GOTO_NEQ:
    ret = NEQ;
    goto GOTO_RET;
*/
GOTO_RET:
    //dyn_set_int(dyn1, ret);
    return ret;

}

/**
 * @param[in, out] dyn1 in and output parameter
 * @param[in]      dyn2 exponent value
 *
 * @returns DYN_TRUE, can be performed onto any combination of types
 */
trilean dyn_op_eq (dyn_c* dyn1, dyn_c* dyn2)
{
    dyn_set_bool(dyn1, dyn_op_cmp(dyn1, dyn2)
                       ? DYN_FALSE
                       : DYN_TRUE);
    return DYN_TRUE;
}

/**
 * @param[in, out] dyn1 in and output parameter
 * @param[in]      dyn2 exponent value
 *
 * @returns DYN_TRUE, can be performed onto any combination of types
 */
trilean dyn_op_ne (dyn_c* dyn1, dyn_c* dyn2)
{
    dyn_set_bool(dyn1, dyn_op_cmp(dyn1, dyn2)
                       ? DYN_TRUE
                       : DYN_FALSE);
    return DYN_TRUE;
}

/**
 * @param[in, out] dyn1 in and output parameter
 * @param[in]      dyn2 exponent value
 *
 * @returns DYN_TRUE, can be performed onto any combination of types
 */
trilean dyn_op_lt (dyn_c* dyn1, dyn_c* dyn2)
{
    dyn_char rslt = dyn_op_cmp (dyn1, dyn2);

    // types not comparable
    if (rslt == 4)
        dyn_free(dyn1);
    else
        dyn_set_bool(dyn1, (rslt == 1)
                           ? DYN_TRUE
                           : DYN_FALSE);
    return DYN_TRUE;
}

/**
 * @param[in, out] dyn1 in and output parameter
 * @param[in]      dyn2 exponent value
 *
 * @returns DYN_TRUE, can be performed onto any combination of types
 */
trilean dyn_op_ge (dyn_c* dyn1, dyn_c* dyn2)
{
    dyn_op_lt(dyn1, dyn2);
    dyn_op_not(dyn1);       // NONE remains NONE

    return DYN_TRUE;
}

/**
 * @param[in, out] dyn1 in and output parameter
 * @param[in]      dyn2 exponent value
 *
 * @returns DYN_TRUE, can be performed onto any combination of types
 */
trilean dyn_op_gt (dyn_c* dyn1, dyn_c* dyn2)
{
    dyn_char rslt = dyn_op_cmp (dyn1, dyn2);

    // types not comparable
    if (rslt == 4)
        dyn_free(dyn1);
    else
        dyn_set_bool(dyn1, (rslt == 2)
                           ? DYN_TRUE
                           : DYN_FALSE );
    return DYN_TRUE;
}

/**
 * @param[in, out] dyn1 in- and output parameter
 * @param[in]      dyn2 exponent value
 *
 * @returns DYN_TRUE, can be performed onto any combination of types
 */
trilean dyn_op_le (dyn_c* dyn1, dyn_c* dyn2)
{
    dyn_op_gt(dyn1, dyn2);
    dyn_op_not(dyn1); // NONE remains NONE

    return DYN_TRUE;
}

/**
 * Can only be applied if the container is of type LIST, SET, or DICT.
 *
 * @param[in, out] in and output parameter
 * @param[in]      dyn2 exponent value
 *
 * @retval DYN_TRUE   if the operation could be applied
 * @retval DYN_FALSE  otherwise
 */
trilean dyn_op_in (dyn_c *element, dyn_c *container)
{
    dyn_c *tmp = DYN_IS_REFERENCE(element) ? element->data.ref : element;

    if(DYN_IS_REFERENCE(container))
        container = container->data.ref;

    switch (DYN_TYPE(container)) {
        case SET:
        case LIST:
        case DICT:
            dyn_set_bool(element, search(container, tmp));
            return DYN_TRUE;
    }

    dyn_free(element);
    return DYN_FALSE;
}


trilean dyn_op_b_not(dyn_c *dyn)
{
    if (DYN_TYPE(dyn) == REFERENCE2)
        DYN_TYPE(dyn) = REFERENCE;

    if(DYN_TYPE(dyn) == REFERENCE)
        dyn_copy(dyn->data.ref, dyn);

    if (DYN_TYPE(dyn)==INTEGER) {
        dyn->data.i = ~dyn->data.i;
        return DYN_TRUE;
    }

    dyn_free(dyn);
    return DYN_FALSE;
}


trilean dyn_op_b_and(dyn_c *dyn1, dyn_c *dyn2)
{
    CHECK_REFERENCE(dyn1, dyn2)

    if (DYN_TYPE(dyn1)==INTEGER && DYN_TYPE(dyn2)==INTEGER) {
        dyn1->data.i = dyn1->data.i & dyn2->data.i;
        return DYN_TRUE;
    }

    dyn_free(dyn1);
    return DYN_FALSE;
}

trilean dyn_op_b_or(dyn_c *dyn1, dyn_c *dyn2)
{
    CHECK_REFERENCE(dyn1, dyn2)

    if (DYN_TYPE(dyn1)==INTEGER && DYN_TYPE(dyn2)==INTEGER) {
        dyn1->data.i = dyn1->data.i | dyn2->data.i;
        return DYN_TRUE;
    }

    dyn_free(dyn1);
    return DYN_FALSE;
}


trilean dyn_op_b_xor(dyn_c *dyn1, dyn_c *dyn2)
{
    CHECK_REFERENCE(dyn1, dyn2)

    if (DYN_TYPE(dyn1)==INTEGER && DYN_TYPE(dyn2)==INTEGER) {
        dyn1->data.i ^= dyn2->data.i;
        return DYN_TRUE;
    }

    dyn_free(dyn1);
    return DYN_FALSE;
}


trilean dyn_op_b_shift_l(dyn_c *dyn1, dyn_c *dyn2)
{
    CHECK_REFERENCE(dyn1, dyn2)

    if (DYN_TYPE(dyn1)==INTEGER && DYN_TYPE(dyn2)==INTEGER) {
        dyn1->data.i <<= dyn2->data.i;
        return DYN_TRUE;
    }

    dyn_free(dyn1);
    return DYN_FALSE;
}

trilean dyn_op_b_shift_r(dyn_c *dyn1, dyn_c *dyn2)
{
    CHECK_REFERENCE(dyn1, dyn2)

    if (DYN_TYPE(dyn1)==INTEGER && DYN_TYPE(dyn2)==INTEGER) {
        dyn1->data.i >>= dyn2->data.i;
        return DYN_TRUE;
    }

    dyn_free(dyn1);
    return DYN_FALSE;
}
