#ifndef BACKEND_H
#define BACKEND_H

#include <stddef.h> // For size_t

// PIPE_BUF is 4096 bytes on Linux
struct WorkerArgs {
	// File descriptor the worker should read from (ideally one obtained from pipe(2))
	int worker_rx;
	// File descriptor the worker should write to (ideally one obtained from pipe(2))
	int worker_tx;
};

typedef struct WorkerMessage {
	char type; // Message type (e.g. 0x00 for NOOP, 0x01 for ECHO, etc.)
	size_t len;
	char data[]; // Flexible array member for message data (if any)
} WorkerMessage;

enum MessageTypes {
	NOOP = 0x00,
	ECHO = 0x01,
	TASK = 0x02,
	WIP = 0x03,
	DONE = 0x04,
	ERR = 0x05
};

int worker(void* args);

#endif /* BACKEND_H */
