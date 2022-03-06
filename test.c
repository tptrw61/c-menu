#include <stdio.h>
#include <stdlib.h>
#include "menu.h"

int size;

void optionFunc(int opNum, void *params) {
    int *count = (int *)params;
    count[opNum-1]++;
    printf("option %d chosen\n", opNum);
    printf("count: %d\n", count[opNum-1]);
    int total = 0;
    for (int i = 0; i < size; i++) {
        total += count[i];
    }
    printf("total count: %d\n\n", total);
}

int main(void) {
    Menu_s *menu = menu_create(0);
    if (menu == NULL) {
        fprintf(stderr, "That sucks\n");
        return 1;
    }
    menu_registerOption(menu, "A", optionFunc);
    menu_registerOption(menu, "B", optionFunc);
    menu_registerOption(menu, "C", optionFunc);
    size = menu_size(menu);
    int *count = malloc(sizeof(int) * size);
    if (count == NULL) {
        fprintf(stderr, "That also sucks\n");
        menu_destroy(menu);
        return 1;
    }
    for (int i = 0; i < size; i++) {
        count[i] = 0;
    }
    menu_registerExitOption(menu, "Exit", menu_dummy);
    menu_displayMenu(menu, count);
    printf("Done\n");
}