#include "lib.h"

void add_one(int* num) {
    if (num == nullptr) {
	return;
    } else {
        (*num)++;
    }
}
