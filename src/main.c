#include <stdio.h>

#include "vec.h"

int main(void) {
    printf("Hello, world!\n");

    Vec array = vec_with_capacity(2);
    array.data[0] = 20;
    array.data[1] = 15;

    vec_free(&array);
}
