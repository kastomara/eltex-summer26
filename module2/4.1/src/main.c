#include "calc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void displayMenu(void) {
    printf("\n=== CONTACT MANAGEMENT SYSTEM ===\n");
    printf("1. Add Contact\n");
    printf("2. View All Contacts\n");
    printf("3. Edit Contact\n");
    printf("4. Delete Contact\n");
    printf("5. Exit\n");
    printf("===================================\n");
    printf("Enter your choice: ");
}

void displayContactUI(const Contact *c) {
    if (c == NULL) return;
    
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

void displayContactsUI(const PhoneBook *pb) {
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
        displayContactUI(current);
        printf("------------------------\n");
        current = current->next;
    }
}

Contact* getContactDataFromUser(int isEdit, Contact *existingContact) {
    Contact *newContact = createContact();
    if (newContact == NULL) {
        printf("Memory allocation failed!\n");
        return NULL;
    }
    
    if (isEdit && existingContact != NULL) {
        strcpy(newContact->surname, existingContact->surname);
        strcpy(newContact->firstName, existingContact->firstName);
        strcpy(newContact->workplace, existingContact->workplace);
        strcpy(newContact->jobTitle, existingContact->jobTitle);
        newContact->phoneCount = existingContact->phoneCount;
        for (int i = 0; i < existingContact->phoneCount; i++) {
            strcpy(newContact->phoneNumbers[i], existingContact->phoneNumbers[i]);
        }
        newContact->emailCount = existingContact->emailCount;
        for (int i = 0; i < existingContact->emailCount; i++) {
            strcpy(newContact->emails[i], existingContact->emails[i]);
        }
        newContact->socialCount = existingContact->socialCount;
        for (int i = 0; i < existingContact->socialCount; i++) {
            strcpy(newContact->socialLinks[i], existingContact->socialLinks[i]);
        }
        newContact->messengerCount = existingContact->messengerCount;
        for (int i = 0; i < existingContact->messengerCount; i++) {
            strcpy(newContact->messengerProfiles[i], existingContact->messengerProfiles[i]);
        }
    }
    
    char buffer[MAX_NAME_LEN];
    char choice;
    
    if (isEdit) {
        printf("\nEnter new information (press Enter to keep current value):\n");
        
        printf("Surname [%s]: ", newContact->surname);
        fgets(buffer, MAX_NAME_LEN, stdin);
        buffer[strcspn(buffer, "\n")] = '\0';
        if (strlen(buffer) > 0) strcpy(newContact->surname, buffer);
        
        printf("First name [%s]: ", newContact->firstName);
        fgets(buffer, MAX_NAME_LEN, stdin);
        buffer[strcspn(buffer, "\n")] = '\0';
        if (strlen(buffer) > 0) strcpy(newContact->firstName, buffer);
        
        printf("Workplace [%s]: ", newContact->workplace);
        fgets(buffer, MAX_WORK_LEN, stdin);
        buffer[strcspn(buffer, "\n")] = '\0';
        if (strlen(buffer) > 0) strcpy(newContact->workplace, buffer);
        
        printf("Job title [%s]: ", newContact->jobTitle);
        fgets(buffer, MAX_JOB_LEN, stdin);
        buffer[strcspn(buffer, "\n")] = '\0';
        if (strlen(buffer) > 0) strcpy(newContact->jobTitle, buffer);
    } else {
        printf("\n--- Add New Contact ---\n");
        
        printf("Enter surname: ");
        fgets(newContact->surname, MAX_NAME_LEN, stdin);
        newContact->surname[strcspn(newContact->surname, "\n")] = '\0';
        
        printf("Enter first name: ");
        fgets(newContact->firstName, MAX_NAME_LEN, stdin);
        newContact->firstName[strcspn(newContact->firstName, "\n")] = '\0';
        
        printf("Enter workplace (optional, press Enter to skip): ");
        fgets(newContact->workplace, MAX_WORK_LEN, stdin);
        newContact->workplace[strcspn(newContact->workplace, "\n")] = '\0';
        
        printf("Enter job title (optional, press Enter to skip): ");
        fgets(newContact->jobTitle, MAX_JOB_LEN, stdin);
        newContact->jobTitle[strcspn(newContact->jobTitle, "\n")] = '\0';
    }
    
    if (isEdit) {
        printf("Edit phone numbers? (y/n): ");
        scanf("%c", &choice);
        clearInputBuffer();
        if (choice == 'y' || choice == 'Y') {
            newContact->phoneCount = 0;
        } else {
            // Сохраняем старые номера
            newContact->phoneCount = existingContact->phoneCount;
            for (int i = 0; i < existingContact->phoneCount; i++) {
                strcpy(newContact->phoneNumbers[i], existingContact->phoneNumbers[i]);
            }
            return newContact;
        }
    } else {
        newContact->phoneCount = 0;
    }
    
    printf("Enter phone numbers (max 3, enter empty line to stop):\n");
    for (int i = 0; i < 3; i++) {
        printf("Phone %d: ", i + 1);
        fgets(newContact->phoneNumbers[i], MAX_PHONE_LEN, stdin);
        newContact->phoneNumbers[i][strcspn(newContact->phoneNumbers[i], "\n")] = '\0';
        if (strlen(newContact->phoneNumbers[i]) == 0) break;
        newContact->phoneCount++;
    }
    
    if (isEdit) {
        printf("Edit email addresses? (y/n): ");
        scanf("%c", &choice);
        clearInputBuffer();
        if (choice == 'y' || choice == 'Y') {
            newContact->emailCount = 0;
        } else {
            newContact->emailCount = existingContact->emailCount;
            for (int i = 0; i < existingContact->emailCount; i++) {
                strcpy(newContact->emails[i], existingContact->emails[i]);
            }
            return newContact;
        }
    } else {
        newContact->emailCount = 0;
    }
    
    printf("Enter email addresses (max 3, enter empty line to stop):\n");
    for (int i = 0; i < 3; i++) {
        printf("Email %d: ", i + 1);
        fgets(newContact->emails[i], MAX_EMAIL_LEN, stdin);
        newContact->emails[i][strcspn(newContact->emails[i], "\n")] = '\0';
        if (strlen(newContact->emails[i]) == 0) break;
        newContact->emailCount++;
    }
    
    if (isEdit) {
        printf("Edit social media links? (y/n): ");
        scanf("%c", &choice);
        clearInputBuffer();
        if (choice == 'y' || choice == 'Y') {
            newContact->socialCount = 0;
        } else {
            newContact->socialCount = existingContact->socialCount;
            for (int i = 0; i < existingContact->socialCount; i++) {
                strcpy(newContact->socialLinks[i], existingContact->socialLinks[i]);
            }
            printf("Edit messenger profiles? (y/n): ");
            scanf("%c", &choice);
            clearInputBuffer();
            if (choice == 'y' || choice == 'Y') {
                newContact->messengerCount = 0;
            } else {
                newContact->messengerCount = existingContact->messengerCount;
                for (int i = 0; i < existingContact->messengerCount; i++) {
                    strcpy(newContact->messengerProfiles[i], existingContact->messengerProfiles[i]);
                }
                return newContact;
            }
        }
    } else {
        newContact->socialCount = 0;
    }
    
    if (!isEdit || (isEdit && choice == 'y')) {
        printf("Enter social media links (max 3, enter empty line to stop):\n");
        for (int i = 0; i < 3; i++) {
            printf("Social link %d: ", i + 1);
            fgets(newContact->socialLinks[i], MAX_SOCIAL_LEN, stdin);
            newContact->socialLinks[i][strcspn(newContact->socialLinks[i], "\n")] = '\0';
            if (strlen(newContact->socialLinks[i]) == 0) break;
            newContact->socialCount++;
        }
    }
    
    if (isEdit) {
        printf("Edit messenger profiles? (y/n): ");
        scanf("%c", &choice);
        clearInputBuffer();
        if (choice == 'y' || choice == 'Y') {
            newContact->messengerCount = 0;
        } else {
            newContact->messengerCount = existingContact->messengerCount;
            for (int i = 0; i < existingContact->messengerCount; i++) {
                strcpy(newContact->messengerProfiles[i], existingContact->messengerProfiles[i]);
            }
            return newContact;
        }
    } else {
        newContact->messengerCount = 0;
    }
    
    printf("Enter messenger profiles (max 3, enter empty line to stop):\n");
    for (int i = 0; i < 3; i++) {
        printf("Messenger %d: ", i + 1);
        fgets(newContact->messengerProfiles[i], MAX_MESSENGER_LEN, stdin);
        newContact->messengerProfiles[i][strcspn(newContact->messengerProfiles[i], "\n")] = '\0';
        if (strlen(newContact->messengerProfiles[i]) == 0) break;
        newContact->messengerCount++;
    }
    
    return newContact;
}

void addContactUI(PhoneBook *pb) {
    Contact *newContact = getContactDataFromUser(0, NULL);
    if (newContact == NULL) return;
    
    if (addContactLogic(pb, newContact)) {
        printf("\nContact added successfully!\n");
    } else {
        printf("Failed to add contact!\n");
        free(newContact);
    }
}

void editContactUI(PhoneBook *pb) {
    if (pb->head == NULL) {
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
    
    Contact *existing = findContact(pb, surname, firstName);
    if (existing == NULL) {
        printf("Contact not found!\n");
        return;
    }
    
    printf("\nCurrent contact information:\n");
    displayContactUI(existing);
    
    Contact *updatedData = getContactDataFromUser(1, existing);
    if (updatedData == NULL) return;
    
    if (editContactLogic(pb, surname, firstName, updatedData)) {
        printf("\nContact updated successfully!\n");
    } else {
        printf("Failed to update contact!\n");
    }
    free(updatedData);
}

void deleteContactUI(PhoneBook *pb) {
    if (pb->head == NULL) {
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
    
    Contact *c = findContact(pb, surname, firstName);
    if (c == NULL) {
        printf("Contact not found!\n");
        return;
    }
    
    printf("\nContact to delete:\n");
    displayContactUI(c);
    
    printf("\nAre you sure you want to delete this contact? (y/n): ");
    char choice;
    scanf("%c", &choice);
    clearInputBuffer();
    
    if (choice == 'y' || choice == 'Y') {
        if (deleteContactLogic(pb, surname, firstName)) {
            printf("Contact deleted successfully!\n");
        } else {
            printf("Failed to delete contact!\n");
        }
    } else {
        printf("Deletion cancelled.\n");
    }
}

int main() {
    PhoneBook phoneBook;
    initPhoneBook(&phoneBook);
    
    int choice;
    
    printf("Welcome to Contact Management System!\n");
    
    while (1) {
        displayMenu();
        scanf("%d", &choice);
        clearInputBuffer();
        
        switch (choice) {
            case 1:
                addContactUI(&phoneBook);
                break;
            case 2:
                displayContactsUI(&phoneBook);
                break;
            case 3:
                editContactUI(&phoneBook);
                break;
            case 4:
                deleteContactUI(&phoneBook);
                break;
            case 5:
                printf("\nExiting program. Goodbye!\n");
                freePhoneBook(&phoneBook);
                return 0;
            default:
                printf("Invalid choice! Please try again.\n");
                break;
        }
    }
    
    return 0;
}