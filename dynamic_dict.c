#include "dynamic.h"
#include <stdio.h>

ss_char dyn_set_dict (dyn_c* dyn, ss_ushort length)
{
    dyn_free(dyn);
    dyn->type = DICT;

    dyn->data.dict = (dyn_dict*) malloc(sizeof(dyn_dict));

    if (dyn->data.dict) {
        DYN_INIT(&dyn->data.dict->value);
        if (dyn_set_list_len(&dyn->data.dict->value, length)) {
            dyn->data.dict->key = (ss_str*) malloc(length * sizeof(ss_str*));
            if (dyn->data.dict->key) {
                ss_ushort i;
                for (i=0; i<length; ++i)
                    dyn->data.dict->key[i] = NULL;

                //dyn->data.dict->meta = 0;

                return 1;
            }
            dyn_free(&dyn->data.dict->value);
        }
        free(dyn->data.dict);
    }

    return 0;
}

dyn_c* dyn_dict_insert(dyn_c* dyn, ss_str key, dyn_c* value)
{
    dyn_dict* dict = dyn->data.dict;
    ss_ushort space = DYN_DICT_SPACE(dict);
    ss_ushort i = dyn_dict_has_key(dyn, key);
    if (i--)
        goto GOTO__CHANGE; //return dyn_dict_change(dyn, i-1, value);

    if (DYN_DICT_LENGTH(dict) == space) {
        dyn_dict_resize(dyn, space + DICT_DEFAULT);
        /*if (dyn_list_resize(&dyn->data.dict->value, space)) {
            dyn->data.dict->key = (ss_str*) realloc(dyn->data.dict->key, space * sizeof(ss_str*));
            if (dyn->data.dict->key) {
                for (i=space - DICT_DEFAULT; i<space; ++i)
                    dyn->data.dict->key[i] = NULL;
            }
        }*/
    }

    i = DYN_DICT_LENGTH(dict);
    dict->key[i] = (ss_str) malloc(ss_strlen(key)+1);
    if (dict->key[i]) {
        ss_strcpy(dict->key[i], key);
        DYN_DICT_LENGTH(dict)++;

GOTO__CHANGE:
        dyn_dict_change(dyn, i, value);
        return dyn_dict_get_i_ref(dyn, i);
    }

    return NULL;
}

ss_char dyn_dict_resize(dyn_c* dyn, ss_ushort size)
{
    dyn_dict* dict = dyn->data.dict;

    ss_ushort space = DYN_DICT_SPACE(dict);

    if (size > space)
        if (dyn_list_resize(&dict->value, size)) {
            dict->key = (ss_str*) realloc(dict->key, size * sizeof(ss_str*));
            if (dict->key) {
                for (; space<size; ++space)
                    dict->key[space] = NULL;
                return 1;
            }
        }

    //fprintf(stderr, "XXX %s\n", dyn_get_string(dict));
    return 0;
}


ss_char dyn_dict_change (dyn_c* dyn, ss_ushort i, dyn_c* value)
{
    return dyn_copy(value, DYN_LIST_GET_REF(&dyn->data.dict->value, i));
}

ss_ushort dyn_dict_has_key (dyn_c* dyn, ss_str key)
{
    ss_char** s_key = dyn->data.dict->key;
    ss_ushort length = DYN_DICT_LENGTH(dyn->data.dict);
    ss_ushort i;
    for (i=0; i<length; ++i, ++s_key) {
        if (!ss_strcmp(*s_key, key))
            return i+1;
    }

    return 0;
}


dyn_c* dyn_dict_get_i_ref (dyn_c* dyn, ss_ushort i)
{
    return dyn_list_get_ref(&dyn->data.dict->value, i);
}


ss_str dyn_dict_get_i_key (dyn_c* dyn, ss_ushort i)
{
    return dyn->data.dict->key[i];
}


ss_char dyn_dict_remove (dyn_c* dyn, ss_str key)
{
    dyn_dict* dict = dyn->data.dict;
    ss_ushort i = dyn_dict_has_key(dyn, key);

    if(i) {
        free(dict->key[--i]);
        dict->key[i] = NULL;
        dyn_free(DYN_DICT_GET_I_REF(dyn, i));
        dict->value.data.list->length--;

        // if not last element
        if (i != dict->value.data.list->length && dict->value.data.list->length) {
            dict->key[i] = dict->key[dict->value.data.list->length];
            dict->key[dict->value.data.list->length] = NULL;
            dyn_move(DYN_DICT_GET_I_REF(dyn, dict->value.data.list->length),
                     DYN_DICT_GET_I_REF(dyn, i));
        }

        return 1;
    }

    return 0;
}


ss_char dyn_dict_empty (dyn_c* dyn)
{
    dyn_dict* dict = dyn->data.dict;

    ss_ushort i = DYN_DICT_LENGTH(dict);
    while (i--) {
        free(dict->key[i]);
        dict->key[i] = NULL;
        dyn_free(DYN_DICT_GET_I_REF(dyn, i));
    }
    dict->value.data.list->length = 0;
    return 1;
}


ss_char dyn_dict_free (dyn_c* dyn)
{
    dyn_dict_empty(dyn);
    dyn_free(&dyn->data.dict->value);
    free(dyn->data.dict->key);
    free(dyn->data.dict);

    return 1;
}


dyn_c* dyn_dict_get (dyn_c* dyn, ss_str key)
{
    ss_ushort pos = dyn_dict_has_key(dyn, key);

    if (pos)
        return DYN_DICT_GET_I_REF(dyn, --pos);

    return NULL;
}


ss_char dyn_dict_copy (dyn_c* dyn, dyn_c* copy)
{
    dyn_dict* dict = dyn->data.dict;
    ss_ushort length = DYN_DICT_LENGTH(dict);

    if (dyn_set_dict(copy, length)) {
        ss_ushort i;
        for (i=0; i<length; ++i) {
            if(!dyn_dict_insert(copy, dict->key[i],
                                 DYN_DICT_GET_I_REF(dyn, i))) {
                dyn_free(copy);
                return 0;
            }
        }

        //copy->data.dict->meta = dyn->data.dict->meta;

        dyn_dict_set_loc(copy);
    }
    return 1;
}


ss_ushort dyn_dict_string_len (dyn_c* dyn)
{
    dyn_dict* dict = dyn->data.dict;
    ss_ushort len = DYN_DICT_LENGTH(dict);
    if (len) {
        ss_ushort i = len;
        while (i--) {
            len += ss_strlen(dict->key[i]);
            len += dyn_string_len(DYN_DICT_GET_I_REF(dyn, i));
            len += 2; // comma and colon
        }
    }
    len += 2;

    return len;
}


void dyn_dict_string_add (dyn_c* dyn, ss_str string)
{
    ss_strcat(string, (ss_str)"{");

    if ( dyn_length(dyn) ) {
        ss_ushort length = dyn->data.dict->value.data.list->length;
        ss_ushort i;
        for (i=0; i<length; ++i) {
            ss_strcat(string, DYN_DICT_GET_I_KEY(dyn, i));
            ss_strcat(string, (ss_str)":");
            dyn_string_add(DYN_DICT_GET_I_REF(dyn, i), string);
            ss_strcat(string, (ss_str)",");
        }
        string[ss_strlen(string)-1] = '}';
    }
    else
        ss_strcat(string, (ss_str)"}");
}

ss_char fct_set_loc (dyn_c* proc, dyn_c* loc)
{
    if (!proc->data.fct->type) {
        dyn_proc *p = (dyn_proc*) proc->data.fct->ptr;
        if (DYN_NOT_NONE(&p->params)) {
            ss_byte i = dyn_dict_has_key(&p->params, (ss_str)"");
            if (i--) {
                dyn_set_ref( DYN_DICT_GET_I_REF(&p->params, i), loc );
                DYN_TYPE(DYN_DICT_GET_I_REF(&p->params, i)) = REFERENCE2;
                return 1;
            }
        }
    }

    return 0;
}


ss_char dyn_dict_set_loc(dyn_c* dyn)
{
    dyn_dict* dict = dyn->data.dict;
    ss_ushort length = DYN_DICT_LENGTH(dict);

    dyn_c * ptr;

    ss_ushort i;
    for (i=0; i<length; ++i) {
        ptr = DYN_DICT_GET_I_REF(dyn, i);
        if (DYN_TYPE(ptr) == FUNCTION) {
            fct_set_loc(ptr, dyn);
            /*if (!ptr->data.fct->type) {
                ptr = (dyn_proc*) ptr->data.fct->ptr;
                if (DYN_NOT_NONE(&p->params)) {
                    ss_byte pos = dyn_dict_has_key(&p->params, "");
                    if (pos--) {
                        dyn_set_ref( DYN_DICT_GET_I_REF(&p->params, pos), ptr );
                        DYN_TYPE(DYN_DICT_GET_I_REF(&p->params, pos)) = REFERENCE2;
                    }
                }
            }*/
        }
        else if (DYN_TYPE(ptr) == DICT)
            dyn_dict_set_loc(ptr);
    }
    return 1;
}
