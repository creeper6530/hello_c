#include "vec.h" // Struct is in header (so that functions can return it by value)

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>

// We know our libc supports this, but for some reason the compiler can't see it,
// so we declare it ourselves based on its man page.
//
// Equivalent to realloc(ptr, nmemb * size) but with overflow checking.
void *reallocarray(void *ptr, size_t nmemb, size_t size);

Vec vec_new(void) {
    return (Vec) {nullptr, 0, 0}; // Need the typecast because C can't infer the type of the compound literal from the return type
}

Result__Vec vec_with_capacity(const size_t elements) {
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
        } else
            new_vec.capacity = elements;
    }

    return (Result__Vec) {
        .state = Ok,
        .data.ok = new_vec
    };
}

Result__void vec_free(Vec * const vec) {
    if (vec == nullptr) {
        fprintf(stderr, "vec_free: received nullptr\n");

        return (Result__void) {
            .state = Err,
            .data.err = ENULLPTR
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

// Returns number of elements, not bytes!
Result__size_t vec_len(const Vec * const vec) {
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

Result__void vec_push(Vec * const vec, const int input) {
    if (vec == nullptr) {
        fprintf(stderr, "vec_push: received nullptr\n");

        return (Result__void) {
            .state = Err,
            .data.err = ENULLPTR
        };
    }

    register const size_t capacity = vec->capacity;

    if (vec->len < capacity) {
        // Resolves value vec->len, then increments it
        vec->data[vec->len++] = input;
    } else {
        register const size_t new_capacity = (capacity == 0 ? VEC_ALLOC_START : capacity * VEC_ALLOC_GROWTH_FACTOR);

        fprintf(stderr, "vec_push: capacity reached, resizing to %zu elements\n", new_capacity);
        // If ptr is NULL, then the call is equivalent to malloc(size), for all values of size.
        int* allocated = reallocarray(vec->data, new_capacity, sizeof(int));

        if (allocated == nullptr) {
            fprintf(stderr, "vec_push: reallocarray failed\n");

            return (Result__void) {
                .state = Err,
                .data.err = EMALLFAIL
            };
        } else {
            // Resolves value vec->len, then increments it
            allocated[vec->len++] = input;

            vec->data = allocated;
            vec->capacity = new_capacity;
        };
    }

    return (Result__void) {
        .state = Ok
    };
}

// Returns the popped element, or an error if the vector is empty
Result__int vec_pop(Vec * const vec) {
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

Result__intptr vec_idx(const Vec * const vec, const ptrdiff_t index) {
    if (vec == nullptr) {
        fprintf(stderr, "vec_idx: received nullptr\n");

        return (Result__intptr) {
            .state = Err,
            .data.err = ENULLPTR
        };
    }

    // Hopefully optimizes out
    register const ptrdiff_t len = (ptrdiff_t) vec->len;

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
