#include "unity.h"
#include "calc.h"
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>

static const char *TEST_FILE = "test_file_001";
static const char *TEST_FILE2 = "test_file_002";
static const char *TEST_DIR = "test_dir_001";

void cleanup_test_files(void) {
    unlink(TEST_FILE);
    unlink(TEST_FILE2);
    rmdir(TEST_DIR);
}

void setUp(void) {
    cleanup_test_files();
}

void tearDown(void) {
    cleanup_test_files();
}

void test_parse_symbolic_standard(void) {
    mode_t mode = 0;
    parse_symbolic("rwxr-xr-x", &mode);
    TEST_ASSERT_TRUE(mode & S_IRUSR);
    TEST_ASSERT_TRUE(mode & S_IWUSR);
    TEST_ASSERT_TRUE(mode & S_IXUSR);
    TEST_ASSERT_TRUE(mode & S_IRGRP);
    TEST_ASSERT_FALSE(mode & S_IWGRP);
    TEST_ASSERT_TRUE(mode & S_IXGRP);
    TEST_ASSERT_TRUE(mode & S_IROTH);
    TEST_ASSERT_FALSE(mode & S_IWOTH);
    TEST_ASSERT_TRUE(mode & S_IXOTH);
}

void test_parse_symbolic_with_dash(void) {
    mode_t mode = 0;
    parse_symbolic("-rwxr-xr-x", &mode);
    TEST_ASSERT_TRUE(mode & S_IRUSR);
    TEST_ASSERT_TRUE(mode & S_IWUSR);
    TEST_ASSERT_TRUE(mode & S_IXUSR);
    TEST_ASSERT_TRUE(mode & S_IRGRP);
    TEST_ASSERT_FALSE(mode & S_IWGRP);
}

void test_parse_symbolic_with_d(void) {
    mode_t mode = 0;
    parse_symbolic("drwxr-xr-x", &mode);
    TEST_ASSERT_TRUE(mode & S_IRUSR);
    TEST_ASSERT_TRUE(mode & S_IWUSR);
    TEST_ASSERT_TRUE(mode & S_IXUSR);
    TEST_ASSERT_TRUE(mode & S_IRGRP);
    TEST_ASSERT_FALSE(mode & S_IWGRP);
}

void test_parse_octal_755(void) {
    mode_t mode = 0;
    parse_octal("755", &mode);
    TEST_ASSERT_EQUAL_INT(0755, mode & 07777);
}

void test_parse_octal_644(void) {
    mode_t mode = 0;
    parse_octal("644", &mode);
    TEST_ASSERT_EQUAL_INT(0644, mode & 07777);
}

void test_parse_octal_with_leading_zero(void) {
    mode_t mode = 0;
    parse_octal("0755", &mode);
    TEST_ASSERT_EQUAL_INT(0755, mode & 07777);
}

void test_parse_octal_invalid(void) {
    mode_t mode = 0;
    parse_octal("999", &mode);
    TEST_ASSERT_EQUAL_INT(0, mode & 07777);
}

void test_parse_permissions_octal_detect(void) {
    mode_t mode = 0;
    int result = parse_permissions("755", &mode);
    TEST_ASSERT_EQUAL_INT(1, result);
    TEST_ASSERT_EQUAL_INT(0755, mode & 07777);
}

void test_parse_permissions_symbolic_detect(void) {
    mode_t mode = 0;
    int result = parse_permissions("rwxr-xr-x", &mode);
    TEST_ASSERT_EQUAL_INT(0, result);
    TEST_ASSERT_TRUE(mode & S_IRUSR);
}

void test_format_permissions_755(void) {
    FilePermissions perms;
    mode_t mode = 0755;
    format_permissions(mode, &perms);
    TEST_ASSERT_EQUAL_STRING("-rwxr-xr-x", perms.symbolic);
    TEST_ASSERT_EQUAL_STRING("0755", perms.octal);
}

void test_format_permissions_644(void) {
    FilePermissions perms;
    mode_t mode = 0644;
    format_permissions(mode, &perms);
    TEST_ASSERT_EQUAL_STRING("-rw-r--r--", perms.symbolic);
    TEST_ASSERT_EQUAL_STRING("0644", perms.octal);
}

void test_format_permissions_directory(void) {
    FilePermissions perms;
    mode_t mode = S_IFDIR | 0755;
    format_permissions(mode, &perms);
    TEST_ASSERT_EQUAL('d', perms.symbolic[0]);
}

void test_get_file_permissions_existing(void) {
    FilePermissions perms;
    system("touch test_file_001");
    system("chmod 755 test_file_001");
    
    int result = get_file_permissions("test_file_001", &perms);
    TEST_ASSERT_EQUAL_INT(0, result);
    TEST_ASSERT_EQUAL_STRING("-rwxr-xr-x", perms.symbolic);
    TEST_ASSERT_EQUAL_STRING("0755", perms.octal);
}

void test_get_file_permissions_nonexistent(void) {
    FilePermissions perms;
    int result = get_file_permissions("/nonexistent_file_12345", &perms);
    TEST_ASSERT_EQUAL_INT(-1, result);
}

void test_modify_permissions_add_u_x(void) {
    mode_t mode = 0644;
    modify_permissions(&mode, "u+x");
    TEST_ASSERT_TRUE(mode & S_IXUSR);
    TEST_ASSERT_TRUE(mode & S_IRUSR);
    TEST_ASSERT_TRUE(mode & S_IWUSR);
}

void test_modify_permissions_add_g_w(void) {
    mode_t mode = 0755;
    modify_permissions(&mode, "g+w");
    TEST_ASSERT_TRUE(mode & S_IWGRP);
}

void test_modify_permissions_remove_go_w(void) {
    mode_t mode = 0777;
    modify_permissions(&mode, "go-w");
    TEST_ASSERT_TRUE(mode & S_IRUSR);
    TEST_ASSERT_TRUE(mode & S_IWUSR);
    TEST_ASSERT_TRUE(mode & S_IXUSR);
    TEST_ASSERT_TRUE(mode & S_IRGRP);
    TEST_ASSERT_FALSE(mode & S_IWGRP);
    TEST_ASSERT_TRUE(mode & S_IXGRP);
    TEST_ASSERT_TRUE(mode & S_IROTH);
    TEST_ASSERT_FALSE(mode & S_IWOTH);
    TEST_ASSERT_TRUE(mode & S_IXOTH);
}

void test_modify_permissions_set_a_rw(void) {
    mode_t mode = 0755;
    modify_permissions(&mode, "a=rw");
    TEST_ASSERT_TRUE(mode & S_IRUSR);
    TEST_ASSERT_TRUE(mode & S_IWUSR);
    TEST_ASSERT_FALSE(mode & S_IXUSR);
    TEST_ASSERT_TRUE(mode & S_IRGRP);
    TEST_ASSERT_TRUE(mode & S_IWGRP);
    TEST_ASSERT_FALSE(mode & S_IXGRP);
    TEST_ASSERT_TRUE(mode & S_IROTH);
    TEST_ASSERT_TRUE(mode & S_IWOTH);
    TEST_ASSERT_FALSE(mode & S_IXOTH);
}

void test_modify_permissions_no_who(void) {
    mode_t mode = 0644;
    modify_permissions(&mode, "+x");
    TEST_ASSERT_TRUE(mode & S_IXUSR);
    TEST_ASSERT_TRUE(mode & S_IXGRP);
    TEST_ASSERT_TRUE(mode & S_IXOTH);
}

void test_file_creation_and_chmod_755(void) {
    FilePermissions perms;
    mode_t expected_mode = 0755;
    char cmd[256];
    
    system("touch test_file_001");
    snprintf(cmd, sizeof(cmd), "chmod %o test_file_001", expected_mode);
    system(cmd);
    
    get_file_permissions("test_file_001", &perms);
    TEST_ASSERT_EQUAL_INT(expected_mode, perms.mode & 07777);
    TEST_ASSERT_EQUAL_STRING("-rwxr-xr-x", perms.symbolic);
    TEST_ASSERT_EQUAL_STRING("0755", perms.octal);
}

void test_directory_creation_and_chmod(void) {
    FilePermissions perms;
    mode_t expected_mode = 0755;
    char cmd[256];
    
    mkdir("test_dir_001", 0755);
    
    struct stat st;
    stat("test_dir_001", &st);
    TEST_ASSERT_TRUE(S_ISDIR(st.st_mode));
    
    snprintf(cmd, sizeof(cmd), "chmod %o test_dir_001", expected_mode);
    system(cmd);
    
    get_file_permissions("test_dir_001", &perms);
    TEST_ASSERT_EQUAL_INT(expected_mode, perms.mode & 07777);
    TEST_ASSERT_EQUAL_STRING("drwxr-xr-x", perms.symbolic);
    TEST_ASSERT_EQUAL_STRING("0755", perms.octal);
    TEST_ASSERT_EQUAL('d', perms.symbolic[0]);
    TEST_ASSERT_TRUE(S_ISDIR(perms.mode));
}

void test_modify_and_apply_to_file(void) {
    FilePermissions perms;
    mode_t initial_mode = 0644;
    char cmd[256];
    
    system("touch test_file_001");
    snprintf(cmd, sizeof(cmd), "chmod %o test_file_001", initial_mode);
    system(cmd);
    
    get_file_permissions("test_file_001", &perms);
    mode_t mode = perms.mode;
    
    modify_permissions(&mode, "u+x");
    TEST_ASSERT_TRUE(mode & S_IXUSR);
    TEST_ASSERT_TRUE(mode & S_IRUSR);
    TEST_ASSERT_TRUE(mode & S_IWUSR);
    
    snprintf(cmd, sizeof(cmd), "chmod %o test_file_001", mode & 07777);
    system(cmd);
    
    get_file_permissions("test_file_001", &perms);
    TEST_ASSERT_TRUE(perms.mode & S_IXUSR);
    TEST_ASSERT_EQUAL_STRING("-rwxr--r--", perms.symbolic);
}

void test_special_bits_setuid(void) {
    FilePermissions perms;
    
    system("touch test_file_001");
    system("chmod 4755 test_file_001");
    
    get_file_permissions("test_file_001", &perms);
    TEST_ASSERT_TRUE(perms.mode & S_ISUID);
    // Проверяем, что бит setuid установлен (символ 's' в позиции выполнения для владельца)
    TEST_ASSERT_EQUAL('s', perms.symbolic[3]);
    TEST_ASSERT_EQUAL_STRING("4755", perms.octal);
}

void test_special_bits_sticky(void) {
    FilePermissions perms;
    
    system("touch test_file_001");
    system("chmod 1777 test_file_001");
    
    get_file_permissions("test_file_001", &perms);
    TEST_ASSERT_TRUE(perms.mode & S_ISVTX);
    // Проверяем, что sticky бит установлен (символ 't' в позиции выполнения для других)
    TEST_ASSERT_EQUAL('t', perms.symbolic[9]);
    TEST_ASSERT_EQUAL_STRING("1777", perms.octal);
}

void test_special_bits_setgid(void) {
    FilePermissions perms;
    
    system("touch test_file_001");
    system("chmod 2755 test_file_001");
    
    get_file_permissions("test_file_001", &perms);
    TEST_ASSERT_TRUE(perms.mode & S_ISGID);
    // Проверяем, что бит setgid установлен (символ 's' в позиции выполнения для группы)
    TEST_ASSERT_EQUAL('s', perms.symbolic[6]);
    TEST_ASSERT_EQUAL_STRING("2755", perms.octal);
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_parse_symbolic_standard);
    RUN_TEST(test_parse_symbolic_with_dash);
    RUN_TEST(test_parse_symbolic_with_d);
    RUN_TEST(test_parse_octal_755);
    RUN_TEST(test_parse_octal_644);
    RUN_TEST(test_parse_octal_with_leading_zero);
    RUN_TEST(test_parse_octal_invalid);
    RUN_TEST(test_parse_permissions_octal_detect);
    RUN_TEST(test_parse_permissions_symbolic_detect);
    RUN_TEST(test_format_permissions_755);
    RUN_TEST(test_format_permissions_644);
    RUN_TEST(test_format_permissions_directory);
    RUN_TEST(test_get_file_permissions_existing);
    RUN_TEST(test_get_file_permissions_nonexistent);
    RUN_TEST(test_modify_permissions_add_u_x);
    RUN_TEST(test_modify_permissions_add_g_w);
    RUN_TEST(test_modify_permissions_remove_go_w);
    RUN_TEST(test_modify_permissions_set_a_rw);
    RUN_TEST(test_modify_permissions_no_who);
    RUN_TEST(test_file_creation_and_chmod_755);
    RUN_TEST(test_directory_creation_and_chmod);
    RUN_TEST(test_modify_and_apply_to_file);
    RUN_TEST(test_special_bits_setuid);
    RUN_TEST(test_special_bits_sticky);
    RUN_TEST(test_special_bits_setgid);
    
    return UNITY_END();
}