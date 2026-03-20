#ifndef RESULT_H_
#define RESULT_H_

enum Result_Discriminant {
	Ok,
	Err,
};

enum Result_Errors {
	ENULLPTR,
	EMALLFAIL,
};

const char * result_err(enum Result_Errors input);

// We simulate a template (generics) by macros; every file expands their own
#define Result(type) typedef struct { \
	enum Result_Discriminant state; \
\
	union { \
		type ok; \
		enum Result_Errors err; \
	} data; \
} Result__##type;

// The type supplied becomes a pointer (because else it would make issues with the struct's typedef-d name)
#define Result_ptr(type) typedef struct { \
	enum Result_Discriminant state; \
\
	union { \
		type *ok; \
		enum Result_Errors err; \
	} data; \
} Result_ptr__##type;

// Simulates a data-less Result, because you can't have void-typed variables
typedef struct {
	enum Result_Discriminant state;

	union {
		enum Result_Errors err;
	} data;
} Result__void;

#endif