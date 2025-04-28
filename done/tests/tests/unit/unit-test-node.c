#include "error.h"
#include "node.h"
#include "test.h"

#include <check.h>
#include <string.h>

START_TEST(test_init_null_args)
{
    node_t node;
    char ip;

    ck_assert_err(node_init(NULL, &ip, 1234, 1), ERR_INVALID_ARGUMENT);
    ck_assert_err(node_init(&node, NULL, 1234, 1), ERR_INVALID_ARGUMENT);
    ck_assert_err(node_init(NULL, NULL, 1234, 1), ERR_INVALID_ARGUMENT);
}
END_TEST

START_TEST(test_end_null_args)
{
    node_end(NULL);
}
END_TEST

START_TEST(test_node_init_assign_addr)
{
    node_t node;
    const char *ip = "192.168.1.1";

    ck_assert_err_none(node_init(&node, ip, 1234, 1));

    ck_assert_str_eq(ip, node.addr);

    node_end(&node);
}
END_TEST

Suite *node_suite()
{
    Suite *s;
    TCase *tc_core;

    s = suite_create("Node layer");

    Add_Test(s, test_init_null_args);
    Add_Test(s, test_end_null_args);

    Add_Test(s, test_node_init_assign_addr);

    return s;
}

TEST_SUITE(node_suite);
