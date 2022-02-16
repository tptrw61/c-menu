#ifndef MENU_H_INCLUDED
#define MENU_H_INCLUDED

#define MENU_OPTION_NAME_SIZE 32

typedef struct menu_s Menu;

//the 'exit' option is never required to be manually added
Menu *menu_createVariable(void);
Menu *menu_createFixed(int options);

//optionName is limited to MENU_OPTION_NAME_SIZE-1 chars
//optionsFunc gets passed int the chosen option along with a void* for any other parameters you want
int menu_registerOption(Menu *menu, const char *optionName, void (*optionFunc)(int, void *));

void menu_displayMenu(Menu *menu, void *param);

void menu_clearMenu(Menu *menu);

void menu_destroy(Menu *menu);

#endif