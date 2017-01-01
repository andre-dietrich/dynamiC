#include "dynamic.h"
#include <stdio.h>

/**
 *
 * @param[in, out] dyn element which is initialized as a dictionary
 * @param[in] length of preallocated memory
 *
 * @retval DYN_TRUE   if memory for the DICT could be allocated
 * @retval DYN_FALSE  otherwise
 */
trilean dyn_set_dict (dyn_c* dyn, dyn_ushort length)
{
    dyn_free(dyn);
    dyn->type = DICT;

    dyn->data.dict = (dyn_dict*) malloc(sizeof(dyn_dict));

    if (dyn->data.dict) {
        DYN_INIT(&dyn->data.dict->value);
        if (dyn_set_list_len(&dyn->data.dict->value, length)) {
            dyn->data.dict->key = (dyn_str*) malloc(length * sizeof(dyn_str*));
            if (dyn->data.dict->key) {
                dyn_ushort i;
                for (i=0; i<length; ++i)
                    dyn->data.dict->key[i] = NULL;

                //dyn->data.dict->meta = 0;

                return DYN_TRUE;
            }
            dyn_free(&dyn->data.dict->value);
        }
        free(dyn->data.dict);
    }

    return DYN_FALSE;
}

/**
 * If the key is already contained within the dictionary, then the current
 * value is overwritten, if not then the new value is added to the end of the
 * list as well as a new key. If the maximal space is exceeded, then new memory
 * is attached as defined in DICT_DEFAULT.
 *
 * @param[in, out] dyn has to be of type DICT
 * @param[in] key
 * @param[in] value to be coppied
 *
 * @returns reference to the newly inserted value, if insertion was not
 * possible, NULL is returned
 */
dyn_c* dyn_dict_insert(dyn_c* dyn, dyn_str key, dyn_c* value)
{
    dyn_dict* dict = dyn->data.dict;
    dyn_ushort space = DYN_DICT_SPACE(dict);
    dyn_ushort i = dyn_dict_has_key(dyn, key);
    if (i--)
        goto GOTO__CHANGE; //return dyn_dict_change(dyn, i-1, value);

    if (DYN_DICT_LENGTH(dict) == space) {
        dyn_dict_resize(dyn, space + DICT_DEFAULT);
        /*if (dyn_list_resize(&dyn->data.dict->value, space)) {
            dyn->data.dict->key = (dyn_str*) realloc(dyn->data.dict->key, space * sizeof(dyn_str*));
            if (dyn->data.dict->key) {
                for (i=space - DICT_DEFAULT; i<space; ++i)
                    dyn->data.dict->key[i] = NULL;
            }
        }*/
    }

    i = DYN_DICT_LENGTH(dict);
    dict->key[i] = (dyn_str) malloc(dyn_strlen(key)+1);
    if (dict->key[i]) {
        dyn_strcpy(dict->key[i], key);
        DYN_DICT_LENGTH(dict)++;

GOTO__CHANGE:
        dyn_dict_change(dyn, i, value);
        return dyn_dict_get_i_ref(dyn, i);
    }

    return NULL;
}

/**
 * Increase or decrease the size of the dictionary to a new size, removed
 * elements have to be of type NONE.
 *
 * @param[in, out] dyn has to be of type DICT
 * @param[in] size new
 *
 * @retval DYN_TRUE   if operation could be performed
 * @retval DYN_FALSE  otherwise
 */
trilean dyn_dict_resize(dyn_c* dyn, dyn_ushort size)
{
    dyn_dict* dict = dyn->data.dict;

    dyn_ushort space = DYN_DICT_SPACE(dict);

    if (size > space)
        if (dyn_list_resize(&dict->value, size)) {
            dict->key = (dyn_str*) realloc(dict->key, size * sizeof(dyn_str*));
            if (dict->key) {
                for (; space<size; ++space)
                    dict->key[space] = NULL;
                return DYN_TRUE;
            }
        }

    return DYN_FALSE;
}


trilean dyn_dict_change (dyn_c* dyn, dyn_ushort i, dyn_c* value)
{
    return dyn_copy(value, DYN_LIST_GET_REF(&dyn->data.dict->value, i));
}

/**
 * Searches the dictionary for a certain key, if this key could be found, then
 * its position plus 1 is returned, to indicate that the key was found, even if
 * it is on position 0, otherwise 0 is returned. Thus, the returned value has
 * to be decreased by one if it is larger than 0.
 *
 * @param dyn to be searched has to be of type DICT
 * @param key C-string to search for
 *
 * @retval 0 if the key was not found
 * @retval position+1 otherwise
 */
dyn_ushort dyn_dict_has_key (dyn_c* dyn, dyn_str key)
{
    dyn_char** s_key = dyn->data.dict->key;
    dyn_ushort length = DYN_DICT_LENGTH(dyn->data.dict);
    dyn_ushort i;
    for (i=0; i<length; ++i, ++s_key) {
        if (!dyn_strcmp(*s_key, key))
            return i+1;
    }

    return 0;
}

/**
 * This function is only used to offer an interface, such that values of the
 * library can also be accsessed externally. Internally it is recommended to
 * use the macro DYN_DICT_GET_I_REF
 *
 * @param dyn has to be of type DICT
 * @param i position of the dynamic value
 *
 * @returns reference to the ith value in dyn
 */
dyn_c* dyn_dict_get_i_ref (dyn_c* dyn, dyn_ushort i)
{
    return dyn_list_get_ref(&dyn->data.dict->value, i);
}

/**
 * This function is only used to offer an interface, such that values of the
 * library can also be accsessed externally. Internally it is recommended to
 * use the macro DYN_DICT_GET_I_KEY
 *
 * @param dyn has to be of type DICT
 * @param i position of the key
 *
 * @returns reference to the ith key (C-string)
 */
dyn_str dyn_dict_get_i_key (dyn_c* dyn, dyn_ushort i)
{
    return dyn->data.dict->key[i];
}

/**
 * Searches the dictionary for a certain key and removes the key value pair.
 * The last element is moved to the freed position to spare space, such that
 * there are no free positions within the linear dictionary representation.
 *
 * @param dyn has to be of type DICT
 * @param key of the key-value pair to remove
 *
 * @retval DYN_TRUE   if the key value pair was found and removed
 * @retval DYN_FALSE  otherwise
 */
trilean dyn_dict_remove (dyn_c* dyn, dyn_str key)
{
    dyn_dict* dict = dyn->data.dict;
    dyn_ushort i = dyn_dict_has_key(dyn, key);

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

        return DYN_TRUE;
    }

    return DYN_FALSE;
}

/**
 * @param dyn to be freed has to be of type DICT
 *
 * @retval DYN_TRUE   if the key value pair was found and removed
 */
void dyn_dict_empty (dyn_c* dyn)
{
    dyn_dict* dict = dyn->data.dict;

    dyn_ushort i = DYN_DICT_LENGTH(dict);
    while (i--) {
        free(dict->key[i]);
        dict->key[i] = NULL;
        dyn_free(DYN_DICT_GET_I_REF(dyn, i));
    }
    dict->value.data.list->length = 0;
}

/**
 * @param dyn has to be of type DICT
 */
void dyn_dict_free (dyn_c* dyn)
{
    dyn_dict_empty(dyn);
    dyn_free(&dyn->data.dict->value);
    free(dyn->data.dict->key);
    free(dyn->data.dict);
}

/**
 * Searches the dictionary for a certain key and removes the key value pair.
 * The last element is moved to the freed position to spare space, such that
 * there are no free positions within the linear dictionary representation.
 *
 * @param dyn has to be of type DICT
 * @param key of the key-value pair to remove
 *
 * @returns reference to the value stored under the given key, if exists,
 *          otherwise NULL
 */
dyn_c* dyn_dict_get (dyn_c* dyn, dyn_str key)
{
    dyn_ushort pos = dyn_dict_has_key(dyn, key);

    if (pos)
        return DYN_DICT_GET_I_REF(dyn, --pos);

    return NULL;
}

trilean dyn_dict_copy (dyn_c* dyn, dyn_c* copy)
{
    dyn_dict* dict = dyn->data.dict;
    dyn_ushort length = DYN_DICT_LENGTH(dict);

    if (dyn_set_dict(copy, length)) {
        dyn_ushort i;
        for (i=0; i<length; ++i) {
            if(!dyn_dict_insert(copy, dict->key[i],
                                 DYN_DICT_GET_I_REF(dyn, i))) {
                dyn_free(copy);
                return DYN_FALSE;
            }
        }
        dyn_dict_set_loc(copy);
    }
    return DYN_TRUE;
}


dyn_ushort dyn_dict_string_len (dyn_c* dyn)
{
    dyn_dict* dict = dyn->data.dict;
    dyn_ushort len = DYN_DICT_LENGTH(dict);
    if (len) {
        dyn_ushort i = len;
        while (i--) {
            len += dyn_strlen(dict->key[i]);
            len += dyn_string_len(DYN_DICT_GET_I_REF(dyn, i));
            len += 2; // comma and colon
        }
    }
    len += 2;

    return len;
}


void dyn_dict_string_add (dyn_c* dyn, dyn_str string)
{
    dyn_strcat(string, (dyn_str)"{");

    if ( dyn_length(dyn) ) {
        dyn_ushort length = dyn->data.dict->value.data.list->length;
        dyn_ushort i;
        for (i=0; i<length; ++i) {
            dyn_strcat(string, DYN_DICT_GET_I_KEY(dyn, i));
            dyn_strcat(string, (dyn_str)":");
            dyn_string_add(DYN_DICT_GET_I_REF(dyn, i), string);
            dyn_strcat(string, (dyn_str)",");
        }
        string[dyn_strlen(string)-1] = '}';
    }
    else
        dyn_strcat(string, (dyn_str)"}");
}

dyn_char fct_set_loc (dyn_c* proc, dyn_c* loc)
{
    if (!proc->data.fct->type) {
        dyn_proc *p = (dyn_proc*) proc->data.fct->ptr;
        if (DYN_NOT_NONE(&p->params)) {
            dyn_byte i = dyn_dict_has_key(&p->params, (dyn_str)"");
            if (i--) {
                dyn_set_ref( DYN_DICT_GET_I_REF(&p->params, i), loc );
                DYN_TYPE(DYN_DICT_GET_I_REF(&p->params, i)) = REFERENCE2;
                return DYN_TRUE;
            }
        }
    }

    return DYN_FALSE;
}


trilean dyn_dict_set_loc(dyn_c* dyn)
{
    dyn_dict* dict = dyn->data.dict;
    dyn_ushort length = DYN_DICT_LENGTH(dict);

    dyn_c * ptr;

    dyn_ushort i;
    for (i=0; i<length; ++i) {
        ptr = DYN_DICT_GET_I_REF(dyn, i);
        if (DYN_TYPE(ptr) == FUNCTION) {
            fct_set_loc(ptr, dyn);
            /*if (!ptr->data.fct->type) {
                ptr = (dyn_proc*) ptr->data.fct->ptr;
                if (DYN_NOT_NONE(&p->params)) {
                    dyn_byte pos = dyn_dict_has_key(&p->params, "");
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
    return DYN_TRUE;
}
