#include <stdio.h>

#include "vec.h"

int main(void) {
    printf("Hello, world!\n");

    Vec array = vec_new();
    vec_push(&array, 20);
    vec_push(&array, 15);

    auto len_result = vec_len(&array);
    size_t len = -1;
    if (len_result.state != Ok)
        return -1;
    else
        len = len_result.data.ok;
    printf("Len: %zu \n", len);

    vec_free(&array);
    return 0;
}
