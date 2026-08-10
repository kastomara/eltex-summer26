#include "calc.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

void initPhoneBook(PhoneBook *pb) {
    pb->count = 0;
}

void clearInputBuffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

void addContact(PhoneBook *pb) {
    if (pb->count >= MAX_CONTACTS) {
        printf("Phone book is full!\n");
        return;
    }

    Contact *newContact = &pb->contacts[pb->count];
    
    printf("\n--- Add New Contact ---\n");
    
    do {
        printf("Enter surname (required): ");
        fgets(newContact->surname, MAX_NAME_LEN, stdin);
        newContact->surname[strcspn(newContact->surname, "\n")] = '\0';
        if (strlen(newContact->surname) == 0) {
            printf("Surname cannot be empty! Please try again.\n");
        }
    } while (strlen(newContact->surname) == 0);
    
    do {
        printf("Enter first name (required): ");
        fgets(newContact->firstName, MAX_NAME_LEN, stdin);
        newContact->firstName[strcspn(newContact->firstName, "\n")] = '\0';
        if (strlen(newContact->firstName) == 0) {
            printf("First name cannot be empty! Please try again.\n");
        }
    } while (strlen(newContact->firstName) == 0);
    
    printf("Enter workplace (optional, press Enter to skip): ");
    fgets(newContact->workplace, MAX_WORK_LEN, stdin);
    newContact->workplace[strcspn(newContact->workplace, "\n")] = '\0';
    
    printf("Enter job title (optional, press Enter to skip): ");
    fgets(newContact->jobTitle, MAX_JOB_LEN, stdin);
    newContact->jobTitle[strcspn(newContact->jobTitle, "\n")] = '\0';
    
    newContact->phoneCount = 0;
    printf("Enter phone numbers (max 3, enter empty line to stop):\n");
    for (int i = 0; i < 3; i++) {
        printf("Phone %d: ", i + 1);
        fgets(newContact->phoneNumbers[i], MAX_PHONE_LEN, stdin);
        newContact->phoneNumbers[i][strcspn(newContact->phoneNumbers[i], "\n")] = '\0';
        if (strlen(newContact->phoneNumbers[i]) == 0) break;
        newContact->phoneCount++;
    }
    
    newContact->emailCount = 0;
    printf("Enter email addresses (max 3, enter empty line to stop):\n");
    for (int i = 0; i < 3; i++) {
        printf("Email %d: ", i + 1);
        fgets(newContact->emails[i], MAX_EMAIL_LEN, stdin);
        newContact->emails[i][strcspn(newContact->emails[i], "\n")] = '\0';
        if (strlen(newContact->emails[i]) == 0) break;
        newContact->emailCount++;
    }
    
    newContact->socialCount = 0;
    printf("Enter social media links (max 3, enter empty line to stop):\n");
    for (int i = 0; i < 3; i++) {
        printf("Social link %d: ", i + 1);
        fgets(newContact->socialLinks[i], MAX_SOCIAL_LEN, stdin);
        newContact->socialLinks[i][strcspn(newContact->socialLinks[i], "\n")] = '\0';
        if (strlen(newContact->socialLinks[i]) == 0) break;
        newContact->socialCount++;
    }
    
    newContact->messengerCount = 0;
    printf("Enter messenger profiles (max 3, enter empty line to stop):\n");
    for (int i = 0; i < 3; i++) {
        printf("Messenger %d: ", i + 1);
        fgets(newContact->messengerProfiles[i], MAX_MESSENGER_LEN, stdin);
        newContact->messengerProfiles[i][strcspn(newContact->messengerProfiles[i], "\n")] = '\0';
        if (strlen(newContact->messengerProfiles[i]) == 0) break;
        newContact->messengerCount++;
    }
    
    pb->count++;
    printf("\nContact added successfully!\n");
}

int findContactIndex(const PhoneBook *pb, const char *surname, const char *firstName) {
    for (int i = 0; i < pb->count; i++) {
        if (strcmp(pb->contacts[i].surname, surname) == 0 &&
            strcmp(pb->contacts[i].firstName, firstName) == 0) {
            return i;
        }
    }
    return -1;
}

void editContact(PhoneBook *pb) {
    if (pb->count == 0) {
        printf("Phone book is empty!\n");
        return;
    }
    
    char surname[MAX_NAME_LEN], firstName[MAX_NAME_LEN];
    printf("\n--- Edit Contact ---\n");
    printf("Enter surname of contact to edit: ");
    fgets(surname, MAX_NAME_LEN, stdin);
    surname[strcspn(surname, "\n")] = '\0';
    
    printf("Enter first name of contact to edit: ");
    fgets(firstName, MAX_NAME_LEN, stdin);
    firstName[strcspn(firstName, "\n")] = '\0';
    
    int index = findContactIndex(pb, surname, firstName);
    if (index == -1) {
        printf("Contact not found!\n");
        return;
    }
    
    Contact *c = &pb->contacts[index];
    printf("\nCurrent contact information:\n");
    displayContact(c);
    
    printf("\nEnter new information (press Enter to keep current value):\n");
    
    char buffer[MAX_NAME_LEN];
    
    printf("Surname [%s]: ", c->surname);
    fgets(buffer, MAX_NAME_LEN, stdin);
    buffer[strcspn(buffer, "\n")] = '\0';
    if (strlen(buffer) > 0) {
        strcpy(c->surname, buffer);
    } else {
        printf("Surname cannot be empty! Keeping current value.\n");
    }
    
    printf("First name [%s]: ", c->firstName);
    fgets(buffer, MAX_NAME_LEN, stdin);
    buffer[strcspn(buffer, "\n")] = '\0';
    if (strlen(buffer) > 0) {
        strcpy(c->firstName, buffer);
    } else {
        printf("First name cannot be empty! Keeping current value.\n");
    }
    
    printf("Workplace [%s]: ", c->workplace);
    fgets(buffer, MAX_WORK_LEN, stdin);
    buffer[strcspn(buffer, "\n")] = '\0';
    if (strlen(buffer) > 0) strcpy(c->workplace, buffer);
    
    printf("Job title [%s]: ", c->jobTitle);
    fgets(buffer, MAX_JOB_LEN, stdin);
    buffer[strcspn(buffer, "\n")] = '\0';
    if (strlen(buffer) > 0) strcpy(c->jobTitle, buffer);
    
    printf("Edit phone numbers? (y/n): ");
    char choice;
    scanf("%c", &choice);
    clearInputBuffer();
    if (choice == 'y' || choice == 'Y') {
        c->phoneCount = 0;
        printf("Enter new phone numbers (max 3, enter empty line to stop):\n");
        for (int i = 0; i < 3; i++) {
            printf("Phone %d: ", i + 1);
            fgets(c->phoneNumbers[i], MAX_PHONE_LEN, stdin);
            c->phoneNumbers[i][strcspn(c->phoneNumbers[i], "\n")] = '\0';
            if (strlen(c->phoneNumbers[i]) == 0) break;
            c->phoneCount++;
        }
    }
    
    printf("Edit email addresses? (y/n): ");
    scanf("%c", &choice);
    clearInputBuffer();
    if (choice == 'y' || choice == 'Y') {
        c->emailCount = 0;
        printf("Enter new email addresses (max 3, enter empty line to stop):\n");
        for (int i = 0; i < 3; i++) {
            printf("Email %d: ", i + 1);
            fgets(c->emails[i], MAX_EMAIL_LEN, stdin);
            c->emails[i][strcspn(c->emails[i], "\n")] = '\0';
            if (strlen(c->emails[i]) == 0) break;
            c->emailCount++;
        }
    }
    
    printf("\nContact updated successfully!\n");
}

void deleteContact(PhoneBook *pb) {
    if (pb->count == 0) {
        printf("Phone book is empty!\n");
        return;
    }
    
    char surname[MAX_NAME_LEN], firstName[MAX_NAME_LEN];
    printf("\n--- Delete Contact ---\n");
    printf("Enter surname of contact to delete: ");
    fgets(surname, MAX_NAME_LEN, stdin);
    surname[strcspn(surname, "\n")] = '\0';
    
    printf("Enter first name of contact to delete: ");
    fgets(firstName, MAX_NAME_LEN, stdin);
    firstName[strcspn(firstName, "\n")] = '\0';
    
    int index = findContactIndex(pb, surname, firstName);
    if (index == -1) {
        printf("Contact not found!\n");
        return;
    }
    
    printf("\nContact to delete:\n");
    displayContact(&pb->contacts[index]);
    
    printf("\nAre you sure you want to delete this contact? (y/n): ");
    char choice;
    scanf("%c", &choice);
    clearInputBuffer();
    
    if (choice == 'y' || choice == 'Y') {
        for (int i = index; i < pb->count - 1; i++) {
            pb->contacts[i] = pb->contacts[i + 1];
        }
        pb->count--;
        printf("Contact deleted successfully!\n");
    } else {
        printf("Deletion cancelled.\n");
    }
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
    if (pb->count == 0) {
        printf("\nPhone book is empty!\n");
        return;
    }
    
    printf("\n--- Phone Book Contacts ---\n");
    printf("Total contacts: %d\n\n", pb->count);
    
    for (int i = 0; i < pb->count; i++) {
        printf("Contact #%d:\n", i + 1);
        displayContact(&pb->contacts[i]);
        printf("------------------------\n");
    }
}
