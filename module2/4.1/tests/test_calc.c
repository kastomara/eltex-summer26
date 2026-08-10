#include "unity.h"
#include "pb_queue.h"
#include <string.h>
#include <stdlib.h>

PhoneBook pb;

void setUp(void) {
    initPhoneBook(&pb);
}

void tearDown(void) {
    freePhoneBook(&pb);
}

void test_initPhoneBook_should_initialize_to_null(void) {
    TEST_ASSERT_NULL(pb.head);
    TEST_ASSERT_NULL(pb.tail);
    TEST_ASSERT_EQUAL_INT(0, pb.count);
}

void test_addContact_should_increase_count(void) {
    Contact *c = createContact();
    strcpy(c->surname, "Smith");
    strcpy(c->firstName, "John");
    c->phoneCount = 0;
    c->emailCount = 0;
    c->socialCount = 0;
    c->messengerCount = 0;
    
    if (pb.head == NULL) {
        pb.head = c;
        pb.tail = c;
    }
    pb.count++;
    
    TEST_ASSERT_EQUAL_INT(1, pb.count);
    TEST_ASSERT_EQUAL_PTR(c, pb.head);
    TEST_ASSERT_EQUAL_PTR(c, pb.tail);
}

void test_addContact_should_store_correct_data(void) {
    Contact *c = createContact();
    strcpy(c->surname, "Doe");
    strcpy(c->firstName, "Jane");
    strcpy(c->workplace, "Company Inc");
    strcpy(c->jobTitle, "Engineer");
    strcpy(c->phoneNumbers[0], "123-456-7890");
    c->phoneCount = 1;
    
    if (pb.head == NULL) {
        pb.head = c;
        pb.tail = c;
    }
    pb.count++;
    
    TEST_ASSERT_EQUAL_STRING("Doe", pb.head->surname);
    TEST_ASSERT_EQUAL_STRING("Jane", pb.head->firstName);
    TEST_ASSERT_EQUAL_STRING("Company Inc", pb.head->workplace);
    TEST_ASSERT_EQUAL_STRING("Engineer", pb.head->jobTitle);
    TEST_ASSERT_EQUAL_STRING("123-456-7890", pb.head->phoneNumbers[0]);
    TEST_ASSERT_EQUAL_INT(1, pb.head->phoneCount);
}

void test_findContact_should_return_correct_contact(void) {
    Contact *c1 = createContact();
    strcpy(c1->surname, "Brown");
    strcpy(c1->firstName, "Bob");
    pb.head = c1;
    pb.tail = c1;
    pb.count = 1;
    
    Contact *c2 = createContact();
    strcpy(c2->surname, "Green");
    strcpy(c2->firstName, "Alice");
    c1->next = c2;
    c2->prev = c1;
    pb.tail = c2;
    pb.count = 2;
    
    Contact *found = findContact(&pb, "Green", "Alice");
    TEST_ASSERT_EQUAL_PTR(c2, found);
}

void test_findContact_should_return_null_when_not_found(void) {
    Contact *c = createContact();
    strcpy(c->surname, "White");
    strcpy(c->firstName, "Tom");
    pb.head = c;
    pb.tail = c;
    pb.count = 1;
    
    Contact *found = findContact(&pb, "Black", "Jim");
    TEST_ASSERT_NULL(found);
}

void test_deleteContact_should_remove_contact(void) {
    Contact *c1 = createContact();
    strcpy(c1->surname, "First");
    strcpy(c1->firstName, "A");
    pb.head = c1;
    pb.tail = c1;
    pb.count = 1;
    
    Contact *c2 = createContact();
    strcpy(c2->surname, "Second");
    strcpy(c2->firstName, "B");
    c1->next = c2;
    c2->prev = c1;
    pb.tail = c2;
    pb.count = 2;
    
    Contact *c3 = createContact();
    strcpy(c3->surname, "Third");
    strcpy(c3->firstName, "C");
    c2->next = c3;
    c3->prev = c2;
    pb.tail = c3;
    pb.count = 3;
    
    if (c2->prev != NULL) {
        c2->prev->next = c2->next;
    }
    if (c2->next != NULL) {
        c2->next->prev = c2->prev;
    }
    free(c2);
    pb.count--;
    
    TEST_ASSERT_EQUAL_INT(2, pb.count);
    TEST_ASSERT_EQUAL_STRING("First", pb.head->surname);
    TEST_ASSERT_EQUAL_STRING("Third", pb.tail->surname);
    TEST_ASSERT_EQUAL_PTR(c3, pb.head->next);
    TEST_ASSERT_EQUAL_PTR(c1, pb.tail->prev);
}

void test_contact_optional_fields_should_be_empty(void) {
    Contact *c = createContact();
    strcpy(c->surname, "Test");
    strcpy(c->firstName, "User");
    strcpy(c->workplace, "");
    strcpy(c->jobTitle, "");
    c->phoneCount = 0;
    c->emailCount = 0;
    c->socialCount = 0;
    c->messengerCount = 0;
    
    pb.head = c;
    pb.tail = c;
    pb.count = 1;
    
    TEST_ASSERT_EQUAL_STRING("Test", pb.head->surname);
    TEST_ASSERT_EQUAL_STRING("User", pb.head->firstName);
    TEST_ASSERT_EQUAL_STRING("", pb.head->workplace);
    TEST_ASSERT_EQUAL_STRING("", pb.head->jobTitle);
    TEST_ASSERT_EQUAL_INT(0, pb.head->phoneCount);
    TEST_ASSERT_EQUAL_INT(0, pb.head->emailCount);
}

void test_multiple_phone_numbers_should_be_stored(void) {
    Contact *c = createContact();
    strcpy(c->surname, "Multiple");
    strcpy(c->firstName, "Phones");
    strcpy(c->phoneNumbers[0], "111-111-1111");
    strcpy(c->phoneNumbers[1], "222-222-2222");
    strcpy(c->phoneNumbers[2], "333-333-3333");
    c->phoneCount = 3;
    
    pb.head = c;
    pb.tail = c;
    pb.count = 1;
    
    TEST_ASSERT_EQUAL_INT(3, pb.head->phoneCount);
    TEST_ASSERT_EQUAL_STRING("111-111-1111", pb.head->phoneNumbers[0]);
    TEST_ASSERT_EQUAL_STRING("222-222-2222", pb.head->phoneNumbers[1]);
    TEST_ASSERT_EQUAL_STRING("333-333-3333", pb.head->phoneNumbers[2]);
}

void test_doubly_linked_list_should_maintain_order(void) {
    Contact *c1 = createContact();
    strcpy(c1->surname, "Adams");
    strcpy(c1->firstName, "John");
    pb.head = c1;
    pb.tail = c1;
    pb.count = 1;
    
    Contact *c2 = createContact();
    strcpy(c2->surname, "Smith");
    strcpy(c2->firstName, "Bob");
    c1->next = c2;
    c2->prev = c1;
    pb.tail = c2;
    pb.count = 2;
    
    Contact *c3 = createContact();
    strcpy(c3->surname, "Brown");
    strcpy(c3->firstName, "Alice");
    
    Contact *current = pb.head;
    while (current->next != NULL && strcmp(c3->surname, current->next->surname) > 0) {
        current = current->next;
    }
    c3->next = current->next;
    c3->prev = current;
    if (current->next != NULL) {
        current->next->prev = c3;
    } else {
        pb.tail = c3;
    }
    current->next = c3;
    pb.count = 3;
    
    TEST_ASSERT_EQUAL_STRING("Adams", pb.head->surname);
    TEST_ASSERT_EQUAL_STRING("Brown", pb.head->next->surname);
    TEST_ASSERT_EQUAL_STRING("Smith", pb.tail->surname);
    TEST_ASSERT_EQUAL_PTR(c2, pb.tail);
    TEST_ASSERT_EQUAL_PTR(c3, pb.head->next);
    TEST_ASSERT_EQUAL_PTR(c1, c3->prev);
}

int main(void) {
    UNITY_BEGIN();
    
    RUN_TEST(test_initPhoneBook_should_initialize_to_null);
    RUN_TEST(test_addContact_should_increase_count);
    RUN_TEST(test_addContact_should_store_correct_data);
    RUN_TEST(test_findContact_should_return_correct_contact);
    RUN_TEST(test_findContact_should_return_null_when_not_found);
    RUN_TEST(test_deleteContact_should_remove_contact);
    RUN_TEST(test_contact_optional_fields_should_be_empty);
    RUN_TEST(test_multiple_phone_numbers_should_be_stored);
    RUN_TEST(test_doubly_linked_list_should_maintain_order);
    
    return UNITY_END();
}