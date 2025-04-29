#include "ring.h"
#include "test.h"

#include <check.h>
#include <string.h>
#include <unistd.h>

static char INITIAL_PATH[4096];
void nope() {}
void restore_path()
{
    int ret = chdir(INITIAL_PATH);
    ck_assert_int_eq(ret, 0); // cannot come back :_-(
}

const char node5_sha[SHA_DIGEST_LENGTH] = {0x11, 0xc3, 0xf8, 0x15, 0x14, 0x63, 0x7c, 0x68, 0xae, 0xf6, 0x49, 0xa1, 0xb9, 0x35, 0x77, 0xba, 0x86, 0x62, 0xc6, 0x99};
const char node3_sha[SHA_DIGEST_LENGTH] = {0x5b, 0x0d, 0x0f, 0x03, 0x6d, 0x57, 0x7a, 0x48, 0x6c, 0x10, 0x1c, 0x15, 0x02, 0x38, 0x16, 0x5f, 0x58, 0x5f, 0xca, 0xfd};
const char node0_sha[SHA_DIGEST_LENGTH] = {0xaa, 0x66, 0xf3, 0xe5, 0xa8, 0xd9, 0xcd, 0xc5, 0xc0, 0xbd, 0x49, 0x70, 0x8b, 0xc5, 0x98, 0x47, 0xe6, 0x91, 0x56, 0x34};
const char node4_sha[SHA_DIGEST_LENGTH] = {0xb2, 0x73, 0xa7, 0x40, 0x03, 0x06, 0x5f, 0x01, 0x12, 0xa1, 0xc3, 0x0a, 0x87, 0x41, 0x6b, 0xa3, 0xe1, 0xa5, 0x02, 0x7b};
const char node1_sha[SHA_DIGEST_LENGTH] = {0xce, 0x5d, 0xa6, 0x04, 0xe4, 0xef, 0x60, 0xdb, 0xe4, 0x50, 0x42, 0xbf, 0xcb, 0x1c, 0x44, 0x5c, 0x7d, 0xb4, 0xec, 0x81};
const char node2_sha[SHA_DIGEST_LENGTH] = {0xf5, 0x3e, 0x1a, 0xd3, 0xc8, 0x53, 0x9a, 0xff, 0x77, 0xce, 0xcb, 0xdc, 0xe7, 0x78, 0x29, 0xff, 0x1e, 0x40, 0x17, 0x0d};
const char node0_coucou[SHA_DIGEST_LENGTH] = {0x5f, 0x26, 0x26, 0x87, 0x54, 0xfc, 0xf2, 0xa5, 0x1f, 0xcf, 0xac, 0xaa, 0xa2, 0xaa, 0xf4, 0xf0, 0xd8, 0x3f, 0x6d, 0x67};
const char node1_coucou[SHA_DIGEST_LENGTH] = {0x91, 0x4f, 0x6a, 0xde, 0x5b, 0x49, 0xa3, 0xa9, 0xbe, 0x25, 0x7f, 0x8a, 0x56, 0xbb, 0xde, 0x9a, 0x83, 0xfa, 0x46, 0xaa};
const char node2_coucou[SHA_DIGEST_LENGTH] = {0xa9, 0x02, 0xe3, 0xa5, 0xaa, 0x4f, 0x73, 0x15, 0x0f, 0x45, 0x94, 0x36, 0xb0, 0x58, 0x0c, 0xb7, 0xad, 0x72, 0xb5, 0x66};

/**
    5 - 8.8.8.8      - 0x11c3
    3 - 8.8.8.8      - 0x5b0d
    0 - 127.0.0.1    - 0xaa66
    4 - 8.8.8.8      - 0xb273
    1 - 192.168.1.10 - 0xce5d
    2 - 192.168.1.10 - 0xf53e
 */

START_TEST(test_ring_get_all)
{
    ring_t ring = {0};

    int ret = chdir(DATA_DIR "/servers-valid");
    ck_assert_int_eq(ret, 0); // data directory does not exist :-(
    ck_assert_err_none(ring_init(&ring));

    node_list_t list = {0};
    ck_assert_err_none(ring_get_nodes_for_key(&ring, &list, 10, "my-key"));

    ck_assert_int_eq(list.size, 3);

    ck_assert_node_eq(list.nodes[0], "127.0.0.1", 1234, node0_sha);
    ck_assert_node_eq(list.nodes[1], "8.8.8.8", 1236, node4_sha);
    ck_assert_node_eq(list.nodes[2], "192.168.1.10", 1235, node1_sha);

    node_list_free(&list);
    ring_free(&ring);
}
END_TEST

START_TEST(test_ring_get_stops_at_wanted)
{
    ring_t ring = {0};

    int ret = chdir(DATA_DIR "/servers-valid");
    ck_assert_int_eq(ret, 0); // data directory does not exist :-(
    ck_assert_err_none(ring_init(&ring));

    node_list_t list = {0};
    ck_assert_err_none(ring_get_nodes_for_key(&ring, &list, 1, "my-key"));

    ck_assert_int_eq(list.size, 1);

    ck_assert_node_eq(list.nodes[0], "127.0.0.1", 1234, node0_sha);

    node_list_free(&list);
    ring_free(&ring);
}
END_TEST

START_TEST(test_ring_get_skips_matched_server)
{
    ring_t ring = {0};

    int ret = chdir(DATA_DIR "/servers-valid");
    ck_assert_int_eq(ret, 0); // data directory does not exist :-(
    ck_assert_err_none(ring_init(&ring));

    node_list_t list = {0};
    ck_assert_err_none(ring_get_nodes_for_key(&ring, &list, 10, "my_key"));

    ck_assert_int_eq(list.size, 3);

    ck_assert_node_eq(list.nodes[0], "8.8.8.8", 1236, node5_sha);
    ck_assert_node_eq(list.nodes[1], "127.0.0.1", 1234, node0_sha);
    ck_assert_node_eq(list.nodes[2], "192.168.1.10", 1235, node1_sha);

    node_list_free(&list);
    ring_free(&ring);
}
END_TEST

START_TEST(test_ring_get_skips_matched_coucou)
{
    ring_t ring = {0};

    int ret = chdir(DATA_DIR "/servers-coucou");
    ck_assert_int_eq(ret, 0); // data directory does not exist :-(
    ck_assert_err_none(ring_init(&ring));

    node_list_t list = {0};
    ck_assert_err_none(ring_get_nodes_for_key(&ring, &list, 10, "coucou"));
    node_list_print(&list);
    
    ck_assert_int_eq(list.size, 3);

    ck_assert_node_eq(list.nodes[0], "127.0.0.1", 1236, node0_coucou);
    ck_assert_node_eq(list.nodes[1], "127.0.0.1", 1235, node1_coucou);
    ck_assert_node_eq(list.nodes[2], "127.0.0.1", 1234, node2_coucou);

    node_list_free(&list);
    ring_free(&ring);
}
END_TEST

Suite *ring_suite()
{
    Suite *s;
    TCase *tc_core;

    getcwd(INITIAL_PATH, 4096);

    s = suite_create("Ring layer");
    Add_Test_With_Fixture(s, test_ring_get_all, nope, restore_path);
    Add_Test_With_Fixture(s, test_ring_get_stops_at_wanted, nope, restore_path);
    Add_Test_With_Fixture(s, test_ring_get_skips_matched_server, nope, restore_path);
    Add_Test_With_Fixture(s, test_ring_get_skips_matched_coucou, nope, restore_path);

    return s;
}

TEST_SUITE(ring_suite);
