/**
 *  @file dynamic_set.c
 *  @author André Dietrich
 *  @date 14 December 2016
 *
 *  @copyright Copyright 2016 André Dietrich. All rights reserved.
 *
 *  @license This project is released under the MIT-License.
 *
 *  @brief Implementation of dynamiC set module as an extension to list.
 *
 *
 */

#include "dynamic.h"

#ifdef S2_SET

/**
 * Generate a set as a list with a maximal available number of preallocated
 * elemens, and change the type of the list to SET. The returned set is empty.
 *
 * @param[in, out] dyn element to be imp input has to be of type LIST
 * @param[in] len of preallocated elements
 *
 * @retval DYN_TRUE   if memory for the SET could be allocated
 * @retval DYN_FALSE  otherwise
 */
trilean dyn_set_set_len (dyn_c* dyn, const dyn_ushort len)
{
    if (dyn_set_list_len(dyn, len)) {
        dyn->type = SET;
        return DYN_TRUE;
    }
    return DYN_FALSE;
}

/**
 * Before a new element is added, it is checked, whether it is already included
 * or not.
 *
 * @param[in, out] set has to be of type SET
 * @param[in] element to be added
 *
 * @retval DYN_TRUE   if memory for the SET could be allocated
 * @retval DYN_FALSE  otherwise
 */
trilean dyn_set_insert (dyn_c* set, dyn_c* element)
{
    dyn_c rslt;
    DYN_INIT(&rslt);
    dyn_set_ref(&rslt, element);

    if (dyn_op_in(&rslt, set)) {
        if (!dyn_get_bool(&rslt)) {
            return dyn_list_push(set, element) ? DYN_TRUE : DYN_FALSE;
        }
    }

    return DYN_TRUE;
}

#endif
