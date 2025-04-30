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

START_TEST(test_cmp_sha_null_args)
{
    node_t node;

    // Output is undefined, so not checked
    node_cmp_sha(&node, NULL);
    node_cmp_sha(NULL, &node);
    node_cmp_sha(NULL, NULL);
}
END_TEST

START_TEST(test_invalid_ip)
{
    node_t node;

    ck_assert_err(node_init(&node, "1234", 1234, 1234), ERR_ADDRESS);
}
END_TEST

START_TEST(test_init_sha)
{
    node_t node;
    char sha[SHA_DIGEST_LENGTH] = {0xdc, 0x0e, 0xa8, 0xc3, 0xcc, 0x5f, 0xd6, 0xfc, 0x6e, 0xee, 0xec, 0x10, 0xc0, 0xc1, 0xac, 0xb7, 0x6d, 0x4d, 0x6f, 0x98};

    ck_assert_err_none(node_init(&node, "1.12.123.234", 1234, 1234));

    ck_assert_node_eq(node, "1.12.123.234", 1234, sha);

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

    Add_Test(s, test_cmp_sha_null_args);
    Add_Test(s, test_invalid_ip);
    Add_Test(s, test_init_sha);

    return s;
}

TEST_SUITE(node_suite);
