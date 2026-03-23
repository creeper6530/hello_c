#ifndef VEC_H_
#define VEC_H_

// https://dev.to/pauljlucas/proper-header-file-etiquette-ola
// In a header file, include other local headers first, if any, followed by system headers, if any.
#include "result.h"

#include <stddef.h> // For size_t and ptrdiff_t

// Number of elements to allocate when the first element is pushed to an empty Vec. Must be > 0.
#define VEC_ALLOC_START ((size_t)1)
static_assert(VEC_ALLOC_START > 0, "VEC_ALLOC_START must be greater than 0");
// A factor by which to multiply the capacity when resizing. Must be > 1.
#define VEC_ALLOC_GROWTH_FACTOR ((size_t)2)
static_assert(VEC_ALLOC_GROWTH_FACTOR > 1, "VEC_ALLOC_GROWTH_FACTOR must be greater than 1");

// Invariants:
// If (data == nullptr), then (capacity == 0) and (len == 0),
// otherwise, (capacity > 0) and (len <= capacity).
// Always (capacity % sizeof(int) == 0) and (len % sizeof(int) == 0).
typedef struct Vec {
    int *data;
    size_t capacity; // In elements
    size_t len; // In elements
} Vec;


typedef Result(Vec) Result__Vec;
typedef Result(size_t) Result__size_t;
typedef Result(int) Result__int;
typedef Result_void_ Result__void;

typedef int* intptr;
typedef Result(intptr) Result__intptr;

Vec vec_new(void);
Result__Vec vec_with_capacity(const size_t elements);
Result__void vec_free(Vec * const vec);

// const int *ptr = pointer to const int = may not change pointee
// int * const int = const pointer to int = may not change pointer
// const int * const int = const pointer to const int = may not change pointer nor pointee
Result__size_t vec_len(const Vec * const vec);
Result__void vec_push(Vec * const vec, const int input);
Result__int vec_pop(Vec * const vec);
Result__intptr vec_idx(const Vec * const vec, const ptrdiff_t index);

#endif /* VEC_H_ */
