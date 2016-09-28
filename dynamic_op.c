#include "dynamic.h"

#define max_type(A, B) (DYN_TYPE(A) > DYN_TYPE(B)) ? DYN_TYPE(A) : DYN_TYPE(B)

#define USEOPCMP

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


ss_ushort search (dyn_c *container, dyn_c *element)
{
    ss_ushort i = 0;

    switch (DYN_TYPE(container)) {
        case DICT: {
            ss_str key = dyn_get_string(element);
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

ss_char dyn_op_neg (dyn_c* dyn)
{
    CHECK_COPY_REFERENCE(dyn)

    switch (DYN_TYPE(dyn)) {
        case BOOL:    dyn->data.b = !dyn->data.b;
                      return DYN_TRUE;
        case INTEGER: dyn->data.i = -dyn->data.i;
                      return DYN_TRUE;
        case FLOAT:   dyn->data.f = -dyn->data.f;
                      return DYN_TRUE;
    }

    dyn_free(dyn);
    return DYN_FALSE;
}


ss_char dyn_op_add (dyn_c* dyn1, dyn_c* dyn2)
{
    CHECK_REFERENCE(dyn1, dyn2)

    dyn_c tmp;
    DYN_INIT(&tmp);

    if (DYN_TYPE(dyn1) && DYN_TYPE(dyn2)) {
        switch (max_type(dyn1, dyn2)) {
            case BOOL:
            case INTEGER: dyn_set_int(dyn1, dyn_get_int(dyn1) + dyn_get_int(dyn2));
                          return DYN_TRUE;
            case FLOAT:   dyn_set_float(dyn1, dyn_get_float(dyn1) + dyn_get_float(dyn2));
                          return DYN_TRUE;
            case STRING:  {
                if (DYN_TYPE(dyn1) == STRING) {
                    dyn1->data.str = (ss_str) realloc(dyn1->data.str, ss_strlen(dyn1->data.str) +
                                                                      dyn_string_len(dyn2) + 1 );
                    dyn_string_add(dyn2, dyn1->data.str);
                }
                else {
                    tmp.type = STRING;
                    tmp.data.str = (ss_str) malloc(dyn_string_len(dyn1) + dyn_string_len(dyn2) + 1);
                    tmp.data.str[0]='\0';
                    dyn_string_add(dyn1, tmp.data.str);
                    dyn_string_add(dyn2, tmp.data.str);
                    dyn_move(&tmp, dyn1);
                }
                return DYN_TRUE;
            }
            case LIST: {
                if (DYN_TYPE(dyn1) == LIST) {
                    //ss_ushort i;
                    //for (i=0; i<DYN_LIST_LEN(dyn2); ++i)
                    //    dyn_list_push(dyn1, DYN_LIST_GET_REF(dyn2, i));
                    dyn_list_push(dyn1, dyn2);
                //} else if (DYN_TYPE(dyn1) == LIST) {
                //    dyn_list_push(dyn1, dyn2);
                } else {
                    dyn_move(dyn1, &tmp);
                    dyn_copy(dyn2, dyn1);
                    dyn_list_insert(dyn1, &tmp, 0);
                }
                return DYN_TRUE;

            }
            case SET: {
                if (DYN_TYPE(dyn1) == DYN_TYPE(dyn2)) {
                    ss_ushort i;
                    for (i=0; i<DYN_LIST_LEN(dyn2); ++i)
                        dyn_set_insert(dyn1, DYN_LIST_GET_REF(dyn2, i));
                } else if (DYN_TYPE(dyn1) == SET) {
                    dyn_set_insert(dyn1, dyn2);
                } else {
                    dyn_move(dyn1, &tmp);
                    dyn_copy(dyn2, dyn1);
                    dyn_set_insert(dyn1, &tmp);
                }
                return DYN_TRUE;
            }
            case DICT: {
                if (DYN_TYPE(dyn1) == DYN_TYPE(dyn2)) {
                    ss_ushort i;
                    for (i=0; i<DYN_DICT_LEN(dyn2); ++i)
                        dyn_dict_insert(dyn1,
                                        DYN_DICT_GET_I_KEY(dyn2, i),
                                        DYN_DICT_GET_I_REF(dyn2, i));
                    //dyn1->data.dict->meta |= dyn2->data.dict->meta;
                }
            }
        }
    }

    dyn_free(dyn1);
    return DYN_FALSE;
}

ss_char dyn_op_sub (dyn_c* dyn1, dyn_c* dyn2)
{
    CHECK_REFERENCE(dyn1, dyn2)

    if ( DYN_TYPE(dyn1) && DYN_TYPE(dyn2) ) {
        switch (max_type(dyn1, dyn2)) {
            case BOOL:
            case INTEGER: dyn_set_int(dyn1, dyn_get_int(dyn1) - dyn_get_int(dyn2));
                          return DYN_TRUE;
            case FLOAT:   dyn_set_float(dyn1, dyn_get_float(dyn1) - dyn_get_float(dyn2));
                          return DYN_TRUE;
            case SET: {
                ss_ushort pos;
                if (DYN_TYPE(dyn1) == DYN_TYPE(dyn2)) {
                    ss_ushort i;
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
                return DYN_TRUE;
            }
        }
    }

    dyn_free(dyn1);
    return DYN_FALSE;
}


ss_char dyn_op_mul (dyn_c* dyn1, dyn_c* dyn2)
{
    CHECK_REFERENCE(dyn1, dyn2)

    if (DYN_TYPE(dyn1) && DYN_TYPE(dyn2)) {
        switch (max_type(dyn1, dyn2)) {
            case BOOL:
            case INTEGER: dyn_set_int(dyn1, dyn_get_int(dyn1) * dyn_get_int(dyn2));
                          return DYN_TRUE;
            case FLOAT:   dyn_set_float(dyn1, dyn_get_float(dyn1) * dyn_get_float(dyn2));
                          return DYN_TRUE;
            case STRING:  {
                ss_ushort i;
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
                        ss_ushort len = ss_strlen(dyn1->data.str);
                        dyn1->data.str = (ss_str) realloc(dyn1->data.str, len * i + 1);

                        ss_str c = &dyn1->data.str[len];
                        ss_ushort j;
                        while(--i) {
                            for(j=0; j<len; ++j) {
                                *c++ = dyn1->data.str[j];
                            }
                        }
                        *c = '\0';
                    }
                }
                return DYN_TRUE;
            }
            case LIST: {
                ss_ushort i;
                if (DYN_TYPE(dyn1) == INTEGER && DYN_TYPE(dyn2) == LIST) {
                    i = dyn_get_int(dyn1);
                    dyn_copy(dyn2, dyn1);
                } else if (DYN_TYPE(dyn1) == LIST && DYN_TYPE(dyn2) == INTEGER)
                    i = dyn_get_int(dyn2);
                else
                    break;

                ss_ushort len = DYN_LIST_LEN(dyn1);
                if (!i) {
                    dyn_set_list_len(dyn1, 1);
                    return DYN_TRUE;
                }
                if (i > 0) {
                    if (dyn_list_resize(dyn1, DYN_LIST_LEN(dyn1)*i ) ) {
                        ss_ushort m, n;
                        for (m=1; m<i; ++m) {
                            for (n=0; n<len; n++)
                                dyn_list_push(dyn1, DYN_LIST_GET_REF(dyn1 ,n));
                        }
                        return DYN_TRUE;
                    }
                }
            }
        }
    }

    dyn_free(dyn1);
    return DYN_FALSE;
}


ss_char dyn_op_div (dyn_c* dyn1, dyn_c* dyn2)
{
    CHECK_REFERENCE(dyn1, dyn2)

    if ( DYN_TYPE(dyn1) && DYN_TYPE(dyn2) ) {
        switch (max_type(dyn1, dyn2)) {
            case BOOL:
            case INTEGER:   dyn_set_int(dyn1, dyn_get_int(dyn1) / dyn_get_int(dyn2));
                            return DYN_TRUE;
            case FLOAT:     dyn_set_float(dyn1, dyn_get_float(dyn1) / dyn_get_float(dyn2));
                            return DYN_TRUE;
        }
    }

    dyn_free(dyn1);
    return DYN_FALSE;
}


ss_char dyn_op_mod (dyn_c* dyn1, dyn_c* dyn2)
{
    CHECK_REFERENCE(dyn1, dyn2)

    if ( DYN_TYPE(dyn1) && DYN_TYPE(dyn2) ) {
        switch (max_type(dyn1, dyn2)) {
            case BOOL:
            case FLOAT:
            case INTEGER: dyn_set_int(dyn1, dyn_get_int(dyn1) % dyn_get_int(dyn2));
                          return DYN_TRUE;
        }
    }

    dyn_free(dyn1);
    return DYN_FALSE;
}

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


ss_char dyn_op_pow (dyn_c* dyn1, dyn_c* dyn2)
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
        ss_int exponent = dyn_get_int(dyn2);

        if (DYN_TYPE(dyn1) == INTEGER) {
            ss_int base = dyn_get_int(dyn1);
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
            ss_float base = dyn_get_float(dyn1);
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


ss_char dyn_get_bool_3 (dyn_c* dyn)
{
    if(DYN_IS_REFERENCE(dyn))
        dyn=dyn->data.ref;

    switch (DYN_TYPE(dyn)) {
        case BOOL:      return dyn->data.b ? DYN_TRUE : DYN_FALSE;
        case INTEGER:   return dyn->data.i ? DYN_TRUE : DYN_FALSE;
        case FLOAT:     return dyn->data.f ? DYN_TRUE : DYN_FALSE;

        case STRING:    return ss_strlen(dyn->data.str) ? DYN_TRUE : DYN_FALSE;

#ifdef S2_SET
        case SET:
#endif
#ifdef S2_LIST
        case LIST:      return DYN_LIST_LEN(dyn) ? DYN_TRUE : DYN_FALSE;
#endif
        case DICT:      return dyn_length(dyn) ? DYN_TRUE : DYN_FALSE;
    }

    return DYN_NONE;
}

ss_char dyn_op_and (dyn_c* dyn1, dyn_c* dyn2)
{
    ss_char o1 = dyn_get_bool_3(dyn1);
    ss_char o2 = dyn_get_bool_3(dyn2);

    if (o1 != DYN_NONE && o2 != DYN_NONE)
        dyn_set_bool(dyn1, o1 && o2);
    else if (o1 == DYN_FALSE || o2 == DYN_FALSE)
        dyn_set_bool(dyn1, DYN_FALSE);
    else
        dyn_free(dyn1);

    return DYN_TRUE;
}

ss_char dyn_op_or (dyn_c* dyn1, dyn_c* dyn2)
{
    ss_char o1 = dyn_get_bool_3(dyn1);
    ss_char o2 = dyn_get_bool_3(dyn2);

    if (o1 != DYN_NONE && o2 != DYN_NONE)
        dyn_set_bool(dyn1, o1 || o2);
    else if (o1 == DYN_TRUE || o2 == DYN_TRUE)
        dyn_set_bool(dyn1, DYN_TRUE);
    else
        dyn_free(dyn1);

    return DYN_TRUE;
}

ss_char dyn_op_xor (dyn_c* dyn1, dyn_c* dyn2)
{
    ss_char o1 = dyn_get_bool_3(dyn1);
    ss_char o2 = dyn_get_bool_3(dyn2);

    if (o1 != DYN_NONE && o2 != DYN_NONE)
        if (o1 == o2)
            dyn_set_bool(dyn1, DYN_FALSE);
        else
            dyn_set_bool(dyn1, DYN_TRUE);
    else
        dyn_free(dyn1);

    return DYN_TRUE;
}

ss_char dyn_op_not (dyn_c* dyn)
{
    ss_char o = dyn_get_bool_3(dyn);

    if (o == DYN_TRUE)
        dyn_set_bool(dyn, DYN_FALSE);
    else if (o == DYN_FALSE)
        dyn_set_bool(dyn, DYN_TRUE);
    else
        dyn_free(dyn);

    return DYN_TRUE;
}

ss_char dyn_op_cmp (dyn_c* dyn1, dyn_c* dyn2)
{
    enum{EQ,LT,GT,NEQ,TYPE,MARK=0xff};//0,1,2,3,4

    dyn_c  *tmp = DYN_IS_REFERENCE(dyn1) ? dyn1->data.ref : dyn1;
    ss_ushort ret;
    dyn_c tmp2;
    DYN_INIT(&tmp2);
    ss_ushort i;

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
            i = ss_strcmp(tmp->data.str, dyn2->data.str);
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
                //put marker into return value
                ret = MARK;
                for (i=0; i<DYN_LIST_LEN(tmp); ++i) {
                    dyn_set_ref(&tmp2, DYN_LIST_GET_REF(tmp, i));
                    dyn_op_cmp(&tmp2, DYN_LIST_GET_REF(dyn2, i));
                    if (ret != MARK && dyn_get_int(&tmp2) != ret)
                        goto GOTO_NEQ;
                    else
                        ret =  dyn_get_int(&tmp2);

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
GOTO_NEQ:
    ret = NEQ;
    goto GOTO_RET;

GOTO_RET:
    dyn_set_int(dyn1, ret);
    return DYN_TRUE;

}

ss_char dyn_op_eq (dyn_c* dyn1, dyn_c* dyn2)
{
#ifdef USEOPCMP
    dyn_op_cmp (dyn1, dyn2);
    if (dyn_get_int(dyn1) == 0 ) dyn_set_bool(dyn1, DYN_TRUE);
    else  dyn_set_bool(dyn1, DYN_FALSE);
    return DYN_TRUE;
#else
    dyn_c *tmp = DYN_IS_REFERENCE(dyn1) ? dyn1->data.ref : dyn1;

    if(DYN_IS_REFERENCE(dyn2))
        dyn2=dyn2->data.ref;

    if (DYN_IS_NONE(tmp) && DYN_IS_NONE(dyn2)) {
GOTO_TRUE:
        dyn_set_bool(dyn1, DYN_TRUE);
        return DYN_TRUE;
    }

    if((DYN_IS_NONE(tmp) && DYN_NOT_NONE(dyn2)) ||
       (DYN_NOT_NONE(tmp) && DYN_IS_NONE(dyn2))) {
GOTO_FALSE:
        dyn_set_bool(dyn1, DYN_FALSE);
        return DYN_TRUE;
    }

    switch (max_type(tmp, dyn2)) {
        case BOOL:      if (dyn_get_bool(tmp) == dyn_get_bool(dyn2))
                            goto GOTO_TRUE;
                        goto GOTO_FALSE;
        case INTEGER:   if (dyn_get_int(tmp) == dyn_get_int(dyn2))
                            goto GOTO_TRUE;
                        goto GOTO_FALSE;
        case FLOAT:     if (dyn_get_float(tmp) == dyn_get_float(dyn2))
                            goto GOTO_TRUE;
                        goto GOTO_FALSE;
        case STRING: {
            if (DYN_TYPE(tmp) != DYN_TYPE(dyn2))
                break;
            if (ss_strcmp(tmp->data.str, dyn2->data.str) == 0)
                goto GOTO_TRUE;
            goto GOTO_FALSE;
        }
        case SET:
        case LIST: {
            if (DYN_TYPE(tmp) != DYN_TYPE(dyn2))
                break;
            if (DYN_LIST_LEN(tmp) == DYN_LIST_LEN(dyn2)) {
                dyn_c tmp2;
                DYN_INIT(&tmp2);

                ss_ushort i;
                for (i=0; i<DYN_LIST_LEN(tmp); ++i) {
                    dyn_set_ref(&tmp2, DYN_LIST_GET_REF(tmp, i));
                    if (DYN_TYPE(tmp) == LIST)
                        dyn_op_eq(&tmp2, DYN_LIST_GET_REF(dyn2, i));
                    else
                        dyn_op_in(&tmp2, dyn2);

                    if (!dyn_get_bool(&tmp2))
                        goto GOTO_FALSE;
                }
                goto GOTO_TRUE;
            }
            goto GOTO_FALSE;
        }
    }

    goto GOTO_FALSE;
#endif
}


ss_char dyn_op_ne (dyn_c* dyn1, dyn_c* dyn2)
{
/*
    if (dyn_op_eq(dyn1, dyn2)) {
        dyn_set_bool(dyn1, !dyn1->data.b);
        return DYN_TRUE;
    }
    return DYN_FALSE;
*/
    return IF((dyn_op_eq(dyn1, dyn2)),
              (dyn_set_bool(dyn1, !dyn1->data.b), DYN_TRUE),
              DYN_FALSE);
}


char dyn_op_lt (dyn_c* dyn1, dyn_c* dyn2)
{
#ifdef USEOPCMP
    dyn_op_cmp (dyn1, dyn2);
    if (dyn_get_int(dyn1) == 4 ){
        dyn_free(dyn1);
        return DYN_FALSE;
    }
    if (dyn_get_int(dyn1) == 1 ) dyn_set_bool(dyn1, DYN_TRUE);
    else dyn_set_bool(dyn1, DYN_FALSE);
    return DYN_TRUE;
#else
    dyn_c *tmp = DYN_IS_REFERENCE(dyn1) ? dyn1->data.ref : dyn1;

    if(DYN_IS_REFERENCE(dyn2))
        dyn2=dyn2->data.ref;

    if (DYN_IS_NONE(tmp) && !DYN_IS_NONE(dyn2)) {
GOTO_TRUE:
        dyn_set_bool(dyn1, DYN_TRUE);
        return DYN_TRUE;
    }
    if (!DYN_IS_NONE(tmp) && DYN_IS_NONE(dyn2)) {
GOTO_FALSE:
        dyn_set_bool(dyn1, DYN_FALSE);
        return DYN_TRUE;
    }

    switch (max_type(tmp, dyn2)) {
        case NONE:    goto GOTO_FALSE;
        case BOOL:    if (dyn_get_bool(tmp) < dyn_get_bool(dyn2))
                          goto GOTO_TRUE;
                      goto GOTO_FALSE;
        case INTEGER: if (dyn_get_int(tmp) < dyn_get_int(dyn2))
                          goto GOTO_TRUE;
                      goto GOTO_FALSE;
        case FLOAT:   if (dyn_get_float(tmp) < dyn_get_float(dyn2))
                          goto GOTO_TRUE;
                      goto GOTO_FALSE;
        case STRING: {
            if (DYN_TYPE(tmp) != DYN_TYPE(dyn2))
                break;
            if (ss_strcmp(tmp->data.str, dyn2->data.str) < 0)
                goto GOTO_TRUE;
            goto GOTO_FALSE;
        }
        case SET:
        case LIST: {
            if (DYN_TYPE(tmp) != DYN_TYPE(dyn2))
                break;
            if ((DYN_TYPE(tmp) == LIST && DYN_LIST_LEN(tmp) == DYN_LIST_LEN(dyn2)) ||
                (DYN_TYPE(tmp) == SET && DYN_LIST_LEN(tmp) <= DYN_LIST_LEN(dyn2))) {

                dyn_c tmp2;
                DYN_INIT(&tmp2);

                ss_ushort i;
                for (i=0; i<DYN_LIST_LEN(tmp); ++i) {
                    dyn_set_ref(&tmp2, DYN_LIST_GET_REF(tmp, i));

                    if (DYN_TYPE(tmp) == LIST)
                        dyn_op_lt(&tmp2, dyn2++);
                    else
                        dyn_op_in(&tmp2, dyn2);

                    if (!dyn_get_bool(&tmp2))
                        goto GOTO_FALSE;
                }
                goto GOTO_TRUE;
            }

            if (DYN_LIST_LEN(tmp) < DYN_LIST_LEN(dyn2))
                goto GOTO_TRUE;
            goto GOTO_FALSE;
        }
    }

    dyn_free(dyn1);
    return DYN_FALSE;
#endif
}

ss_char dyn_op_ge (dyn_c* dyn1, dyn_c* dyn2)
{
    if (dyn_op_lt(dyn1, dyn2)) {
        dyn_set_bool(dyn1, !dyn1->data.b);
        return DYN_TRUE;
    }

    return DYN_FALSE;
}

ss_char dyn_op_gt (dyn_c* dyn1, dyn_c* dyn2)
{
#ifdef USEOPCMP
    dyn_op_cmp (dyn1, dyn2);
    if (dyn_get_int(dyn1) == 4 ){
        dyn_free(dyn1);
        return DYN_FALSE;
    }
    if (dyn_get_int(dyn1) == 2 ) dyn_set_bool(dyn1, DYN_TRUE);
    else dyn_set_bool(dyn1, DYN_FALSE);
    return DYN_TRUE;
#else
    dyn_c *tmp = DYN_IS_REFERENCE(dyn1) ? dyn1->data.ref : dyn1;

    if(DYN_IS_REFERENCE(dyn2))
        dyn2=dyn2->data.ref;

    if (!DYN_IS_NONE(tmp) &&  DYN_IS_NONE(dyn2)) {
GOTO_TRUE:
        dyn_set_bool(dyn1, DYN_TRUE);
        return DYN_TRUE;
    }
    if (DYN_IS_NONE(tmp) &&  DYN_IS_NONE(dyn2)) {
GOTO_FALSE:
        dyn_set_bool(dyn1, DYN_FALSE);
        return DYN_TRUE;
    }
    if (DYN_IS_NONE(tmp) && !DYN_IS_NONE(dyn2)) {
        goto GOTO_FALSE;
    }

    dyn_c tmp2;
    DYN_INIT(&tmp2);
    ss_ushort i;

    switch (max_type(tmp, dyn2)) {
        case BOOL:    if (dyn_get_bool(tmp) > dyn_get_bool(dyn2))
                          goto GOTO_TRUE;
                      goto GOTO_FALSE;
        case INTEGER: if (dyn_get_int(tmp) > dyn_get_int(dyn2))
                          goto GOTO_TRUE;
                      goto GOTO_FALSE;
        case FLOAT:   if (dyn_get_float(tmp) > dyn_get_float(dyn2))
                          goto GOTO_TRUE;
                      goto GOTO_FALSE;
        case STRING:  {
            if (DYN_TYPE(tmp) != DYN_TYPE(dyn2))
                break;
            if (ss_strcmp(tmp->data.str, dyn2->data.str) > 0)
                goto GOTO_TRUE;
            goto GOTO_FALSE;
        }
        case LIST: {
            if (DYN_TYPE(tmp) != DYN_TYPE(dyn2))
                break;
            if (DYN_LIST_LEN(tmp) == DYN_LIST_LEN(dyn2)) {
                for (i=0; i<DYN_LIST_LEN(dyn1); ++i) {
                    dyn_set_ref(&tmp2, DYN_LIST_GET_REF(tmp, i));
                    dyn_op_gt(&tmp2, dyn2++);

                    if (!dyn_get_bool(&tmp2))
                        goto GOTO_FALSE;
                }
                goto GOTO_TRUE;
            }
            if (DYN_LIST_LEN(tmp) > DYN_LIST_LEN(dyn2))
                goto GOTO_TRUE;
            goto GOTO_FALSE;
        }
        case SET: {
            if (DYN_TYPE(tmp) != DYN_TYPE(dyn2))
                break;
            if (DYN_LIST_LEN(tmp) >= DYN_LIST_LEN(dyn2)) {
                for (i=0; i<DYN_LIST_LEN(dyn2); ++i) {
                    dyn_set_ref(&tmp2, DYN_LIST_GET_REF(dyn2, i));
                    dyn_op_in(&tmp2, tmp);

                    if (!dyn_get_bool(&tmp2))
                        goto GOTO_FALSE;
                }
                goto GOTO_TRUE;
            }
            goto GOTO_FALSE;
        }
    }

    dyn_free(dyn1);
    return DYN_FALSE;
#endif
}


ss_char dyn_op_le (dyn_c* dyn1, dyn_c* dyn2)
{
    if (dyn_op_gt(dyn1, dyn2)) {
        dyn_set_bool(dyn1, !dyn1->data.b);
        return DYN_TRUE;
    }

    return DYN_FALSE;
}


ss_char dyn_op_in (dyn_c *dyn1, dyn_c *dyn2)
{
    dyn_c *tmp = DYN_IS_REFERENCE(dyn1) ? dyn1->data.ref : dyn1;

    if(DYN_IS_REFERENCE(dyn2))
        dyn2 = dyn2->data.ref;

    switch (DYN_TYPE(dyn2)) {
        case SET:
        case LIST:
        case DICT:
            dyn_set_bool(dyn1, search(dyn2, tmp));
            return DYN_TRUE;
    }

    dyn_free(dyn1);
    return DYN_FALSE;
}


char dyn_op_b_not(dyn_c *dyn)
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

char dyn_op_b_and(dyn_c *dyn1, dyn_c *dyn2)
{
    CHECK_REFERENCE(dyn1, dyn2)

    if (DYN_TYPE(dyn1)==INTEGER && DYN_TYPE(dyn2)==INTEGER) {
        dyn1->data.i = dyn1->data.i & dyn2->data.i;
        return DYN_TRUE;
    }

    dyn_free(dyn1);
    return DYN_FALSE;
}

char dyn_op_b_or(dyn_c *dyn1, dyn_c *dyn2)
{
    CHECK_REFERENCE(dyn1, dyn2)

    if (DYN_TYPE(dyn1)==INTEGER && DYN_TYPE(dyn2)==INTEGER) {
        dyn1->data.i = dyn1->data.i | dyn2->data.i;
        return DYN_TRUE;
    }

    dyn_free(dyn1);
    return DYN_FALSE;
}


char dyn_op_b_xor(dyn_c *dyn1, dyn_c *dyn2)
{
    CHECK_REFERENCE(dyn1, dyn2)

    if (DYN_TYPE(dyn1)==INTEGER && DYN_TYPE(dyn2)==INTEGER) {
        dyn1->data.i = dyn1->data.i ^ dyn2->data.i;
        return DYN_TRUE;
    }

    dyn_free(dyn1);
    return DYN_FALSE;
}


char dyn_op_b_shift_l(dyn_c *dyn1, dyn_c *dyn2)
{
    CHECK_REFERENCE(dyn1, dyn2)

    if (DYN_TYPE(dyn1)==INTEGER && DYN_TYPE(dyn2)==INTEGER) {
        dyn1->data.i = dyn1->data.i << dyn2->data.i;
        return DYN_TRUE;
    }

    dyn_free(dyn1);
    return DYN_FALSE;
}

char dyn_op_b_shift_r(dyn_c *dyn1, dyn_c *dyn2)
{
    CHECK_REFERENCE(dyn1, dyn2)

    if (DYN_TYPE(dyn1)==INTEGER && DYN_TYPE(dyn2)==INTEGER) {
        dyn1->data.i = dyn1->data.i >> dyn2->data.i;
        return DYN_TRUE;
    }

    dyn_free(dyn1);
    return DYN_FALSE;
}
