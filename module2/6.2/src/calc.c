#include "calc.h"
#include <stdio.h>
#include <string.h>

void clearInputBuffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

void addContact(PhoneBook *pb) {
    Contact *newContact = createContact();
    if (newContact == NULL) return;
    
    printf("\n--- Add New Contact ---\n");
    
    printf("Enter surname: ");
    fgets(newContact->surname, 50, stdin);
    newContact->surname[strcspn(newContact->surname, "\n")] = '\0';
    
    printf("Enter first name: ");
    fgets(newContact->firstName, 50, stdin);
    newContact->firstName[strcspn(newContact->firstName, "\n")] = '\0';
    
    printf("Enter workplace (optional, press Enter to skip): ");
    fgets(newContact->workplace, 50, stdin);
    newContact->workplace[strcspn(newContact->workplace, "\n")] = '\0';
    
    printf("Enter job title (optional, press Enter to skip): ");
    fgets(newContact->jobTitle, 50, stdin);
    newContact->jobTitle[strcspn(newContact->jobTitle, "\n")] = '\0';
    
    newContact->phoneCount = 0;
    printf("Enter phone numbers (max 3, enter empty line to stop):\n");
    for (int i = 0; i < 3; i++) {
        printf("Phone %d: ", i + 1);
        fgets(newContact->phoneNumbers[i], 20, stdin);
        newContact->phoneNumbers[i][strcspn(newContact->phoneNumbers[i], "\n")] = '\0';
        if (strlen(newContact->phoneNumbers[i]) == 0) break;
        newContact->phoneCount++;
    }
    
    newContact->emailCount = 0;
    printf("Enter email addresses (max 3, enter empty line to stop):\n");
    for (int i = 0; i < 3; i++) {
        printf("Email %d: ", i + 1);
        fgets(newContact->emails[i], 50, stdin);
        newContact->emails[i][strcspn(newContact->emails[i], "\n")] = '\0';
        if (strlen(newContact->emails[i]) == 0) break;
        newContact->emailCount++;
    }
    
    newContact->socialCount = 0;
    printf("Enter social media links (max 3, enter empty line to stop):\n");
    for (int i = 0; i < 3; i++) {
        printf("Social link %d: ", i + 1);
        fgets(newContact->socialLinks[i], 100, stdin);
        newContact->socialLinks[i][strcspn(newContact->socialLinks[i], "\n")] = '\0';
        if (strlen(newContact->socialLinks[i]) == 0) break;
        newContact->socialCount++;
    }
    
    newContact->messengerCount = 0;
    printf("Enter messenger profiles (max 3, enter empty line to stop):\n");
    for (int i = 0; i < 3; i++) {
        printf("Messenger %d: ", i + 1);
        fgets(newContact->messengerProfiles[i], 100, stdin);
        newContact->messengerProfiles[i][strcspn(newContact->messengerProfiles[i], "\n")] = '\0';
        if (strlen(newContact->messengerProfiles[i]) == 0) break;
        newContact->messengerCount++;
    }
    
    insertContact(pb, newContact);
    printf("\nContact added successfully!\n");
}

void editContact(PhoneBook *pb) {
    if (pb->head == NULL) {
        printf("Phone book is empty!\n");
        return;
    }
    
    char surname[50], firstName[50];
    printf("\n--- Edit Contact ---\n");
    printf("Enter surname of contact to edit: ");
    fgets(surname, 50, stdin);
    surname[strcspn(surname, "\n")] = '\0';
    
    printf("Enter first name of contact to edit: ");
    fgets(firstName, 50, stdin);
    firstName[strcspn(firstName, "\n")] = '\0';
    
    Contact *c = findContact(pb, surname, firstName);
    if (c == NULL) {
        printf("Contact not found!\n");
        return;
    }
    
    printf("\nCurrent contact information:\n");
    displayContact(c);
    
    printf("\nEnter new information (press Enter to keep current value):\n");
    
    char buffer[50];
    
    printf("Surname [%s]: ", c->surname);
    fgets(buffer, 50, stdin);
    buffer[strcspn(buffer, "\n")] = '\0';
    if (strlen(buffer) > 0) strcpy(c->surname, buffer);
    
    printf("First name [%s]: ", c->firstName);
    fgets(buffer, 50, stdin);
    buffer[strcspn(buffer, "\n")] = '\0';
    if (strlen(buffer) > 0) strcpy(c->firstName, buffer);
    
    printf("Workplace [%s]: ", c->workplace);
    fgets(buffer, 50, stdin);
    buffer[strcspn(buffer, "\n")] = '\0';
    if (strlen(buffer) > 0) strcpy(c->workplace, buffer);
    
    printf("Job title [%s]: ", c->jobTitle);
    fgets(buffer, 50, stdin);
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
            fgets(c->phoneNumbers[i], 20, stdin);
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
            fgets(c->emails[i], 50, stdin);
            c->emails[i][strcspn(c->emails[i], "\n")] = '\0';
            if (strlen(c->emails[i]) == 0) break;
            c->emailCount++;
        }
    }
    
    printf("\nContact updated successfully!\n");
}

void deleteContact(PhoneBook *pb) {
    if (pb->head == NULL) {
        printf("Phone book is empty!\n");
        return;
    }
    
    char surname[50], firstName[50];
    printf("\n--- Delete Contact ---\n");
    printf("Enter surname of contact to delete: ");
    fgets(surname, 50, stdin);
    surname[strcspn(surname, "\n")] = '\0';
    
    printf("Enter first name of contact to delete: ");
    fgets(firstName, 50, stdin);
    firstName[strcspn(firstName, "\n")] = '\0';
    
    Contact *c = findContact(pb, surname, firstName);
    if (c == NULL) {
        printf("Contact not found!\n");
        return;
    }
    
    printf("\nContact to delete:\n");
    displayContact(c);
    
    printf("\nAre you sure you want to delete this contact? (y/n): ");
    char choice;
    scanf("%c", &choice);
    clearInputBuffer();
    
    if (choice == 'y' || choice == 'Y') {
        removeContact(pb, c);
        printf("Contact deleted successfully!\n");
    } else {
        printf("Deletion cancelled.\n");
    }
}