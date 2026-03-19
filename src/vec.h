#ifndef VEC_H_
#define VEC_H_

typedef struct Vec {
    int* data;
    size_t len; // In bytes
    size_t capacity; // In bytes
} Vec;

Vec vec_new(void);
Vec vec_with_capacity(size_t capacity);
void vec_free(Vec*);
void vec_push(Vec*, int);

#endif
