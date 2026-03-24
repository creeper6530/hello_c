#ifndef LIB_H_
#define LIB_H_

#include <stdlib.h>

struct Box {
	int *data;
};

// Creates a new empty Box.
static inline struct Box box_new(void) {
    return (struct Box) {nullptr};
}

// Frees the allocation inside the box and sets it to empty, discarding the contained value if any.
// No-op if the box is empty already or the passed pointer is null.
static inline void box_free(struct Box *box) {
	if (box == nullptr) {
		return;
	}

	free(box->data);
	box->data = nullptr;
}

struct Box box_init(int value);
bool box_store(struct Box *box, int value);
int box_load(struct Box *box);
int box_take(struct Box *box);
int *box_leak(struct Box *box);
bool box_is_empty(const struct Box *box);

#endif /* LIB_H_ */
