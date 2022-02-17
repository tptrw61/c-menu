#ifndef MENU_H_INCLUDED
#define MENU_H_INCLUDED

#define MENU_OPTION_NAME_SIZE 32

typedef struct menu_s Menu;

//if size is 0 or negative, it will create a variable sized menu
//otherwise it will create a fixed sized menu
Menu *menu_create(int optionCount);

Menu *menu_createVariable(void);
Menu *menu_createFixed(int optionCount);

//optionName is limited to MENU_OPTION_NAME_SIZE-1 chars
//optionsFunc gets passed int the chosen option along with a void* for any other parameters you want
int menu_registerOption(Menu *menu, const char *optionName, void (*optionFunc)(int, void *));
//when this option is chosen, the menu will exit out of the menu loop AFTER optionFunc is called
int menu_registerExitOption(Menu *menu, const char *optionName, void (*optionFunc)(int, void *));

void menu_displayMenu(Menu *menu, void *param);

void menu_clearMenu(Menu *menu);

void menu_destroy(Menu *menu);

#endif