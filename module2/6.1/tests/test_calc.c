#include "unity.h"
#include "calc.h"
#include <string.h>
#include <stdlib.h>

PhoneBook pb;

void setUp(void) {
    initPhoneBook(&pb);
}

void tearDown(void) {
    freePhoneBook(&pb);
}

void test_addContact_should_add_contact_to_phonebook(void) {
    Contact *c = createContact();
    TEST_ASSERT_NOT_NULL(c);
    
    strcpy(c->surname, "Smith");
    strcpy(c->firstName, "John");
    strcpy(c->workplace, "Google");
    strcpy(c->jobTitle, "Developer");
    strcpy(c->phoneNumbers[0], "+7-999-123-45-67");
    c->phoneCount = 1;
    c->emailCount = 0;
    c->socialCount = 0;
    c->messengerCount = 0;

    insertContact(&pb, c);

    TEST_ASSERT_EQUAL_INT(1, pb.count);
    TEST_ASSERT_NOT_NULL(pb.head);
    TEST_ASSERT_EQUAL_PTR(pb.head, pb.tail);
    TEST_ASSERT_EQUAL_STRING("Smith", pb.head->surname);
    TEST_ASSERT_EQUAL_STRING("John", pb.head->firstName);
    TEST_ASSERT_EQUAL_STRING("Google", pb.head->workplace);
    TEST_ASSERT_EQUAL_STRING("Developer", pb.head->jobTitle);
    TEST_ASSERT_EQUAL_STRING("+7-999-123-45-67", pb.head->phoneNumbers[0]);
    TEST_ASSERT_EQUAL_INT(1, pb.head->phoneCount);
}

void test_findContact_should_find_existing_contact(void) {
    Contact *c1 = createContact();
    strcpy(c1->surname, "Brown");
    strcpy(c1->firstName, "Alice");
    c1->phoneCount = 0;
    c1->emailCount = 0;
    c1->socialCount = 0;
    c1->messengerCount = 0;
    insertContact(&pb, c1);
    
    Contact *c2 = createContact();
    strcpy(c2->surname, "Green");
    strcpy(c2->firstName, "Bob");
    c2->phoneCount = 0;
    c2->emailCount = 0;
    c2->socialCount = 0;
    c2->messengerCount = 0;
    insertContact(&pb, c2);
    
    Contact *c3 = createContact();
    strcpy(c3->surname, "White");
    strcpy(c3->firstName, "Charlie");
    c3->phoneCount = 0;
    c3->emailCount = 0;
    c3->socialCount = 0;
    c3->messengerCount = 0;
    insertContact(&pb, c3);

    Contact *found = findContact(&pb, "Green", "Bob");
    
    TEST_ASSERT_NOT_NULL(found);
    TEST_ASSERT_EQUAL_STRING("Green", found->surname);
    TEST_ASSERT_EQUAL_STRING("Bob", found->firstName);
    TEST_ASSERT_EQUAL_PTR(c2, found);

    Contact *not_found = findContact(&pb, "Black", "Dave");
    TEST_ASSERT_NULL(not_found);
}

void test_removeContact_should_delete_contact_from_phonebook(void) {
    Contact *c1 = createContact();
    strcpy(c1->surname, "Adams");
    strcpy(c1->firstName, "John");
    c1->phoneCount = 0;
    c1->emailCount = 0;
    c1->socialCount = 0;
    c1->messengerCount = 0;
    insertContact(&pb, c1);
    
    Contact *c2 = createContact();
    strcpy(c2->surname, "Brown");
    strcpy(c2->firstName, "Jane");
    c2->phoneCount = 0;
    c2->emailCount = 0;
    c2->socialCount = 0;
    c2->messengerCount = 0;
    insertContact(&pb, c2);
    
    Contact *c3 = createContact();
    strcpy(c3->surname, "Smith");
    strcpy(c3->firstName, "Bob");
    c3->phoneCount = 0;
    c3->emailCount = 0;
    c3->socialCount = 0;
    c3->messengerCount = 0;
    insertContact(&pb, c3);
    
    TEST_ASSERT_EQUAL_INT(3, pb.count);

    removeContact(&pb, c2);
    
    TEST_ASSERT_EQUAL_INT(2, pb.count);
    TEST_ASSERT_EQUAL_STRING("Adams", pb.head->surname);
    TEST_ASSERT_EQUAL_STRING("Smith", pb.tail->surname);
    TEST_ASSERT_EQUAL_PTR(c3, pb.head->next);
    TEST_ASSERT_EQUAL_PTR(c1, pb.tail->prev);
    TEST_ASSERT_NULL(pb.head->prev);
    TEST_ASSERT_NULL(pb.tail->next);
}

void test_getContactCount_should_return_correct_count(void) {
    TEST_ASSERT_EQUAL_INT(0, getContactCount(&pb));

    for (int i = 0; i < 5; i++) {
        Contact *c = createContact();
        sprintf(c->surname, "Surname%d", i);
        sprintf(c->firstName, "Name%d", i);
        c->phoneCount = 0;
        c->emailCount = 0;
        c->socialCount = 0;
        c->messengerCount = 0;
        insertContact(&pb, c);
    }

    TEST_ASSERT_EQUAL_INT(5, getContactCount(&pb));

    removeContact(&pb, pb.head);
    TEST_ASSERT_EQUAL_INT(4, getContactCount(&pb));
}

void test_displayContacts_should_show_all_contacts(void) {
    Contact *c1 = createContact();
    strcpy(c1->surname, "Anderson");
    strcpy(c1->firstName, "Tom");
    strcpy(c1->workplace, "Microsoft");
    strcpy(c1->jobTitle, "Manager");
    strcpy(c1->phoneNumbers[0], "+1-555-111-2233");
    strcpy(c1->phoneNumbers[1], "+1-555-111-2244");
    c1->phoneCount = 2;
    strcpy(c1->emails[0], "tom@microsoft.com");
    c1->emailCount = 1;
    c1->socialCount = 0;
    c1->messengerCount = 0;
    insertContact(&pb, c1);
    
    Contact *c2 = createContact();
    strcpy(c2->surname, "Baker");
    strcpy(c2->firstName, "Sarah");
    strcpy(c2->workplace, "Apple");
    strcpy(c2->jobTitle, "Designer");
    strcpy(c2->phoneNumbers[0], "+1-555-222-3344");
    c2->phoneCount = 1;
    c2->emailCount = 0;
    strcpy(c2->socialLinks[0], "https://linkedin.com/in/sarah");
    c2->socialCount = 1;
    strcpy(c2->messengerProfiles[0], "@sarah_design");
    c2->messengerCount = 1;
    insertContact(&pb, c2);

    TEST_ASSERT_EQUAL_INT(2, pb.count);
    TEST_ASSERT_EQUAL_STRING("Anderson", pb.head->surname);
    TEST_ASSERT_EQUAL_STRING("Baker", pb.tail->surname);

    Contact *found1 = findContact(&pb, "Anderson", "Tom");
    TEST_ASSERT_NOT_NULL(found1);
    TEST_ASSERT_EQUAL_STRING("Microsoft", found1->workplace);
    TEST_ASSERT_EQUAL_STRING("Manager", found1->jobTitle);
    TEST_ASSERT_EQUAL_INT(2, found1->phoneCount);
    TEST_ASSERT_EQUAL_STRING("+1-555-111-2233", found1->phoneNumbers[0]);
    TEST_ASSERT_EQUAL_STRING("+1-555-111-2244", found1->phoneNumbers[1]);
    TEST_ASSERT_EQUAL_INT(1, found1->emailCount);
    TEST_ASSERT_EQUAL_STRING("tom@microsoft.com", found1->emails[0]);

    Contact *found2 = findContact(&pb, "Baker", "Sarah");
    TEST_ASSERT_NOT_NULL(found2);
    TEST_ASSERT_EQUAL_STRING("Apple", found2->workplace);
    TEST_ASSERT_EQUAL_STRING("Designer", found2->jobTitle);
    TEST_ASSERT_EQUAL_INT(1, found2->phoneCount);
    TEST_ASSERT_EQUAL_STRING("+1-555-222-3344", found2->phoneNumbers[0]);
    TEST_ASSERT_EQUAL_INT(1, found2->socialCount);
    TEST_ASSERT_EQUAL_STRING("https://linkedin.com/in/sarah", found2->socialLinks[0]);
    TEST_ASSERT_EQUAL_INT(1, found2->messengerCount);
    TEST_ASSERT_EQUAL_STRING("@sarah_design", found2->messengerProfiles[0]);
}

int main(void) {
    UNITY_BEGIN();
    
    RUN_TEST(test_addContact_should_add_contact_to_phonebook);
    RUN_TEST(test_findContact_should_find_existing_contact);
    RUN_TEST(test_removeContact_should_delete_contact_from_phonebook);
    RUN_TEST(test_getContactCount_should_return_correct_count);
    RUN_TEST(test_displayContacts_should_show_all_contacts);
    
    return UNITY_END();
}