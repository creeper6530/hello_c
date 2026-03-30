#include "backend.h"

#include <unistd.h>
#include <errno.h>
#include <assert.h>
#include <stdlib.h>

int worker(void* untyped_args) {
	if (untyped_args == nullptr)
		return -1;
	struct WorkerArgs* args = untyped_args;

	int worker_rx = args->worker_rx;
	int worker_tx = args->worker_tx;

	// PIPE_BUF is 4096 bytes on Linux
	char buf[4096];

	while (true) {
		auto len = read(worker_rx, &buf, sizeof buf);
		assert(len != -1);

		return -1;
	}
}
