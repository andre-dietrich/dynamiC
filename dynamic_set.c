#include "dynamic.h"

#ifdef S2_SET
ss_char dyn_set_set_len (dyn_c* set, ss_ushort len)
{
    if (dyn_set_list_len(set, len)) {
        set->type = SET;
        return 1;
    }
    return 0;
}

ss_char dyn_set_insert (dyn_c* set, dyn_c* element)
{
    dyn_c rslt;
    DYN_INIT(&rslt);
    dyn_set_ref(&rslt, element);

    if (dyn_op_in(&rslt, set)) {
        if (!dyn_get_bool(&rslt)) {
            dyn_list_push(set, element);
            return 1;
        }
    }

    return 0;
}

#endif
