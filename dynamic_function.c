/**
 *  @file dynamic_function.c
 *  @author André Dietrich
 *  @date 14 December 2016
 *
 *  @copyright Copyright 2016 André Dietrich. All rights reserved.
 *
 *  @license This project is released under the MIT-License.
 *
 *  @brief Implementation of dynamiC functions/procedures module.
 *
 *
 */

#include "dynamic.h"

trilean dyn_set_fct(dyn_c* dyn, void *ptr, const dyn_ushort type, dyn_const_str info)
{
    dyn_free(dyn);

    dyn->type = FUNCTION;

    //dyn->data.fct = NULL;
    dyn->data.fct = (dyn_fct*) malloc(sizeof(dyn_fct));

    if (dyn->data.fct) {
        dyn->data.fct->type = type;
        dyn->data.fct->info = NULL;
        if (info!=NULL) {
            if (dyn_strlen(info)) {
                dyn->data.fct->info = (dyn_str) malloc( dyn_strlen(info)+1 );
                if (dyn->data.fct->info) {
                    dyn_strcpy( dyn->data.fct->info, info );
                }
            }
        }

        if (type < DYN_FCT_PROC) {
            dyn->data.fct->ptr = ptr;
            return DYN_TRUE;
        }
        else
        {
            dyn_char* proc = (dyn_char*) malloc(type);

            if (proc) {
                dyn_char* code = ptr;
                dyn_ushort  i;
                for (i=0; i<type; ++i){
                    proc[i] = code[i];
                }
                dyn->data.fct->ptr = (void*)proc;
                return DYN_TRUE;
            }
        }
    }

    free(dyn->data.fct);

    return DYN_FALSE;
}

void dyn_fct_free(dyn_c* dyn)
{
    if (dyn->data.fct->type > DYN_FCT_PROC) {
        free(dyn->data.fct->ptr);
    }

    if (dyn->data.fct->info != NULL)
        free(dyn->data.fct->info);

    free(dyn->data.fct);
}

trilean dyn_fct_copy(const dyn_c* dyn, dyn_c* copy)
{
    return dyn_set_fct( copy,
                        dyn->data.fct->ptr,
                        dyn->data.fct->type,
                        dyn->data.fct->info);
}
