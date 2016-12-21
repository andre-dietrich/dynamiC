/**
 *  @file       dynamic_types.h
 *  @author     André Dietrich
 *  @date       9 April 2016
 *
 *  @copyright  Copyright 2016 André Dietrich. All rights reserved.
 *
 *  @license    This project is released under the BSD-License.
 *
 *  @brief      Definition of internal settings and applied data types.
 *
 *
 */

#include <stdint.h>

#ifndef DYNAMIC_TYPES_C_H
#define DYNAMIC_TYPES_C_H

/** @brief basic return type for truth values
 */
typedef enum {
    DYN_FALSE =  0, ///< represents boolean false
    DYN_TRUE  =  1, ///< represents boolean true
    DYN_NONE  = -1  ///< represents a third unknown state, which is not true or false
} trilean;

/** @brief Basic 8bit signed integer
 */
typedef char          ss_char;
/** @brief Basic 8bit integer (0 - 255)
 */
typedef unsigned char ss_byte;
/** @brief Standard dynamic C string
 */
typedef char*         ss_str;
/** @brief 16bit unsigned integer
 */
typedef uint16_t      ss_ushort;
/** @brief 16bit signed integer
 */
typedef int16_t       ss_short;
/** @brief 32bit signed integer, standard Integer type.
 */
typedef int32_t       ss_int;
/** @brief 32bit unsigned integer
 */
typedef uint32_t      ss_uint;
/** @brief basic float definition (32 bit)
 */
typedef float         ss_float;

/**
 * @brief Basic data type definitions
 */
enum TYPE {
    NONE,               ///< None type without any value
    BOOL,               ///< Boolean 0==False, 1==True
    BYTE,               ///< not used
    INTEGER,            ///< signed integer 32 bit
    FLOAT,              ///< float 32 bit
    STRING,             ///< char*
    LIST,               ///< list of type dyn_list
    SET,                ///< set of type dyn_list
    DICT,               ///< dictionary of type dyn_dict
    TEMP,               ///< not used
    FUNCTION,           ///< function pointer of type dyn_fct
    EXTERN,             ///< void*
    REFERENCE,          ///< dyn_c* for internal usage only
    REFERENCE2,         ///< dyn_c*, explicite reference
    MISCELLANEOUS       ///< can be used for different purposes
};

/** @brief common dynamic data type
 */
typedef struct dynamic dyn_c;
/** @brief common dynamic list data type
 */
typedef struct dynamic_list dyn_list;
/** @brief common dynamic dictionary data type
 */
typedef struct dynamic_dict dyn_dict;
/** @brief common dynamic procedure/bytecode data type
 */
typedef struct dynamic_procedure dyn_proc;
/** @brief common dynamic function pointer
 */
typedef struct dynamic_function dyn_fct;

/**
 * @brief Basic container for dynamic data types.
 *
 * Basic container consisting of two parts, a union data for storing or
 * referencing to values. The type value contains enum TYPE in order to define
 * which value of data has to be used.
 */
struct dynamic {

    union {               /*@{*/
        ss_char     b;    //!< boolean value
        ss_int      i;    //!< basic integer
        ss_float    f;    //!< float value
        ss_str      str;  //!< pointer to character-array
        dyn_list*   list; //!< pointer to dynamic list
        dyn_dict*   dict; //!< pointer to dynamic dictionary
        dyn_fct*    fct;  //!< pointer to function
        void*       ex;   //!< external (pointer to everything)
        dyn_c*      ref;  //!< reference pointer to dynamic elements
                          /*@}*/
    } data;
    char type;            //!< type definition
} __attribute__ ((packed));

/**
 * @brief Basic container for lists.
 *
 * todo.
 */
struct dynamic_list {
     ss_ushort length;      //!< elements in use
     ss_ushort space;       //!< elements available
     dyn_c     *container;  //!< pointer to an array of dynamic elements
} __attribute__ ((packed));

/**
 * @brief Basic container for dictionaries.
 *
 * todo.
 */
struct dynamic_dict {
     ss_str*   key;         //!< array to C strings used as identifiers
     dyn_c     value;       //!< dynamic element of type dyn_list
} __attribute__ ((packed));

/**
 * @brief Basic container for bytecode.
 *
 * todo.
 */
struct dynamic_procedure {
     dyn_c     params;      //!< type NONE for no, Otherwise dictionaries are used
     ss_ushort length;      //!< bytecode length
     ss_char*  code;        //!< pointer to bytecode
} __attribute__ ((packed));

/**
 * @brief Basic container/pointer to functions.
 *
 * todo.
 */
struct dynamic_function {
     void*     ptr;         //!< pointer to function
     ss_byte   type;        //!< 0 basic C-fct, 1 system C-fct, 2 procedure
     ss_str    info;        //!< info string
} __attribute__ ((packed));


#endif // DYNAMIC_TYPES_C_H
