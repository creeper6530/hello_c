#include "lib.h"

#include <errno.h>

/* Initialises a new Box with the given value.

If the memory allocation fails, the function returns an empty box and sets errno.
Possible error codes:
ENOMEM: Memory allocation failed.
*/
struct Box box_init(int value) {
    register int *allocation = malloc(sizeof value);
    
    if (allocation == nullptr) {
        errno = ENOMEM;
        return (struct Box) {nullptr};
    }

    *allocation = value;
    return (struct Box) {allocation};
}

/* Stores the given value in the box. If the box is empty, it allocates memory.

If the return value is true, the function has succeeded, otherwise it has failed and errno is set to indicate the error.
Possible error codes:
EFAULT: The passed pointer is null.
ENOMEM: Memory allocation failed.
*/
bool box_store(struct Box *box, int value) {
    if (box == nullptr) {
        errno = EFAULT;
        return false;
    }

    if (box->data == nullptr) {
        register int *allocation = malloc(sizeof value);

        if (allocation == nullptr) {
            errno = ENOMEM;
            return false;
        }
        box->data = allocation;
    }
    
    *box->data = value;
    return true;
}

/* Loads the value from the box and returns it.

If the return value is -1, the function may have failed and set an errno.
Possible error codes:
0: Success.
EFAULT: The passed pointer is null or the box is empty.

If the function succeeds, errno is set to 0 to indicate success.
*/
int box_load(struct Box *box) {
    if (box == nullptr) {
        errno = EFAULT;
        return -1;
    }

    if (box->data == nullptr) {
        errno = EFAULT;
        return -1;
    }

    errno = 0;
    return *box->data;
}

/* Frees the allocation inside the box and sets it to empty, returning the contained value if any.

If the return value is -1, the function may have failed and set an errno.
Possible error codes:
0: Success.
EFAULT: The passed pointer is null or the data pointer inside the box is null.

If the function succeeds, errno is set to 0 to indicate success.
*/
int box_take(struct Box *box) {
	if (box == nullptr) {
		errno = EFAULT;
		return -1;
	}

    if (box->data == nullptr) {
        errno = EFAULT; // No value to take, but not an error
        return -1;
    }

    register int value = *box->data;
    free(box->data);
    box->data = nullptr;

    errno = 0;
    return value;
}

/* Returns the contained pointer and sets the box to empty. The caller is responsible for freeing the returned pointer.

The function returns nullptr if the box is empty or the passed pointer is null.
*/
int *box_leak(struct Box *box) {
    if (box == nullptr) {
        return nullptr;
    }

    register int *ptr = box->data;
    box->data = nullptr; // Prevent double free
    return ptr;
}

/* Returns true if the box is empty, false otherwise.

If the return value is false, the function may have failed and set an errno.
Possible error codes:
0: Success.
EFAULT: The passed pointer is null.

If the function succeeds, errno is set to 0 to indicate success.
*/
bool box_is_empty(const struct Box *box) {
    if (box == nullptr) {
        errno = EFAULT;
        return false;
    }

    errno = 0;
    return (box->data == nullptr);
}
