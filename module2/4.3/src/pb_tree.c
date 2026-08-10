#include "pb_tree.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

int height(TreeNode *node) {
    return node ? node->height : 0;
}

int max(int a, int b) {
    return (a > b) ? a : b;
}

TreeNode* createNode(const Contact *contact) {
    TreeNode *node = (TreeNode*)malloc(sizeof(TreeNode));
    if (!node) {
        return NULL;
    }
    node->contact = *contact;
    node->left = NULL;
    node->right = NULL;
    node->height = 1;
    return node;
}

TreeNode* rotateRight(TreeNode *y) {
    TreeNode *x = y->left;
    TreeNode *T2 = x->right;

    x->right = y;
    y->left = T2;

    y->height = max(height(y->left), height(y->right)) + 1;
    x->height = max(height(x->left), height(x->right)) + 1;

    return x;
}

TreeNode* rotateLeft(TreeNode *x) {
    TreeNode *y = x->right;
    TreeNode *T2 = y->left;

    y->left = x;
    x->right = T2;

    x->height = max(height(x->left), height(x->right)) + 1;
    y->height = max(height(y->left), height(y->right)) + 1;

    return y;
}

int getBalance(TreeNode *node) {
    return node ? height(node->left) - height(node->right) : 0;
}

int compareContacts(const Contact *a, const Contact *b) {
    int surnameCmp = strcmp(a->surname, b->surname);
    if (surnameCmp != 0) return surnameCmp;
    return strcmp(a->firstName, b->firstName);
}

TreeNode* insertNode(TreeNode *node, const Contact *contact, int *inserted) {
    if (!node) {
        *inserted = 1;
        return createNode(contact);
    }

    int cmp = compareContacts(contact, &node->contact);
    
    if (cmp < 0) {
        node->left = insertNode(node->left, contact, inserted);
    } else if (cmp > 0) {
        node->right = insertNode(node->right, contact, inserted);
    } else {
        *inserted = 0;
        return node;
    }

    node->height = 1 + max(height(node->left), height(node->right));

    int balance = getBalance(node);

    if (balance > 1 && compareContacts(contact, &node->left->contact) < 0) {
        return rotateRight(node);
    }

    if (balance < -1 && compareContacts(contact, &node->right->contact) > 0) {
        return rotateLeft(node);
    }

    if (balance > 1 && compareContacts(contact, &node->left->contact) > 0) {
        node->left = rotateLeft(node->left);
        return rotateRight(node);
    }

    if (balance < -1 && compareContacts(contact, &node->right->contact) < 0) {
        node->right = rotateRight(node->right);
        return rotateLeft(node);
    }

    return node;
}

TreeNode* findMinNode(TreeNode *node) {
    TreeNode *current = node;
    while (current && current->left) {
        current = current->left;
    }
    return current;
}

TreeNode* deleteNode(TreeNode *node, const char *surname, const char *firstName, int *deleted) {
    if (!node) {
        *deleted = 0;
        return NULL;
    }

    Contact searchContact;
    strcpy(searchContact.surname, surname);
    strcpy(searchContact.firstName, firstName);
    
    int cmp = compareContacts(&searchContact, &node->contact);

    if (cmp < 0) {
        node->left = deleteNode(node->left, surname, firstName, deleted);
    } else if (cmp > 0) {
        node->right = deleteNode(node->right, surname, firstName, deleted);
    } else {
        *deleted = 1;
        
        if (!node->left) {
            TreeNode *temp = node->right;
            free(node);
            return temp;
        } else if (!node->right) {
            TreeNode *temp = node->left;
            free(node);
            return temp;
        }

        TreeNode *temp = findMinNode(node->right);
        node->contact = temp->contact;
        node->right = deleteNode(node->right, temp->contact.surname, temp->contact.firstName, deleted);
    }

    if (!node) return NULL;

    node->height = 1 + max(height(node->left), height(node->right));

    int balance = getBalance(node);

    if (balance > 1 && getBalance(node->left) >= 0) {
        return rotateRight(node);
    }

    if (balance > 1 && getBalance(node->left) < 0) {
        node->left = rotateLeft(node->left);
        return rotateRight(node);
    }

    if (balance < -1 && getBalance(node->right) <= 0) {
        return rotateLeft(node);
    }

    if (balance < -1 && getBalance(node->right) > 0) {
        node->right = rotateRight(node->right);
        return rotateLeft(node);
    }

    return node;
}

TreeNode* findNode(TreeNode *node, const char *surname, const char *firstName) {
    if (!node) return NULL;
    
    Contact searchContact;
    strcpy(searchContact.surname, surname);
    strcpy(searchContact.firstName, firstName);
    
    int cmp = compareContacts(&searchContact, &node->contact);
    
    if (cmp < 0) {
        return findNode(node->left, surname, firstName);
    } else if (cmp > 0) {
        return findNode(node->right, surname, firstName);
    } else {
        return node;
    }
}

int findContactIndex(const PhoneBook *pb, const char *surname, const char *firstName) {
    TreeNode *node = findNode(pb->root, surname, firstName);
    return node ? 0 : -1;
}

int updateContact(PhoneBook *pb, const char *surname, const char *firstName, const Contact *updatedData) {
    TreeNode *node = findNode(pb->root, surname, firstName);
    if (!node || !updatedData) {
        return 0;
    }
    
    // Обновляем все поля контакта
    strcpy(node->contact.surname, updatedData->surname);
    strcpy(node->contact.firstName, updatedData->firstName);
    strcpy(node->contact.workplace, updatedData->workplace);
    strcpy(node->contact.jobTitle, updatedData->jobTitle);
    
    node->contact.phoneCount = updatedData->phoneCount;
    for (int i = 0; i < updatedData->phoneCount; i++) {
        strcpy(node->contact.phoneNumbers[i], updatedData->phoneNumbers[i]);
    }
    
    node->contact.emailCount = updatedData->emailCount;
    for (int i = 0; i < updatedData->emailCount; i++) {
        strcpy(node->contact.emails[i], updatedData->emails[i]);
    }
    
    node->contact.socialCount = updatedData->socialCount;
    for (int i = 0; i < updatedData->socialCount; i++) {
        strcpy(node->contact.socialLinks[i], updatedData->socialLinks[i]);
    }
    
    node->contact.messengerCount = updatedData->messengerCount;
    for (int i = 0; i < updatedData->messengerCount; i++) {
        strcpy(node->contact.messengerProfiles[i], updatedData->messengerProfiles[i]);
    }
    
    return 1;
}

void traverseTree(TreeNode *node, Contact **contacts, int *index) {
    if (!node) return;
    
    traverseTree(node->left, contacts, index);
    if (contacts) {
        contacts[*index] = &node->contact;
    }
    (*index)++;
    traverseTree(node->right, contacts, index);
}

void initPhoneBook(PhoneBook *pb) {
    pb->root = NULL;
    pb->count = 0;
}

void clearInputBuffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

void generateTestContactsLogic(PhoneBook *pb, int count) {
    const char *surnames[] = {
        "Иванов", "Петров", "Сидоров", "Козлов", "Смирнов",
        "Волков", "Морозов", "Новиков", "Федоров", "Егоров",
        "Алексеев", "Дмитриев", "Николаев", "Михайлов", "Григорьев",
        "Андреев", "Васильев", "Сергеев", "Павлов", "Семенов"
    };
    
    const char *firstNames[] = {
        "Александр", "Дмитрий", "Максим", "Сергей", "Андрей",
        "Алексей", "Артем", "Илья", "Кирилл", "Михаил",
        "Никита", "Даниил", "Егор", "Матвей", "Иван",
        "Роман", "Олег", "Виктор", "Станислав", "Константин"
    };
    
    const char *workplaces[] = {
        "ООО Ромашка", "ЗАО ТехноСервис", "ОАО СтройИндустрия",
        "ИП Петров", "ООО ПромТех", "ЗАО МегаСтрой",
        "АО Инновации", "ООО Бизнес-Центр", "ЗАО Альянс",
        "ООО ТехноПром"
    };
    
    const char *jobTitles[] = {
        "Директор", "Менеджер", "Инженер", "Программист",
        "Бухгалтер", "Секретарь", "Водитель", "Строитель",
        "Аналитик", "Дизайнер", "Преподаватель", "Врач"
    };
    
    const char *phones[] = {
        "+7(495)123-45-67", "+7(495)234-56-78", "+7(495)345-67-89",
        "+7(495)456-78-90", "+7(495)567-89-01", "+7(495)678-90-12",
        "+7(495)789-01-23", "+7(495)890-12-34", "+7(495)901-23-45"
    };
    
    const char *emails[] = {
        "example@mail.ru", "test@yandex.ru", "user@gmail.com",
        "contact@inbox.ru", "info@company.ru", "admin@website.com"
    };
    
    int generated = 0;
    int attempts = 0;
    const int maxAttempts = count * 10;
    
    while (generated < count && attempts < maxAttempts) {
        attempts++;
        
        Contact newContact;
        int surnameIdx = rand() % (sizeof(surnames) / sizeof(surnames[0]));
        int firstNameIdx = rand() % (sizeof(firstNames) / sizeof(firstNames[0]));
        
        strcpy(newContact.surname, surnames[surnameIdx]);
        strcpy(newContact.firstName, firstNames[firstNameIdx]);

        TreeNode *existing = findNode(pb->root, newContact.surname, newContact.firstName);
        if (existing) continue;

        int workplaceIdx = rand() % (sizeof(workplaces) / sizeof(workplaces[0]));
        strcpy(newContact.workplace, workplaces[workplaceIdx]);
        
        int jobIdx = rand() % (sizeof(jobTitles) / sizeof(jobTitles[0]));
        strcpy(newContact.jobTitle, jobTitles[jobIdx]);

        newContact.phoneCount = rand() % 3 + 1;
        for (int i = 0; i < newContact.phoneCount; i++) {
            int phoneIdx = rand() % (sizeof(phones) / sizeof(phones[0]));
            strcpy(newContact.phoneNumbers[i], phones[phoneIdx]);
        }

        newContact.emailCount = rand() % 3;
        for (int i = 0; i < newContact.emailCount; i++) {
            int emailIdx = rand() % (sizeof(emails) / sizeof(emails[0]));
            strcpy(newContact.emails[i], emails[emailIdx]);
        }
        
        newContact.socialCount = (rand() % 4 == 0) ? 1 : 0;
        if (newContact.socialCount > 0) {
            strcpy(newContact.socialLinks[0], "https://vk.com/id123456");
        }
        
        newContact.messengerCount = (rand() % 4 == 0) ? 1 : 0;
        if (newContact.messengerCount > 0) {
            strcpy(newContact.messengerProfiles[0], "@username");
        }

        int inserted = 0;
        pb->root = insertNode(pb->root, &newContact, &inserted);
        if (inserted) {
            generated++;
            pb->count++;
        }
    }
}

void freeTree(TreeNode *node) {
    if (!node) return;
    freeTree(node->left);
    freeTree(node->right);
    free(node);
}

void freePhoneBook(PhoneBook *pb) {
    freeTree(pb->root);
    pb->root = NULL;
    pb->count = 0;
}