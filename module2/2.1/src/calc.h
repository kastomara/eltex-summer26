#ifndef CALC_H
#define CALC_H

#define MAX_CONTACTS 100
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

typedef struct {
    Contact contacts[MAX_CONTACTS];
    int count;
} PhoneBook;

void initPhoneBook(PhoneBook *pb);
void addContact(PhoneBook *pb);
void editContact(PhoneBook *pb);
void deleteContact(PhoneBook *pb);
void displayContacts(const PhoneBook *pb);
void displayContact(const Contact *c);
int findContactIndex(const PhoneBook *pb, const char *surname, const char *firstName);
void clearInputBuffer(void);

#endif