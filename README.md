# dynamiC

Dynamic type system for C with support for None, Boolean, Integer, Float, List,
Sets, Dictionaries and others ...

## Documentation

https://andre-dietrich.github.io/dynamiC/

## Source

https://github.com/andre-dietrich/dynamiC

## Installation

```bash
$ git clone https://github.com/andre-dietrich/dynamiC
```

## Usage

You can either generate a `libdynC` with `make lib` for dynamically linking or
simply include this repository into your project and include it into your
Makefile.

### Basic data types

It is possible to define various basic data types as listed below:

```c
#include "dynamic.h"

dyn_c var;
DYN_INIT(&var);   // initialization with type NONE (undefined)

dyn_set_bool(&var, DYN_TRUE);
dyn_set_int(&var, 3);
dyn_set_float(&var, 3.14159);
dyn_set_string(&var, "PI");

dyn_set_extern(&var, void* anything ...);
```


List have to be initialized with a default buffer length, this buffer is
automatically increased of decreased as the list grows or shrinks:

```c
dyn_c list;
DYN_INIT(&list);

dyn_set_list_len(&list, 10);
dyn_length(&list); // results in 0

dyn_list_push(&list, &var);
dyn_list_push(&list, &var);
dyn_length(&list); // results in 2

// a placeholder to append elements of type NONE
dyn_list_push_none(&list);
dyn_list_push_none(&list);
dyn_length(&list); // results in 4

// to refer to an element use
dyn_list_get(&list, 1);
dyn_list_get(&list, 2);

// cleaning up
dyn_list_pop(&list);
dyn_list_pop(&list, 2);
```

Any element can be coppied with the following function, previously allocated
memory get cleaned up and new one gets created:

```c
dyn_copy(&list, &var);  // dyn_copy(from, to);
dyn_type(&var) == LIST; // otherwise NONE, BOOL, INTEGER, FLOAT, STRING, ...
```


Any type of dynamically allocated memory can be freed with one function, the
resulting elements are reset to type NONE:

```c
dyn_free(&list);
dyn_free(&var);
```

### Complex structures

Further structures are sets and dictionaries, as for lists, memory is
automatically increased or decreased:

```c
dyn_c set;
DYN_INIT(&set);
dyn_set_set_len(&set, 1);

DYN_INIT(&var);
dyn_set_insert(&set, &var);
dyn_set_insert(&set, &var);
dyn_set_insert(&set, &var);

dyn_set_int(&var, -22);
dyn_set_insert(&set, &var);
dyn_set_insert(&set, &var);
dyn_set_insert(&set, &var);

dyn_length(&set); // results in 2
dyn_get_string(&set); // "{,-22}" first element of set is of type NONE
```


```c
dyn_c dict;
DYN_INIT(&dict);
dyn_set_dict(&dict, 1);

dyn_dict_insert(&dict, &var, "key");
dyn_dict_insert(&dict, &var, "another key");
dyn_dict_insert(&dict, &set, "key3");

dyn_length(&dict); // results in 3
dyn_get_string(&dict); // "{'key': -22, 'another key': -22, 'key3': {,-22}}"

var = dyn_dict_get(&dict, "key"); // get reference

dyn_set_insert(&var, another dynamic element ...);
```

### Operations

Operations are the commonly used `+, -, *, /, <, ==,... bitshift ...` among
others. All of them are defined within dynamic_op.h with the naming convention
`dyn_op_operator` with `(operator == add, sub, le, eq, not, pow, ...)`. Their
implementation mimics the common Python operators and the result of an operation
is stored within the first function parameter, which gets rewritten:

```c
dyn_c op1;
DYN_INIT(&op1);
dyn_set_int(&op1, 2);

dyn_c op2;
DYN_INIT(&op1);
dyn_set_int(&op1, 2);

dyn_op_add(&op1, &op2);
dyn_get_int(&op1); // returns 4

dyn_op_pow(&op1, &op2);
dyn_get_int(&op1); // returns 16
```

Depending on the used datatypes, different behavior is exposed for some
operations, the highest datatype defines the result type, to which the other
parameter is casted, see the enumeration in dynamic_types.h.

```c
dyn_set_string(&op1, "string");

dyn_op_add(&op1, &op2); // results in "string2"
```

Furhtermore, and unlike in Python, the order of parameters results in different
results:

```c
dyn_op_add(&op2, &op1); // results in "2string2"

// or ...
dyn_op_add(&list, &element); // results in [ ..., element]
dyn_op_add(&element, &list); // results in [ element, ...]
```

Boolean operations can also be used for sets:

```c
dyn_op_and(&set1, &set2); // union
dyn_op_or(&set1, &set2);  // difference
```

Otherwise, and as in Python, dynamic elements are cast to boolean values:
```c
dyn_get_bool(&none);      // DYN_FALSE
dyn_get_bool(&integer);   // DYN_FALSE if zero, otherwise DYN_TRUE
dyn_get_bool(&float__);   // DYN_FALSE if zero, otherwise DYN_TRUE
dyn_get_bool(&string);    // DYN_FALSE if empty, otherwise DYN_TRUE
dyn_get_bool(&list);      // DYN_FALSE if empty, otherwise DYN_TRUE
dyn_get_bool(&set);       // DYN_FALSE if empty, otherwise DYN_TRUE
dyn_get_bool(&dict);      // DYN_FALSE if empty, otherwise DYN_TRUE

dyn_get_bool(&everythin_else);  // DYN_FALSE
```

Boolean operations perform as usual, except if a value is NONE or associated
with... Then a ternary operations gets applied

```c
dyn_op_and(&true,  &none); // NONE
dyn_op_and(&false, &none); // DYN_FALSE

dyn_op_or(&true,  &none);  // DYN_TRUE
dyn_op_or(&false, &none);  // NONE

dyn_op_not(&none);         // NONE
```

This behavior is quite usefull, if for example relational operations are
applied onto dynamic elements, which cannot be compared, such as:

```c
dyn_op_le(&string,  &list);        // NONE
dyn_op_gt(&procedure,  &external); // NONE
```

The result element is of type NONE and can still be used in the further
evaluation ...

## License

This project is licensed under the MIT License - see the LICENSE.md file for
details

Copyright (C) 2016-2017

André Dietrich <dietrich@ivs.cs.uni-magdeburg.de>
