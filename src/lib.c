#include "lib.h"

void add_one(int* num) {
    if (num == nullptr) {
	return;
    } else {
        (*num)++;
    }
}

Vec vec_new(void) {
    Vec ret = {nullptr, 1, 0};

    return ret;
}
