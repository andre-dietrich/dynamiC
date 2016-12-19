#include "dynamic.h"

#define LST_CONT(X)   X->data.list->container
#define LST_SPACE(X)  X->data.list->space

ss_char dyn_set_list_len (dyn_c* list, ss_ushort len)
{
    dyn_free(list);

    list->data.list = (dyn_list*) malloc(sizeof(dyn_list));

    if (list->data.list) {

        list->data.list->container = (dyn_c*) malloc(len * sizeof(dyn_c));

        if (list->data.list->container) {
            list->type = LIST;

            list->data.list->space = len;
            list->data.list->length = 0;

            while (len--)
                DYN_INIT(&list->data.list->container[len]);

            return 1;
        }
        free(list->data.list);
    }
    return 0;
}


ss_char dyn_list_free (dyn_c* list)
{
    ss_ushort length = DYN_LIST_LEN(list);

    dyn_c *ptr = list->data.list->container;
    while (length--) {
        dyn_free(ptr++);
    }

    free(list->data.list->container);
    free(list->data.list);

    return 1;
}


ss_char dyn_list_resize (dyn_c* list, ss_ushort size)
{
    ss_ushort i;
//    ss_ushort len = dyn_length(list);

    dyn_c* new_list = (dyn_c*) realloc(list->data.list->container, size * sizeof(dyn_c));

    if (new_list) {
        list->data.list->container = new_list;

        if (list->data.list->space < size) {
            for(i=DYN_LIST_LEN(list); i<size; ++i)
                DYN_INIT(&list->data.list->container[i]);
        }

        list->data.list->space = size;
        return 1;
    }

    return 0;
}


dyn_c* dyn_list_push (dyn_c* list, dyn_c* element)
{
    if (DYN_LIST_LEN(list) == LST_SPACE(list))
        if (!dyn_list_resize(list, list->data.list->space + LIST_DEFAULT))
            return NULL;

    dyn_copy(element, &list->data.list->container[ list->data.list->length++ ]);

    return &list->data.list->container[ list->data.list->length-1 ];
}

dyn_c* dyn_list_push_none (dyn_c* list)
{
    if (DYN_LIST_LEN(list) == LST_SPACE(list))
        if (!dyn_list_resize(list, list->data.list->space + LIST_DEFAULT))
            return NULL;

    return &list->data.list->container[ list->data.list->length++ ];
}

ss_char dyn_list_remove (dyn_c* list, ss_ushort i)
{
    if (DYN_LIST_LEN(list) > i) {
        for(;i<DYN_LIST_LEN(list)-1; ++i) {
            dyn_move(DYN_LIST_GET_REF(list, i+1),
                     DYN_LIST_GET_REF(list, i));
        }
        dyn_list_popi(list, 1);
    }
    return 1;
}


ss_char dyn_list_insert (dyn_c* list, dyn_c* element, ss_ushort i)
{
    ss_ushort n = DYN_LIST_LEN(list);
    if (n >= i) {
        dyn_list_push_none(list);
        ++n;
        while(--n > i) {
            dyn_move(DYN_LIST_GET_REF(list, n-1), DYN_LIST_GET_REF(list, n));
        }
        dyn_move(element, DYN_LIST_GET_REF(list, i));
    }
    return 1;
}


ss_char dyn_list_pop(dyn_c* list, dyn_c* element)
{
    dyn_move(DYN_LIST_GET_END(list), element);
    list->data.list->length--;

    if (LST_SPACE(list) - DYN_LIST_LEN(list) > LIST_DEFAULT)
        if (!dyn_list_resize(list, list->data.list->space - LIST_DEFAULT))
            return 0;

    return 1;
}


ss_char dyn_list_popi (dyn_c* list, ss_short i)
{
    while(i--)
        dyn_free(&list->data.list->container[ --list->data.list->length ]);

    return 1;
}


ss_char dyn_list_get (dyn_c* list, dyn_c* element, ss_short i)
{
    dyn_free(element);
    ss_short len = DYN_LIST_LEN(list);

    if (i >= 0) {
        if (i <= len) {
            return dyn_copy(&list->data.list->container[ i ], element);
        }
    } else if ( -i <= len ) {
        return dyn_copy(&list->data.list->container[ len+i ], element);
    }

    return 0;
}


dyn_c* dyn_list_get_ref (dyn_c* list, ss_short i)
{
    if (i >= 0 && i<=DYN_LIST_LEN(list))
        return &list->data.list->container[i];
    else if (i < 0 && -i <= DYN_LIST_LEN(list))
        return &list->data.list->container[ DYN_LIST_LEN(list)+i ];
    return NULL;
}


ss_char dyn_list_copy (dyn_c* dyn, dyn_c* copy)
{
    ss_ushort len = DYN_LIST_LEN(dyn);

    if (dyn_set_list_len(copy, len)) {
        dyn = dyn->data.list->container;
        while (len--) {
            if (!dyn_list_push(copy, dyn++)) {
                dyn_free(copy);
                return 0;
            }
        }
        return 1;
    }
    return 0;
}


ss_ushort dyn_list_string_len (dyn_c* dyn)
{
    ss_ushort size = DYN_LIST_LEN(dyn)+1;
    ss_ushort len = 2 + size;

    while (--size)
        len += dyn_string_len(DYN_LIST_GET_REF(dyn, size-1));

    return len;
}


void dyn_list_string_add (dyn_c* dyn, ss_str str)
{
    ss_strcat(str, (ss_str)"[");
    ss_ushort len = DYN_LIST_LEN(dyn);

    if (len == 0) {
        ss_strcat(str, (ss_str)"]");
        return;
    }

    ss_ushort i;
    for (i=0; i<len; i++) {
        dyn_string_add(DYN_LIST_GET_REF(dyn, i), str);
        ss_strcat(str, (ss_str)",");
    }
    str[ss_strlen(str)-1] = ']';
}
