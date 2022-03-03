#include <stdio.h>
#include "menu.h"

void optionFunc(int opNum, void *params) {
    printf("option %d chosen\n", opNum);
}
void dummy(int _1, void *_2) {}

int main(void) {
    Menu *menu = menu_create(0);
    if (menu == NULL) {
        fprintf(stderr, "That sucks\n");
        return 1;
    }
    menu_registerOption(menu, "A", optionFunc);
    menu_registerOption(menu, "B", optionFunc);
    menu_registerOption(menu, "C", optionFunc);
    menu_registerExitOption(menu, "Exit", dummy);
    menu_displayMenu(menu, NULL);
    printf("Done\n");
}