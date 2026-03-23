#include "vec.h"

#include <stdio.h>
#include <assert.h>

int main(void) {
    printf("Hello, world!\n");

    Vec array = vec_new();

    vec_push(&array, 20);
    vec_push(&array, 15);
    auto pop_result = vec_pop(&array);
    assert(pop_result.state == Ok && pop_result.data.ok == 15);

    vec_push(&array, 10);
    vec_push(&array, 5);

    auto len_result = vec_len(&array);
    size_t len;

    if (len_result.state != Ok) {
        result_perror(len_result.data.err);
        vec_free(&array); // Mitigate a memory leak, discard result
        return -1;
    } else {
        len = len_result.data.ok;
    }

    printf("Len: %zu \n", len);

    register ptrdiff_t slen = (ptrdiff_t) len;

    for (ptrdiff_t i = -slen; i < slen; i++) {
        auto res = vec_idx(&array, i);

        assert(res.state == Ok);
        //fprintf(stderr, "%i\n", *res.data.ok);
    }

    vec_free(&array);
    return 0;
}
