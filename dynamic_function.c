#include "dynamic.h"



ss_char dyn_set_fct(dyn_c* dyn, void *ptr, ss_byte type, ss_str info)
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
            if (ss_strlen(info)) {
                dyn->data.fct->info = (ss_str) malloc( ss_strlen(info)+1 );
                if (dyn->data.fct->info) {
                    ss_strcpy( dyn->data.fct->info, info );
                }
            }
        }
        return 1;
    }

    free(dyn->data.fct);

    return 0;
}


ss_char dyn_set_fct_ss(dyn_c* dyn, dyn_c* params,
                     ss_ushort length, ss_char* code,
                     ss_str info)
{
    if( dyn_set_fct(dyn, NULL, 0, info) ) {
        dyn_proc *proc = (dyn_proc*) malloc(sizeof(dyn_proc));

        if (proc) {
            proc->length = length;
            proc->code = (ss_char*) malloc(length);

            if (proc->code) {
                ss_ushort  i;
                for (i=0; i<length; ++i){
                    proc->code[i] = code[i];
                }

                DYN_INIT(&proc->params);
                if (dyn_copy(params, &proc->params)){
                    dyn->data.fct->ptr = proc;
                    return 1;
                }
                free(proc->code);
            }
            free(proc);
        }
        dyn->data.fct->type = 1;
        dyn_fct_free(dyn);
    }

    return 0;
}


ss_char dyn_fct_free(dyn_c* dyn)
{
    if (!dyn->data.fct->type) {
        dyn_proc *proc = (dyn_proc*) dyn->data.fct->ptr;
        dyn_free(&proc->params);
        free(proc->code);
        free(proc);
    }

    if (dyn->data.fct->info != NULL)
        free(dyn->data.fct->info);
    free(dyn->data.fct);

    return 1;
}

ss_char dyn_fct_copy(dyn_c* dyn, dyn_c* copy)
{
    if (dyn->data.fct->type)
        return dyn_set_fct( copy,
                            dyn->data.fct->ptr,
                            dyn->data.fct->type,
                            dyn->data.fct->info);

    dyn_proc *proc = (dyn_proc*)dyn->data.fct->ptr;

    return dyn_set_fct_ss( copy,
                           &proc->params,
                           proc->length,
                           proc->code,
                           dyn->data.fct->info);
}


ss_char* dyn_fct_get_ss (dyn_c* dyn)
{
    return ((dyn_proc*)dyn->data.fct->ptr)->code;
}
