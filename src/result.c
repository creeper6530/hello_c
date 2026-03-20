#include <stdio.h>

#include "result.h"

const char * result_strerror(enum Result_Errors input) {
	switch (input) {
		case ENULLPTR: return "Null pointer passed as input.";
		case EMALLFAIL: return "Memory allocation failed.";

		case EEMPTY: return "The data structure is empty.";
		case EBADIDX: return "The provided index is out of bounds.";
		
		default: return "Unknown error, should not happen!";
	};
}

void result_perror(enum Result_Errors input) {
	fprintf(stderr, "%s\n", result_strerror(input));
}