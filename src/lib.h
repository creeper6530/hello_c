#ifndef LIB_H_
#define LIB_H_

#include <stddef.h>

#define PI (3)
void add_one(int* num);

typedef struct {
	int *data;
	size_t len;
	size_t cap;
} Vec;

Vec vec_new(void);

#endif /* LIB_H_ */
