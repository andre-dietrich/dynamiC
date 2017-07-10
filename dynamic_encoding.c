#include "dynamic_encoding.h"

dyn_ushort dyn_encoding_length(const dyn_c *dyn)
{
    dyn_ushort bytes = 1; // OP-CODE one byte
    dyn_int i = 0;

START:


    switch (DYN_TYPE(dyn)) {
        case INTEGER: {
            i = dyn_get_int(dyn);
            bytes = (i > -128 && i < 127) ? 2          // char
                    : ((i > -32768 && i < 32767) ? 3   // short int
                       : 5);                           // int
            break;
        }
        case FLOAT:
            bytes = 5;
            break;
        case STRING:
            bytes += dyn_strlen(dyn->data.str);
            break;

        case SET:
        case LIST: {
            i = DYN_LIST_LEN(dyn);
            while (--i) {
                bytes += dyn_encoding_length( DYN_LIST_GET_REF(dyn, i) );
            }
            bytes += 2;
            break;
        }
        case DICT: {
            i = DYN_DICT_LEN(dyn);
            while (--i) {
                bytes += dyn_encoding_length( DYN_DICT_GET_I_REF(dyn, i) );
                bytes += dyn_strlen( DYN_DICT_GET_I_KEY(dyn, i) );
            }
            bytes += 2;
            break;
        }
        case FUNCTION: {
            bytes += dyn_strlen(dyn->data.fct->info)+2; // string-info + 1 byte for type info

            // C-function
            bytes += (dyn->data.fct->type < 2) ? sizeof(void*) : dyn->data.fct->type;
            break;
        }
        case REFERENCE2:
        case REFERENCE: dyn=dyn->data.ref;
                        goto START;
  }

  return bytes; //
}

dyn_char*  copy_buffer(dyn_char* from, dyn_char* to, dyn_ushort len)
{
    while (--len) {
        *to++ = *from++;
    }

    return to;
}

dyn_char*  dyn_encode(dyn_char *to, const dyn_c *from)
{
    dyn_ushort i=0;

START:
    switch (DYN_TYPE(from)) {
        case NONE:
            *to++ = ENC_NONE;
            break;

        case BOOL:
            *to++ = from->data.b ? ENC_TRUE
                                 : ENC_FALSE;
            break;

        case INTEGER: {
            if (from->data.i > -128 && from->data.i < 127) {
                *to++ = ENC_INT1;
                *to++ = (char) from->data.i;
            }
            else if (from->data.i > -32768 && from->data.i < 32767) {
                *to++ = ENC_INT2;
                to = copy_buffer(((char *)&from->data.i), to, 3);
            }
            else {
                *to++ = ENC_INT4;
                to = copy_buffer(((char *)&from->data.i), to, 5);
            }
            break;
        }

        case FLOAT:   *to++ = ENC_FLOAT;
                      to = copy_buffer((char *)&from->data.f, to, 5);
                      break;

        case STRING:  *to++ = ENC_STRING;
                      to = copy_buffer(from->data.str, to, dyn_string_len(from)+1);
                      break;

        case SET:
        case LIST:    {
            for(; i<DYN_LIST_LEN(from); ++i) {
                to = dyn_encode(to, DYN_LIST_GET_REF(from, i));
            }
            *to++ = DYN_TYPE(from) == LIST
                    ? ENC_LIST
                    : ENC_SET;

            to = copy_buffer((char *)&i, to, 3);
            break;
        }
        case DICT:    {
            for(; i<DYN_LIST_LEN(from); ++i) {
                to = dyn_encode(to, DYN_DICT_GET_I_REF(from, i));
            }

            *to++ = ENC_DICT;
            to = copy_buffer((char *)&i, to, 3);
//            for(i=0; i<DYN_LIST_LEN(from); ++i) {
//                to = copy_buffer( DYN_DICT_GET_I_KEY(from, i),
//                                  to,
  //                                3);
//                to = dyn_encode(to, );
//            }
            //to = copy_buffer(from->data.str, to, dyn_string_len(from)+1);

        }
        case REFERENCE2:
        case REFERENCE: from=from->data.ref;
                        goto START;
    }

    *to = ENC_HALT;

    return to;
}

dyn_char* dyn_decode_all(dyn_char *from, dyn_c *to)
{
    dyn_c tmp;
    DYN_INIT(&tmp);
    dyn_set_list_len(&tmp, 2);

    dyn_char code = ENC_TRUE;

    while (code != ENC_HALT) {
       code = *from++;
       from = dyn_decode(code, from, &tmp);
    }

    dyn_move(DYN_LIST_GET_REF(&tmp, 0), to);
    dyn_free(&tmp);
    return from;
}

dyn_char* dyn_decode(dyn_char code, dyn_char *from, dyn_c *to)
{
    if (code == ENC_HALT)
      return from;

    //dyn_char code;

    dyn_c tmp;
    DYN_INIT(&tmp);

    switch (code) {
        case ENC_TRUE:
        case ENC_FALSE:
            //dyn_set_bool(&tmp, code ? 0 : 1);
            dyn_set_bool(&tmp, code == ENC_TRUE ? 1 : 0);
            break;
        case ENC_INT1:
        case ENC_INT2:
        case ENC_INT4:
            dyn_set_int(&tmp, code == ENC_INT1
                              ? *((dyn_char*) from)
                              : ( code == ENC_INT2
                                  ? *((dyn_short*) from )
                                  : *((dyn_int*) from)));

            from += (code == ENC_INT1)
                    ? 1
                    : ( code == ENC_INT2
                        ? 2
                        : 4 );
            break;

        case ENC_FLOAT:
            dyn_set_float(&tmp, *((dyn_float*) from));
            from += 4;
            break;

        case ENC_STRING:
            dyn_set_string(&tmp, from);
            from += dyn_string_len(&tmp)+1;
            break;

        case ENC_SET:
        case ENC_LIST: {
            dyn_ushort len = *((dyn_ushort*) from);
            from += 2;
            dyn_ushort i = len + 1;

            if (code == ENC_LIST) {
                dyn_set_list_len(&tmp, len);
                while (--i)
                    dyn_move( DYN_LIST_GET_REF_END(to, i),
                              dyn_list_push_none(&tmp));
            } else { // SET
                dyn_set_set_len(&tmp, len);
                while (--i)
                    dyn_set_insert(&tmp, DYN_LIST_GET_REF_END(to, i));
            }

            dyn_list_popi(to, len);
        }
        //case DYN_ENC_DICT:

    }

    dyn_move( &tmp, dyn_list_push_none(to));

    return from;
}
