#include "tinterp.h"
#include <check.h>
#include <stdlib.h>

uint32_t glob_argc;
tinterp_arg_t glob_argv[TINTERP_CMD_ARGS_MAX];
char* glob_args;
uint32_t glob_cb_cnt;

void check_callback(uint32_t argc, tinterp_arg_t* argv)
{
    ck_assert_msg(argc == glob_argc, "argc: expected %d, got %d", glob_argc, argc);
    for (uint32_t i = 0; i < argc; i++) {
            switch (glob_args[i]) {
            case 's':
                ck_assert_msg(strcmp(argv[i].s, glob_argv[i].s) == 0, "argv[%0d]: expected \"%s\", got \"%s\"", i, glob_argv[i].s, argv[i].s);
                break;
            case 'f':
                ck_assert_msg(argv[i].f == glob_argv[i].f, "argv[%0d]: expected %f, got %f", i, glob_argv[i].f, argv[i].f);
                break;
            case 'i':
                ck_assert_msg(argv[i].ui == glob_argv[i].ui, "argv[%0d]: expected 0x%08x, got 0x%08x", i, glob_argv[i].ui, argv[i].ui);
                break;
            }
        }
    glob_cb_cnt++;
}

void empty_callback(uint32_t argc, tinterp_arg_t* argv)
{
    (void)argc;
    (void)argv;
    ck_assert_msg(0, "Empty callback should not be invoked!");
}

START_TEST(test_cmd_help)
{
    char buf[TINTERP_BUF_SIZE];

    tinterp_set_callback(TINTERP_CMD_HELP, &check_callback);
    tinterp_set_callback(TINTERP_CMD_GET_TIME, &empty_callback);
    tinterp_set_callback(TINTERP_CMD_SET_TIME, &empty_callback);
    tinterp_set_callback(TINTERP_CMD_GET_NAME, &empty_callback);
    tinterp_set_callback(TINTERP_CMD_SET_NAME, &empty_callback);
    glob_argc = 0;
    glob_args = "";
    glob_cb_cnt = 0;

    sscanf(" #ПоМОщь;%/*()+=- ;№!  ", "%[^\n]", buf);
    tinterp_parse(buf);

    ck_assert_msg(glob_cb_cnt == 1, "No check callbacks were invoked! Counter = %d", glob_cb_cnt);
}
END_TEST

START_TEST(test_cmd_set_time)
{
    char buf[TINTERP_BUF_SIZE];

    tinterp_set_callback(TINTERP_CMD_HELP, &empty_callback);
    tinterp_set_callback(TINTERP_CMD_GET_TIME, &empty_callback);
    tinterp_set_callback(TINTERP_CMD_SET_TIME, &check_callback);
    tinterp_set_callback(TINTERP_CMD_GET_NAME, &empty_callback);
    tinterp_set_callback(TINTERP_CMD_SET_NAME, &empty_callback);
    glob_argc = 3;
    glob_args = "iii";
    glob_argv[0].ui = 0x16;
    glob_argv[1].ui = 0x25;
    glob_argv[2].ui = 0x30;
    glob_cb_cnt = 0;

    sscanf("   УСТАНОВИТЬ№:?врЕМЯ;%/*()+=- 22;№!37 48  ", "%[^\n]", buf);
    tinterp_parse(buf);

    sscanf("установить ВРЕМЯ;%/*()+=-0x16;№!0x25 0x30  ", "%[^\n]", buf);
    tinterp_parse(buf);

    ck_assert_msg(glob_cb_cnt == 2, "No check callbacks were invoked! Counter = %d", glob_cb_cnt);
}
END_TEST

START_TEST(test_cmd_get_time)
{
    char buf[TINTERP_BUF_SIZE];

    tinterp_set_callback(TINTERP_CMD_HELP, &empty_callback);
    tinterp_set_callback(TINTERP_CMD_GET_TIME, &check_callback);
    tinterp_set_callback(TINTERP_CMD_SET_TIME, &empty_callback);
    tinterp_set_callback(TINTERP_CMD_GET_NAME, &empty_callback);
    tinterp_set_callback(TINTERP_CMD_SET_NAME, &empty_callback);
    glob_argc = 0;
    glob_args = "";
    glob_cb_cnt = 0;

    sscanf("+время;%/*()+=- ;№!  ", "%[^\n]", buf);
    tinterp_parse(buf);

    ck_assert_msg(glob_cb_cnt == 1, "No check callbacks were invoked! Counter = %d", glob_cb_cnt);
}
END_TEST

START_TEST(test_cmd_set_name)
{
    char buf[TINTERP_BUF_SIZE];

    tinterp_set_callback(TINTERP_CMD_HELP, &empty_callback);
    tinterp_set_callback(TINTERP_CMD_GET_TIME, &empty_callback);
    tinterp_set_callback(TINTERP_CMD_SET_TIME, &empty_callback);
    tinterp_set_callback(TINTERP_CMD_GET_NAME, &empty_callback);
    tinterp_set_callback(TINTERP_CMD_SET_NAME, &check_callback);
    glob_argc = 1;
    glob_args = "s";
    char* name = "awesomeимя0";
    glob_argv[0].s = name;
    glob_cb_cnt = 0;

    sscanf("   УСТАНОВИТЬ№:?иМЯ;%/*()+=-AwesomeИМЯ0 ", "%[^\n]", buf);
    tinterp_parse(buf);

    ck_assert_msg(glob_cb_cnt == 1, "No check callbacks were invoked! Counter = %d", glob_cb_cnt);
}
END_TEST

START_TEST(test_cmd_get_name)
{
    char buf[TINTERP_BUF_SIZE];

    tinterp_set_callback(TINTERP_CMD_HELP, &empty_callback);
    tinterp_set_callback(TINTERP_CMD_GET_TIME, &empty_callback);
    tinterp_set_callback(TINTERP_CMD_SET_TIME, &empty_callback);
    tinterp_set_callback(TINTERP_CMD_GET_NAME, &check_callback);
    tinterp_set_callback(TINTERP_CMD_SET_NAME, &empty_callback);
    glob_argc = 0;
    glob_args = "";
    glob_cb_cnt = 0;

    sscanf("+имя;%/*()+=- ;№!  ", "%[^\n]", buf);
    tinterp_parse(buf);

    ck_assert_msg(glob_cb_cnt == 1, "No check callbacks were invoked! Counter = %d", glob_cb_cnt);
}
END_TEST

Suite* module_suite(void)
{
    Suite* s;
    TCase* tc_core;

    s = suite_create("Module");

    // Core test case
    tc_core = tcase_create("Core");
    tcase_add_test(tc_core, test_cmd_help);
    tcase_add_test(tc_core, test_cmd_set_time);
    tcase_add_test(tc_core, test_cmd_get_time);
    tcase_add_test(tc_core, test_cmd_set_name);
    tcase_add_test(tc_core, test_cmd_get_name);
    suite_add_tcase(s, tc_core);

    return s;
}

int main(void)
{
    int number_failed;
    Suite* s;
    SRunner* sr;

    s = module_suite();
    sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
