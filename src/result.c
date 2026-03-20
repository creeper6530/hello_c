#include "result.h"

const char* result_err(enum Result_Errors input) {
	switch (input) {
		case ENULLPTR: return "Null pointer passed as input.";
		case EMALLFAIL: return "Memory allocation failed.";
		default: return "Unknown error.";
	};
}