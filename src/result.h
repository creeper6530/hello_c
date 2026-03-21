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

/* Simulated template (generics) by macros. Underlying code:
```C
typedef struct Result__type {
    union {
		type ok;
		enum Result_Errors err;
	} data;
	enum Result_Discriminant state;
} Result__type;
```

Every file expands their own.*/
#define Result(type) typedef struct Result__##type { \
	union { \
		type ok; \
		enum Result_Errors err; \
	} data; \
\
	enum Result_Discriminant state; \
} Result__##type;

// The type supplied becomes a pointer (because else it would make issues with the struct's typedef-d name)
/* Simulated template (generics) by macros. Real type is a pointer to passed type. Underlying code:

```C
typedef struct Result_ptr__type {
	union {
		type *ok;
		enum Result_Errors err;
	} data;
	enum Result_Discriminant state;
} Result_ptr__type;
```

Every file expands their own. */
#define Result_ptr(type) typedef struct Result_ptr__##type { \
	union { \
		type *ok; \
		enum Result_Errors err; \
	} data; \
\
	enum Result_Discriminant state; \
} Result_ptr__##type;

// Simulates a Result__void that contains no Ok data, because you can't have void-typed variables
// If the state is Ok, the data is uninitialized.
typedef struct Result__void {
	union {
		enum Result_Errors err;
	} data;

	enum Result_Discriminant state;
} Result__void;

#endif /* RESULT_H_ */
