#include <stdio.h>
#include "menu.h"

void optionFunc(int opNum, void *params) {
    int *count = (int *)params;
    ++*count;
    printf("option %d chosen\n", opNum);
    printf("count: %d\n\n", *count);
}

int main(void) {
    Menu *menu = menu_create(0);
    if (menu == NULL) {
        fprintf(stderr, "That sucks\n");
        return 1;
    }
    int count = 0;
    menu_registerOption(menu, "A", optionFunc);
    menu_registerOption(menu, "B", optionFunc);
    menu_registerOption(menu, "C", optionFunc);
    menu_registerExitOption(menu, "Exit", menu_dummy);
    menu_displayMenu(menu, &count);
    printf("Done\n");
}