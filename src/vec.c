#include <stdlib.h>
#include <stddef.h>

#include "vec.h" // Struct is in header (so that functions can return it by value)

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
        ret.state = Err;
        ret.data.err = ENULLPTR;
        return ret;
    }

    free(vec->data); // free(nullptr) is a no-op
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
        ret.state = Err;
        ret.data.err = ENULLPTR;

        return ret;
    }

    ret.state = Ok;
    ret.data.ok = vec->len / sizeof(int);
    return ret;
}

Result__void vec_push(Vec *vec, int input) {
    Result__void ret;

    if (vec == nullptr) {
        ret.state = Err;
        ret.data.err = ENULLPTR;
        return ret;
    }

    size_t len_copy = vec->len;
    size_t capacity_copy = vec->capacity;

    if (vec->data == nullptr) {
        size_t new_capacity = ALLOC_START * sizeof(int);
        int *allocated = malloc(new_capacity);

        if (allocated != nullptr) {
            allocated[0] = input;

            vec->data = allocated;
            vec->capacity = new_capacity;
            vec->len = sizeof(input);
        } else {
            ret.state = Err;
            ret.data.err = EMALLFAIL;
            return ret;
        };

    } else if (len_copy + sizeof(input) > capacity_copy) {
        size_t new_capacity = len_copy * 2 * sizeof(int);
        int *allocated = realloc(vec->data, new_capacity); // reallocarray() is not in a standard

        if (allocated != nullptr) {
            allocated[len_copy] = input;

            vec->data = allocated;
            vec->capacity = new_capacity;
            vec->len += sizeof(int);
        } else {
            ret.state = Err;
            ret.data.err = EMALLFAIL;
            return ret;
        }

    } else {
        vec->data[len_copy] = input;
        vec->len += sizeof(int);
    };

    ret.state = Ok;
    return ret;
}
