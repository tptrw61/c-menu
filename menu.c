#include "menu.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <errno.h>

#define MENU_FIXED 1
#define MENU_VARIABLE 2

#define DEFAULT_BUFFER_SIZE 80

//#define USE_MY_GETLINE

#if defined(_WIN32) || defined(_WIN64) || defined(__CYGWIN__)
#ifndef USE_MY_GETLINE
#define USE_MY_GETLINE
#endif//USE_MY_GETLINE
#define USING_WINDOWS
#define sscanf sscanf_s
#endif

typedef Menu_s MenuFixed;
typedef Menu_s MenuVariable;

typedef struct {
    char name[MENU_OPTION_NAME_SIZE];
    void (*func)(int, void *);
    int willCont;
} Item;

typedef struct Node {
    Item item;
    struct Node *next;
} Node;

struct GenericMenu_s {
    int type;
    int size;

    int max;
    Item *array;
    Node *list;
    Node *last;
};

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

static int mh_strIsNum(const char *s);
int64_t mh_getLine(char **buf, size_t *n, FILE *stream);
int64_t mh_manualGetLine(char **buf, size_t *n, FILE *stream);

Menu_s *menu_create(int size) {
    if (size >= 1) {
        return menu_createFixed(size);
    } else {
        return menu_createVariable();
    }
}

Menu_s *menu_createVariable(void) {
    MenuVariable *menu = (MenuVariable *)malloc(sizeof(MenuVariable));
    if (menu == NULL) {
        return NULL;
    }
    menu->last = menu->list = NULL;
    menu->size = 0;
    menu->type = MENU_VARIABLE;
    return (Menu_s *)menu;
}
Menu_s *menu_createFixed(int size) {
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
    return (Menu_s *)menu;
}

int menu_registerOption(Menu_s *menu, const char *optionName, void (*optionFunc)(int, void *)) {
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

int menu_registerExitOption(Menu_s *menu, const char *optionName, void (*optionFunc)(int, void *)) {
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

void menu_displayMenu(Menu_s *menu, void *param) {
    if (menu == NULL) {
        return;
    }
    if (menu->size == 0) { //theres no menu here
        return;
    }
    int choice;
    int cont = 0;
    do {
        if (menu->type == MENU_FIXED) {
            m_printMenuFixed((MenuFixed *)menu);
        } else if (menu->type == MENU_VARIABLE) {
            m_printMenuVariable((MenuVariable *) menu);
        }
        menuh_getIntRange("Enter your selection: ", &choice, 1, menu->size);
        if (menu->type == MENU_FIXED) {
            cont = m_chooseMenuFixed((MenuFixed *)menu, choice, param);
        } else if (menu->type == MENU_VARIABLE) {
            cont = m_chooseMenuVariable((MenuVariable *) menu, choice, param);
        }
    } while (cont);
}

void menu_clearMenu(Menu_s *menu) {
    if (menu == NULL) {
        return;
    }
    if (menu->type == MENU_FIXED) {
        m_clearMenuFixed((MenuFixed *)menu);
    } else if (menu->type == MENU_VARIABLE) {
        m_clearMenuVariable((MenuVariable *) menu);
    }
}

void menu_destroy(Menu_s *menu) {
    if (menu == NULL) {
        return;
    }
    if (menu->type == MENU_FIXED) {
        m_destroyFixed((MenuFixed *)menu);
    } else if (menu->type == MENU_VARIABLE) {
        m_destroyVariable((MenuVariable *) menu);
    }
}

void menu_dummy(int _1, void *_2) {}

int menu_size(Menu_s *menu) {
    return menu == NULL ? 0 : menu->size;
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
    (menu->size)++;
    return 1;
}


void m_printMenuFixed(MenuFixed *menu) {
    for (int i = 0; i < menu->size; i++) {
        printf("%d: %s\n", i+1, menu->array[i].name);
    }
}

void m_printMenuVariable(MenuVariable *menu) {
    Node *cur = menu->list;
    int i = 1;
    while (cur != NULL) {
        printf("%d: %s\n", i, cur->item.name);
        i++;
        cur = cur->next;
    }
}

//return 0 if its an exit function
int m_chooseMenuFixed(MenuFixed *menu, int choice, void *param) {
    int i = choice - 1; //choice is in the interval [1,size] not [0,size)
    menu->array[i].func(choice, param);
    return menu->array[i].willCont;
}

int m_chooseMenuVariable(MenuVariable *menu, int choice, void *param) {
    Node *cur = menu->list;
    int i = 1;
    while (i < choice) {
        i++;
        cur = cur->next;
    }
    cur->item.func(choice, param);
    return cur->item.willCont;
}


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



//TODO finish these
void menuh_getInt(const char *question, int *ptr) {
    //use getline, then check if string is a number, save number and return OR
    //repeat as necessary
    if (ptr == NULL) return;
    char *s = NULL;
    size_t n = 0;
    while (1) {
        if (question != NULL) {
            printf("%s", question);
        }
        mh_getLine(&s, &n, stdin);
        if (mh_strIsNum(s)) {
            sscanf(s, "%d", ptr);
            break;
        }
    }
    free(s);
}

void menuh_getIntRange(const char *question, int *ptr, int min, int max) {
    menuh_getInt(question, ptr);
    while (*ptr < min || *ptr > max) {
        menuh_getInt(question, ptr);
    }
}

int mh_strIsNum(const char *s) {
    if (s == NULL) return 0;
    //get to first non-whitespace character
    while (isspace(*s)) {
        s++;
    }
    if (*s == '\0') return 0; // if we hit the end theres no number
    if (isdigit(*s) || *s == '-') { //in case its negative
        s++;
    }
    while (isdigit(*s)) {
        s++;
    }
    if (*s == '\0') return 1; //hit the end the str after a number
    while (isspace(*s)) {
        s++;
    }
    return *s == '\0'; //if its not the end of the str, its something else and therefore not a number string
}

int64_t mh_getLine(char **buf, size_t *n, FILE *stream) {
#ifdef USE_MY_GETLINE
    return mh_manualGetLine(buf, n, stream);
#else
    return getline(buf, n, stream);
#endif
}

//errors EINVAL, ENOMEM
int64_t mh_manualGetLine(char **buf, size_t *n, FILE *stream) {
    if (buf == NULL || n == NULL || stream == NULL) { //set errno do to bad param
        errno = ENOMEM;
        return -1;
    }
    if (*buf != NULL && *n == 0) {
        errno = ENOMEM;
        return -1;
    }
    //check if buffer is null
    if (*buf == NULL) {
        *buf = malloc(DEFAULT_BUFFER_SIZE);
        if (*buf == NULL) {
            errno = ENOMEM;
            return -1;
        }
        *n = DEFAULT_BUFFER_SIZE;
    }
    size_t i = 0;
    int read;
    while (1) {
        //check if resize is required
        //doing this first cause ill likely have to put a '\0' at index i
        if (i == *n) {
            char *newbuf = realloc(*buf, *n + DEFAULT_BUFFER_SIZE);
            if (newbuf == NULL) {
                //should i move all read characters back to the stream using ungetc?
                errno = ENOMEM;
                return -1;
            }
            *n += DEFAULT_BUFFER_SIZE;
            *buf = newbuf;
        }
        read = fgetc(stream);
        if (read == EOF || read == '\n' || read == '\0') {
            break;
        } else {
            (*buf)[i] = (char)read;
            i++;
        }
    }
    (*buf)[i] = '\0';
    return i;
}
