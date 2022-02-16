#include "menu.h"
#include <stdlib.h>
#include <stdio.h>

#define MENU_FIXED 1
#define MENU_VARIABLE 2

struct menu_s {
    char type;
    int size;
};

typedef struct {
    char name[MENU_OPTION_NAME_SIZE];
    void (*func)(int, void *);
    int willCont;
} Item;

typedef struct {
    char type;
    int size;

    int max;
    Item *array;
} MenuFixed;

typedef struct Node {
    Item *item;
    struct Node *next;
} Node;

typedef struct {
    char type;
    int size;

    Node *list;
    Node *last;
} MenuVariable;

static int _registerOptionFixed(MenuFixed *menu, const char *name, void (*func)(int, void *), int willCont);
static int _registerOptionVariable(MenuVariable *menu, const char *name, void (*func)(int, void *), int willCont);

static void _printMenuFixed(MenuFixed *menu);
static void _printMenuVariable(MenuVariable *menu);

//return 0 if its an exit function
static int _chooseMenuFixed(MenuFixed *menu, int choice, void *param);
static int _chooseMenuVariable(MenuVariable *menu, int choice, void *param);

static void _clearMenuFixed(MenuFixed *menu);
static void _clearMenuVariable(MenuVariable *menu);

static void _destroyFixed(MenuFixed *menu);
static void _destroyVariable(MenuVariable *menu);

int menu_registerOption(Menu *menu, const char *optionName, void (*optionFunc)(int, void *)) {
    if (menu == NULL || optionName == NULL || optionFunc == NULL) {
        return 0;
    }
    char name[MENU_OPTION_NAME_SIZE];
    strncpy(name, optionName, MENU_OPTION_NAME_SIZE-1);
    name[MENU_OPTION_NAME_SIZE-1] = '\0';
    if (menu->type == MENU_FIXED) {
        return _registerOptionFixed((MenuFixed *)menu, name, optionFunc, 1);
    }
    if (menu->type == MENU_VARIABLE) {
        return _registerOptionVariable((MenuVariable *) menu, name, optionFunc, 1);
    }
    return 0;
}

int menu_registerExitOption(Menu *menu, const char *optionName, void (*optionFunc)(int, void *)) {
    if (menu == NULL || optionName == NULL || optionFunc == NULL) {
        return 0;
    }
    char name[MENU_OPTION_NAME_SIZE];
    strncpy(name, optionName, MENU_OPTION_NAME_SIZE-1);
    name[MENU_OPTION_NAME_SIZE-1] = '\0';
    if (menu->type == MENU_FIXED) {
        return _registerOptionFixed((MenuFixed *)menu, name, optionFunc, 0);
    }
    if (menu->type == MENU_VARIABLE) {
        return _registerOptionVariable((MenuVariable *) menu, name, optionFunc, 0);
    }
    return 0;
}

void menu_displayMenu(Menu *menu, void *param) {
    if (menu == NULL) {
        return;
    }
    int choice;
    int cont;
    do {
        if (menu->type == MENU_FIXED) {
            _printMenuFixed((MenuFixed *)menu);
        } else if (menu->type == MENU_VARIABLE) {
            _printMenuVariable((MenuVariable *) menu);
        }
        printf("Enter your selection: ");
        scanf("%d", &choice);
        if (choice < 0 || choice > menu->size) {
            printf("Invalid Choice\n");
            continue;
        }
        if (menu->type == MENU_FIXED) {
            cont = _chooseMenuFixed((MenuFixed *)menu, choice, param);
        } else if (menu->type == MENU_VARIABLE) {
            cont = _chooseMenuVariable((MenuVariable *) menu, choice, param);
        }
    } while (cont);
}

void menu_clearMenu(Menu *menu) {
    if (menu == NULL) {
        return;
    }
    if (menu->type == MENU_FIXED) {
        _clearMenuFixed((MenuFixed *)menu);
    } else if (menu->type == MENU_VARIABLE) {
        _clearMenuVariable((MenuVariable *) menu);
    }
}

void menu_destroy(Menu *menu) {
    if (menu == NULL) {
        return;
    }
    if (menu->type == MENU_FIXED) {
        _destroyFixed((MenuFixed *)menu);
    } else if (menu->type == MENU_VARIABLE) {
        _destroyVariable((MenuVariable *) menu);
    }
}

