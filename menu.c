#include "menu.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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
    Item item;
    struct Node *next;
} Node;

typedef struct {
    char type;
    int size;

    Node *list;
    Node *last;
} MenuVariable;

static int m_registerOptionFixed(MenuFixed *menu, const char *name, void (*func)(int, void *), int willCont);
static int m_registerOptionVariable(MenuVariable *menu, const char *name, void (*func)(int, void *), int willCont);

static void m_printMenuFixed(MenuFixed *menu);
static void m_printMenuVariable(MenuVariable *menu);

//return 0 if its an exit function
static int m_chooseMenuFixed(MenuFixed *menu, int choice, void *param);
static int m_chooseMenuVariable(MenuVariable *menu, int choice, void *param);

static void m_clearMenuFixed(MenuFixed *menu);
static void m_clearMenuVariable(MenuVariable *menu);

static void m_destroyFixed(MenuFixed *menu);
static void m_destroyVariable(MenuVariable *menu);

Menu *menu_createVariable(void) {
    MenuVariable *menu = (MenuVariable *)malloc(sizeof(MenuVariable));
    if (menu == NULL) {
        return NULL;
    }
    menu->last = menu->list = NULL;
    menu->size = 0;
    menu->type = MENU_VARIABLE;
    return (Menu *)menu;
}
Menu *menu_createFixed(int size) {
    if (size <= 0) {
        return NULL;
    }
    MenuFixed *menu = (MenuFixed *)malloc(sizeof(MenuFixed));
    if (menu == NULL) {
        return NULL;
    }
    menu->array = (Item *)malloc(sizeof(Item) * size);
    if (menu->array == NULL) {
        free(menu);
        return NULL;
    }
    menu->max = size;
    menu->size = 0;
    menu->type = MENU_FIXED;
    return (Menu *)menu;
}

int menu_registerOption(Menu *menu, const char *optionName, void (*optionFunc)(int, void *)) {
    if (menu == NULL || optionName == NULL || optionFunc == NULL) {
        return 0;
    }
    char name[MENU_OPTION_NAME_SIZE];
    strncpy(name, optionName, MENU_OPTION_NAME_SIZE-1);
    name[MENU_OPTION_NAME_SIZE-1] = '\0';
    if (menu->type == MENU_FIXED) {
        return m_registerOptionFixed((MenuFixed *)menu, name, optionFunc, 1);
    }
    if (menu->type == MENU_VARIABLE) {
        return m_registerOptionVariable((MenuVariable *) menu, name, optionFunc, 1);
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
        return m_registerOptionFixed((MenuFixed *)menu, name, optionFunc, 0);
    }
    if (menu->type == MENU_VARIABLE) {
        return m_registerOptionVariable((MenuVariable *) menu, name, optionFunc, 0);
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
            m_printMenuFixed((MenuFixed *)menu);
        } else if (menu->type == MENU_VARIABLE) {
            m_printMenuVariable((MenuVariable *) menu);
        }
        printf("Enter your selection: ");
        scanf("%d", &choice);
        if (choice < 0 || choice > menu->size) {
            printf("Invalid Choice\n");
            continue;
        }
        if (menu->type == MENU_FIXED) {
            cont = m_chooseMenuFixed((MenuFixed *)menu, choice, param);
        } else if (menu->type == MENU_VARIABLE) {
            cont = m_chooseMenuVariable((MenuVariable *) menu, choice, param);
        }
    } while (cont);
}

void menu_clearMenu(Menu *menu) {
    if (menu == NULL) {
        return;
    }
    if (menu->type == MENU_FIXED) {
        m_clearMenuFixed((MenuFixed *)menu);
    } else if (menu->type == MENU_VARIABLE) {
        m_clearMenuVariable((MenuVariable *) menu);
    }
}

void menu_destroy(Menu *menu) {
    if (menu == NULL) {
        return;
    }
    if (menu->type == MENU_FIXED) {
        m_destroyFixed((MenuFixed *)menu);
    } else if (menu->type == MENU_VARIABLE) {
        m_destroyVariable((MenuVariable *) menu);
    }
}



int m_registerOptionFixed(MenuFixed *menu, const char *name, void (*func)(int, void *), int willCont) {
    if (menu->size == menu->max) {
        return 0;
    }
    strncpy(menu->array[menu->size].name, name, MENU_OPTION_NAME_SIZE);
    menu->array[menu->size].name[MENU_OPTION_NAME_SIZE] = '\0';
    menu->array[menu->size].func = func;
    menu->array[menu->size].willCont = willCont;
    (menu->size)++;
    return 1;
}

int m_registerOptionVariable(MenuVariable *menu, const char *name, void (*func)(int, void *), int willCont) {
    Node *newNode = (Node *)malloc(sizeof(Node));
    if (newNode == NULL) {
        return 0;
    }
    newNode->next = NULL;
    strncpy(newNode->item.name, name, MENU_OPTION_NAME_SIZE);
    newNode->item.name[MENU_OPTION_NAME_SIZE-1] = '\0';
    newNode->item.func = func;
    newNode->item.willCont = willCont;
    if (menu->size == 0) {
        menu->list = menu->last = newNode;
    } else {
        menu->last->next = newNode;
        menu->last = newNode;
    }
    return 1;
}

//TODO print & choose funcs

void m_clearMenuFixed(MenuFixed *menu) {
    menu->size = 0;
}

void m_clearMenuVariable(MenuVariable *menu) {
    Node *n;
    while (menu->list != NULL) {
        n = menu->list;
        menu->list = n->next;
        free(n);
    }
    menu->last = NULL;
    menu->size = 0;
}

void m_destroyFixed(MenuFixed *menu) {
    free(menu->array);
    free(menu);
}

void m_destroyVariable(MenuVariable *menu) {
    m_clearMenuVariable(menu);
    free(menu);
}
