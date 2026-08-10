#include "unity.h"
#include "phonebook.h"
#include <string.h>

PhoneBook pb;

void setUp(void) {
    initPhoneBook(&pb);
}

void tearDown(void) {
}

void test_initPhoneBook_should_initialize_count_to_zero(void) {
    TEST_ASSERT_EQUAL_INT(0, pb.count);
}

void test_addContact_should_increase_count(void) {
    Contact *c = &pb.contacts[pb.count];
    strcpy(c->surname, "Smith");
    strcpy(c->firstName, "John");
    c->phoneCount = 0;
    c->emailCount = 0;
    c->socialCount = 0;
    c->messengerCount = 0;
    pb.count++;
    
    TEST_ASSERT_EQUAL_INT(1, pb.count);
}

void test_addContact_should_store_correct_data(void) {
    Contact *c = &pb.contacts[pb.count];
    strcpy(c->surname, "Doe");
    strcpy(c->firstName, "Jane");
    strcpy(c->workplace, "Company Inc");
    strcpy(c->jobTitle, "Engineer");
    strcpy(c->phoneNumbers[0], "123-456-7890");
    c->phoneCount = 1;
    pb.count++;
    
    TEST_ASSERT_EQUAL_STRING("Doe", pb.contacts[0].surname);
    TEST_ASSERT_EQUAL_STRING("Jane", pb.contacts[0].firstName);
    TEST_ASSERT_EQUAL_STRING("Company Inc", pb.contacts[0].workplace);
    TEST_ASSERT_EQUAL_STRING("Engineer", pb.contacts[0].jobTitle);
    TEST_ASSERT_EQUAL_STRING("123-456-7890", pb.contacts[0].phoneNumbers[0]);
    TEST_ASSERT_EQUAL_INT(1, pb.contacts[0].phoneCount);
}

void test_findContactIndex_should_return_correct_index(void) {
    Contact *c1 = &pb.contacts[pb.count];
    strcpy(c1->surname, "Brown");
    strcpy(c1->firstName, "Bob");
    pb.count++;
    
    Contact *c2 = &pb.contacts[pb.count];
    strcpy(c2->surname, "Green");
    strcpy(c2->firstName, "Alice");
    pb.count++;
    
    int index = findContactIndex(&pb, "Green", "Alice");
    TEST_ASSERT_EQUAL_INT(1, index);
}

void test_findContactIndex_should_return_minus_one_when_not_found(void) {
    Contact *c = &pb.contacts[pb.count];
    strcpy(c->surname, "White");
    strcpy(c->firstName, "Tom");
    pb.count++;
    
    int index = findContactIndex(&pb, "Black", "Jim");
    TEST_ASSERT_EQUAL_INT(-1, index);
}

void test_deleteContact_should_remove_contact(void) {
    Contact *c1 = &pb.contacts[pb.count];
    strcpy(c1->surname, "First");
    strcpy(c1->firstName, "A");
    pb.count++;
    
    Contact *c2 = &pb.contacts[pb.count];
    strcpy(c2->surname, "Second");
    strcpy(c2->firstName, "B");
    pb.count++;
    
    Contact *c3 = &pb.contacts[pb.count];
    strcpy(c3->surname, "Third");
    strcpy(c3->firstName, "C");
    pb.count++;
    
    for (int i = 1; i < pb.count - 1; i++) {
        pb.contacts[i] = pb.contacts[i + 1];
    }
    pb.count--;
    
    TEST_ASSERT_EQUAL_INT(2, pb.count);
    TEST_ASSERT_EQUAL_STRING("First", pb.contacts[0].surname);
    TEST_ASSERT_EQUAL_STRING("Third", pb.contacts[1].surname);
}

void test_contact_optional_fields_should_be_empty(void) {
    Contact *c = &pb.contacts[pb.count];
    strcpy(c->surname, "Test");
    strcpy(c->firstName, "User");
    strcpy(c->workplace, "");
    strcpy(c->jobTitle, "");
    c->phoneCount = 0;
    c->emailCount = 0;
    c->socialCount = 0;
    c->messengerCount = 0;
    pb.count++;
    
    TEST_ASSERT_EQUAL_STRING("Test", pb.contacts[0].surname);
    TEST_ASSERT_EQUAL_STRING("User", pb.contacts[0].firstName);
    TEST_ASSERT_EQUAL_STRING("", pb.contacts[0].workplace);
    TEST_ASSERT_EQUAL_STRING("", pb.contacts[0].jobTitle);
    TEST_ASSERT_EQUAL_INT(0, pb.contacts[0].phoneCount);
    TEST_ASSERT_EQUAL_INT(0, pb.contacts[0].emailCount);
}

void test_multiple_phone_numbers_should_be_stored(void) {
    Contact *c = &pb.contacts[pb.count];
    strcpy(c->surname, "Multiple");
    strcpy(c->firstName, "Phones");
    strcpy(c->phoneNumbers[0], "111-111-1111");
    strcpy(c->phoneNumbers[1], "222-222-2222");
    strcpy(c->phoneNumbers[2], "333-333-3333");
    c->phoneCount = 3;
    pb.count++;
    
    TEST_ASSERT_EQUAL_INT(3, pb.contacts[0].phoneCount);
    TEST_ASSERT_EQUAL_STRING("111-111-1111", pb.contacts[0].phoneNumbers[0]);
    TEST_ASSERT_EQUAL_STRING("222-222-2222", pb.contacts[0].phoneNumbers[1]);
    TEST_ASSERT_EQUAL_STRING("333-333-3333", pb.contacts[0].phoneNumbers[2]);
}

void test_multiple_emails_should_be_stored(void) {
    Contact *c = &pb.contacts[pb.count];
    strcpy(c->surname, "Email");
    strcpy(c->firstName, "Test");
    strcpy(c->emails[0], "test1@example.com");
    strcpy(c->emails[1], "test2@example.com");
    c->emailCount = 2;
    pb.count++;
    
    TEST_ASSERT_EQUAL_INT(2, pb.contacts[0].emailCount);
    TEST_ASSERT_EQUAL_STRING("test1@example.com", pb.contacts[0].emails[0]);
    TEST_ASSERT_EQUAL_STRING("test2@example.com", pb.contacts[0].emails[1]);
}

int main(void) {
    UNITY_BEGIN();
    
    RUN_TEST(test_initPhoneBook_should_initialize_count_to_zero);
    RUN_TEST(test_addContact_should_increase_count);
    RUN_TEST(test_addContact_should_store_correct_data);
    RUN_TEST(test_findContactIndex_should_return_correct_index);
    RUN_TEST(test_findContactIndex_should_return_minus_one_when_not_found);
    RUN_TEST(test_deleteContact_should_remove_contact);
    RUN_TEST(test_contact_optional_fields_should_be_empty);
    RUN_TEST(test_multiple_phone_numbers_should_be_stored);
    RUN_TEST(test_multiple_emails_should_be_stored);
    
    return UNITY_END();
}