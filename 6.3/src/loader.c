#include "calc.h"

static actionData* loadedActions = NULL;
static int loadedCount = 0;
static int capacity = 0;

typedef char (*get_symbol_func)(void);
typedef char* (*get_name_func)(void);
typedef action (*get_func_func)(void);

static int isLibraryFile(const char* filename) {
    const char* ext = strrchr(filename, '.');
    if (!ext) return 0;
    return (strcmp(ext, ".dylib") == 0) || 
           (strcmp(ext, ".so") == 0) || 
           (strcmp(ext, ".dll") == 0);
}

static int loadSingleLibrary(const char* libPath) {
    void* handle;
    get_symbol_func get_symbol;
    get_name_func get_name;
    get_func_func get_func;
    
    handle = dlopen(libPath, RTLD_LAZY);
    if (!handle) {
        fprintf(stderr, "Не удалось загрузить %s: %s\n", libPath, dlerror());
        return 0;
    }
    
    get_symbol = (get_symbol_func)dlsym(handle, "get_symbol");
    if (!get_symbol) {
        fprintf(stderr, "Не найден символ get_symbol в %s\n", libPath);
        dlclose(handle);
        return 0;
    }
    
    get_name = (get_name_func)dlsym(handle, "get_name");
    if (!get_name) {
        fprintf(stderr, "Не найден символ get_name в %s\n", libPath);
        dlclose(handle);
        return 0;
    }
    
    get_func = (get_func_func)dlsym(handle, "get_func");
    if (!get_func) {
        fprintf(stderr, "Не найден символ get_func в %s\n", libPath);
        dlclose(handle);
        return 0;
    }
    
    if (loadedCount >= capacity) {
        capacity = capacity == 0 ? 10 : capacity * 2;
        loadedActions = (actionData*)realloc(loadedActions, sizeof(actionData) * capacity);
        if (!loadedActions) {
            dlclose(handle);
            return 0;
        }
    }
    
    loadedActions[loadedCount].symb = get_symbol();
    loadedActions[loadedCount].name = strdup(get_name());
    loadedActions[loadedCount].fun = get_func();
    loadedActions[loadedCount].handle = handle;
    
    loadedCount++;
    
    printf("Загружена: %s (%c)\n", 
           loadedActions[loadedCount-1].name, 
           loadedActions[loadedCount-1].symb);
    
    return 1;
}

int loadLibraries(const char* libDir) {
    DIR* dir;
    struct dirent* entry;
    struct stat statbuf;
    char libPath[512];
    int loaded = 0;
    
    dir = opendir(libDir);
    if (!dir) {
        fprintf(stderr, "Не удалось открыть директорию: %s\n", libDir);
        return 0;
    }
    
    printf("\nЗагрузка библиотек из: %s\n", libDir);
    
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        
        snprintf(libPath, sizeof(libPath), "%s/%s", libDir, entry->d_name);
        
        if (stat(libPath, &statbuf) == 0 && S_ISREG(statbuf.st_mode)) {
            if (isLibraryFile(entry->d_name)) {
                if (loadSingleLibrary(libPath)) {
                    loaded++;
                }
            }
        }
    }
    
    closedir(dir);
    
    if (loaded == 0) {
        fprintf(stderr, "Не найдено ни одной библиотеки в %s\n", libDir);
        return 0;
    }
    
    printf("\nЗагружено библиотек: %d\n", loaded);
    return 1;
}

void unloadLibraries() {
    printf("\nВыгрузка библиотек...\n");
    for (int i = 0; i < loadedCount; i++) {
        if (loadedActions[i].handle) {
            dlclose(loadedActions[i].handle);
            printf("Выгружена: %s (%c)\n", loadedActions[i].name, loadedActions[i].symb);
        }
        if (loadedActions[i].name) {
            free(loadedActions[i].name);
        }
    }
    free(loadedActions);
    loadedActions = NULL;
    loadedCount = 0;
    capacity = 0;
    printf("Все библиотеки выгружены\n");
}

action findAct(char symb) {
    for (int i = 0; i < loadedCount; i++) {
        if (loadedActions[i].symb == symb) {
            return loadedActions[i].fun;
        }
    }
    return NULL;
}

char* findName(char symb) {
    for (int i = 0; i < loadedCount; i++) {
        if (loadedActions[i].symb == symb) {
            return loadedActions[i].name;
        }
    }
    return "Неизвестная операция";
}

int getActionCount() {
    return loadedCount;
}

char getActionSymbol(int index) {
    if (index < 0 || index >= loadedCount) {
        return '\0';
    }
    return loadedActions[index].symb;
}

char* getActionName(int index) {
    if (index < 0 || index >= loadedCount) {
        return "Неизвестная операция";
    }
    return loadedActions[index].name;
}

action getActionFunc(int index) {
    if (index < 0 || index >= loadedCount) {
        return NULL;
    }
    return loadedActions[index].fun;
}

void menu() {
    printf("\n===== КАЛЬКУЛЯТОР =====\n");
    for (int i = 0; i < loadedCount; i++) {
        printf("%d. %s (%c)\n", i + 1, loadedActions[i].name, loadedActions[i].symb);
    }
    printf("0. Выход\n");
    printf("=======================\n");
    printf("Или введите выражение: 2 + 3\n");
    printf("========================\n");
}