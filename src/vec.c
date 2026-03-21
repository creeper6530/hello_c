#include "vec.h" // Struct is in header (so that functions can return it by value)

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>

Vec vec_new(void) {
    Vec new_vec = {nullptr, 0, 0};
    return new_vec;
}

Result__Vec vec_with_capacity(size_t elements) {
    Vec new_vec = {nullptr, 0, 0};
    Result__Vec ret;

    if (elements != 0) {
        size_t new_capacity = elements * sizeof(int);
        new_vec.data = malloc(new_capacity);
        
        if (new_vec.data == nullptr) {
            fprintf(stderr, "vec_with_capacity: malloc failed");

            ret.state = Err;
            ret.data.err = EMALLFAIL;
            return ret;
        } else
            new_vec.capacity = new_capacity;
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
    ret.data.ok = (vec->len / sizeof(int));
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

    // We read len multiple times, so we copy it to a local variable to avoid multiple dereferences
    size_t len_copy = vec->len;

    if (vec->data == nullptr) {
        size_t new_capacity = ALLOC_START * sizeof(int);
        int *allocated = malloc(new_capacity);

        if (allocated == nullptr) {
            fprintf(stderr, "vec_push: malloc failed");

            ret.state = Err;
            ret.data.err = EMALLFAIL;
            return ret;
        } else {
            allocated[0] = input;

            vec->data = allocated;
            vec->capacity = new_capacity;
            vec->len = sizeof(input);
        };

    } else if (len_copy + sizeof(input) > vec->capacity) {
        size_t new_capacity = len_copy * 2 * sizeof(int);
        int *allocated = realloc(vec->data, new_capacity); // reallocarray() is not in a standard

        if (allocated == nullptr) {
            fprintf(stderr, "vec_push: realloc failed");

            ret.state = Err;
            ret.data.err = EMALLFAIL;
            return ret;
        } else {
            allocated[len_copy] = input;

            vec->data = allocated;
            vec->capacity = new_capacity;
            vec->len += sizeof(int);
        }

    } else {
        vec->data[len_copy] = input;
        vec->len += sizeof(int);
    };

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

    vec->len -= sizeof(int);
    int last_idx = (vec->len / sizeof(int)); // No -1 because len is already decremented
    int popped = vec->data[last_idx];

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