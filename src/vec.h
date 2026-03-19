#ifndef VEC_H_
#define VEC_H_

typedef struct Vec {
    int *data;
    size_t len; // In bytes
    size_t capacity; // In bytes
} Vec;

Vec vec_new(void);
Vec vec_with_capacity(size_t elements);
void vec_free(Vec *vec);

size_t vec_len(Vec *vec);
void vec_push(Vec *vec, int data);

#endif
