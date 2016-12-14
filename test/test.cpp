#include "gtest/gtest.h"

extern "C" {
    #include "dynamic.h"
}

TEST(Data, Initaialization){
    char* str;
    dyn_c test;
    DYN_INIT(&test);

    dyn_set_bool(&test, 0);
    ASSERT_EQ(BOOL,  test.type );
    ASSERT_EQ(0,     dyn_get_bool  (&test));
    ASSERT_EQ(0,     dyn_get_int   (&test));
    ASSERT_EQ(0.0,   dyn_get_float (&test));
    str=dyn_get_string(&test);
    ASSERT_STREQ("0", str ); free(str);

    dyn_set_bool(&test, 1);
    ASSERT_EQ(BOOL,   test.type );
    ASSERT_EQ(1,      dyn_get_bool  (&test));
    ASSERT_EQ(1,      dyn_get_int   (&test));
    ASSERT_EQ(1.0,    dyn_get_float (&test));
    str=dyn_get_string(&test);
    ASSERT_STREQ("1", str ); free(str);

    dyn_set_int(&test, 33);
    ASSERT_EQ(INTEGER, test.type );
    ASSERT_EQ(1,       dyn_get_bool  (&test) );
    ASSERT_EQ(33,      dyn_get_int   (&test) );
    ASSERT_EQ(33.0,    dyn_get_float (&test) );
    str=dyn_get_string(&test);
    ASSERT_STREQ("33", str ); free(str);

    dyn_set_float(&test, 33.33);
    ASSERT_EQ(FLOAT,   test.type );
    ASSERT_EQ(1,       dyn_get_bool  (&test) );
    ASSERT_EQ(33,      dyn_get_int   (&test) );
    ASSERT_EQ(33.33f,  dyn_get_float (&test) );
    str=dyn_get_string(&test);
    ASSERT_STREQ("33.330002", str); free(str);


    dyn_set_string(&test, "abc");

    ASSERT_EQ(STRING,   test.type );
    str=dyn_get_string(&test);
    ASSERT_STREQ("abc", str ); free(str);

    dyn_free(&test);
}


TEST(Operation, Arithmetic){
    char * str;
    dyn_c o1;
    DYN_INIT(&o1);
    dyn_set_int(&o1, 33);

    dyn_c o2;
    DYN_INIT(&o2);
    dyn_set_int(&o2, -12);

    dyn_c rslt;
    DYN_INIT(&rslt);

    dyn_op_add(&rslt, &o1, &o2);  ASSERT_EQ(33- 12,     dyn_get_int(&rslt) );
    dyn_op_sub(&rslt, &o1, &o2);  ASSERT_EQ(33+ 12,     dyn_get_int(&rslt) );
    dyn_op_mul(&rslt, &o1, &o2);  ASSERT_EQ(33* -12,    dyn_get_int(&rslt) );
    dyn_op_div(&rslt, &o1, &o2);  ASSERT_EQ(33/ -12,    dyn_get_int(&rslt) );
    dyn_op_mod(&rslt, &o1, &o2);  ASSERT_EQ(33% -12,    dyn_get_int(&rslt) );

    dyn_set_float(&o2, -12.2f);
    dyn_op_add(&rslt, &o1, &o2);  ASSERT_EQ(33- 12.2f,  dyn_get_float(&rslt) );
    dyn_op_sub(&rslt, &o1, &o2);  ASSERT_EQ(33+ 12.2f,  dyn_get_float(&rslt) );
    dyn_op_mul(&rslt, &o1, &o2);  ASSERT_EQ(33* -12.2f, dyn_get_float(&rslt) );
    dyn_op_div(&rslt, &o1, &o2);  ASSERT_EQ(33/ -12.2f, dyn_get_float(&rslt) );
    dyn_op_mod(&rslt, &o1, &o2);  ASSERT_EQ((float)(33% -12), dyn_get_float(&rslt) );

    dyn_set_string(&o1, "abc");
    dyn_set_int(&o2, 3);
    str=dyn_get_string(&o2);
    ASSERT_STREQ("3", str ); free(str);

    dyn_op_add(&rslt, &o1, &o2);
    str=dyn_get_string(&rslt);
    ASSERT_STREQ("abc3", str ); free(str);

    dyn_op_add(&rslt, &o2, &o1);
    str=dyn_get_string(&rslt);
    ASSERT_STREQ("3abc", str ); free(str);

    dyn_op_mul(&rslt, &o1, &o2);
    str=dyn_get_string(&rslt);
    ASSERT_STREQ("abcabcabc", str ); free(str);

    dyn_free(&rslt);
    dyn_free(&o1);
    dyn_free(&o2);
}


TEST(Comparison, Equality){
    dyn_c n;    DYN_INIT(&n);
    dyn_c b;    DYN_INIT(&b);   dyn_set_bool(&b, 1);
    dyn_c i;    DYN_INIT(&i);   dyn_set_int(&i, 33);
    dyn_c f;    DYN_INIT(&f);   dyn_set_float(&f, 33.0);
    dyn_c s;    DYN_INIT(&s);   dyn_set_string(&s, "33");

    dyn_c rslt; DYN_INIT(&rslt);

    dyn_op_eq(&rslt, &b, &b);   ASSERT_EQ(1, dyn_get_bool(&rslt) );
    dyn_op_eq(&rslt, &i, &i);   ASSERT_EQ(1, dyn_get_bool(&rslt) );
    dyn_op_eq(&rslt, &f, &f);   ASSERT_EQ(1, dyn_get_bool(&rslt) );
    dyn_op_eq(&rslt, &s, &s);   ASSERT_EQ(1, dyn_get_bool(&rslt) );

    dyn_op_eq(&rslt, &b, &i);   ASSERT_EQ(0, dyn_get_bool(&rslt) );
    dyn_op_eq(&rslt, &b, &f);   ASSERT_EQ(0, dyn_get_bool(&rslt) );
    dyn_op_eq(&rslt, &b, &s);   ASSERT_EQ(0, dyn_get_bool(&rslt) );

    dyn_op_eq(&rslt, &i, &f);   ASSERT_EQ(1, dyn_get_bool(&rslt) );
    dyn_op_eq(&rslt, &i, &s);   ASSERT_EQ(1, dyn_get_bool(&rslt) );
    dyn_op_eq(&rslt, &f, &s);   ASSERT_EQ(0, dyn_get_bool(&rslt) );


    dyn_free(&f);
    dyn_free(&i);
    dyn_free(&b);
    dyn_free(&s);
    dyn_free(&rslt);
}


TEST(Comparison, UnEquality){
    dyn_c n;  DYN_INIT(&n);
    dyn_c b;  DYN_INIT(&b);     dyn_set_bool(&b, 1);
    dyn_c i;  DYN_INIT(&i);     dyn_set_int(&i, 33);
    dyn_c f;  DYN_INIT(&f);     dyn_set_float(&f, 33.0);
    dyn_c s;  DYN_INIT(&s);     dyn_set_string(&s, "33");

    dyn_c rslt;  DYN_INIT(&rslt);

    dyn_op_ne(&rslt, &b, &b);   ASSERT_EQ(0, dyn_get_bool(&rslt));
    dyn_op_ne(&rslt, &i, &i);   ASSERT_EQ(0, dyn_get_bool(&rslt));
    dyn_op_ne(&rslt, &f, &f);   ASSERT_EQ(0, dyn_get_bool(&rslt));
    dyn_op_ne(&rslt, &s, &s);   ASSERT_EQ(0, dyn_get_bool(&rslt));
    dyn_op_ne(&rslt, &b, &i);   ASSERT_EQ(1, dyn_get_bool(&rslt));
    dyn_op_ne(&rslt, &b, &f);   ASSERT_EQ(1, dyn_get_bool(&rslt));
    dyn_op_ne(&rslt, &b, &s);   ASSERT_EQ(1, dyn_get_bool(&rslt));
    dyn_op_ne(&rslt, &i, &f);   ASSERT_EQ(0, dyn_get_bool(&rslt));
    dyn_op_ne(&rslt, &i, &s);   ASSERT_EQ(0, dyn_get_bool(&rslt));
    dyn_op_ne(&rslt, &f, &s);   ASSERT_EQ(1, dyn_get_bool(&rslt));

    dyn_free(&f);
    dyn_free(&i);
    dyn_free(&b);
    dyn_free(&s);
    dyn_free(&rslt);
}


TEST(Comparison, LessThan){
    dyn_c n;  DYN_INIT(&n);
    dyn_c b;  DYN_INIT(&b);     dyn_set_bool(&b, 1);
    dyn_c i;  DYN_INIT(&i);     dyn_set_int(&i, 33);
    dyn_c f;  DYN_INIT(&f);     dyn_set_float(&f, 33.0);
    dyn_c s;  DYN_INIT(&s);     dyn_set_string(&s, "33");

    dyn_c rslt;  DYN_INIT(&rslt);

    dyn_op_lt(&rslt, &b, &b);   ASSERT_EQ(0, dyn_get_bool(&rslt));
    dyn_op_lt(&rslt, &i, &i);   ASSERT_EQ(0, dyn_get_bool(&rslt));
    dyn_op_lt(&rslt, &f, &f);   ASSERT_EQ(0, dyn_get_bool(&rslt));
    dyn_op_lt(&rslt, &s, &s);   ASSERT_EQ(0, dyn_get_bool(&rslt));
    dyn_op_lt(&rslt, &b, &i);   ASSERT_EQ(1, dyn_get_bool(&rslt));
    dyn_op_lt(&rslt, &b, &f);   ASSERT_EQ(1, dyn_get_bool(&rslt));
    dyn_op_lt(&rslt, &b, &s);   ASSERT_EQ(1, dyn_get_bool(&rslt));
    dyn_op_lt(&rslt, &i, &f);   ASSERT_EQ(0, dyn_get_bool(&rslt));
    dyn_op_lt(&rslt, &i, &s);   ASSERT_EQ(0, dyn_get_bool(&rslt));
    dyn_op_lt(&rslt, &f, &s);   ASSERT_EQ(0, dyn_get_bool(&rslt));

    dyn_free(&f);
    dyn_free(&i);
    dyn_free(&b);
    dyn_free(&s);
    dyn_free(&rslt);
}


TEST(Comparison, GreaterThan){
    dyn_c n;  DYN_INIT(&n);
    dyn_c b;  DYN_INIT(&b);     dyn_set_bool(&b, 1);
    dyn_c i;  DYN_INIT(&i);     dyn_set_int(&i, 33);
    dyn_c f;  DYN_INIT(&f);     dyn_set_float(&f, 33.0);
    dyn_c s;  DYN_INIT(&s);     dyn_set_string(&s, "33");

    dyn_c rslt;  DYN_INIT(&rslt);

    dyn_op_lt(&rslt, &b, &b);   ASSERT_EQ(0, dyn_get_bool(&rslt));
    dyn_op_lt(&rslt, &i, &i);   ASSERT_EQ(0, dyn_get_bool(&rslt));
    dyn_op_lt(&rslt, &f, &f);   ASSERT_EQ(0, dyn_get_bool(&rslt));
    dyn_op_lt(&rslt, &s, &s);   ASSERT_EQ(0, dyn_get_bool(&rslt));
    dyn_op_lt(&rslt, &b, &i);   ASSERT_EQ(1, dyn_get_bool(&rslt));
    dyn_op_lt(&rslt, &b, &f);   ASSERT_EQ(1, dyn_get_bool(&rslt));
    dyn_op_lt(&rslt, &b, &s);   ASSERT_EQ(1, dyn_get_bool(&rslt));
    dyn_op_lt(&rslt, &i, &f);   ASSERT_EQ(0, dyn_get_bool(&rslt));
    dyn_op_lt(&rslt, &i, &s);   ASSERT_EQ(0, dyn_get_bool(&rslt));
    dyn_op_lt(&rslt, &f, &s);   ASSERT_EQ(0, dyn_get_bool(&rslt));

    dyn_free(&f);
    dyn_free(&i);
    dyn_free(&b);
    dyn_free(&s);
    dyn_free(&rslt);
}



TEST(Logic, Bool){
    dyn_c True;
    dyn_c False;
    dyn_c rslt;

    DYN_INIT(&True);    dyn_set_bool(&True, 1);
    DYN_INIT(&False);   dyn_set_bool(&False, 0);

    dyn_op_not(&rslt, &True);           ASSERT_EQ(0, dyn_get_bool(&rslt));
    dyn_op_not(&rslt, &False);          ASSERT_EQ(1, dyn_get_bool(&rslt));

    dyn_op_and(&rslt, &True, &True);    ASSERT_EQ(1, dyn_get_bool(&rslt));
    dyn_op_and(&rslt, &True, &False);   ASSERT_EQ(0, dyn_get_bool(&rslt));
    dyn_op_and(&rslt, &False, &True);   ASSERT_EQ(0, dyn_get_bool(&rslt));
    dyn_op_and(&rslt, &False, &False);  ASSERT_EQ(0, dyn_get_bool(&rslt));

    dyn_op_or(&rslt, &True, &True);     ASSERT_EQ(1, dyn_get_bool(&rslt));
    dyn_op_or(&rslt, &True, &False);    ASSERT_EQ(1, dyn_get_bool(&rslt));
    dyn_op_or(&rslt, &False, &True);    ASSERT_EQ(1, dyn_get_bool(&rslt));
    dyn_op_or(&rslt, &False, &False);   ASSERT_EQ(0, dyn_get_bool(&rslt));

    dyn_op_xor(&rslt, &True, &True);    ASSERT_EQ(0, dyn_get_bool(&rslt));
    dyn_op_xor(&rslt, &True, &False);   ASSERT_EQ(1, dyn_get_bool(&rslt));
    dyn_op_xor(&rslt, &False, &True);   ASSERT_EQ(1, dyn_get_bool(&rslt));
    dyn_op_xor(&rslt, &False, &False);  ASSERT_EQ(0, dyn_get_bool(&rslt));

    dyn_free(&True);
    dyn_free(&False);
    dyn_free(&rslt);
}


TEST(Logic, Unknown){
    dyn_c True;     DYN_INIT(&True);        dyn_set_bool(&True, 1);
    dyn_c False;    DYN_INIT(&False);       dyn_set_bool(&False, 0);
    dyn_c Unknown;  DYN_INIT(&Unknown);
    dyn_c rslt;     DYN_INIT(&rslt);

    dyn_op_not(&rslt, &Unknown);            ASSERT_EQ(-1, dyn_get_bool_3(&rslt));

    dyn_op_and(&rslt, &True, &Unknown);     ASSERT_EQ(-1, dyn_get_bool_3(&rslt));
    dyn_op_and(&rslt, &Unknown, &True);     ASSERT_EQ(-1, dyn_get_bool_3(&rslt));
    dyn_op_and(&rslt, &False, &Unknown);    ASSERT_EQ( 0, dyn_get_bool_3(&rslt));
    dyn_op_and(&rslt, &Unknown, &False);    ASSERT_EQ( 0, dyn_get_bool_3(&rslt));
    dyn_op_and(&rslt, &Unknown, &Unknown);  ASSERT_EQ(-1, dyn_get_bool_3(&rslt));

    dyn_op_or(&rslt, &True, &Unknown);      ASSERT_EQ( 1, dyn_get_bool_3(&rslt));
    dyn_op_or(&rslt, &Unknown, &True);      ASSERT_EQ( 1, dyn_get_bool_3(&rslt));
    dyn_op_or(&rslt, &False, &Unknown);     ASSERT_EQ(-1, dyn_get_bool_3(&rslt));
    dyn_op_or(&rslt, &Unknown, &False);     ASSERT_EQ(-1, dyn_get_bool_3(&rslt));
    dyn_op_or(&rslt, &Unknown, &Unknown);   ASSERT_EQ(-1, dyn_get_bool_3(&rslt));

    dyn_op_xor(&rslt, &True, &Unknown);     ASSERT_EQ(-1, dyn_get_bool_3(&rslt));
    dyn_op_xor(&rslt, &Unknown, &True);     ASSERT_EQ(-1, dyn_get_bool_3(&rslt));
    dyn_op_xor(&rslt, &False, &Unknown);    ASSERT_EQ(-1, dyn_get_bool_3(&rslt));
    dyn_op_xor(&rslt, &Unknown, &False);    ASSERT_EQ(-1, dyn_get_bool_3(&rslt));
    dyn_op_xor(&rslt, &Unknown, &Unknown);  ASSERT_EQ(-1, dyn_get_bool_3(&rslt));

    dyn_free(&True);
    dyn_free(&False);
    dyn_free(&Unknown);
    dyn_free(&rslt);
}



TEST(List, Basic){
    char * str;
    dyn_c o1;
    DYN_INIT(&o1);
    dyn_set_int(&o1, 33);

    dyn_c o2;
    DYN_INIT(&o2);
    DYN_SET_LIST(&o2);

    dyn_list_push(&o2, &o1);
    dyn_list_push(&o2, &o1);
    dyn_set_string(&o1, "123456");
    dyn_list_push(&o2, &o1);

    ASSERT_EQ(3, dyn_length(&o2));

    dyn_copy( &o2, &o1 );
    ASSERT_EQ(3, dyn_length(&o1));

    dyn_list_push(&o2, &o1);

    ASSERT_EQ(4, dyn_length(&o2));

    dyn_list_pop(&o2, &o1);
    ASSERT_EQ(3, dyn_length(&o2));
    ASSERT_EQ(3, dyn_length(&o1));

    dyn_free(&o2);
    dyn_free(&o1);
}

int main(int argc, char **argv) {

    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
