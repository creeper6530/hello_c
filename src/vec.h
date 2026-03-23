#ifndef VEC_H
#define VEC_H

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

/* A simple vector (resizable array) that owns its allocation.

Invariants:
If (data == nullptr), then (capacity == 0) and (len == 0),
otherwise, (capacity > 0) and (len <= capacity).
*/
typedef struct Vec {
    int *data;
    size_t capacity; // In elements
    size_t len; // In elements
} Vec;

/* A view into a Vec (or generally any array) that doesn't allow mutation nor has ownership of the allocation.
Also could be called a fat pointer.

Invariants:
(data != nullptr) and (len != 0). Unconditionally.
*/
typedef struct Slice {
    int *const data; // T *const name = const pointer to T (you can't modify the pointer)
    const size_t len; // In elements
} Slice;
// TODO: Is it wise to keep these as const?

// Automatically typedef-s these
Result(Vec)
Result(Slice)
Result(size_t)
Result(int)

typedef int* intptr;
Result(intptr)

// MUST be `static` to prevent multiple conflicting definitions
// SHOULD be `inline` else there's no point in having this in a header
static inline Vec vec_new(void) {
    // Need the typecast because C can't infer the type of the compound literal from the return type
    return (Vec) {nullptr, 0, 0};
}

Result__Vec vec_with_capacity(size_t elements);
Result__void vec_free(Vec *vec);

Result__void vec_push(Vec *vec, int input);
Result__int vec_pop(Vec *vec);
Result__void vec_shrink_to_fit(Vec *vec);

// `const T *name` = pointer to `const T` (you can't modify T)
// https://stackoverflow.com/a/21476937
// https://cdecl.org/ or https://cdecl.plus/
Result__size_t vec_len(const Vec *vec);
Result__intptr vec_idx(const Vec *vec, ptrdiff_t index);
Result__Slice vec_as_slice(const Vec *vec);

// --------------------------------------------------

Result__size_t slice_len(const Slice *slice);
Result__intptr slice_idx(const Slice *slice, ptrdiff_t index);

#endif /* VEC_H */
