#include <stdio.h>

#include "header.h"
#include "lib.c"

int main(void) {
    int num = PI;
    add_one(&num);
    printf("num=%d\n", num);

    printf("Hello, world!\n");
}
