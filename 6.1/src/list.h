#ifndef LIST_H
#define LIST_H

typedef struct Contact {
    char surname[50];
    char firstName[50];
    char workplace[50];
    char jobTitle[50];
    char phoneNumbers[3][20];
    int phoneCount;
    char emails[3][50];
    int emailCount;
    char socialLinks[3][100];
    int socialCount;
    char messengerProfiles[3][100];
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
void freePhoneBook(PhoneBook *pb);
Contact* createContact(void);
void insertContact(PhoneBook *pb, Contact *newContact);
void removeContact(PhoneBook *pb, Contact *contact);
Contact* findContact(const PhoneBook *pb, const char *surname, const char *firstName);
void displayContact(const Contact *c);
void displayContacts(const PhoneBook *pb);
int getContactCount(const PhoneBook *pb);

#endif