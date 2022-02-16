#include "menu.h"
#include <stdlib.h>

#define MENU_FIXED 1
#define MENU_VARIABLE 2

struct menu_s {
    char type;
};

typedef struct {
    char name[MENU_OPTION_NAME_SIZE];
    void (*func)(int, void *);
} Item;

typedef struct {
    char type;

    int size;
    int next;
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

static int _registerOptionFixed(MenuFixed *menu, const char *name, void (*func)(int, void *));
static int _registerOptionVariable(MenuVariable *menu, const char *name, void (*func)(int, void *));

static int _displayMenuFixed(MenuFixed *menu, void *param);
static int _displayMenuVariable(MenuVariable *menu, void *param);

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
        return _registerOptionFixed((MenuFixed *)menu, name, optionFunc);
    }
    if (menu->type == MENU_VARIABLE) {
        return _registerOptionVariable((MenuVariable *) menu, name, optionFunc);
    }
    return 0;
}

void menu_displayMenu(Menu *menu, void *param) {
    if (menu == NULL) {
        return 0;
    }
    if (menu->type == MENU_FIXED) {
        _displayMenuFixed((MenuFixed *)menu, param);
    } else if (menu->type == MENU_VARIABLE) {
        _displayMenuVariable((MenuVariable *) menu, param);
    }
}

void menu_clearMenu(Menu *menu) {
    if (menu == NULL) {
        return 0;
    }
    if (menu->type == MENU_FIXED) {
        _clearMenuFixed((MenuFixed *)menu);
    } else if (menu->type == MENU_VARIABLE) {
        _clearMenuVariable((MenuVariable *) menu);
    }
}

void menu_destroy(Menu *menu) {
    if (menu == NULL) {
        return 0;
    }
    if (menu->type == MENU_FIXED) {
        _destroyFixed((MenuFixed *)menu);
    } else if (menu->type == MENU_VARIABLE) {
        _destroyVariable((MenuVariable *) menu);
    }
}

