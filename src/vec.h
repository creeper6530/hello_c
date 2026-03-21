#ifndef VEC_H_
#define VEC_H_

// https://dev.to/pauljlucas/proper-header-file-etiquette-ola
// In a header file, include other local headers first, if any, followed by system headers, if any.
#include "result.h"

#include <stddef.h> // For size_t and ptrdiff_t

#define ALLOC_START ((size_t)32)

// Invariants:
// If (data == nullptr), then (capacity == 0) and (len == 0),
// otherwise, (capacity > 0) and (len <= capacity).
// Always (capacity % sizeof(int) == 0) and (len % sizeof(int) == 0).
typedef struct Vec {
    int *data;
    size_t capacity; // In bytes
    size_t len; // In bytes
} Vec;


Result(Vec)
Result(size_t)
Result(int)

Vec vec_new(void);
Result__Vec vec_with_capacity(size_t elements);
Result__void vec_free(Vec *vec);

Result__size_t vec_len(Vec *vec);
Result__void vec_push(Vec *vec, int input);
Result__int vec_pop(Vec *vec);
Result__int vec_idx(Vec *vec, ptrdiff_t index);

#endif /* VEC_H_ */
