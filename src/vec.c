#include "vec.h" // Struct is in header (so that functions can return it by value)

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>

// We know our libc supports this, but for some reason the compiler can't see it,
// so we declare it ourselves based on its man page.
//
// Equivalent to realloc(ptr, nmemb * size) but with overflow checking.
void *reallocarray(void *ptr, size_t nmemb, size_t size);

// vec_new() defined in header

Result__Vec vec_with_capacity(size_t elements) {
    Vec new_vec = {nullptr, 0, 0};

    if (elements != 0) {
        // Zero-initializes the allocated mem, unlike reallocarray used in vec_push()
        new_vec.data = calloc(elements, sizeof(int));
        
        if (new_vec.data == nullptr) {
            fprintf(stderr, "vec_with_capacity: malloc failed\n");

            return (Result__Vec) {
                .state = Err,
                .data.err = EMALLFAIL
            };
        };

        new_vec.capacity = elements;
    }

    return (Result__Vec) {
        .state = Ok,
        .data.ok = new_vec
    };
}

Result__void vec_free(Vec *vec) {
    if (vec == nullptr) {
        fprintf(stderr, "vec_free: received nullptr\n");

        return (Result__void) {
            .state = Err,
            ./*data.*/err = ENULLPTR // See https://gcc.gnu.org/onlinedocs/gcc/Unnamed-Fields.html
        };
    }

    free(vec->data); // free(nullptr) is a no-op

    // Reset the Vec to a valid empty state in case the caller tries to use it again
    vec->data = nullptr;
    vec->capacity = 0;
    vec->len = 0;

    // Remaining fields not specified (the union) are zero-initialized
    return (Result__void) {
        .state = Ok
    };
}

Result__void vec_push(Vec *vec, int input) {
    if (vec == nullptr) {
        fprintf(stderr, "vec_push: received nullptr\n");

        return (Result__void) {
            .state = Err,
            ./*data.*/err = ENULLPTR
        };
    }

    register const size_t capacity = vec->capacity;

    if (vec->len < capacity) {
        // Resolves value vec->len, then increments it
        vec->data[vec->len++] = input;
    } else {
        register size_t new_capacity = (capacity == 0 ? VEC_ALLOC_START : capacity * VEC_ALLOC_GROWTH_FACTOR);

        fprintf(stderr, "vec_push: capacity reached, resizing to %zu elements\n", new_capacity);
        // If ptr is NULL, then the call is equivalent to malloc(size), for all values of size.
        int *allocated = reallocarray(vec->data, new_capacity, sizeof(int));

        if (allocated == nullptr) {
            fprintf(stderr, "vec_push: reallocarray failed\n");

            return (Result__void) {
                .state = Err,
                ./*data.*/err = EMALLFAIL
            };
        };

        // Resolves value vec->len, then increments it
        allocated[vec->len++] = input;

        vec->data = allocated;
        vec->capacity = new_capacity;
    }

    return (Result__void) {
        .state = Ok
    };
}

// Returns the popped element, or an error if the vector is empty
Result__int vec_pop(Vec *vec) {
    if (vec == nullptr) {
        fprintf(stderr, "vec_pop: received nullptr\n");

        return (Result__int) {
            .state = Err,
            .data.err = ENULLPTR
        };
    }

    // Don't care whether data is nullptr because of invariants
    if (vec->len == 0) {
        fprintf(stderr, "vec_pop: vector is empty\n");

        return (Result__int) {
            .state = Err,
            .data.err = EEMPTY
        };
    }

    return (Result__int) {
        .state = Ok,
        .data.ok = vec->data[--vec->len] // First decrements vec->len, then resolves the value
    };
}

Result__void vec_shrink_to_fit(Vec *vec) {
    if (vec == nullptr) {
        return (Result__void) {
            .state = Err,
            ./*data.*/err = ENULLPTR
        };
    }

    if (vec->len == 0) {
        *vec = (Vec) {nullptr, 0, 0};

        return (Result__void) {
            .state = Ok
        };
    }

    int *allocated = reallocarray(vec->data, vec->len, sizeof(int));

    if (allocated == nullptr) {
        return (Result__void) {
            .state = Err,
            ./*data.*/err = EMALLFAIL
        };
    }

    vec->data = allocated;
    vec->capacity = vec->len;

    return (Result__void) {
        .state = Ok
    };
}

// --------------------------------------------------

// Returns number of elements, not bytes!
// `const T *name` = pointer to `const T` (you can't modify T)
Result__size_t vec_len(const Vec *vec) {
    if (vec == nullptr) {
        fprintf(stderr, "vec_len: received nullptr\n");

        return (Result__size_t) {
            .state = Err,
            .data.err = ENULLPTR
        };
    }

    return (Result__size_t) {
        .state = Ok,
        .data.ok = vec->len
    };
}

Result__intptr vec_idx(const Vec *vec, ptrdiff_t index) {
    if (vec == nullptr) {
        fprintf(stderr, "vec_idx: received nullptr\n");

        return (Result__intptr) {
            .state = Err,
            .data.err = ENULLPTR
        };
    }

    // Hopefully optimizes out
    register ptrdiff_t len = (ptrdiff_t) vec->len;

    // Range of permitted indices: -len..len = -len..=(len-1)
    //if ( index >= len || index < -len ) {
    if ( !( index >= -len && index < len )) {
        fprintf(stderr, "vec_idx: index out of bounds\n");

        return (Result__intptr) {
            .state = Err,
            .data.err = EBADIDX
        };
    }

    return (Result__intptr) {
        .state = Ok,
        .data.ok = index >= 0 ?
            vec->data + index :
            vec->data + (len + index) // Index is already negative, so use `+` for subtraction
    };
}

Result__Slice vec_as_slice(const Vec *vec) {
    if (vec == nullptr) {
        fprintf(stderr, "vec_as_slice: received nullptr\n");

        return (Result__Slice) {
            .state = Err,
            .data.err = ENULLPTR
        };
    }

    return (Result__Slice) {
        .state = Ok,
        .data.ok = (Slice) {
            .data = vec->data,
            .len = vec->len
        }
    };
}

// ==================================================

Result__size_t slice_len(const Slice *slice) {
    if (slice == nullptr) {
        fprintf(stderr, "slice_len: received nullptr\n");

        return (Result__size_t) {
            .state = Err,
            .data.err = ENULLPTR
        };
    }

    return (Result__size_t) {
        .state = Ok,
        .data.ok = slice->len
    };
}

Result__intptr slice_idx(const Slice *slice, ptrdiff_t index) {
    if (slice == nullptr) {
        fprintf(stderr, "slice_idx: received nullptr\n");

        return (Result__intptr) {
            .state = Err,
            .data.err = ENULLPTR
        };
    }

    register ptrdiff_t len = (ptrdiff_t) slice->len;

    // Range of permitted indices: -len..len = -len..=(len-1)
    if ( !( index >= -len && index < len )) {
        fprintf(stderr, "slice_idx: index out of bounds\n");

        return (Result__intptr) {
            .state = Err,
            .data.err = EBADIDX
        };
    }

    return (Result__intptr) {
        .state = Ok,
        .data.ok = index >= 0 ?
            slice->data + index :
            slice->data + (len + index) // Index is already negative, so use `+` for subtraction
    };
}
