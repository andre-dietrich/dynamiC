 /**
  * @file dynamic.h
  * @author André Dietrich
  * @date 9 April 2016
  * @brief Dynamic data types as part of the SelectScript-VM implementation.
  *
  * @license MIT
  *
  * @see https://ess-ovgu.github.io/dynamiC
  */

#ifndef DYN_C_H
#define DYN_C_H

#include <stdlib.h>

#include "dynamic_types.h"
#include "dynamic_defines.h"
#include "dynamic_string.h"


/**
 * \defgroup DynamicBase
 *
 * @brief basic functionality for scalar dynamic data types
 *
 * These functions and macros cover basic scalar data types, their handling,
 * initialization, etc. Every module includes its own functions for memory
 * allocation and freeing, but functions such as dyn_free, dyn_get_string,
 * as well es others provide basic interfaces, which hide the additional and
 * type specific functionality which is required to deal with different dynamic
 * data types.
 *
 * @{
 */
//! Mandatory initialization for dynamic elements (NONE)
#define   DYN_INIT(dyn)       (dyn)->type=NONE
//! Return type value of a dynamic element @see TYPE
#define   DYN_TYPE(dyn)       (dyn)->type
//! Check if dynamic element is of type NONE
#define   DYN_IS_NONE(dyn)    !DYN_TYPE(dyn)
//! Check if dynamic element is not of type NONE
#define   DYN_NOT_NONE(dyn)   DYN_TYPE(dyn)
//! Check if dynamic element is of type REFERENCE
#define   DYN_IS_REFERENCE(dyn) \
          DYN_TYPE(dyn)==REFERENCE || DYN_TYPE(dyn)==REFERENCE2

//! Return type value of a dynamic element @see TYPE
TYPE       dyn_type            (const dyn_c* dyn);
//! free allocated memory
void       dyn_free            (dyn_c* dyn);
//! Deep copy dynamic element
trilean    dyn_copy            (const dyn_c* dyn,  dyn_c* copy);
//! Move dynamic element to new reference, from is of type NONE afterwards
void       dyn_move            (dyn_c* from, dyn_c* to);
/** @brief Reterns the length of an element.
 *
 *  @param[in] dyn value to check
 *  @return length
 */
dyn_ushort dyn_length          (const dyn_c* dyn);
//! Return the number of allocated bytes
dyn_uint   dyn_size            (const dyn_c* dyn);

//! Set dynamic element to NONE
void       dyn_set_none        (dyn_c* dyn);
//! Set dynamic element to BOOL (DYN_TRUE or DYN_FALSE)
void       dyn_set_bool        (dyn_c* dyn, const dyn_char  v);
//! Set dynamic element to INTEGER
void       dyn_set_int         (dyn_c* dyn, const dyn_int   v);
//! Set dynamic element to FLOAT
void       dyn_set_float       (dyn_c* dyn, const dyn_float v);
//! Set dynamic element to point to an arbitrary value
void       dyn_set_extern      (dyn_c* dyn, const void*     v);
//! Set dynamic element to STRING
trilean    dyn_set_string      (dyn_c* dyn, const dyn_str   v);
//! Set dynamic element as reference to another dynamic element
void       dyn_set_ref         (dyn_c* ref, dyn_c* orig);

//! Return boolean value of an dynamic element
trilean    dyn_get_bool        (const dyn_c* dyn);
/** @brief Returns the trinary truth value (DYN_TRUE|DYN_FALSE|DYN_NONE) of an
 *         element.
 *
 *  Only an element of type NONE results in a return value of DYN_NONE, all
 *  other types result in a boolean value, as defined in @see dyn_get_bool
 *
 *  @param[in] dyn value to check
 *  @returnval DYN_TRUE  ( 1)
 *  @returnval DYN_FALSE ( 0)
 *  @returnval DYN_NONE  (-1)
 */
trilean    dyn_get_bool_3      (const dyn_c* dyn);
//! Return integer value of a dynamic element
dyn_int    dyn_get_int         (const dyn_c* dyn);
//! Return float value of a dynamic element
dyn_float  dyn_get_float       (const dyn_c* dyn);
//! Return string representation value of a dynamic element
dyn_str    dyn_get_string      (const dyn_c* dyn);
//! Return pointer, stored in dyn->data.ex
const void* dyn_get_extern     (const dyn_c* dyn);
//! Add string representation of dynamic element to string
void       dyn_string_add      (const dyn_c* dyn, dyn_str string);
//! Calculate length of string representation of dynamic element
dyn_ushort dyn_string_len      (const dyn_c* dyn);
/**@}*/



/**
 * \defgroup DynamicList
 *
 * @brief All list related functions.
 *
 * @{
 */
//! Initialize dyn as list with default length
#define    DYN_SET_LIST(dyn)           dyn_set_list_len(dyn, LIST_DEFAULT)
//! Return list length
#define    DYN_LIST_LEN(dyn)           (dyn)->data.list->length
//! Return the reference to the ith element within a dynamic list
#define    DYN_LIST_GET_REF(dyn,i)     &(dyn)->data.list->container[i]
//! Return the reference to the last element within a list
#define    DYN_LIST_GET_END(dyn) \
           &(dyn)->data.list->container[DYN_LIST_LEN(dyn)-1]
//! Return the reference to the ith element starting from the last
#define    DYN_LIST_GET_REF_END(dyn,i) \
           &(dyn)->data.list->container[DYN_LIST_LEN(dyn)-i]

//! Set dynamic element to list with maximal length
trilean    dyn_set_list_len    (dyn_c* dyn, dyn_ushort len);
//! Push new element to the end of a list
dyn_c*     dyn_list_push       (dyn_c* list, const dyn_c* element);
//! Push NONE element to the end of a list
dyn_c*     dyn_list_push_none  (dyn_c* list);
//! Pop the last element from the list and move it to param element
trilean    dyn_list_pop        (dyn_c* list, dyn_c* element);
//! Copy the ith element of a list to param element
trilean    dyn_list_get        (const dyn_c* list, dyn_c* element, const dyn_short i);
//! Return a reference to the ith element within list, negative values are allowed
dyn_c*     dyn_list_get_ref    (const dyn_c* list, const dyn_short i);
//! Pop i elements from the end of a list
trilean    dyn_list_popi       (dyn_c* list, const dyn_short i);
//! Free the allocated memory of the entire list and set it to NONE
void       dyn_list_free       (dyn_c* list);
//! Make a deep copy of the entire list
trilean    dyn_list_copy       (const dyn_c* list, dyn_c* copy);
//! Delete the ith element from a list
trilean    dyn_list_remove     (dyn_c* list, dyn_ushort i);
//! Insert a new element at the ith position into a list
trilean    dyn_list_insert     (dyn_c* list, dyn_c* element, const dyn_ushort i);
//! Change the maximal space of a list
trilean    dyn_list_resize     (dyn_c* list, const dyn_ushort size);
//! Return the length of the string representation of a list
dyn_ushort dyn_list_string_len (const dyn_c* list);
//! Add string representation of a list to str
void       dyn_list_string_add (const dyn_c* list, dyn_str str);
/**@}*/


/**
 * \defgroup DynamicSet
 *
 * @brief Extension of type LIST to handle and implement SET
 *
 * @{
 */
#ifdef S2_SET
//! Initialize dynamic element as empty set with maximal length
trilean    dyn_set_set_len     (dyn_c* set, const dyn_ushort len);
//! Insert new element into set, if and only if it is not included yet
trilean    dyn_set_insert      (dyn_c* set, dyn_c* element);
// Delete element from a set
//dyn_char   dyn_set_remove      (dyn_c* set, dyn_c* element);
#endif
/**@}*/


/**
 * \defgroup DynamicDictionary
 * @{
 */

//! Return number of elements within a dictionary
#define    DYN_DICT_LEN(dyn) \
           dyn->data.dict->value.data.list->length
//! Return a reference to the ith element stored within a dictionary
#define    DYN_DICT_GET_I_REF(dyn,i) \
           &(dyn)->data.dict->value.data.list->container[i]
//! Return a reference to the ith key stored within a dictionary
#define    DYN_DICT_GET_I_KEY(dyn,i)  (dyn)->data.dict->key[i]
//! Return the maximal usable number of elements of a dictionary
#define    DYN_DICT_SPACE(dyn)         dyn->value.data.list->space
//! Return the number of elements stored within a dictionary
#define    DYN_DICT_LENGTH(dyn)        dyn->value.data.list->length

//! Set dyn to a dictionary with a max. length of elements
trilean    dyn_set_dict        (dyn_c* dyn, const dyn_ushort length);
//! Replace the ith element in a dictionary with a new value
trilean    dyn_dict_change     (dyn_c* dyn, const dyn_ushort i, const dyn_c *value);
//! Insert a new key-value pair into the dictionary
dyn_c*     dyn_dict_insert     (dyn_c* dyn, const dyn_str key, dyn_c *value);
//! Remove key-value pair from dictionary
trilean    dyn_dict_remove     (dyn_c* dyn, const dyn_str key);
//! Get the reference to value stored at key
dyn_c*     dyn_dict_get        (const dyn_c* dyn, const dyn_str key);
//! Set the loc reference in all procedures (object-oriented)
trilean    dyn_dict_set_loc    (dyn_c* dyn);
//! Set the available space for elements
trilean    dyn_dict_resize     (dyn_c* dyn, const dyn_ushort size);

//! Get the reference to ith value in dict
dyn_c*     dyn_dict_get_i_ref (const dyn_c* dyn, const dyn_ushort i);
//! Get the reference to ith key in dict
dyn_str    dyn_dict_get_i_key (const dyn_c* dyn, const dyn_ushort i);

//! Check if dict has key and return its position - 1 (returns 0 if not found)
dyn_ushort dyn_dict_has_key   (const dyn_c* dyn, const dyn_str key);
//! todo
void       dyn_dict_empty     (dyn_c* dyn);
//! Free all allocated memory
void       dyn_dict_free      (dyn_c* dyn);
//! Copy the entire dict
trilean    dyn_dict_copy      (const dyn_c* dyn, dyn_c* copy);

//! Calculate the required string length
dyn_ushort dyn_dict_string_len(const dyn_c* dyn);
//! Add the dict-string representation to string
void       dyn_dict_string_add(const dyn_c* dyn, dyn_str string);
/**@}*/

/**
 * \defgroup DynamicFunction
 * @{
 */
trilean   dyn_set_fct          (dyn_c* dyn, void *ptr, const dyn_byte type, const dyn_str info);
trilean   dyn_set_fct_ss       (dyn_c* dyn, dyn_c* params, dyn_ushort length, dyn_char* code, dyn_str info);
dyn_str   dyn_fct_get_ss       (const dyn_c* dyn);
void      dyn_fct_free         (dyn_c* dyn);
trilean   dyn_fct_copy         (const dyn_c* dyn, dyn_c* copy);
/**@}*/

/**
 * \defgroup DynamicOperations
 *
 * @brief Defintion of all dynamic operations, including arithmetic, logical,
 *        relational, and bit-operations.
 *
 * These functions cover all basic operations, which are common for most
 * programming languages. In principle most operations known from Python are
 * offered with a similar behavior, but with two differences. Dynamic
 * operations evaluate a result based on the data type with the highes priority
 * and not on the basis of left-hand and right-hand parameters. That means, for
 * dynple (see the defintion of every function):
 *
 *  @code
 *  // LIST type is higher than STRING type
 *  dyn_op_add([1,2,3], "test string") == [1,2,3,"test string"]
 *  dyn_op_add("test string", [1,2,3]) == ["test string",1,2,3]
 *
 *  // STRING type is higher than INTEGER type
 *  dyn_op_mul(3, "abc") == "abcabcabc"
 *  dyn_op_mul("abc", 3) == "abcabcabc"
 *
 *  // FLOAT type is higher than INTEGER type
 *  dyn_op_div(9.0, 3) == 3.0
 *  dyn_op_div(9, 3.0) == 3.0
 *  @endcode
 *
 * The result is always stored and returned within the first parameter passed
 * to an operator function. If this parameter is a dynamic reference of TYPE
 * REFERENCE or REFERENCE2, then a new element is created, the original element
 * remains. If the operation was performed without any error, then DYN_TRUE is
 * returned by every function of that module, otherwise DYN_FALSE.
 *
 * A second characteristic of the operations applied is that a trinary logical
 * system is applied to simplify error-handling amongst other benefits. Trinary
 * means that next to the two boolean values DYN_TRUE and DYN_FALSE, the NONE
 * type is used to represent another unknown or undefined state. The benefit is,
 * that comparing two distinct dynamic types such as a list and a string with
 * less than or greater than results in that unknown/undefined state and not
 * in an error. This trinary logical system is also perfect for applying
 * logical operations, see therefor the corresponding logical functions.
 *
 * @see https://en.wikipedia.org/wiki/Three-valued_logic
 *
 * @{
 */

//! Negate dynamic value in param dyn
trilean dyn_op_neg(dyn_c *dyn);
//! Add dyn2 to dyn1
trilean dyn_op_add(dyn_c *dyn1, dyn_c *dyn2);
//! Subtract dyn2 from dyn1
trilean dyn_op_sub(dyn_c *dyn1, dyn_c *dyn2);
//! Multiply dyn1 with dyn2
trilean dyn_op_mul(dyn_c *dyn1, dyn_c *dyn2);
//! Divide dyn1 by dyn2
trilean dyn_op_div(dyn_c *dyn1, dyn_c *dyn2);
//! dyn1 Modulo dyn2
trilean dyn_op_mod(dyn_c *dyn1, dyn_c *dyn2);
//! dyn1 to the power of dyn2
trilean dyn_op_pow(dyn_c *dyn1, dyn_c *dyn2);

//! Logical (trinary) AND operation
trilean dyn_op_and(dyn_c *dyn1, dyn_c *dyn2);
//! Logical (trinary) OR operation
trilean dyn_op_or (dyn_c *dyn1, dyn_c *dyn2);
//! Logical (trinary) XOR operation
trilean dyn_op_xor(dyn_c *dyn1, dyn_c *dyn2);
//! Logical (trinary) Negation
trilean dyn_op_not(dyn_c *dyn);

//! Relational Equality
trilean dyn_op_eq (dyn_c *dyn1, dyn_c *dyn2);
//! Relational Not Equal
trilean dyn_op_ne (dyn_c *dyn1, dyn_c *dyn2);
//! Relational Less Than
trilean dyn_op_lt (dyn_c *dyn1, dyn_c *dyn2);
//! Relational Less Than or Equal
trilean dyn_op_le (dyn_c *dyn1, dyn_c *dyn2);
//! Relational Greater Than
trilean dyn_op_gt (dyn_c *dyn1, dyn_c *dyn2);
//! Relational Greater Than or Equal
trilean dyn_op_ge (dyn_c *dyn1, dyn_c *dyn2);

//! Check if dyn2 is element of dyn1
trilean dyn_op_in (dyn_c *element, dyn_c *container);

//! Binary complement
trilean dyn_op_b_not(dyn_c *dyn);
//! Binary AND
trilean dyn_op_b_and(dyn_c *dyn1, dyn_c *dyn2);
//! Binary OR
trilean dyn_op_b_or (dyn_c *dyn1, dyn_c *dyn2);
//! Binary XOR
trilean dyn_op_b_xor(dyn_c *dyn1, dyn_c *dyn2);
//! Binary shift left
trilean dyn_op_b_shift_l(dyn_c *dyn1, dyn_c *dyn2);
//! Binary shift right
trilean dyn_op_b_shift_r(dyn_c *dyn1, dyn_c *dyn2);
/**@}*/

#endif //DYN_C_H
