#ifndef CALC_H
#define CALC_H

#define MAX_NAME_LEN 50
#define MAX_WORK_LEN 50
#define MAX_JOB_LEN 50
#define MAX_PHONE_LEN 20
#define MAX_EMAIL_LEN 50
#define MAX_SOCIAL_LEN 100
#define MAX_MESSENGER_LEN 100

typedef struct {
    char surname[MAX_NAME_LEN];
    char firstName[MAX_NAME_LEN];
    char workplace[MAX_WORK_LEN];
    char jobTitle[MAX_JOB_LEN];
    char phoneNumbers[3][MAX_PHONE_LEN];
    int phoneCount;
    char emails[3][MAX_EMAIL_LEN];
    int emailCount;
    char socialLinks[3][MAX_SOCIAL_LEN];
    int socialCount;
    char messengerProfiles[3][MAX_MESSENGER_LEN];
    int messengerCount;
} Contact;

typedef struct TreeNode {
    Contact contact;
    struct TreeNode *left;
    struct TreeNode *right;
    int height;
} TreeNode;

typedef struct {
    TreeNode *root;
    int count;
} PhoneBook;

void initPhoneBook(PhoneBook *pb);
void freePhoneBook(PhoneBook *pb);

TreeNode* createNode(const Contact *contact);
TreeNode* insertNode(TreeNode *node, const Contact *contact, int *inserted);
TreeNode* deleteNode(TreeNode *node, const char *surname, const char *firstName, int *deleted);
TreeNode* findNode(TreeNode *node, const char *surname, const char *firstName);
int findContactIndex(const PhoneBook *pb, const char *surname, const char *firstName);
int updateContact(PhoneBook *pb, const char *surname, const char *firstName, const Contact *updatedData);
int compareContacts(const Contact *a, const Contact *b);
void generateTestContactsLogic(PhoneBook *pb, int count);

int height(TreeNode *node);
int max(int a, int b);
int getBalance(TreeNode *node);
TreeNode* rotateRight(TreeNode *y);
TreeNode* rotateLeft(TreeNode *x);
TreeNode* findMinNode(TreeNode *node);
void traverseTree(TreeNode *node, Contact **contacts, int *index);
void clearInputBuffer(void);

#endif