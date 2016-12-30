#ifndef TEST_DYN_C_H
#define TEST_DYN_C_H

extern "C" {
    #include "dynamic.h"
}


dyn_c None_;
dyn_c True_;
dyn_c False_;

dyn_c Int_0;
dyn_c Int_1;
dyn_c Int_12;
dyn_c Int_n22;

dyn_c Float_0;
dyn_c Float_12;
dyn_c Float_n22;
dyn_c Float_n22_222;

dyn_c Str_;
dyn_c Str_22;
dyn_c Str_abc;
dyn_c Str_WXYZ;

dyn_c List_;
dyn_c List_0_1_n22;
dyn_c List_abc_0_False_abc;

dyn_c Set_;
dyn_c Set_0_1_n22;
dyn_c Set_a_1_99_x;

dyn_c Dict_;
dyn_c Dict_a1;
dyn_c Dict_a1_b2_c3;



void test_param_init() {
    DYN_INIT(&None_);

    DYN_INIT(&True_);
    dyn_set_bool(&True_, DYN_TRUE);
    DYN_INIT(&False_);
    dyn_set_bool(&False_, DYN_FALSE);

    DYN_INIT(&Int_0);
    dyn_set_int(&Int_0, 0);
    DYN_INIT(&Int_1);
    dyn_set_int(&Int_1, 1);
    DYN_INIT(&Int_12);
    dyn_set_int(&Int_12, 12);
    DYN_INIT(&Int_n22);
    dyn_set_int(&Int_n22, -22);

    DYN_INIT(&Float_0);
    dyn_set_float(&Float_0, 0.0);
    DYN_INIT(&Float_12);
    dyn_set_float(&Float_12, 12.0);
    DYN_INIT(&Float_n22);
    dyn_set_float(&Float_n22, -22.0);
    DYN_INIT(&Float_n22_222);
    dyn_set_float(&Float_n22_222, -22.222);

    DYN_INIT(&Str_);
    dyn_set_string(&Str_, "");
    DYN_INIT(&Str_22);
    dyn_set_string(&Str_22, "22");
    DYN_INIT(&Str_abc);
    dyn_set_string(&Str_abc, "abc");
    DYN_INIT(&Str_WXYZ);
    dyn_set_string(&Str_WXYZ, "WXYZ");

    DYN_INIT(&List_);
    dyn_set_list_len(&List_, 1);

    DYN_INIT(&List_0_1_n22);
    dyn_set_list_len(&List_0_1_n22, 3);
    dyn_list_push(&List_0_1_n22, &Int_0);
    dyn_list_push(&List_0_1_n22, &Int_1);
    dyn_list_push(&List_0_1_n22, &Int_n22);


    DYN_INIT(&List_abc_0_False_abc);
    dyn_set_list_len(&List_abc_0_False_abc, 4);
    dyn_list_push(&List_abc_0_False_abc, &Str_abc);
    dyn_list_push(&List_abc_0_False_abc, &Int_0);
    dyn_list_push(&List_abc_0_False_abc, &False_);
    dyn_list_push(&List_abc_0_False_abc, &Str_abc);

    DYN_INIT(&Set_);
    dyn_set_set_len(&Set_, 1);

    DYN_INIT(&Set_0_1_n22);
    dyn_set_list_len(&Set_0_1_n22, 3);
    dyn_set_insert(&Set_0_1_n22, &Int_0);
    dyn_set_insert(&Set_0_1_n22, &Int_1);
    dyn_set_insert(&Set_0_1_n22, &Int_n22);

    // todo


    DYN_INIT(&Dict_);
    dyn_set_dict(&Dict_, 1);

    DYN_INIT(&Dict_a1);
    dyn_set_dict(&Dict_a1, 1);
    dyn_dict_insert(&Dict_a1, (dyn_str)"a", &Int_1);

    // todo
};

void test_params_free() {
    dyn_free(&None_);

    dyn_free(&True_);
    dyn_free(&False_);

    dyn_free(&Int_0);
    dyn_free(&Int_1);
    dyn_free(&Int_12);
    dyn_free(&Int_n22);

    dyn_free(&Float_0);
    dyn_free(&Float_12);
    dyn_free(&Float_n22);
    dyn_free(&Float_n22_222);

    dyn_free(&Str_);
    dyn_free(&Str_22);
    dyn_free(&Str_abc);
    dyn_free(&Str_WXYZ);

    dyn_free(&List_);
    dyn_free(&List_0_1_n22);
    dyn_free(&List_abc_0_False_abc);

    dyn_free(&Set_);
    dyn_free(&Set_0_1_n22);

    // todo


    dyn_free(&Dict_);
    dyn_free(&Dict_a1);

    // todo
};

#endif
