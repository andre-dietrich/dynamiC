#include "gtest/gtest.h"

#include "test_defs.h"

#define DYN_TEST_BEGIN dyn_c rslt; DYN_INIT(&rslt); char* string = NULL
#define DYN_TEST_END   dyn_free(&rslt); \
        if (string) free(string);

#define DYN_TEST_1(X, RSLT, RSLT_FCT) \
        dyn_set_ref(&rslt, X); \
        DYN_TEST_FCT(&rslt); \
        ASSERT_EQ(RSLT, RSLT_FCT(&rslt))

#define DYN_TEST_2(X, Y, RSLT, RSLT_FCT) \
        dyn_set_ref(&rslt, X); \
        DYN_TEST_FCT(&rslt, Y); \
        ASSERT_EQ(RSLT, RSLT_FCT(&rslt))

#define DYN_TEST_2_STRING(X, Y, RSLT) \
        dyn_set_ref(&rslt, X); \
        DYN_TEST_FCT(&rslt, Y); \
        string = dyn_get_string(&rslt); \
        ASSERT_STREQ(RSLT, string); \
        free(string); \
        string = NULL


TEST(Operations_Arithmetic, Negation){
    #define DYN_TEST_FCT dyn_op_neg
    DYN_TEST_BEGIN;

    DYN_TEST_1(&None_,    DYN_NONE,   dyn_get_bool_3);
    DYN_TEST_1(&False_,   DYN_TRUE,   dyn_get_bool);
    DYN_TEST_1(&True_,    DYN_FALSE,  dyn_get_bool);
    DYN_TEST_1(&Int_0,    0,          dyn_get_int);
    DYN_TEST_1(&Int_1,    -1,         dyn_get_int);
    DYN_TEST_1(&Int_n22,  22,         dyn_get_int);
    DYN_TEST_1(&Float_0,  0.0,        dyn_get_float);
    DYN_TEST_1(&Float_n22,22.0,       dyn_get_float);
    DYN_TEST_1(&Float_n22_222, (dyn_float)22.222, dyn_get_float);

    DYN_TEST_1(&Str_abc, NONE, dyn_type);
    DYN_TEST_1(&List_0_1_n22, NONE, dyn_type);
    DYN_TEST_1(&Set_0_1_n22, NONE, dyn_type);

    DYN_TEST_END;
    #undef DYN_TEST_FCT
}

TEST(Operations_Arithmetic, Addition){

    #define DYN_TEST_FCT dyn_op_add
    DYN_TEST_BEGIN;

    DYN_TEST_2(&True_,  &True_,     2,      dyn_get_int);
    DYN_TEST_2(&False_, &False_,    0,      dyn_get_int);
    DYN_TEST_2(&Int_1,  &True_,     2,      dyn_get_int);
    DYN_TEST_2(&Int_1,  &Int_12,    13,     dyn_get_int);
    DYN_TEST_2(&Int_1,  &Int_n22,   -21,    dyn_get_int);
    DYN_TEST_2(&Int_1,  &Float_12,  13.0,   dyn_get_int);

    DYN_TEST_2_STRING(&Str_,    &Int_12,  "12");
    DYN_TEST_2_STRING(&Int_n22,  &Str_,   "-22");
    DYN_TEST_2_STRING(&Str_22,  &Int_12,  "2212");
    DYN_TEST_2_STRING(&Int_n22, &Str_abc, "-22abc");

    DYN_TEST_2(&List_,        &Str_abc,       1, dyn_length);
    DYN_TEST_2(&Str_abc,      &List_,         1, dyn_length);
    DYN_TEST_2(&Str_abc,      &List_0_1_n22,  4, dyn_length);
    DYN_TEST_2(&List_0_1_n22, &Str_abc,       4, dyn_length);

    DYN_TEST_2(&List_0_1_n22, &List_0_1_n22,  4, dyn_length);

    DYN_TEST_END;
    #undef DYN_TEST_FCT
}


TEST(Operations_Logical, AND){
    #define DYN_TEST_FCT dyn_op_and
    DYN_TEST_BEGIN;

    DYN_TEST_2(&None_,  &None_,   DYN_NONE,   dyn_get_bool_3);
    DYN_TEST_2(&None_,  &True_,   DYN_NONE,   dyn_get_bool_3);
    DYN_TEST_2(&True_,  &None_,   DYN_NONE,   dyn_get_bool_3);
    DYN_TEST_2(&None_,  &False_,  DYN_FALSE,  dyn_get_bool_3);
    DYN_TEST_2(&False_, &None_,   DYN_FALSE,  dyn_get_bool_3);
    DYN_TEST_2(&True_,  &False_,  DYN_FALSE,  dyn_get_bool_3);
    DYN_TEST_2(&False_, &True_,   DYN_FALSE,  dyn_get_bool_3);
    DYN_TEST_2(&False_, &False_,  DYN_FALSE,  dyn_get_bool_3);
    DYN_TEST_2(&True_,  &True_,   DYN_TRUE,   dyn_get_bool_3);

    DYN_TEST_END;
    #undef DYN_TEST_FCT
}

TEST(Operations_Logical, NOT){

    #define DYN_TEST_FCT dyn_op_not
    DYN_TEST_BEGIN;

    DYN_TEST_1(&None_,  DYN_NONE,   dyn_get_bool_3);
    DYN_TEST_1(&True_,  DYN_FALSE,  dyn_get_bool_3);
    DYN_TEST_1(&False_, DYN_TRUE,   dyn_get_bool_3);

    DYN_TEST_END;
    #undef DYN_TEST_FCT
}

TEST(Operations_Logical, OR){

    #define DYN_TEST_FCT dyn_op_or
    DYN_TEST_BEGIN;

    DYN_TEST_2(&None_,  &None_,   DYN_NONE,   dyn_get_bool_3);
    DYN_TEST_2(&None_,  &True_,   DYN_TRUE,   dyn_get_bool_3);
    DYN_TEST_2(&True_,  &None_,   DYN_TRUE,   dyn_get_bool_3);
    DYN_TEST_2(&None_,  &False_,  DYN_NONE,   dyn_get_bool_3);
    DYN_TEST_2(&False_, &None_,   DYN_NONE,   dyn_get_bool_3);
    DYN_TEST_2(&True_,  &False_,  DYN_TRUE,   dyn_get_bool_3);
    DYN_TEST_2(&False_, &True_,   DYN_TRUE,   dyn_get_bool_3);
    DYN_TEST_2(&True_,  &True_,   DYN_TRUE,   dyn_get_bool_3);
    DYN_TEST_2(&False_, &False_,  DYN_FALSE,  dyn_get_bool_3);

    DYN_TEST_END;
    #undef DYN_TEST_FCT
}

TEST(Operations_Logical, XOR){

    #define DYN_TEST_FCT dyn_op_xor
    DYN_TEST_BEGIN;

    DYN_TEST_2(&None_,  &None_,   DYN_NONE,   dyn_get_bool_3);
    DYN_TEST_2(&None_,  &True_,   DYN_NONE,   dyn_get_bool_3);
    DYN_TEST_2(&True_,  &None_,   DYN_NONE,   dyn_get_bool_3);
    DYN_TEST_2(&None_,  &False_,  DYN_NONE,   dyn_get_bool_3);
    DYN_TEST_2(&False_, &None_,   DYN_NONE,   dyn_get_bool_3);
    DYN_TEST_2(&True_,  &False_,  DYN_TRUE,   dyn_get_bool_3);
    DYN_TEST_2(&False_, &True_,   DYN_TRUE,   dyn_get_bool_3);
    DYN_TEST_2(&True_,  &True_,   DYN_FALSE,  dyn_get_bool_3);
    DYN_TEST_2(&False_, &False_,  DYN_FALSE,  dyn_get_bool_3);

    DYN_TEST_END;
    #undef DYN_TEST_FCT
}



int main(int argc, char **argv) {

    test_param_init();

    testing::InitGoogleTest(&argc, argv);

    int rslt = RUN_ALL_TESTS();

    test_params_free();

    return rslt;
}
