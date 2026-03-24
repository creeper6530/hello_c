#include <stdio.h>

#include "lib.h"

int main(void) {
    int num = PI;
    add_one(&num);
    printf("num=%d\n", num);

    printf("Hello, world!\n");
}
