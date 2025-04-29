#include "node.h"
#include "node_list.h"
#include "test.h"

#include <arpa/inet.h>
#include <check.h>
#include <unistd.h>

static char INITIAL_PATH[4096];
void nope() {}
void restore_path()
{
    int ret = chdir(INITIAL_PATH);
    ck_assert_int_eq(ret, 0); // cannot come back :_-(
}

START_TEST(test_add)
{
    node_list_t list = {0};

    node_t node0 = {0};
    ck_assert_err_none(node_init(&node0, "127.0.0.1", 10000, 0));

    node_t node1 = {0};
    ck_assert_err_none(node_init(&node1, "127.0.0.1", 10001, 1));

    ck_assert_err_none(node_list_add(&list, node0));
    ck_assert_int_eq(list.size, 1);

    ck_assert_err_none(node_list_add(&list, node1));
    ck_assert_int_eq(list.size, 2);

    node_end(&node0);
    node_end(&node1);

    node_list_free(&list);
}
END_TEST

START_TEST(test_free_null)
{
    // Should not crash
    node_list_free(NULL);
}
END_TEST

int comp(const node_t *a, const node_t *b)
{
    return a->sha[0] - b->sha[0];
}

START_TEST(test_sort)
{
    node_list_t list = {0};

    node_t node0 = {0};
    ck_assert_err_none(node_init(&node0, "127.0.0.1", 10000, 0));
    // For this test we don't care about the SHA computing, so we override the sha with a simple value.
    memset(node0.sha, 0, SHA_DIGEST_LENGTH);

    node_t node1 = {0};
    ck_assert_err_none(node_init(&node1, "127.0.0.1", 10001, 1));
    memset(node1.sha, 1, SHA_DIGEST_LENGTH);

    node_t node2 = {0};
    ck_assert_err_none(node_init(&node2, "127.0.0.1", 10002, 2));
    memset(node2.sha, 2, SHA_DIGEST_LENGTH);

    ck_assert_err_none(node_list_add(&list, node1));
    ck_assert_err_none(node_list_add(&list, node0));
    ck_assert_err_none(node_list_add(&list, node2));

    node_list_sort(&list, comp);

    ck_assert_two_nodes_eq(list.nodes[0], node0);
    ck_assert_two_nodes_eq(list.nodes[1], node1);
    ck_assert_two_nodes_eq(list.nodes[2], node2);

    node_end(&node0);
    node_end(&node1);
    node_end(&node2);

    node_list_free(&list);
}
END_TEST

START_TEST(test_get_single_nodes)
{
    node_list_t list = {0};

    int ret = chdir(DATA_DIR "/servers-valid-single");;
    ck_assert_int_eq(ret, 0); // data directory does not exist :-(
    ck_assert_err_none(get_nodes(&list));

    ck_assert_int_eq(list.size, 3);

    const char node0_sha[SHA_DIGEST_LENGTH] = {0xaa, 0x66, 0xf3, 0xe5, 0xa8, 0xd9, 0xcd, 0xc5, 0xc0, 0xbd, 0x49, 0x70, 0x8b, 0xc5, 0x98, 0x47, 0xe6, 0x91, 0x56, 0x34};
    const char node1_sha[SHA_DIGEST_LENGTH] = {0xce, 0x5d, 0xa6, 0x04, 0xe4, 0xef, 0x60, 0xdb, 0xe4, 0x50, 0x42, 0xbf, 0xcb, 0x1c, 0x44, 0x5c, 0x7d, 0xb4, 0xec, 0x81};
    const char node2_sha[SHA_DIGEST_LENGTH] = {0x5b, 0x0d, 0x0f, 0x03, 0x6d, 0x57, 0x7a, 0x48, 0x6c, 0x10, 0x1c, 0x15, 0x02, 0x38, 0x16, 0x5f, 0x58, 0x5f, 0xca, 0xfd};

    ck_assert_node_eq(list.nodes[0], "127.0.0.1", 1234, node0_sha);
    ck_assert_node_eq(list.nodes[1], "192.168.1.10", 1235, node1_sha);
    ck_assert_node_eq(list.nodes[2], "8.8.8.8", 1236, node2_sha);

    node_list_free(&list);
}
END_TEST

START_TEST(test_get_multiple_nodes)
{
    node_list_t list = {0};

    int ret = chdir(DATA_DIR "/servers-valid");
    ck_assert_int_eq(ret, 0); // data directory does not exist :-(
    ck_assert_err_none(get_nodes(&list));

    ck_assert_int_eq(list.size, 6);

    const char node0_sha[SHA_DIGEST_LENGTH] = {0xaa, 0x66, 0xf3, 0xe5, 0xa8, 0xd9, 0xcd, 0xc5, 0xc0, 0xbd, 0x49, 0x70, 0x8b, 0xc5, 0x98, 0x47, 0xe6, 0x91, 0x56, 0x34};
    const char node1_sha[SHA_DIGEST_LENGTH] = {0xce, 0x5d, 0xa6, 0x04, 0xe4, 0xef, 0x60, 0xdb, 0xe4, 0x50, 0x42, 0xbf, 0xcb, 0x1c, 0x44, 0x5c, 0x7d, 0xb4, 0xec, 0x81};
    const char node2_sha[SHA_DIGEST_LENGTH] = {0xf5, 0x3e, 0x1a, 0xd3, 0xc8, 0x53, 0x9a, 0xff, 0x77, 0xce, 0xcb, 0xdc, 0xe7, 0x78, 0x29, 0xff, 0x1e, 0x40, 0x17, 0x0d};
    const char node3_sha[SHA_DIGEST_LENGTH] = {0x5b, 0x0d, 0x0f, 0x03, 0x6d, 0x57, 0x7a, 0x48, 0x6c, 0x10, 0x1c, 0x15, 0x02, 0x38, 0x16, 0x5f, 0x58, 0x5f, 0xca, 0xfd};
    const char node4_sha[SHA_DIGEST_LENGTH] = {0xb2, 0x73, 0xa7, 0x40, 0x03, 0x06, 0x5f, 0x01, 0x12, 0xa1, 0xc3, 0x0a, 0x87, 0x41, 0x6b, 0xa3, 0xe1, 0xa5, 0x02, 0x7b};
    const char node5_sha[SHA_DIGEST_LENGTH] = {0x11, 0xc3, 0xf8, 0x15, 0x14, 0x63, 0x7c, 0x68, 0xae, 0xf6, 0x49, 0xa1, 0xb9, 0x35, 0x77, 0xba, 0x86, 0x62, 0xc6, 0x99};

    ck_assert_node_eq(list.nodes[0], "127.0.0.1", 1234, node0_sha);
    ck_assert_node_eq(list.nodes[1], "192.168.1.10", 1235, node1_sha);
    ck_assert_node_eq(list.nodes[2], "192.168.1.10", 1235, node2_sha);
    ck_assert_node_eq(list.nodes[3], "8.8.8.8", 1236, node3_sha);
    ck_assert_node_eq(list.nodes[4], "8.8.8.8", 1236, node4_sha);
    ck_assert_node_eq(list.nodes[5], "8.8.8.8", 1236, node5_sha);

    node_list_free(&list);
}
END_TEST

START_TEST(test_getnodes_count_must_be_positive)
{
    node_list_t list = {0};

    int ret = chdir(DATA_DIR "/servers-invalid-count2");
    ck_assert_int_eq(ret, 0); // data directory does not exist :-(
    ck_assert_err(get_nodes(&list), ERR_INVALID_CONFIG);
}
END_TEST

Suite *node_list_suite(void)
{
    Suite *s;
    TCase *tc_core;

    getcwd(INITIAL_PATH, 4096);

    s = suite_create("Dynamic node list");
    Add_Test(s, test_add);
    Add_Test(s, test_free_null);

    Add_Test(s, test_sort);

    Add_Test_With_Fixture(s, test_get_single_nodes, nope, restore_path);
    Add_Test_With_Fixture(s, test_get_multiple_nodes, nope, restore_path);
    Add_Test_With_Fixture(s, test_getnodes_count_must_be_positive, nope, restore_path);

    return s;
}

TEST_SUITE(node_list_suite);
