#include "dynamic.h"
#include <stdio.h>

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

ss_char dyn_type (dyn_c* dyn)
{
    return DYN_TYPE(dyn);
}

void dyn_set_none (dyn_c* dyn)
{
    dyn_free(dyn);
    dyn->data.i = 0;
}

void dyn_set_bool (dyn_c* dyn, ss_char v)
{
    dyn_free(dyn);
    dyn->type = BOOL;
    dyn->data.b = v;
}

void dyn_set_int (dyn_c* dyn, ss_int v)
{
    dyn_free(dyn);
    dyn->type = INTEGER;
    dyn->data.i = v;
}

void dyn_set_float (dyn_c* dyn, ss_float v)
{
    dyn_free(dyn);
    dyn->type = FLOAT;
    dyn->data.f = v;
}

void dyn_set_extern (dyn_c* dyn, void* v)
{
    dyn_free(dyn);
    dyn->type = EXTERN;
    dyn->data.ex = v;
}

ss_char dyn_set_string (dyn_c* dyn, char const * v)
{
    dyn_free(dyn);

    dyn->data.str = (ss_str) malloc(ss_strlen((ss_str)v)+1);

    if (dyn->data.str) {
        dyn->type = STRING;
        ss_strcpy(dyn->data.str, (ss_str)v);
        return 1;
    }
    return 0;
}

void dyn_set_ref (dyn_c* ref, dyn_c* orig)
{
    dyn_free(ref);

    ref->type = REFERENCE;
    ref->data.ref = DYN_IS_REFERENCE(orig) ? orig->data.ref : orig;
}

ss_uint dyn_size (dyn_c* dyn)
{
    ss_uint bytes = sizeof(dyn_c);

    switch (DYN_TYPE(dyn)) {
        case STRING: {
            bytes += ss_strlen(dyn->data.str)+1;
            break;
        }
#ifdef S2_SET
        case SET:
#endif
        case LIST: {
            bytes += sizeof(dyn_list);

            ss_ushort len = dyn->data.list->space;
            ss_ushort i;
            for (i=0; i<len; ++i)
                bytes += dyn_size( DYN_LIST_GET_REF(dyn, i) );

            break;
        }
        case DICT: {
            bytes += sizeof(dyn_dict);
            bytes += dyn_size(&dyn->data.dict->value);

            ss_ushort len = dyn->data.dict->value.data.list->space;
            ss_ushort i;
            for (i=0; i<len; ++i) {
                if (dyn->data.dict->key[i])
                    bytes += ss_strlen(dyn->data.dict->key[i]);
                bytes += 1;
            }

            break;
        }
//        case PROCEDURE: {
//            bytes += sizeof(dyn_proc);
//            bytes += dyn->data.proc->length;
//            bytes += dyn_size(&dyn->data.proc->params);
//            break;
//        }

        case FUNCTION: {
            bytes += sizeof(dyn_fct);
            bytes += ss_strlen(dyn->data.fct->info)+1;
            break;
        }
    }

    return bytes;
}

ss_char dyn_get_bool (dyn_c* dyn)
{
START:
    switch (DYN_TYPE(dyn)) {
        case BOOL:      return dyn->data.b ? 1 : 0;
        case INTEGER:   return dyn->data.i ? 1 : 0;
        case FLOAT:     return dyn->data.f ? 1 : 0;
        case STRING:    return ss_strlen(dyn->data.str) ? 1 : 0;
#ifdef S2_SET
        case SET:
#endif
        case LIST:      return DYN_LIST_LEN(dyn) ? 1 : 0;
        case DICT:      return DYN_DICT_LEN(dyn) ? 1 : 0;
        case REFERENCE2:
        case REFERENCE: dyn=dyn->data.ref;
                        goto START;
    }

    return 0;
}

ss_int dyn_get_int (dyn_c* dyn)
{
START:
    switch (DYN_TYPE(dyn)) {
        case BOOL:      return dyn->data.b ? 1 : 0;
        case INTEGER:   return dyn->data.i;
        case FLOAT:     return (ss_int)dyn->data.f;
        case REFERENCE2:
        case REFERENCE: dyn=dyn->data.ref;
                        goto START;
    }
    return 0;
}


ss_float dyn_get_float (dyn_c* dyn)
{
START:
    switch (DYN_TYPE(dyn)) {
        case BOOL:      return dyn->data.b ? 1 : 0;
        case INTEGER:   return (ss_float)dyn->data.i;
        case FLOAT:     return dyn->data.f;
        case REFERENCE2:
        case REFERENCE: dyn=dyn->data.ref;
                        goto START;
    }
    return 0;
}

void* dyn_get_extern (dyn_c* dyn)
{
    if (DYN_IS_REFERENCE(dyn))
        dyn = dyn->data.ref;

    if(DYN_TYPE(dyn) == EXTERN)
        return dyn->data.ex;

    return NULL;
}

ss_str dyn_get_string (dyn_c* dyn)
{
    if (DYN_IS_REFERENCE(dyn))
        dyn = dyn->data.ref;

    ss_str string = (ss_str) malloc(dyn_string_len(dyn) + 1);
    if (string) {
        string[0] = '\0';
        if (DYN_TYPE(dyn) == STRING)
            ss_strcpy(string, dyn->data.str);
        else
            dyn_string_add(dyn, string);
    }

    return string;
}

void dyn_string_add (dyn_c* dyn, ss_str string)
{
START:
    switch (DYN_TYPE(dyn)) {
        case BOOL:      ss_strcat(string, dyn->data.b ? (ss_str)"1": (ss_str)"0");
                        break;
        case INTEGER:   ss_itoa(&string[ss_strlen(string)], dyn->data.i);
                        break;
        case FLOAT:     ss_ftoa(&string[ss_strlen(string)], dyn->data.f);
                        break;
        case STRING:    ss_strcat(string, dyn->data.str);
                        break;
        case EXTERN:    ss_strcat(string, (ss_str)"ex");
                        //ss_itoa(&string[ss_strlen(string)], *((ss_int*) dyn->data.ex));
                        break;
        case FUNCTION:  ss_strcat(string, (ss_str)"FCT");
                        break;
        case LIST:      dyn_list_string_add(dyn, string);
                        break;
#ifdef S2_SET
        case SET: {     ss_uint i = ss_strlen(string);
                        dyn_list_string_add(dyn, string);
                        string[i] = '{';
                        string[ss_strlen(string)-1] = '}';
                        break;
        }
#endif
        case DICT:      dyn_dict_string_add (dyn, string);
                        break;
        case REFERENCE2:
        case REFERENCE: dyn=dyn->data.ref;
                        goto START;
        case MISCELLANEOUS: ss_strcat(string, (ss_str)"$");
    }
}

ss_ushort dyn_string_len (dyn_c* dyn)
{
    ss_ushort len = 0;

START:

    switch (DYN_TYPE(dyn)) {
        case MISCELLANEOUS:
        case BOOL:      len = 1;                        break;
        case INTEGER:   len = ss_itoa_len(dyn->data.i); break;
        case FLOAT:     len = ss_ftoa_len(dyn->data.f); break;
        case EXTERN:    len = 2; // + ss_itoa_len(*((ss_int*) dyn->data.ex));
                        break;
        case FUNCTION:  len = 3;                        break;
        case STRING:    len = ss_strlen(dyn->data.str); break;
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

ss_char dyn_copy (dyn_c* dyn, dyn_c* copy)
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

    return 1;
}


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


ss_ushort dyn_length (dyn_c* dyn)
{
START:
    switch (DYN_TYPE(dyn)) {
        case STRING:    return ss_strlen(dyn->data.str);
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
