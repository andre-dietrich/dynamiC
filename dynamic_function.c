#include "dynamic.h"
#include <stdio.h>


ss_char dyn_set_fct(dyn_c* dyn, void *ptr, ss_ushort type, ss_str info)
{
    dyn_free(dyn);

    dyn->data.fct = (dyn_fct*) malloc(sizeof(dyn_fct));

    if (dyn->data.fct) {
        dyn->type = FUNCTION;
        dyn->data.fct->ptr  = ptr;
        dyn->data.fct->tp   = type;
        dyn->data.fct->info = NULL;
        dyn->data.fct->params = NULL;

        if (info) {
            if (info[0]) {
                dyn->data.fct->info = (ss_str) malloc( ss_strlen(info)+1 );
                if (dyn->data.fct->info) {
                    ss_strcpy( dyn->data.fct->info, info );
                }
            }
        }

        return 1;
    }

    return 0;
}


ss_char dyn_set_fct_ss(dyn_c* dyn, ss_char* code, ss_ushort length,
                                   ss_str   info, dyn_c* params)
{
    if( dyn_set_fct(dyn, NULL, length, info) ) {
        char* ptr = (ss_char*) malloc(length);

        if (ptr) {
            ss_ushort  i;
            for (i=0; i<length; ++i)
                ptr[i] = code[i];

            dyn_c* params2 = (dyn_c*) malloc(sizeof(dyn_c));
            if (params2) {
                fprintf(stderr, "SSSSSSSSSSSSSSSSSS (%s)\n", dyn_get_string(params));
                DYN_INIT(params2);
                if (dyn_copy(params, params2)) {
                    dyn->data.fct->ptr    = (void*) ptr;
                    dyn->data.fct->params = (void*) params2;
                    return 1;
                }
                free(params2);
            }
            free(ptr);
        }
        dyn->data.fct->tp = 0;
        dyn_fct_free(dyn);
    }

    return 0;
}


ss_char dyn_fct_free(dyn_c* dyn)
{
    if (dyn->data.fct->tp > 1) {
        dyn_free(dyn->data.fct->params);
        free(dyn->data.fct->params);
        free(dyn->data.fct->ptr);
    }

    if (dyn->data.fct->info)
        free(dyn->data.fct->info);

    free(dyn->data.fct);

    return 1;
}

ss_char dyn_fct_copy(dyn_c* dyn, dyn_c* copy)
{
    if (dyn->data.fct->tp <= 1)
        return dyn_set_fct( copy,
                            dyn->data.fct->ptr,
                            dyn->data.fct->tp,
                            dyn->data.fct->info);

    return dyn_set_fct_ss(  copy,
                            (char*)dyn->data.fct->ptr,
                            dyn->data.fct->tp,
                            dyn->data.fct->info,
                            (dyn_c*) dyn->data.fct->params);
}


ss_char* dyn_fct_get_ss (dyn_c* dyn)
{
    return (ss_char*) dyn->data.fct->ptr;
}
