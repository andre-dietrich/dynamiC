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
        dyn->data.fct->ptr  = ptr;
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
        return DYN_TRUE;
    }

    free(dyn->data.fct);

    return DYN_FALSE;
}


trilean dyn_set_fct_ss(dyn_c* dyn, dyn_c* params,
                       dyn_ushort length, dyn_char* code,
                       dyn_const_str info)
{
    if( dyn_set_fct(dyn, NULL, length, info) ) {
        dyn_proc *proc = (dyn_proc*) malloc(sizeof(dyn_proc));

        if (proc) {
            proc->code = (dyn_char*) malloc(length);

            if (proc->code) {
                dyn_ushort  i;
                for (i=0; i<length; ++i){
                    proc->code[i] = code[i];
                }

                DYN_INIT(&proc->params);
                if (dyn_copy(params, &proc->params)){
                    dyn->data.fct->ptr = proc;
                    return DYN_TRUE;
                }
                free(proc->code);
            }
            free(proc);
        }
        //dyn->data.fct->type = 1;
        dyn_fct_free(dyn);
    }

    return DYN_FALSE;
}


void dyn_fct_free(dyn_c* dyn)
{
    if (dyn->data.fct->type >= DYN_FCT_PROC) {
        dyn_proc *proc = (dyn_proc*) dyn->data.fct->ptr;
        dyn_free(&proc->params);
        free(proc->code);
        free(proc);
    }

    if (dyn->data.fct->info != NULL)
        free(dyn->data.fct->info);

    free(dyn->data.fct);
}

trilean dyn_fct_copy(const dyn_c* dyn, dyn_c* copy)
{
    if (dyn->data.fct->type < DYN_FCT_PROC)
        return dyn_set_fct( copy,
                            dyn->data.fct->ptr,
                            dyn->data.fct->type,
                            dyn->data.fct->info);

    dyn_proc *proc = (dyn_proc*)dyn->data.fct->ptr;

    return dyn_set_fct_ss( copy,
                           &proc->params,
                           dyn->data.fct->type,
                           proc->code,
                           dyn->data.fct->info);
}


dyn_str dyn_fct_get_ss (const dyn_c* dyn)
{
    return ((dyn_proc*)dyn->data.fct->ptr)->code;
}
