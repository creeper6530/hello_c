#ifndef RESULT_H_
#define RESULT_H_

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

/* Simulated template (generics) by macros. Expands into an anonymous struct.
You must typedef them into a name before using them,
as two anonymous structs are not the same type even with same fields.

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
