#include "args.h"
#include "client.h"
#include "error.h"
#include "test.h"

#include <check.h>
#include <stdlib.h>

START_TEST(test_init_null_args)
{
    client_t c = {0};
    int argc = 0;
    char *rem_args[] = {NULL};
    char **args = rem_args;

    

    ck_assert_err(client_init(NULL, 0, &argc, &args), ERR_INVALID_ARGUMENT);
    ck_assert_err(client_init(&c, 0, NULL, &args), ERR_INVALID_ARGUMENT);
    ck_assert_err(client_init(&c, 0, &argc, NULL), ERR_INVALID_ARGUMENT);

    ck_assert_err(client_init(NULL, 0, NULL, &args), ERR_INVALID_ARGUMENT);
    ck_assert_err(client_init(&c, 0, NULL, NULL), ERR_INVALID_ARGUMENT);
    ck_assert_err(client_init(NULL, 0, &argc, NULL), ERR_INVALID_ARGUMENT);

    ck_assert_err(client_init(NULL, 0, NULL, NULL), ERR_INVALID_ARGUMENT);
}
END_TEST

START_TEST(test_end_null_args)
{
    client_end(NULL);
}
END_TEST

START_TEST(test_empty_yields_defaults)
{
    client_t c = {0};
    int argc = 0;
    char *rem_args[] = {NULL};
    char **args = rem_args;

    ck_assert_err_none(client_init(&c, 0, &argc, &args));

    ck_assert_int_eq(c.args.get_needed, 2);
    ck_assert_int_eq(c.args.put_needed, 2);
    ck_assert_int_eq(c.args.total_servers, 3);

    client_end(&c);
}
END_TEST

START_TEST(test_all_args)
{
    client_t c = {0};
    int argc = 6;
    char *rem_args[] = {"-w", "5", "-r", "5", "-n", "9", NULL};
    char **args = rem_args;

    ck_assert_err_none(client_init(&c, GET_NEEDED | PUT_NEEDED | TOTAL_SERVERS, &argc, &args));

    ck_assert_int_eq(c.args.get_needed, 5);
    ck_assert_int_eq(c.args.put_needed, 5);
    ck_assert_int_eq(c.args.total_servers, 9);

    ck_assert_int_eq(argc, 0);
    ck_assert_ptr_eq(args, &rem_args[6]);

    client_end(&c);
}
END_TEST

START_TEST(test_W_implies_N)
{
    client_t c = {0};
    int argc = 2;
    char *rem_args[] = {"-w", "1", NULL};
    char **args = rem_args;

    ck_assert_err_none(client_init(&c, GET_NEEDED | PUT_NEEDED | TOTAL_SERVERS, &argc, &args));

    ck_assert_int_eq(c.args.get_needed, 1);
    ck_assert_int_eq(c.args.put_needed, 1);
    ck_assert_int_eq(c.args.total_servers, 1);

    ck_assert_int_eq(argc, 0);
    ck_assert_ptr_eq(args, &rem_args[2]);

    client_end(&c);
}
END_TEST

START_TEST(test_R_implies_N)
{
    client_t c = {0};
    int argc = 2;
    char *rem_args[] = {"-r", "1", NULL};
    char **args = rem_args;

    ck_assert_err_none(client_init(&c, GET_NEEDED | PUT_NEEDED | TOTAL_SERVERS, &argc, &args));

    ck_assert_int_eq(c.args.get_needed, 1);
    ck_assert_int_eq(c.args.put_needed, 1);
    ck_assert_int_eq(c.args.total_servers, 1);

    ck_assert_int_eq(argc, 0);
    ck_assert_ptr_eq(args, &rem_args[2]);

    client_end(&c);
}
END_TEST

START_TEST(test_N_only)
{
    client_t c = {0};
    int argc = 2;
    char *rem_args[] = {"-n", "7", NULL};
    char **args = rem_args;

    ck_assert_err_none(client_init(&c, GET_NEEDED | PUT_NEEDED | TOTAL_SERVERS, &argc, &args));

    ck_assert_int_eq(c.args.get_needed, 2);
    ck_assert_int_eq(c.args.put_needed, 2);
    ck_assert_int_eq(c.args.total_servers, 7);

    ck_assert_int_eq(argc, 0);
    ck_assert_ptr_eq(args, &rem_args[2]);

    client_end(&c);
}
END_TEST

START_TEST(test_N_clamps_defaults)
{
    client_t c = {0};
    int argc = 2;
    char *rem_args[] = {"-n", "1", NULL};
    char **args = rem_args;

    ck_assert_err_none(client_init(&c, GET_NEEDED | PUT_NEEDED | TOTAL_SERVERS, &argc, &args));

    ck_assert_int_eq(c.args.get_needed, 1);
    ck_assert_int_eq(c.args.put_needed, 1);
    ck_assert_int_eq(c.args.total_servers, 1);

    ck_assert_int_eq(argc, 0);
    ck_assert_ptr_eq(args, &rem_args[2]);

    client_end(&c);
}
END_TEST

START_TEST(test_W_cannot_surpass_N)
{
    client_t c = {0};
    int argc = 4;
    char *rem_args[] = {"-n", "2", "-w", "3", NULL};
    char **args = rem_args;

    ck_assert_err(client_init(&c, GET_NEEDED | PUT_NEEDED | TOTAL_SERVERS, &argc, &args), ERR_INVALID_COMMAND);
}
END_TEST

START_TEST(test_R_cannot_surpass_N)
{
    client_t c = {0};
    int argc = 4;
    char *rem_args[] = {"-n", "2", "-r", "3", NULL};
    char **args = rem_args;

    ck_assert_err(client_init(&c, GET_NEEDED | PUT_NEEDED | TOTAL_SERVERS, &argc, &args), ERR_INVALID_COMMAND);
}
END_TEST

START_TEST(test_N_strictly_positive)
{
    client_t c = {0};
    int argc = 2;
    char *rem_args[] = {"-n", "0", NULL};
    char **args = rem_args;

    ck_assert_err(client_init(&c, GET_NEEDED | PUT_NEEDED | TOTAL_SERVERS, &argc, &args), ERR_INVALID_COMMAND);

    args = rem_args;
    argc = 2;
    rem_args[1] = "-1";
    ck_assert_err(client_init(&c, GET_NEEDED | PUT_NEEDED | TOTAL_SERVERS, &argc, &args), ERR_INVALID_COMMAND);
}
END_TEST

START_TEST(test_ignores_unwanted_flags)
{
    client_t c = {0};
    int argc = 6;
    char *rem_args[] = {"-n", "5", "-r", "3", "-w", "3", NULL};
    char **args = rem_args;

    ck_assert_err_none(client_init(&c, PUT_NEEDED | TOTAL_SERVERS, &argc, &args));

    ck_assert_int_eq(c.args.get_needed, 2);
    ck_assert_int_eq(c.args.put_needed, 3);
    ck_assert_int_eq(c.args.total_servers, 5);

    ck_assert_int_eq(argc, 0);
    ck_assert_ptr_eq(args, &rem_args[6]);

    client_end(&c);

    args = rem_args;
    argc = 6;
    ck_assert_err_none(client_init(&c, GET_NEEDED | TOTAL_SERVERS, &argc, &args));

    ck_assert_int_eq(c.args.get_needed, 3);
    ck_assert_int_eq(c.args.put_needed, 2);
    ck_assert_int_eq(c.args.total_servers, 5);

    ck_assert_int_eq(argc, 0);
    ck_assert_ptr_eq(args, &rem_args[6]);

    client_end(&c);
}
END_TEST

START_TEST(test_stops_at_ddash)
{
    client_t c = {0};
    int argc = 5;
    char *rem_args[] = {"-n", "7", "--", "-r", "5", NULL};
    char **args = rem_args;

    ck_assert_err_none(client_init(&c, GET_NEEDED | PUT_NEEDED | TOTAL_SERVERS, &argc, &args));

    ck_assert_int_eq(c.args.get_needed, 2);
    ck_assert_int_eq(c.args.put_needed, 2);
    ck_assert_int_eq(c.args.total_servers, 7);

    ck_assert_int_eq(argc, 2);
    ck_assert_ptr_eq(args, &rem_args[3]);

    client_end(&c);
}
END_TEST

START_TEST(test_expect_flag)
{
    client_t c = {0};
    int argc = 1;
    char *rem_args[] = {"asdfas", "5", NULL};
    char **args = rem_args;

    ck_assert_err(client_init(&c, GET_NEEDED | PUT_NEEDED | TOTAL_SERVERS, &argc, &args), ERR_INVALID_COMMAND);
}
END_TEST

START_TEST(test_flag_must_be_in_NRW)
{
    client_t c = {0};
    int argc = 1;
    char *rem_args[] = {"-x", "5", NULL};
    char **args = rem_args;

    ck_assert_err(client_init(&c, GET_NEEDED | PUT_NEEDED | TOTAL_SERVERS, &argc, &args), ERR_INVALID_COMMAND);
}
END_TEST

START_TEST(test_flag_require_arg)
{
    client_t c = {0};
    int argc = 1;
    char *rem_args[] = {"-n", NULL};
    char **args = rem_args;

    ck_assert_err(client_init(&c, GET_NEEDED | PUT_NEEDED | TOTAL_SERVERS, &argc, &args), ERR_INVALID_COMMAND);
}
END_TEST

START_TEST(test_flag_require_number)
{
    client_t c = {0};
    int argc = 2;
    char *rem_args[] = {"-n", "abc", NULL};
    char **args = rem_args;

    ck_assert_err(client_init(&c, GET_NEEDED | PUT_NEEDED | TOTAL_SERVERS, &argc, &args), ERR_INVALID_COMMAND);

    argc = 2;
    args = rem_args;
    rem_args[1] = "1234abc";
    ck_assert_err(client_init(&c, GET_NEEDED | PUT_NEEDED | TOTAL_SERVERS, &argc, &args), ERR_INVALID_COMMAND);
}
END_TEST

Suite *client_suite(void)
{
    Suite *s;
    TCase *tc_core;

    s = suite_create("Client init/end");

    Add_Test(s, test_init_null_args);
    Add_Test(s, test_end_null_args);
    Add_Test(s, test_all_args);
    Add_Test(s, test_empty_yields_defaults);
    Add_Test(s, test_W_implies_N);
    Add_Test(s, test_R_cannot_surpass_N);
    Add_Test(s, test_N_strictly_positive);
    Add_Test(s, test_flag_require_arg);
    Add_Test(s, test_flag_require_number);

    return s;
}

TEST_SUITE(client_suite);
