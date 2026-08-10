<<<<<<< HEAD
#include "pb_tree.h"
=======
#include "calc.h"
>>>>>>> a2f58e574dc0213e328b25463792a31c11913793
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

void displayMenu(void) {
    printf("\n=== CONTACT MANAGEMENT SYSTEM ===\n");
    printf("1. Add Contact\n");
    printf("2. View All Contacts\n");
    printf("3. Edit Contact\n");
    printf("4. Delete Contact\n");
    printf("5. Generate Test Contacts\n");
    printf("6. Visualize Tree\n");
    printf("7. Exit\n");
    printf("===================================\n");
    printf("Enter your choice: ");
}

void displayContactUI(const Contact *c) {
    if (!c) return;
    
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
    if (pb->count == 0) {
        printf("\nPhone book is empty!\n");
        return;
    }
    
    printf("\n--- Phone Book Contacts ---\n");
    printf("Total contacts: %d\n\n", pb->count);

    Contact **contacts = (Contact**)malloc(pb->count * sizeof(Contact*));
    if (!contacts) {
        printf("Memory allocation error!\n");
        return;
    }
    
    int index = 0;
    traverseTree(pb->root, contacts, &index);
    
    for (int i = 0; i < pb->count; i++) {
        printf("Contact #%d:\n", i + 1);
        displayContactUI(contacts[i]);
        printf("------------------------\n");
    }
    
    free(contacts);
}

int getContactDataFromUser(Contact *contact, int isEdit, const Contact *existingContact) {
    char buffer[MAX_NAME_LEN];
    char choice;
    
    if (!contact) return 0;

    if (!isEdit) {
        contact->surname[0] = '\0';
        contact->firstName[0] = '\0';
        contact->workplace[0] = '\0';
        contact->jobTitle[0] = '\0';
        contact->phoneCount = 0;
        contact->emailCount = 0;
        contact->socialCount = 0;
        contact->messengerCount = 0;
    } else if (existingContact) {
        // Копируем существующие данные
        *contact = *existingContact;
    }
    
    if (isEdit && existingContact) {
        printf("\nEnter new information (press Enter to keep current value):\n");
        
        printf("Surname [%s]: ", contact->surname);
        fgets(buffer, MAX_NAME_LEN, stdin);
        buffer[strcspn(buffer, "\n")] = '\0';
        if (strlen(buffer) > 0) strcpy(contact->surname, buffer);
        
        printf("First name [%s]: ", contact->firstName);
        fgets(buffer, MAX_NAME_LEN, stdin);
        buffer[strcspn(buffer, "\n")] = '\0';
        if (strlen(buffer) > 0) strcpy(contact->firstName, buffer);
        
        printf("Workplace [%s]: ", contact->workplace);
        fgets(buffer, MAX_WORK_LEN, stdin);
        buffer[strcspn(buffer, "\n")] = '\0';
        if (strlen(buffer) > 0) strcpy(contact->workplace, buffer);
        
        printf("Job title [%s]: ", contact->jobTitle);
        fgets(buffer, MAX_JOB_LEN, stdin);
        buffer[strcspn(buffer, "\n")] = '\0';
        if (strlen(buffer) > 0) strcpy(contact->jobTitle, buffer);
    } else {
        printf("\n--- Add New Contact ---\n");
        
        printf("Enter surname: ");
        fgets(contact->surname, MAX_NAME_LEN, stdin);
        contact->surname[strcspn(contact->surname, "\n")] = '\0';
        
        printf("Enter first name: ");
        fgets(contact->firstName, MAX_NAME_LEN, stdin);
        contact->firstName[strcspn(contact->firstName, "\n")] = '\0';
        
        printf("Enter workplace (optional, press Enter to skip): ");
        fgets(contact->workplace, MAX_WORK_LEN, stdin);
        contact->workplace[strcspn(contact->workplace, "\n")] = '\0';
        
        printf("Enter job title (optional, press Enter to skip): ");
        fgets(contact->jobTitle, MAX_JOB_LEN, stdin);
        contact->jobTitle[strcspn(contact->jobTitle, "\n")] = '\0';
        
        contact->phoneCount = 0;
    }
    
    if (isEdit && existingContact) {
        printf("Edit phone numbers? (y/n): ");
        scanf("%c", &choice);
        clearInputBuffer();
        if (choice == 'y' || choice == 'Y') {
            contact->phoneCount = 0;
        } else {
            return 1; // Сохраняем старые номера
        }
    }
    
    printf("Enter phone numbers (max 3, enter empty line to stop):\n");
    for (int i = 0; i < 3; i++) {
        printf("Phone %d: ", i + 1);
        fgets(contact->phoneNumbers[i], MAX_PHONE_LEN, stdin);
        contact->phoneNumbers[i][strcspn(contact->phoneNumbers[i], "\n")] = '\0';
        if (strlen(contact->phoneNumbers[i]) == 0) break;
        contact->phoneCount++;
    }
    
    if (isEdit && existingContact) {
        printf("Edit email addresses? (y/n): ");
        scanf("%c", &choice);
        clearInputBuffer();
        if (choice == 'y' || choice == 'Y') {
            contact->emailCount = 0;
        } else {
            return 1;
        }
    } else {
        contact->emailCount = 0;
    }
    
    printf("Enter email addresses (max 3, enter empty line to stop):\n");
    for (int i = 0; i < 3; i++) {
        printf("Email %d: ", i + 1);
        fgets(contact->emails[i], MAX_EMAIL_LEN, stdin);
        contact->emails[i][strcspn(contact->emails[i], "\n")] = '\0';
        if (strlen(contact->emails[i]) == 0) break;
        contact->emailCount++;
    }
    
    if (isEdit && existingContact) {
        printf("Edit social media links? (y/n): ");
        scanf("%c", &choice);
        clearInputBuffer();
        if (choice == 'y' || choice == 'Y') {
            contact->socialCount = 0;
        } else {
            // Проверяем мессенджеры
            printf("Edit messenger profiles? (y/n): ");
            scanf("%c", &choice);
            clearInputBuffer();
            if (choice == 'y' || choice == 'Y') {
                contact->messengerCount = 0;
            } else {
                return 1;
            }
        }
    } else {
        contact->socialCount = 0;
    }
    
    if (!isEdit || (isEdit && (choice == 'y' || choice == 'Y'))) {
        printf("Enter social media links (max 3, enter empty line to stop):\n");
        for (int i = 0; i < 3; i++) {
            printf("Social link %d: ", i + 1);
            fgets(contact->socialLinks[i], MAX_SOCIAL_LEN, stdin);
            contact->socialLinks[i][strcspn(contact->socialLinks[i], "\n")] = '\0';
            if (strlen(contact->socialLinks[i]) == 0) break;
            contact->socialCount++;
        }
    }
    
    if (isEdit && existingContact) {
        printf("Edit messenger profiles? (y/n): ");
        scanf("%c", &choice);
        clearInputBuffer();
        if (choice == 'y' || choice == 'Y') {
            contact->messengerCount = 0;
        } else {
            return 1;
        }
    } else {
        contact->messengerCount = 0;
    }
    
    printf("Enter messenger profiles (max 3, enter empty line to stop):\n");
    for (int i = 0; i < 3; i++) {
        printf("Messenger %d: ", i + 1);
        fgets(contact->messengerProfiles[i], MAX_MESSENGER_LEN, stdin);
        contact->messengerProfiles[i][strcspn(contact->messengerProfiles[i], "\n")] = '\0';
        if (strlen(contact->messengerProfiles[i]) == 0) break;
        contact->messengerCount++;
    }
    
    return 1;
}

void addContactUI(PhoneBook *pb) {
    Contact newContact;
    memset(&newContact, 0, sizeof(Contact));
    
    if (!getContactDataFromUser(&newContact, 0, NULL)) {
        printf("Failed to get contact data!\n");
        return;
    }
    
    int inserted = 0;
    pb->root = insertNode(pb->root, &newContact, &inserted);
    if (inserted) {
        pb->count++;
        printf("\nContact added successfully!\n");
    } else {
        printf("\nContact with this name already exists!\n");
    }
}

void editContactUI(PhoneBook *pb) {
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
    
    TreeNode *node = findNode(pb->root, surname, firstName);
    if (!node) {
        printf("Contact not found!\n");
        return;
    }
    
    printf("\nCurrent contact information:\n");
    displayContactUI(&node->contact);
    
    Contact updatedData;
    memset(&updatedData, 0, sizeof(Contact));
    
    if (!getContactDataFromUser(&updatedData, 1, &node->contact)) {
        printf("Failed to get updated data!\n");
        return;
    }
    
    if (updateContact(pb, surname, firstName, &updatedData)) {
        printf("\nContact updated successfully!\n");
    } else {
        printf("Failed to update contact!\n");
    }
}

void deleteContactUI(PhoneBook *pb) {
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
    
    TreeNode *node = findNode(pb->root, surname, firstName);
    if (!node) {
        printf("Contact not found!\n");
        return;
    }
    
    printf("\nContact to delete:\n");
    displayContactUI(&node->contact);
    
    printf("\nAre you sure you want to delete this contact? (y/n): ");
    char choice;
    scanf("%c", &choice);
    clearInputBuffer();
    
    if (choice == 'y' || choice == 'Y') {
        int deleted = 0;
        pb->root = deleteNode(pb->root, surname, firstName, &deleted);
        if (deleted) {
            pb->count--;
            printf("Contact deleted successfully!\n");
        } else {
            printf("Error deleting contact!\n");
        }
    } else {
        printf("Deletion cancelled.\n");
    }
}

void generateTestContactsUI(PhoneBook *pb) {
    int count;
    printf("Enter number of test contacts to generate (1-100): ");
    scanf("%d", &count);
    clearInputBuffer();
    if (count < 1) count = 1;
    if (count > 100) count = 100;
    
    printf("\nGenerating %d test contacts...\n", count);
    generateTestContactsLogic(pb, count);
    printf("Successfully generated contacts! Total contacts: %d\n", pb->count);
}

void visualizeTreeRecursiveUI(TreeNode *node, int level, char *prefix, int isLast) {
    if (!node) return;

    printf("%s", prefix);

    if (level > 0) {
        if (isLast) {
            printf("└── ");
        } else {
            printf("├── ");
        }
    }

    printf("%s %s (h=%d, b=%d)\n", 
           node->contact.surname, 
           node->contact.firstName,
           node->height,
           getBalance(node));

    char newPrefix[1000];
    if (level == 0) {
        strcpy(newPrefix, "");
    } else {
        sprintf(newPrefix, "%s%s", prefix, isLast ? "    " : "│   ");
    }

    int hasLeft = node->left != NULL;
    int hasRight = node->right != NULL;
    
    if (hasLeft || hasRight) {
        if (hasLeft) {
            visualizeTreeRecursiveUI(node->left, level + 1, newPrefix, !hasRight);
        } else {
            printf("%s├── (empty)\n", newPrefix);
        }
        
        if (hasRight) {
            visualizeTreeRecursiveUI(node->right, level + 1, newPrefix, 1);
        } else {
            printf("%s└── (empty)\n", newPrefix);
        }
    }
}

void visualizeTreeUI(const PhoneBook *pb) {
    if (!pb->root) {
        printf("\nTree is empty!\n");
        return;
    }
    
    printf("\n=== BINARY TREE VISUALIZATION ===\n");
    printf("Format: Surname FirstName (height=h, balance=b)\n");
    printf("Legend: ├── left child, └── right child\n\n");
    
    visualizeTreeRecursiveUI(pb->root, 0, "", 1);
    
    printf("\nTotal nodes: %d\n", pb->count);
    printf("Tree height: %d\n", pb->root->height);
    printf("==================================\n");
}

int main() {
    PhoneBook phoneBook;
    initPhoneBook(&phoneBook);

    srand(time(NULL));
    
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
                generateTestContactsUI(&phoneBook);
                break;
            case 6:
                visualizeTreeUI(&phoneBook);
                break;
            case 7:
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