#ifndef RESULT_H_
#define RESULT_H_

#include <stdint.h> // For *int*_t types

// Represented by a bool in memory (standard allows it)
enum Result_Discriminant : bool {
	Ok = true,
	Err = false,
};

enum Result_Errors : uint8_t {
	ENOERR = 0, // Not an actual error, just a placeholder for "no error"

	ENULLPTR,
	EMALLFAIL,

	EEMPTY,
	EBADIDX,
};

const char * result_strerror(enum Result_Errors input);
void result_perror(enum Result_Errors input);

/* Simulated template (generics) by macros. Expands into an anonymous struct.
If you need multitoken types (like `void*` or `long long`), first typedef them into one token.

Underlying code:
```C
struct {
    union {
		type ok;
		enum Result_Errors err;
	} data;
	enum Result_Discriminant state;
}
```

Every file expands their own.*/
#define Result(type) \
struct { \
	union { \
		type ok; \
		enum Result_Errors err; \
	} data; \
\
	enum Result_Discriminant state; \
}

// Simulates a Result(void) that contains no Ok data, because you can't have void-typed variables
// If the state is Ok, the data is uninitialized.
#define Result_void_ \
struct { \
	union { \
		enum Result_Errors err; \
	} data; \
\
	enum Result_Discriminant state; \
}

#endif /* RESULT_H_ */
