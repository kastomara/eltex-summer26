#include "../src/list.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void initPhoneBook(PhoneBook *pb) {
    pb->head = NULL;
    pb->tail = NULL;
    pb->count = 0;
}

void freePhoneBook(PhoneBook *pb) {
    Contact *current = pb->head;
    while (current != NULL) {
        Contact *next = current->next;
        free(current);
        current = next;
    }
    pb->head = NULL;
    pb->tail = NULL;
    pb->count = 0;
}

Contact* createContact(void) {
    Contact *newContact = (Contact*)malloc(sizeof(Contact));
    if (newContact == NULL) {
        printf("Memory allocation failed!\n");
        return NULL;
    }
    newContact->prev = NULL;
    newContact->next = NULL;
    newContact->phoneCount = 0;
    newContact->emailCount = 0;
    newContact->socialCount = 0;
    newContact->messengerCount = 0;
    return newContact;
}

void insertContact(PhoneBook *pb, Contact *newContact) {
    if (pb->head == NULL) {
        pb->head = newContact;
        pb->tail = newContact;
    } else {
        Contact *current = pb->head;
        int compare = strcmp(newContact->surname, current->surname);
        if (compare < 0 || (compare == 0 && strcmp(newContact->firstName, current->firstName) < 0)) {
            newContact->next = pb->head;
            pb->head->prev = newContact;
            pb->head = newContact;
        } else {
            while (current->next != NULL) {
                compare = strcmp(newContact->surname, current->next->surname);
                if (compare < 0 || (compare == 0 && strcmp(newContact->firstName, current->next->firstName) < 0)) {
                    break;
                }
                current = current->next;
            }
            newContact->next = current->next;
            newContact->prev = current;
            if (current->next != NULL) {
                current->next->prev = newContact;
            } else {
                pb->tail = newContact;
            }
            current->next = newContact;
        }
    }
    pb->count++;
}

void removeContact(PhoneBook *pb, Contact *contact) {
    if (contact->prev != NULL) {
        contact->prev->next = contact->next;
    } else {
        pb->head = contact->next;
    }
    
    if (contact->next != NULL) {
        contact->next->prev = contact->prev;
    } else {
        pb->tail = contact->prev;
    }
    
    free(contact);
    pb->count--;
}

Contact* findContact(const PhoneBook *pb, const char *surname, const char *firstName) {
    Contact *current = pb->head;
    while (current != NULL) {
        if (strcmp(current->surname, surname) == 0 &&
            strcmp(current->firstName, firstName) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

void displayContact(const Contact *c) {
    printf("\nName: %s %s\n", c->firstName, c->surname);
    
    if (strlen(c->workplace) > 0) {
        printf("Workplace: %s\n", c->workplace);
    }
    if (strlen(c->jobTitle) > 0) {
        printf("Job Title: %s\n", c->jobTitle);
    }
    
    if (c->phoneCount > 0) {
        printf("Phone numbers:\n");
        for (int i = 0; i < c->phoneCount; i++) {
            printf("  - %s\n", c->phoneNumbers[i]);
        }
    }
    
    if (c->emailCount > 0) {
        printf("Email addresses:\n");
        for (int i = 0; i < c->emailCount; i++) {
            printf("  - %s\n", c->emails[i]);
        }
    }
    
    if (c->socialCount > 0) {
        printf("Social media links:\n");
        for (int i = 0; i < c->socialCount; i++) {
            printf("  - %s\n", c->socialLinks[i]);
        }
    }
    
    if (c->messengerCount > 0) {
        printf("Messenger profiles:\n");
        for (int i = 0; i < c->messengerCount; i++) {
            printf("  - %s\n", c->messengerProfiles[i]);
        }
    }
}

void displayContacts(const PhoneBook *pb) {
    if (pb->head == NULL) {
        printf("\nPhone book is empty!\n");
        return;
    }
    
    printf("\n--- Phone Book Contacts ---\n");
    printf("Total contacts: %d\n\n", pb->count);
    
    Contact *current = pb->head;
    int index = 1;
    while (current != NULL) {
        printf("Contact #%d:\n", index++);
        displayContact(current);
        printf("------------------------\n");
        current = current->next;
    }
}

int getContactCount(const PhoneBook *pb) {
    return pb->count;
}