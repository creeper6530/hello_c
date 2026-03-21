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
    Vec new_vec = {nullptr, 0, 0};
    return new_vec;
}

Result__Vec vec_with_capacity(size_t elements) {
    Vec new_vec = {nullptr, 0, 0};
    Result__Vec ret;

    if (elements != 0) {
        // Zero-initializes the allocated mem, unlike reallocarray used in vec_push()
        new_vec.data = calloc(elements, sizeof(int));
        
        if (new_vec.data == nullptr) {
            fprintf(stderr, "vec_with_capacity: malloc failed");

            ret.state = Err;
            ret.data.err = EMALLFAIL;
            return ret;
        } else
            new_vec.capacity = elements;
    }

    ret.state = Ok;
    ret.data.ok = new_vec;
    return ret;
}

Result__void vec_free(Vec *vec) {
    Result__void ret;

    if (vec == nullptr) {
        fprintf(stderr, "vec_free: received nullptr");

        ret.state = Err;
        ret.data.err = ENULLPTR;
        return ret;
    }

    free(vec->data); // free(nullptr) is a no-op

    // Reset the Vec to a valid empty state in case the caller tries to use it again
    vec->data = nullptr;
    vec->capacity = 0;
    vec->len = 0;

    ret.state = Ok;
    return ret;
}

// Returns number of elements, not bytes!
Result__size_t vec_len(Vec *vec) {
    Result__size_t ret;

    if (vec == nullptr) {
        fprintf(stderr, "vec_len: received nullptr");

        ret.state = Err;
        ret.data.err = ENULLPTR;
        return ret;
    }

    ret.state = Ok;
    ret.data.ok = (vec->len);
    return ret;
}

Result__void vec_push(Vec *vec, int input) {
    Result__void ret;

    if (vec == nullptr) {
        fprintf(stderr, "vec_push: received nullptr");

        ret.state = Err;
        ret.data.err = ENULLPTR;
        return ret;
    }

    // Hopefully the compiler optimizes the double dereferences into a single memory accesses
    if (vec->len < vec->capacity) {
        // Resolves value vec->len, then increments it
        vec->data[vec->len++] = input;
    } else {
        size_t new_capacity = (vec->capacity == 0 ? VEC_ALLOC_START : vec->capacity * VEC_ALLOC_GROWTH_FACTOR);

        fprintf(stderr, "vec_push: capacity reached, resizing to %zu elements\n", new_capacity);
        // If ptr is NULL, then the call is equivalent to malloc(size), for all values of size.
        int* allocated = reallocarray(vec->data, new_capacity, sizeof(int));

        if (allocated == nullptr) {
            fprintf(stderr, "vec_push: reallocarray failed");

            ret.state = Err;
            ret.data.err = EMALLFAIL;
            return ret;
        } else {
            // Resolves value vec->len, then increments it
            allocated[vec->len++] = input;

            vec->data = allocated;
            vec->capacity = new_capacity;
        };
    }

    ret.state = Ok;
    return ret;
}

// Returns the popped element, or an error if the vector is empty
Result__int vec_pop(Vec *vec) {
    Result__int ret;

    if (vec == nullptr) {
        fprintf(stderr, "vec_pop: received nullptr");

        ret.state = Err;
        ret.data.err = ENULLPTR;
        return ret;
    }

    // Don't care whether data is nullptr because of invariants
    if (vec->len == 0) {
        fprintf(stderr, "vec_pop: vector is empty");

        ret.state = Err;
        ret.data.err = EEMPTY;

        return ret;
    }

    // First decrements vec->len, then resolves the value
    int popped = vec->data[--vec->len];

    ret.state = Ok;
    ret.data.ok = popped;
    return ret;
}

Result__int vec_idx(Vec *vec, ptrdiff_t index) {
    Result__int ret;

    if (vec == nullptr) {
        fprintf(stderr, "vec_idx: received nullptr");

        ret.state = Err;
        ret.data.err = ENULLPTR;
        return ret;
    }

    if (
        index < 0 ||
        (size_t)index >= (vec->len / sizeof(int))
    ) {
        // TODO: Implement negative indexing (like Python)
        fprintf(stderr, "vec_idx: index out of bounds");

        ret.state = Err;
        ret.data.err = EBADIDX;
        return ret;
    }

    int element = vec->data[index];

    ret.state = Ok;
    ret.data.ok = element;
    return ret;
}