#ifndef MENU_H_INCLUDED
#define MENU_H_INCLUDED

#define MENU_OPTION_NAME_SIZE 32

typedef struct GenericMenu_s Menu_s;

//if size is 0 or negative, it will create a variable sized menu
//otherwise it will create a fixed sized menu
Menu_s *menu_create(int optionCount);

Menu_s *menu_createVariable(void);
Menu_s *menu_createFixed(int optionCount);

//optionName is limited to MENU_OPTION_NAME_SIZE-1 chars
//optionsFunc gets passed int the chosen option along with a void* for any other parameters you want
int menu_registerOption(Menu_s *menu, const char *optionName, void (*optionFunc)(int, void *));
//when this option is chosen, the menu will exit out of the menu loop AFTER optionFunc is called
int menu_registerExitOption(Menu_s *menu, const char *optionName, void (*optionFunc)(int, void *));

//edit menu option functions will go here
int menu_editOptionName(Menu_s *menu, int optionNum, const char *newName);

void menu_displayMenu(Menu_s *menu, void *param);

void menu_clearMenu(Menu_s *menu);

void menu_destroy(Menu_s *menu);

void menu_dummy(int, void *);

int menu_size(Menu_s *menu);

//some helper stuff

void menuh_getInt(const char *question, int *ptr);
//gets an int in the range [min, max]
void menuh_getIntRange(const char *question, int *ptr, int min, int max);

#endif