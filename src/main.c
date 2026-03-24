#include "lib.h"

#include <stdio.h>

int main(void) {
    int num = PI;
    add_one(&num);
    printf("num=%d\n", num);

    printf("Hello, world!\n");
    return 0;
}
