// Automatically includes `result.h`
#include "vec.h"

#include <stdio.h>
#include <assert.h>

int main(void) {
    printf("Hello, world!\n");

    Vec array = vec_new();

    vec_push(&array, 20);
    vec_push(&array, 15);
    auto pop_res = vec_pop(&array);
    assert(result_unwrap__int(&pop_res) == 15);

    fprintf(stderr, "Shrinking to fit!\n");
    vec_shrink_to_fit(&array);
    vec_push(&array, 10);
    vec_push(&array, 5);

    auto res_len = vec_len(&array);
    size_t len = result_unwrap__size_t(&res_len);
    printf("Len: %zu \n", len);

    register ptrdiff_t slen = (ptrdiff_t) len;

    for (ptrdiff_t i = -slen; i < slen; i++) {
        auto res = vec_idx(&array, i);
        result_unwrap__intptr(&res);
    }

    vec_free(&array);
    return 0;
}
