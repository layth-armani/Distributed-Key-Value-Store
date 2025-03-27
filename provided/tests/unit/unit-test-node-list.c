#include "node.h"
#include "node_list.h"
#include "test.h"

#include <arpa/inet.h>
#include <check.h>
#include <unistd.h>

START_TEST(test_add)
{
    node_list_t list = {0};

    node_t node0 = {0};
    ck_assert_err_none(node_init(&node0, "127.0.0.1", 10000, 0));

    node_t node1 = {0};
    ck_assert_err_none(node_init(&node0, "127.0.0.1", 10001, 1));

    ck_assert_err_none(node_list_add(&list, node0));

    ck_assert_int_eq(list.size, 1);

    ck_assert_err_none(node_list_add(&list, node1));

    ck_assert_int_eq(list.size, 2);

    node_list_free(&list);
}
END_TEST

START_TEST(test_free_null)
{
    // Should not crash
    node_list_free(NULL);
}
END_TEST

Suite *node_list_suite(void)
{
    Suite *s;
    TCase *tc_core;

    s = suite_create("Dynamic node list");
    Add_Test(s, test_add);
    Add_Test(s, test_free_null);

    // TODO week06 : test get_nodes() does proper init

    return s;
}

TEST_SUITE(node_list_suite);
