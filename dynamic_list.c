/**
 *  @file dynamic_list.c
 *  @author André Dietrich
 *  @date 14 December 2016
 *
 *  @copyright Copyright 2016 André Dietrich. All rights reserved.
 *
 *  @license This project is released under the MIT-License.
 *
 *  @brief Implementation of dynamiC list module.
 *
 *
 */

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
trilean dyn_set_list_len (dyn_c* dyn, dyn_ushort len)
{
    dyn_free(dyn);

    dyn_list *list = (dyn_list*) malloc(sizeof(dyn_list));

    if (list) {

        list->container = (dyn_c*) malloc(len * sizeof(dyn_c));

        if (list->container) {
            list->space = len;
            list->length = 0;

            // Initialize all elements in list with NONE
            while (len--)
                DYN_INIT(&list->container[len]);

            dyn->type = LIST;
            dyn->data.list = list;
            return DYN_TRUE;
        }
        free(list);
    }
    return DYN_FALSE;
}

/**
 * @param[in, out] list  input put has to be a list
 */
void dyn_list_free (dyn_c* dyn)
{
    dyn_ushort len = DYN_LIST_LEN(dyn);

    // free all elements within the allocated container element
    dyn_c *ptr = dyn->data.list->container;
    while (len--) {
        dyn_free(ptr++);
    }

    free(dyn->data.list->container);
    free(dyn->data.list);
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
trilean dyn_list_resize (dyn_c* list, dyn_ushort size)
{
    dyn_list *ptr = list->data.list;

    dyn_c* new_list = (dyn_c*) realloc(ptr->container, size * sizeof(dyn_c));

    if (new_list) {
        ptr->container = new_list;

        if (ptr->space < size) {
            dyn_ushort i = ptr->length;
            for(; i<size; ++i)
                DYN_INIT(&ptr->container[i]);
        }

        ptr->space = size;
        return DYN_TRUE;
    }

    return DYN_FALSE;
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
dyn_c* dyn_list_push (dyn_c* list, const dyn_c* element)
{
    dyn_list *ptr = list->data.list;

    if (ptr->length == ptr->space)
        if (!dyn_list_resize(list, ptr->space + LIST_DEFAULT))
            return NULL;

    dyn_copy(element, &ptr->container[ ptr->length++ ]);

    return &ptr->container[ ptr->length-1 ];
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
    dyn_list *ptr = list->data.list;
    if (ptr->length == ptr->space)
        if (!dyn_list_resize(list, ptr->space + LIST_DEFAULT))
            return NULL;

    return &ptr->container[ ptr->length++ ];
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
trilean dyn_list_remove (dyn_c* list, dyn_ushort i)
{
    dyn_list *ptr = list->data.list;
    if (ptr->length > i) {
        for(; i<ptr->length-1; ++i) {
            dyn_move(&ptr->container[i+1], &ptr->container[i]);
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
trilean dyn_list_insert (dyn_c* list, dyn_c* element, const dyn_ushort i)
{
    dyn_ushort n = DYN_LIST_LEN(list);
    if (n >= i) {
        dyn_list_push_none(list);

        dyn_c *ptr = list->data.list->container;

        ++n;
        while(--n > i)
            dyn_move(&ptr[n-1], &ptr[n]);

        dyn_move(element, &ptr[i]);
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
trilean dyn_list_pop(dyn_c* list, dyn_c* element)
{
    dyn_list *ptr = list->data.list;

    dyn_move(&ptr->container[--ptr->length], element);

    if (ptr->space - ptr->length > LIST_DEFAULT)
        if (!dyn_list_resize(list, ptr->space - LIST_DEFAULT))
            return DYN_FALSE;

    return DYN_TRUE;
}

/**
 * @param[in, out] list input has to be of type LIST
 * @param[in] i number of elements to be popped
 */
trilean dyn_list_popi (dyn_c* list, dyn_short i)
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
trilean dyn_list_get (const dyn_c* list, dyn_c* element, const dyn_short i)
{
    dyn_free(element);

    dyn_c* ptr = dyn_list_get_ref(list, i);

    if (ptr) {
        return dyn_copy(ptr, element);
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
dyn_c* dyn_list_get_ref (const dyn_c* list, const dyn_short i)
{
    dyn_list *ptr = list->data.list;
    if (i >= 0 && i<= ptr->length)
        return &ptr->container[i];
    else if (i < 0 && -i <= ptr->length)
        return &ptr->container[ ptr->length+i ];
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
trilean dyn_list_copy (const dyn_c* list, dyn_c* copy)
{
    dyn_ushort len = DYN_LIST_LEN(list);

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
dyn_ushort dyn_list_string_len (const dyn_c* list)
{
    dyn_ushort size = DYN_LIST_LEN(list)+1;
    dyn_ushort len = 2 + size;

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
void dyn_list_string_add (const dyn_c* list, dyn_str str)
{
    dyn_strcat(str, "[");
    dyn_ushort len = DYN_LIST_LEN(list);

    if (len == 0) {
        dyn_strcat(str, "]");
        return;
    }

    dyn_ushort i;
    for (i=0; i<len; i++) {
        dyn_string_add(DYN_LIST_GET_REF(list, i), str);
        dyn_strcat(str, ",");
    }
    str[dyn_strlen(str)-1] = ']';
}
