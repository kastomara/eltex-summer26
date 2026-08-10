#ifndef CALC_H
#define CALC_H

#define MAX_NAME_LEN 50
#define MAX_WORK_LEN 50
#define MAX_JOB_LEN 50
#define MAX_PHONE_LEN 20
#define MAX_EMAIL_LEN 50
#define MAX_SOCIAL_LEN 100
#define MAX_MESSENGER_LEN 100

typedef struct Contact {
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
    struct Contact *prev;
    struct Contact *next;
} Contact;

typedef struct {
    Contact *head;
    Contact *tail;
    int count;
} PhoneBook;

void initPhoneBook(PhoneBook *pb);
Contact* createContact(void);
void freePhoneBook(PhoneBook *pb);

int addContactLogic(PhoneBook *pb, Contact *newContact);
Contact* findContact(const PhoneBook *pb, const char *surname, const char *firstName);
int deleteContactLogic(PhoneBook *pb, const char *surname, const char *firstName);
int editContactLogic(PhoneBook *pb, const char *surname, const char *firstName, Contact *updatedData);

void clearInputBuffer(void);

#endif