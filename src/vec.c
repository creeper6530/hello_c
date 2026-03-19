#include <stdlib.h>
#include <stddef.h>

#include "vec.h"

// Vector of int-s
typedef struct Vec {
    void* heap;
    size_t len; // In bytes
    size_t capacity; // In bytes
} Vec;

Vec vec_new(void) {
    Vec new_vec = {
        nullptr,
        0,
        0
    };
    return new_vec;
}

void vec_push(Vec* vec, int data) {
    if (vec == nullptr) { return; };

    if (vec->heap == nullptr) {
        
    } else if (vec->len + sizeof(data) > vec->capacity) {

    } else {
        
    }
}