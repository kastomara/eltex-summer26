<<<<<<< HEAD
#include "phonebook.h"
=======
#include "calc.h"
>>>>>>> a2f58e574dc0213e328b25463792a31c11913793
#include <stdio.h>
#include <stdlib.h>

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
                addContact(&phoneBook);
                break;
            case 2:
                displayContacts(&phoneBook);
                break;
            case 3:
                editContact(&phoneBook);
                break;
            case 4:
                deleteContact(&phoneBook);
                break;
            case 5:
                printf("\nExiting program. Goodbye!\n");
                return 0;
            default:
                printf("Invalid choice! Please try again.\n");
                break;
        }
    }
    
    return 0;
}