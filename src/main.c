#include "lib.h"

#include <stdio.h>
#include <assert.h>

int main(void) {
    struct Box box1 = box_init(42);
    
    struct Box box2 = box_new();
    box_store(&box2, 42);
    defer box_free(&box2);

    assert(box_load(&box1) == box_take(&box2));
    assert(box_is_empty(&box2));
    assert(!box_is_empty(&box1));

    box_store(&box1, 100);
    int *ptr = box_leak(&box1);
    defer free(ptr);

    if (*ptr != 100) {
        printf("Expected 100, got %d\n", *ptr);
        return 1;
    };

    puts("Hello, world!");
    return 0;
}
