#include "dynamic.h"

#define LST_CONT(X)   X->data.list->container
#define LST_SPACE(X)  X->data.list->space


/**
 * Takes in any kind of dynamic paramter, frees all allocated memory and
 * allocates a new array of dynamic elemens with a length defined in paramter
 * len. The len paramter us used to denote the max space available space,
 * initially the lenght of a list is marked as empty. Every application of
 * of dyn_list_push increases the internal counter of dyn->data.list->len,
 * until the max value dyn->data.list->space is reached, if so, new memory
 * is allocated automatically. Every popped value by applying dyn_list_pop
 * decreases the internal counter.
 *
 * @param[in, out] dyn  input any, output LIST
 * @param[in]      len
 *
 * @retval DYN_TRUE   if the required memory could be allocated
 * @retval DYN_FALSE  otherwise
 */
ss_char dyn_set_list_len (dyn_c* dyn, ss_ushort len)
{
    dyn_free(dyn);

    dyn->data.list = (dyn_list*) malloc(sizeof(dyn_list));

    if (dyn->data.list) {

        dyn->data.list->container = (dyn_c*) malloc(len * sizeof(dyn_c));

        if (dyn->data.list->container) {
            dyn->type = LIST;

            dyn->data.list->space = len;
            dyn->data.list->length = 0;

            // Initialize all elements in list with NONE
            while (len--)
                DYN_INIT(&dyn->data.list->container[len]);

            return DYN_TRUE;
        }
        free(dyn->data.list);
    }
    return DYN_FALSE;
}

/**
 * @param[in, out] list  input put has to be a list
 *
 * @retval DYN_TRUE   if the required memory could be allocated
 * @retval DYN_FALSE  otherwise
 */
ss_char dyn_list_free (dyn_c* list)
{
    ss_ushort len = DYN_LIST_LEN(list);

    // free all elements within the allocated container element
    dyn_c *ptr = list->data.list->container;
    while (len--) {
        dyn_free(ptr++);
    }

    free(list->data.list->container);
    free(list->data.list);

    return DYN_TRUE;
}

/**
 * Resize the maximal usable space, if the size decreases then the removed
 * elements have to be removed previously.
 *
 * @param[in, out] list  input put has to be a list
 * @param[in] size new maximal available space
 *
 * @retval DYN_TRUE   if the required memory could be allocated
 * @retval DYN_FALSE  otherwise
 */
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

/**
 * Pushes (copies) an additional element to the end of a list and increases the
 * length value by one. If the maximal available space is reached, then new
 * memory is allocated and the maximal space value gets increased, this
 * additional size value is defined in LIST_DEFAULT.
 *
 * @param[in, out] list input has to be of type LIST
 * @param[in]      element to be pushed
 *
 * @returns a reference to the element at the end of the list
 */
dyn_c* dyn_list_push (dyn_c* list, dyn_c* element)
{
    if (DYN_LIST_LEN(list) == LST_SPACE(list))
        if (!dyn_list_resize(list, list->data.list->space + LIST_DEFAULT))
            return NULL;

    dyn_copy(element, &list->data.list->container[ list->data.list->length++ ]);

    return &list->data.list->container[ list->data.list->length-1 ];
}

/**
 * Increses the length value of the list by one and returns a reference to the
 * last NONE element of the list. This reference can be used to move a larger
 * element to the end of a list, without copying.
 *
 * @param[in, out] list input has to be of type LIST
 *
 * @returns reference to the pushed NONE value
 */
dyn_c* dyn_list_push_none (dyn_c* list)
{
    if (DYN_LIST_LEN(list) == LST_SPACE(list))
        if (!dyn_list_resize(list, list->data.list->space + LIST_DEFAULT))
            return NULL;

    return &list->data.list->container[ list->data.list->length++ ];
}

/**
 * Delete an element from the list at position i, successive elements are moved
 * to close this gap. This function can be interpeted as the opposite to
 * \p dyp_list_insert
 *
 * @code
 * // pseudo code
 * dyn_list_remove([0,1,2,3,4], 2) == [0,1,3,4]
 * @endcode
 *
 * @param[in, out] list  input has to be of type LIST
 * @param[in] i position to remove
 *
 * @retval DYN_TRUE   if the required memory could be allocated
 * @retval DYN_FALSE  otherwise
 */
ss_char dyn_list_remove (dyn_c* list, ss_ushort i)
{
    if (DYN_LIST_LEN(list) > i) {
        for(;i<DYN_LIST_LEN(list)-1; ++i) {
            dyn_move(DYN_LIST_GET_REF(list, i+1),
                     DYN_LIST_GET_REF(list, i));
        }
        dyn_list_popi(list, 1);
    }
    return DYN_TRUE;
}

/**
 * Insert a new element at position i to a list, all other elements with pos>i
 * are shifted "to the left" befor the element gets inserted (moved).
 *
 * @code
 * // pseudo code
 * dyn_list_insert([0,1,3,4], "22", 2) == [0,1,"22",3,4]
 * @endcode
 *
 * @param[in, out] list input has to be of type LIST
 * @param[in] element to be inserted
 * @param[in] i position
 *
 * @retval DYN_TRUE   if the required memory could be allocated
 */
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
    return DYN_TRUE;
}

/**
 * Pops the last element from the list and moves its content to parameter
 * element. The length of the list decreases by one.
 *
 * @param[in, out] list input has to be of type LIST
 * @param[in, out] element where last dynamic value is moved to
 *
 * @retval DYN_TRUE   if worked properly
 */
ss_char dyn_list_pop(dyn_c* list, dyn_c* element)
{
    dyn_move(DYN_LIST_GET_END(list), element);
    list->data.list->length--;

    if (LST_SPACE(list) - DYN_LIST_LEN(list) > LIST_DEFAULT)
        if (!dyn_list_resize(list, list->data.list->space - LIST_DEFAULT))
            return DYN_FALSE;

    return DYN_TRUE;
}

/**
 * @param[in, out] list input has to be of type LIST
 * @param[in] i number of elements to be popped
 */
ss_char dyn_list_popi (dyn_c* list, ss_short i)
{
    while(i--)
        dyn_free(&list->data.list->container[ --list->data.list->length ]);

    return DYN_TRUE;
}

/**
 * Copies the i value in the list to \p element. If the i value is negative,
 * then the i value is relative to the last value (-1). If only the reference
 * to the value in the list is required, use \p dyn_list_get_ref
 *
 * @see dyn_list_get_ref
 *
 * @param[in] list input has to be of type LIST
 * @param[in, out] element to copy to
 * @param[in] i position in list
 *
 * @retval DYN_TRUE  if the element was found and coppied
 * @retval DYN_FALSE otherwise
 */
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

    return DYN_FALSE;
}

/**
 * Returns the reference of the i value in list, if a negative position value
 * is used, then the position is calculated from the end. And if the i position
 * exceeds the length of the list, NULL is returned.
 *
 * @see dyn_list_get
 *
 * @param list input has to be of type LIST
 * @param i position in list
 *
 * @returns reference to the ith value
 */
dyn_c* dyn_list_get_ref (dyn_c* list, ss_short i)
{
    if (i >= 0 && i<=DYN_LIST_LEN(list))
        return &list->data.list->container[i];
    else if (i < 0 && -i <= DYN_LIST_LEN(list))
        return &list->data.list->container[ DYN_LIST_LEN(list)+i ];
    return NULL;
}

/**
 * @see dyn_copy
 *
 * @param[in] list original
 * @param[in, out] copy new list
 *
 * @retval DYN_TRUE  if the element was found and coppied
 * @retval DYN_FALSE otherwise
 */
ss_char dyn_list_copy (dyn_c* list, dyn_c* copy)
{
    ss_ushort len = DYN_LIST_LEN(list);

    if (dyn_set_list_len(copy, len)) {
        list = list->data.list->container;
        while (len--) {
            if (!dyn_list_push(copy, list++)) {
                dyn_free(copy);
                return DYN_FALSE;
            }
        }
        return DYN_TRUE;
    }
    return DYN_FALSE;
}

/**
 * The length of the list string representation is calculated by the length of
 * all included elemens plus 2 brackets and the number of commas.
 *
 * @param list input has to be of type LIST
 *
 * @returns length of string
 */
ss_ushort dyn_list_string_len (dyn_c* list)
{
    ss_ushort size = DYN_LIST_LEN(list)+1;
    ss_ushort len = 2 + size;

    while (--size)
        len += dyn_string_len(DYN_LIST_GET_REF(list, size-1));

    return len;
}

/**
 * Generates a string representation of the list and all included element,
 * segregated by commas, and attaches it to the end of the string.
 *
 * @param[in] list input has to be of type LIST
 * @param[in, out] str with added list representation
 */
void dyn_list_string_add (dyn_c* list, ss_str str)
{
    ss_strcat(str, (ss_str)"[");
    ss_ushort len = DYN_LIST_LEN(list);

    if (len == 0) {
        ss_strcat(str, (ss_str)"]");
        return;
    }

    ss_ushort i;
    for (i=0; i<len; i++) {
        dyn_string_add(DYN_LIST_GET_REF(list, i), str);
        ss_strcat(str, (ss_str)",");
    }
    str[ss_strlen(str)-1] = ']';
}
