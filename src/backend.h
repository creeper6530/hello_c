#ifndef BACKEND_H
#define BACKEND_H

#include <stdatomic.h> // For atomic_bool
#include <stddef.h> // For size_t

// PIPE_BUF is 4096 bytes on Linux
struct WorkerArgs {
	// File descriptor the worker should read from (ideally one obtained from pipe(2))
	int worker_rx;
	// File descriptor the worker should write to (ideally one obtained from pipe(2))
	int worker_tx;
	// An atomic flag to signal if the worker has exited
	// Should be initialized by the caller to true, and set to false by the worker before exiting
	atomic_bool* worker_running;
};

typedef struct WorkerMessage {
	unsigned char type; // Message type (e.g. 0x00 for NOOP, 0x01 for ECHO, etc.)
	size_t len;
	unsigned char data[]; // Flexible array member for message data (if any)
} WorkerMessage;

int worker(void* args);

#endif /* BACKEND_H */
