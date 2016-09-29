/****************************************************************************
 * Copyright (C) 2016 by André Dietrich                                     *
 *                                                                          *
 * todo.                                                                    *
 ****************************************************************************/

 /**
  * @file dynamic.h
  * @author André Dietrich
  * @date 9 April 2016
  * @brief Dynamic data types as part of the SelectScript VM implementation.
  *
  * todo.
  * @see http://todo
  */

#ifndef DYN_C_H
#define DYN_C_H

#include "ss_string.h"
#include <stdlib.h>

#include "dynamic_types.h"
#include "dynamic_defines.h"
#include "functional_c.h"

/**
 * @brief Used data types within the SelectScript.
 *
 * todo.
 */

enum TYPE { NONE,       /**< None type without any value.   */
            BOOL,       /**< Boolean 0==Fales, 1==True.     */
            BYTE,       /**< Ranging from 0 to 255.         */
            INTEGER,
            FLOAT,
            STRING,
            LIST,
            SET,      // todo
            DICT,
            TEMP,     // todo
//            PROCEDURE,
            FUNCTION,
            EXTERN,
            REFERENCE,
            REFERENCE2,
            MISCELLANEOUS};

typedef struct dynamic dyn_c;
typedef struct dynamic_list dyn_list;
typedef struct dynamic_dict dyn_dict;
typedef struct dynamic_procedure dyn_proc;
typedef struct dynamic_function dyn_fct;


/**
 * @brief Basic container for dynamic data types.
 *
 * todo.
 */
struct dynamic {
    union {
        ss_char     b;
        ss_int      i;
        ss_float    f;
        ss_str      str;
        dyn_list*   list;
        dyn_dict*   dict;
        dyn_fct*    fct;
        void*       ex;
        dyn_c*      ref;
    } data;
    char type;
} __attribute__ ((packed));

struct dynamic_list {
     ss_ushort length;
     ss_ushort space;
     dyn_c     *container;
} __attribute__ ((packed));

struct dynamic_dict {
     ss_str*   key;
     //ss_int    meta : 24;
     dyn_c     value;
} __attribute__ ((packed));

struct dynamic_procedure {
     dyn_c     params;
     ss_ushort length;
     ss_char*  code;
} __attribute__ ((packed));

struct dynamic_function {
     void*     ptr;
     ss_byte   type; // 0 basic fct, 1 sys fct
     ss_str    info;
} __attribute__ ((packed));

/*---------------------------------------------------------------------------*/
#define   DYN_INIT(X)         (X)->type=NONE
#define   DYN_TYPE(X)         (X)->type
#define   DYN_IS_NONE(X)      !DYN_TYPE(X)
#define   DYN_NOT_NONE(X)     DYN_TYPE(X)
#define   DYN_IS_REFERENCE(X) DYN_TYPE(X)==REFERENCE || DYN_TYPE(X)==REFERENCE2
/*---------------------------------------------------------------------------*/
void      dyn_free            (dyn_c* dyn);
ss_char   dyn_copy            (dyn_c* dyn,  dyn_c* copy);
void      dyn_move            (dyn_c* from, dyn_c* to);
ss_uint   dyn_size            (dyn_c* dyn);
ss_char   dyn_type            (dyn_c* dyn);

ss_str    dyn_get_string      (dyn_c* dyn);
void      dyn_string_add      (dyn_c* dyn, ss_str string);
ss_ushort dyn_string_len      (dyn_c* dyn);
/*---------------------------------------------------------------------------*/
void      dyn_set_none        (dyn_c* dyn);
void      dyn_set_bool        (dyn_c* dyn, ss_char  v);
void      dyn_set_int         (dyn_c* dyn, ss_int   v);
void      dyn_set_float       (dyn_c* dyn, ss_float v);
void      dyn_set_extern      (dyn_c* dyn, void*    v);
ss_char   dyn_set_string      (dyn_c* dyn, char const * v);
void      dyn_set_ref         (dyn_c* ref, dyn_c* orig);
/*---------------------------------------------------------------------------*/
#define   DYN_SET_LIST(X)     dyn_set_list_len(X, LIST_DEFAULT)
#define   DYN_LIST_LEN(X)     (X)->data.list->length
ss_char   dyn_set_list_len    (dyn_c* list, ss_ushort len);
ss_char   dyn_list_push       (dyn_c* list, dyn_c* element);
ss_char   dyn_list_pop        (dyn_c* list, dyn_c* element);
ss_char   dyn_list_get        (dyn_c* list, dyn_c* element, ss_short i);
#define   DYN_LIST_GET_REF(X,i)     &(X)->data.list->container[i]
#define   DYN_LIST_GET_END(X)       &(X)->data.list->container[DYN_LIST_LEN(X)-1]
#define   DYN_LIST_GET_REF_END(X,i) &(X)->data.list->container[DYN_LIST_LEN(X)-i]
dyn_c*    dyn_list_get_ref    (dyn_c* list, ss_short i);
ss_char   dyn_list_popi       (dyn_c* list, ss_short i);
ss_char   dyn_list_free       (dyn_c* list);
ss_char   dyn_list_copy       (dyn_c* list, dyn_c* copy);
ss_char   dyn_list_remove     (dyn_c* list, ss_ushort i);
ss_char   dyn_list_insert     (dyn_c* list, dyn_c* element, ss_ushort i);
ss_char   dyn_list_resize     (dyn_c* list, ss_ushort size);
ss_ushort dyn_list_string_len (dyn_c* dyn);
void      dyn_list_string_add (dyn_c* dyn, ss_str string);
/*---------------------------------------------------------------------------*/
ss_char   dyn_set_dict        (dyn_c* dyn, ss_ushort length);
ss_char   dyn_dict_change     (dyn_c* dyn, ss_ushort i, dyn_c *value);
ss_char   dyn_dict_insert     (dyn_c* dyn, ss_str key,  dyn_c *value);
ss_char   dyn_dict_remove     (dyn_c* dyn, ss_str key);
dyn_c*    dyn_dict_get        (dyn_c* dyn, ss_str key);
ss_char   dyn_dict_set_loc    (dyn_c* dyn);
/*---------------------------------------------------------------------------*/
#define   DYN_DICT_LEN(X)         X->data.dict->value.data.list->length   //DYN_LIST_LEN((X)->data.dict->value)
//#define   DYN_DICT_LEN(X)         dyn_length(X)   //DYN_LIST_LEN((X)->data.dict->value)
//#define   DYN_DICT_GET_I_REF(X,i) dyn_list_get_ref(&(X)->data.dict->value, i)
#define   DYN_DICT_GET_I_REF(X,i)  &(X)->data.dict->value.data.list->container[i]
#define   DYN_DICT_GET_I_KEY(X,i)  (X)->data.dict->key[i]
#define   DYN_DICT_SPACE(X)         X->value.data.list->space
#define   DYN_DICT_LENGTH(X)        X->value.data.list->length

//#define   DYN_DICT_META_SET(X,i)  X->data.dict->meta |= 1 << i
//#define   DYN_DICT_META_SET(X,i)  X->data.dict->meta |= 1 << i

dyn_c*    dyn_dict_get_i_ref (dyn_c* dyn, ss_ushort i);
ss_str    dyn_dict_get_i_key (dyn_c* dyn, ss_ushort i);

ss_ushort dyn_dict_has_key   (dyn_c* dyn, ss_str key);
ss_char   dyn_dict_empty     (dyn_c* dyn);
ss_char   dyn_dict_free      (dyn_c* dyn);
ss_char   dyn_dict_copy      (dyn_c* dyn, dyn_c* copy);
ss_ushort dyn_dict_string_len(dyn_c* dyn);
void      dyn_dict_string_add(dyn_c* dyn, ss_str string);
/*---------------------------------------------------------------------------*/
//ss_char   dyn_set_proc        (dyn_c* dyn, dyn_c* params,
//                               ss_ushort length, ss_char* code,
//                               ss_str info);
//ss_char   dyn_proc_copy       (dyn_c* dyn, dyn_c* copy);
//ss_char   dyn_proc_free       (dyn_c* dyn);
//ss_char*  dyn_proc_get        (dyn_c* dyn);


ss_char  dyn_set_fct          (dyn_c* dyn, void *ptr, ss_byte type, ss_str info);
ss_char  dyn_set_fct_ss       (dyn_c* dyn, dyn_c* params, ss_ushort length, ss_char* code, ss_str info);
ss_char* dyn_fct_get_ss       (dyn_c* dyn);
ss_char  dyn_fct_free         (dyn_c* dyn);
ss_char  dyn_fct_copy         (dyn_c* dyn, dyn_c* copy);

/*---------------------------------------------------------------------------*/
#ifdef S2_SET
ss_char   dyn_set_set_len     (dyn_c* set, ss_ushort len);
ss_char   dyn_set_insert      (dyn_c* set, dyn_c* element);
ss_char   dyn_set_remove      (dyn_c* set, dyn_c* element);
#endif
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
ss_ushort dyn_length          (dyn_c* dyn);
ss_char   dyn_get_bool        (dyn_c* dyn);
ss_int    dyn_get_int         (dyn_c* dyn);
ss_float  dyn_get_float       (dyn_c* dyn);
void*     dyn_get_extern      (dyn_c* dyn);

ss_char   dyn_get_bool_3      (dyn_c* dyn);
/*---------------------------------------------------------------------------*/
char dyn_op_neg(dyn_c *dyn);
char dyn_op_add(dyn_c *dyn1, dyn_c *dyn2);
char dyn_op_sub(dyn_c *dyn1, dyn_c *dyn2);
char dyn_op_mul(dyn_c *dyn1, dyn_c *dyn2);
char dyn_op_div(dyn_c *dyn1, dyn_c *dyn2);
char dyn_op_mod(dyn_c *dyn1, dyn_c *dyn2);
char dyn_op_pow(dyn_c *dyn1, dyn_c *dyn2);

char dyn_op_and(dyn_c *dyn1, dyn_c *dyn2);
char dyn_op_or (dyn_c *dyn1, dyn_c *dyn2);
char dyn_op_xor(dyn_c *dyn1, dyn_c *dyn2);
char dyn_op_not(dyn_c *dyn);

char dyn_op_eq (dyn_c *dyn1, dyn_c *dyn2);
char dyn_op_ne (dyn_c *dyn1, dyn_c *dyn2);
char dyn_op_lt (dyn_c *dyn1, dyn_c *dyn2);
char dyn_op_le (dyn_c *dyn1, dyn_c *dyn2);
char dyn_op_gt (dyn_c *dyn1, dyn_c *dyn2);
char dyn_op_ge (dyn_c *dyn1, dyn_c *dyn2);

char dyn_op_in (dyn_c *dyn1, dyn_c *dyn2);

char dyn_op_and(dyn_c *dyn1, dyn_c *dyn2);
char dyn_op_or (dyn_c *dyn1, dyn_c *dyn2);
char dyn_op_xor(dyn_c *dyn1, dyn_c *dyn2);

char dyn_op_b_not(dyn_c *dyn);
char dyn_op_b_and(dyn_c *dyn1, dyn_c *dyn2);
char dyn_op_b_or (dyn_c *dyn1, dyn_c *dyn2);
char dyn_op_b_xor(dyn_c *dyn1, dyn_c *dyn2);
char dyn_op_b_shift_l(dyn_c *dyn1, dyn_c *dyn2);
char dyn_op_b_shift_r(dyn_c *dyn1, dyn_c *dyn2);

#endif //DYN_C_H
