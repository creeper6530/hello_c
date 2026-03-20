#ifndef VEC_H_
#define VEC_H_

#define ALLOC_START ((size_t)32)

// Invariants:
// If (data == nullptr), then (capacity == 0) and (len == 0)
typedef struct Vec {
    int *data;
    size_t len; // In bytes
    size_t capacity; // In bytes
} Vec;

#include "result.h"

Result(size_t)
Result(Vec)

Vec vec_new(void);
Result__Vec vec_with_capacity(size_t elements);
Result__void vec_free(Vec *vec);

Result__size_t vec_len(Vec *vec);
Result__void vec_push(Vec *vec, int input);

#endif
