#include "gtest/gtest.h"

extern "C" {
    #include "dynamic.h"
}

TEST(String, Basic){
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
