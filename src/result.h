#ifndef RESULT_H
#define RESULT_H

#include <assert.h>

// Represented by a bool in memory (standard allows it)
enum Result_Discriminant : bool {
	Ok = true,
	Err = false,
};

enum Result_Errors : unsigned char {
	ENOERR = 0, // Not an actual error, just a placeholder for "no error"

	ENULLPTR,
	EMALLFAIL,

	EEMPTY,
	EBADIDX,
};

const char * result_strerror(enum Result_Errors input);
void result_perror(enum Result_Errors input);

/* Simulated template (generics) by macros. Expands into an anonymous struct and typedef-s it.
Also defines a static inline function that asserts it's Ok and unwraps it
If you need multitoken types (like `void*` or `long long`), first typedef them into one token.

Underlying code:
```C
typedef struct {
    union {
		type ok;
		enum Result_Errors err;
	} data;
	enum Result_Discriminant state;
} Result__type;

static inline type result_unwrap_type (const Result__type *res) {
	assert(res != nullptr);
	assert(res->state == Ok);
	return res->data.ok;
}
```

Every file expands their own.*/
#define Result(type) \
typedef struct { \
	union { \
		type ok; \
		enum Result_Errors err; \
	} data; \
\
	enum Result_Discriminant state; \
} Result__##type; \
\
static inline type result_unwrap__##type (const Result__##type *res) { \
	assert(res != nullptr); \
	assert(res->state == Ok); \
	return res->data.ok; \
}

// Simulates a Result(void) that contains no Ok data, because you can't have void-typed variables
// If the state is Ok, the data is uninitialized.
typedef struct {
	union {
		enum Result_Errors err;
	}/* data*/; // See https://gcc.gnu.org/onlinedocs/gcc/Unnamed-Fields.html

	enum Result_Discriminant state;
} Result__void;

static inline void result_unwrap__void (const Result__void *res) {
	assert(res != nullptr);
	assert(res->state == Ok);
}

#endif /* RESULT_H */
