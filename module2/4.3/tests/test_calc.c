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

void test_initPhoneBook_should_initialize_to_null(void) {
    TEST_ASSERT_NULL(pb.root);
    TEST_ASSERT_EQUAL_INT(0, pb.count);
}

void test_insertNode_should_insert_single_contact(void) {
    Contact contact;
    strcpy(contact.surname, "Smith");
    strcpy(contact.firstName, "John");
    contact.phoneCount = 0;
    contact.emailCount = 0;
    contact.socialCount = 0;
    contact.messengerCount = 0;
    
    int inserted = 0;
    pb.root = insertNode(pb.root, &contact, &inserted);
    pb.count = inserted ? 1 : 0;
    
    TEST_ASSERT_EQUAL_INT(1, inserted);
    TEST_ASSERT_EQUAL_INT(1, pb.count);
    TEST_ASSERT_NOT_NULL(pb.root);
    TEST_ASSERT_EQUAL_STRING("Smith", pb.root->contact.surname);
    TEST_ASSERT_EQUAL_STRING("John", pb.root->contact.firstName);
    TEST_ASSERT_EQUAL_INT(1, pb.root->height);
}

void test_insertNode_should_balance_tree(void) {
    Contact contacts[3];
    
    strcpy(contacts[0].surname, "C");
    strcpy(contacts[0].firstName, "Contact");
    contacts[0].phoneCount = 0;
    contacts[0].emailCount = 0;
    contacts[0].socialCount = 0;
    contacts[0].messengerCount = 0;
    
    strcpy(contacts[1].surname, "A");
    strcpy(contacts[1].firstName, "Contact");
    contacts[1].phoneCount = 0;
    contacts[1].emailCount = 0;
    contacts[1].socialCount = 0;
    contacts[1].messengerCount = 0;
    
    strcpy(contacts[2].surname, "B");
    strcpy(contacts[2].firstName, "Contact");
    contacts[2].phoneCount = 0;
    contacts[2].emailCount = 0;
    contacts[2].socialCount = 0;
    contacts[2].messengerCount = 0;
    
    int inserted = 0;
    pb.root = insertNode(pb.root, &contacts[0], &inserted);
    pb.root = insertNode(pb.root, &contacts[1], &inserted);
    pb.root = insertNode(pb.root, &contacts[2], &inserted);
    pb.count = 3;

    TEST_ASSERT_EQUAL_STRING("B", pb.root->contact.surname);
    TEST_ASSERT_EQUAL_STRING("A", pb.root->left->contact.surname);
    TEST_ASSERT_EQUAL_STRING("C", pb.root->right->contact.surname);
    TEST_ASSERT_EQUAL_INT(2, pb.root->height);
}

void test_findNode_should_return_correct_contact(void) {
    Contact contact1, contact2;
    strcpy(contact1.surname, "Adams");
    strcpy(contact1.firstName, "John");
    contact1.phoneCount = 0;
    contact1.emailCount = 0;
    contact1.socialCount = 0;
    contact1.messengerCount = 0;
    
    strcpy(contact2.surname, "Brown");
    strcpy(contact2.firstName, "Bob");
    contact2.phoneCount = 0;
    contact2.emailCount = 0;
    contact2.socialCount = 0;
    contact2.messengerCount = 0;
    
    int inserted = 0;
    pb.root = insertNode(pb.root, &contact1, &inserted);
    pb.root = insertNode(pb.root, &contact2, &inserted);
    pb.count = 2;
    
    TreeNode *found = findNode(pb.root, "Brown", "Bob");
    TEST_ASSERT_NOT_NULL(found);
    TEST_ASSERT_EQUAL_STRING("Brown", found->contact.surname);
    TEST_ASSERT_EQUAL_STRING("Bob", found->contact.firstName);
}

void test_findNode_should_return_null_when_not_found(void) {
    Contact contact;
    strcpy(contact.surname, "Smith");
    strcpy(contact.firstName, "John");
    contact.phoneCount = 0;
    contact.emailCount = 0;
    contact.socialCount = 0;
    contact.messengerCount = 0;
    
    int inserted = 0;
    pb.root = insertNode(pb.root, &contact, &inserted);
    pb.count = 1;
    
    TreeNode *found = findNode(pb.root, "Jones", "Mary");
    TEST_ASSERT_NULL(found);
}

void test_deleteNode_should_remove_leaf(void) {
    Contact contacts[3];
    strcpy(contacts[0].surname, "B");
    strcpy(contacts[0].firstName, "Contact");
    contacts[0].phoneCount = 0;
    contacts[0].emailCount = 0;
    contacts[0].socialCount = 0;
    contacts[0].messengerCount = 0;
    
    strcpy(contacts[1].surname, "A");
    strcpy(contacts[1].firstName, "Contact");
    contacts[1].phoneCount = 0;
    contacts[1].emailCount = 0;
    contacts[1].socialCount = 0;
    contacts[1].messengerCount = 0;
    
    strcpy(contacts[2].surname, "C");
    strcpy(contacts[2].firstName, "Contact");
    contacts[2].phoneCount = 0;
    contacts[2].emailCount = 0;
    contacts[2].socialCount = 0;
    contacts[2].messengerCount = 0;
    
    int inserted = 0;
    pb.root = insertNode(pb.root, &contacts[0], &inserted);
    pb.root = insertNode(pb.root, &contacts[1], &inserted);
    pb.root = insertNode(pb.root, &contacts[2], &inserted);
    pb.count = 3;
    
    int deleted = 0;
    pb.root = deleteNode(pb.root, "A", "Contact", &deleted);
    pb.count -= deleted ? 1 : 0;
    
    TEST_ASSERT_EQUAL_INT(1, deleted);
    TEST_ASSERT_EQUAL_INT(2, pb.count);
    TEST_ASSERT_NULL(pb.root->left);
    TEST_ASSERT_EQUAL_STRING("C", pb.root->right->contact.surname);
    TEST_ASSERT_EQUAL_INT(2, pb.root->height);
}

void test_deleteNode_should_remove_node_with_one_child(void) {
    Contact contacts[2];
    strcpy(contacts[0].surname, "B");
    strcpy(contacts[0].firstName, "Contact");
    contacts[0].phoneCount = 0;
    contacts[0].emailCount = 0;
    contacts[0].socialCount = 0;
    contacts[0].messengerCount = 0;
    
    strcpy(contacts[1].surname, "A");
    strcpy(contacts[1].firstName, "Contact");
    contacts[1].phoneCount = 0;
    contacts[1].emailCount = 0;
    contacts[1].socialCount = 0;
    contacts[1].messengerCount = 0;
    
    int inserted = 0;
    pb.root = insertNode(pb.root, &contacts[0], &inserted);
    pb.root = insertNode(pb.root, &contacts[1], &inserted);
    pb.count = 2;
    
    int deleted = 0;
    pb.root = deleteNode(pb.root, "B", "Contact", &deleted);
    pb.count -= deleted ? 1 : 0;
    
    TEST_ASSERT_EQUAL_INT(1, deleted);
    TEST_ASSERT_EQUAL_INT(1, pb.count);
    TEST_ASSERT_EQUAL_STRING("A", pb.root->contact.surname);
    TEST_ASSERT_NULL(pb.root->left);
    TEST_ASSERT_NULL(pb.root->right);
}

void test_deleteNode_should_remove_node_with_two_children(void) {
    Contact contacts[3];
    strcpy(contacts[0].surname, "B");
    strcpy(contacts[0].firstName, "Contact");
    contacts[0].phoneCount = 0;
    contacts[0].emailCount = 0;
    contacts[0].socialCount = 0;
    contacts[0].messengerCount = 0;
    
    strcpy(contacts[1].surname, "A");
    strcpy(contacts[1].firstName, "Contact");
    contacts[1].phoneCount = 0;
    contacts[1].emailCount = 0;
    contacts[1].socialCount = 0;
    contacts[1].messengerCount = 0;
    
    strcpy(contacts[2].surname, "C");
    strcpy(contacts[2].firstName, "Contact");
    contacts[2].phoneCount = 0;
    contacts[2].emailCount = 0;
    contacts[2].socialCount = 0;
    contacts[2].messengerCount = 0;
    
    int inserted = 0;
    pb.root = insertNode(pb.root, &contacts[0], &inserted);
    pb.root = insertNode(pb.root, &contacts[1], &inserted);
    pb.root = insertNode(pb.root, &contacts[2], &inserted);
    pb.count = 3;
    
    int deleted = 0;
    pb.root = deleteNode(pb.root, "B", "Contact", &deleted);
    pb.count -= deleted ? 1 : 0;
    
    TEST_ASSERT_EQUAL_INT(1, deleted);
    TEST_ASSERT_EQUAL_INT(2, pb.count);
    TEST_ASSERT_EQUAL_STRING("C", pb.root->contact.surname);
    TEST_ASSERT_EQUAL_STRING("A", pb.root->left->contact.surname);
    TEST_ASSERT_NULL(pb.root->right);
}

void test_updateContact_should_update_existing_contact(void) {
    Contact contact, updated;
    strcpy(contact.surname, "Smith");
    strcpy(contact.firstName, "John");
    strcpy(contact.workplace, "Old Company");
    contact.phoneCount = 0;
    contact.emailCount = 0;
    contact.socialCount = 0;
    contact.messengerCount = 0;
    
    int inserted = 0;
    pb.root = insertNode(pb.root, &contact, &inserted);
    pb.count = 1;
    
    strcpy(updated.surname, "Smith");
    strcpy(updated.firstName, "John");
    strcpy(updated.workplace, "New Company");
    strcpy(updated.jobTitle, "Manager");
    updated.phoneCount = 0;
    updated.emailCount = 0;
    updated.socialCount = 0;
    updated.messengerCount = 0;
    
    int result = updateContact(&pb, "Smith", "John", &updated);
    
    TEST_ASSERT_EQUAL_INT(1, result);
    TreeNode *found = findNode(pb.root, "Smith", "John");
    TEST_ASSERT_EQUAL_STRING("New Company", found->contact.workplace);
    TEST_ASSERT_EQUAL_STRING("Manager", found->contact.jobTitle);
}

void test_traverseTree_should_visit_all_nodes_in_order(void) {
    Contact contacts[3];
    strcpy(contacts[0].surname, "B");
    strcpy(contacts[0].firstName, "Contact1");
    contacts[0].phoneCount = 0;
    contacts[0].emailCount = 0;
    contacts[0].socialCount = 0;
    contacts[0].messengerCount = 0;
    
    strcpy(contacts[1].surname, "A");
    strcpy(contacts[1].firstName, "Contact2");
    contacts[1].phoneCount = 0;
    contacts[1].emailCount = 0;
    contacts[1].socialCount = 0;
    contacts[1].messengerCount = 0;
    
    strcpy(contacts[2].surname, "C");
    strcpy(contacts[2].firstName, "Contact3");
    contacts[2].phoneCount = 0;
    contacts[2].emailCount = 0;
    contacts[2].socialCount = 0;
    contacts[2].messengerCount = 0;
    
    int inserted = 0;
    pb.root = insertNode(pb.root, &contacts[0], &inserted);
    pb.root = insertNode(pb.root, &contacts[1], &inserted);
    pb.root = insertNode(pb.root, &contacts[2], &inserted);
    pb.count = 3;
    
    Contact **list = (Contact**)malloc(pb.count * sizeof(Contact*));
    int index = 0;
    traverseTree(pb.root, list, &index);
    
    TEST_ASSERT_EQUAL_INT(3, index);
    TEST_ASSERT_EQUAL_STRING("A", list[0]->surname);
    TEST_ASSERT_EQUAL_STRING("B", list[1]->surname);
    TEST_ASSERT_EQUAL_STRING("C", list[2]->surname);
    
    free(list);
}

int main(void) {
    UNITY_BEGIN();
    
    RUN_TEST(test_initPhoneBook_should_initialize_to_null);
    RUN_TEST(test_insertNode_should_insert_single_contact);
    RUN_TEST(test_insertNode_should_balance_tree);
    RUN_TEST(test_findNode_should_return_correct_contact);
    RUN_TEST(test_findNode_should_return_null_when_not_found);
    RUN_TEST(test_deleteNode_should_remove_leaf);
    RUN_TEST(test_deleteNode_should_remove_node_with_one_child);
    RUN_TEST(test_deleteNode_should_remove_node_with_two_children);
    RUN_TEST(test_updateContact_should_update_existing_contact);
    RUN_TEST(test_traverseTree_should_visit_all_nodes_in_order);
    
    return UNITY_END();
}