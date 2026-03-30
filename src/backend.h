#ifndef BACKEND_H
#define BACKEND_H

// PIPE_BUF is 4096 bytes on Linux
struct WorkerArgs {
	// File descriptor the worker should read from (ideally one obtained from pipe(2))
	int worker_rx;
	// File descriptor the worker should write to (ideally one obtained from pipe(2))
	int worker_tx;
};
int worker(void* args);

#endif /* BACKEND_H */
