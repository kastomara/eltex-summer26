#include "pb_queue.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void initPhoneBook(PhoneBook *pb) {
    pb->head = NULL;
    pb->tail = NULL;
    pb->count = 0;
}

void clearInputBuffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

Contact* createContact(void) {
    Contact *newContact = (Contact*)malloc(sizeof(Contact));
    if (newContact == NULL) {
        return NULL;
    }
    newContact->prev = NULL;
    newContact->next = NULL;
    newContact->phoneCount = 0;
    newContact->emailCount = 0;
    newContact->socialCount = 0;
    newContact->messengerCount = 0;

    newContact->surname[0] = '\0';
    newContact->firstName[0] = '\0';
    newContact->workplace[0] = '\0';
    newContact->jobTitle[0] = '\0';
    
    for (int i = 0; i < 3; i++) {
        newContact->phoneNumbers[i][0] = '\0';
        newContact->emails[i][0] = '\0';
        newContact->socialLinks[i][0] = '\0';
        newContact->messengerProfiles[i][0] = '\0';
    }
    
    return newContact;
}

int addContactLogic(PhoneBook *pb, Contact *newContact) {
    if (pb == NULL || newContact == NULL) {
        return 0;
    }
    
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
    return 1;
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

int deleteContactLogic(PhoneBook *pb, const char *surname, const char *firstName) {
    if (pb == NULL || pb->head == NULL) {
        return 0;
    }
    
    Contact *c = findContact(pb, surname, firstName);
    if (c == NULL) {
        return 0;
    }
    
    if (c->prev != NULL) {
        c->prev->next = c->next;
    } else {
        pb->head = c->next;
    }
    
    if (c->next != NULL) {
        c->next->prev = c->prev;
    } else {
        pb->tail = c->prev;
    }
    
    free(c);
    pb->count--;
    return 1;
}

int editContactLogic(PhoneBook *pb, const char *surname, const char *firstName, Contact *updatedData) {
    Contact *c = findContact(pb, surname, firstName);
    if (c == NULL || updatedData == NULL) {
        return 0;
    }

    strcpy(c->surname, updatedData->surname);
    strcpy(c->firstName, updatedData->firstName);
    strcpy(c->workplace, updatedData->workplace);
    strcpy(c->jobTitle, updatedData->jobTitle);
    
    c->phoneCount = updatedData->phoneCount;
    for (int i = 0; i < c->phoneCount; i++) {
        strcpy(c->phoneNumbers[i], updatedData->phoneNumbers[i]);
    }
    
    c->emailCount = updatedData->emailCount;
    for (int i = 0; i < c->emailCount; i++) {
        strcpy(c->emails[i], updatedData->emails[i]);
    }
    
    c->socialCount = updatedData->socialCount;
    for (int i = 0; i < c->socialCount; i++) {
        strcpy(c->socialLinks[i], updatedData->socialLinks[i]);
    }
    
    c->messengerCount = updatedData->messengerCount;
    for (int i = 0; i < c->messengerCount; i++) {
        strcpy(c->messengerProfiles[i], updatedData->messengerProfiles[i]);
    }
    
    return 1;
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