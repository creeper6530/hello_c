#include <stdlib.h>
#include <stddef.h>

#include "vec.h" // Struct is in header (so that functions can return it by value)

Vec vec_new(void) {
    Vec new_vec = {nullptr, 0, 0};
    return new_vec;
}

Vec vec_with_capacity(size_t elements) {
    Vec new_vec = {nullptr, 0, 0};

    if (elements != 0) {
        size_t capacity = elements * sizeof(int);
        new_vec.data = malloc(capacity);
        
        if (new_vec.data != nullptr)
            new_vec.capacity = capacity;
    }

    return new_vec;
}

void vec_free(Vec *vec) {
    if (vec == nullptr) return;

    free(vec->data); // free(nullptr) is a no-op
    vec->data = nullptr;
    vec->len = 0;
    vec->capacity = 0;
}

size_t vec_len(Vec *vec) {
    if (vec == nullptr) return (size_t)-1; // TODO: Better error handling

    return vec->len;
}

void vec_push(Vec *vec, int data) {
    if (vec == nullptr) return;

    if (vec->data == nullptr) {
        return;
    } else if (vec->len + sizeof(data) > vec->capacity) {
        return;
    } else {
        return;
    }
}
