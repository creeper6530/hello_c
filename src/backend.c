#ifndef _GNU_SOURCE
#define _GNU_SOURCE // For pipe2 and O_DIRECT
#endif

#include "backend.h"

#include <assert.h>
#include <string.h> // For memset

#include <unistd.h> // For read, write, close
#include <errno.h>

#include <stdlib.h>
#include <threads.h>
//#include <stdio.h>


int worker(void* untyped_args) {

	// ------------------------------ UNPACK ARGS

	assert(untyped_args != nullptr);
	struct WorkerArgs* args = untyped_args;

	int worker_rx = args->worker_rx;
	int worker_tx = args->worker_tx;
	atomic_bool* worker_running = args->worker_running;

	// ------------------------------ MAIN LOOP

	// PIPE_BUF is 4096 bytes on Linux
	unsigned char buf[4096];

	while (true) {

		// ------------------------------ READ MESSAGE

		memset(buf, 0, sizeof buf); // Clear the buffer before each read
		// Our pipes are set up blockingly, so this will yield the thread until there's something to read
		auto bytes_read = read(worker_rx, &buf, sizeof buf);

		if (bytes_read == -1) {
			// Theoretically possible errnos: EAGAIN, EWOULDBLOCK, EBADF, EFAULT, EINTR, EINVAL, EIO, EISDIR
			// Practically possible errnos: EINTR, EIO

			// It will be easier not to handle EINTR
			atomic_store_explicit(worker_running, false, memory_order_release);
			return 1; // Read error
		} else if (bytes_read == 0) {
			// EOF - the frontend has closed the write end of the pipe
			atomic_store_explicit(worker_running, false, memory_order_release);
			return 0; // Clean exit
		}
		
		// ------------------------------ PROCESS MESSAGE

		switch (buf[0]) {
			case NOOP: {
				break;
			}

			case ECHO: { // Just write the same data back to the frontend

				WorkerMessage* msg = (WorkerMessage*) buf;
				assert(msg->len == bytes_read - sizeof(WorkerMessage)); // The length field should match the actual data length

				char* string = alloca(msg->len + 1);
				string[msg->len] = 0;
				memcpy(string, msg->data, msg->len);
				//fprintf(stderr, "%s\n", string);

				// If the frontend closes the read end (which it shouldn't), we get a SIGPIPE that (by default) terminates us.
				auto bytes_written = write(worker_tx, &buf, bytes_read);
				
				if (bytes_written == -1) {
					// Theoretically possible errnos: EAGAIN, EWOULDBLOCK, EBADF, EDESTADDRREQ, EDQUOT, EFAULT, EFBIG, EINTR, EINVAL, EIO, ENOSPC, EPERM, EPIPE
					// Practically possible errnos: EINTR, EIO

					atomic_store_explicit(worker_running, false, memory_order_release);
					return 2; // Write error

				// Technically encompasses the previous condition
				} else if (bytes_written != bytes_read) {
					// This should never happen with pipes, but just in case
					atomic_store_explicit(worker_running, false, memory_order_release);
					return 2; // Write error
				}

				break;
			}

			case TASK: {
				break;
			}
			
			default: {
				WorkerMessage* msg = alloca(sizeof(WorkerMessage));
				memset(msg, 0, sizeof(WorkerMessage));
				msg->type = ERR;
				msg->len = 0;

				auto bytes_written = write(worker_tx, &msg, bytes_read);

				if (bytes_written != sizeof(WorkerMessage)) {
					atomic_store_explicit(worker_running, false, memory_order_release);
					return 2;
				}

				break;
			}
		}
		/*continue;
		exit: break; // Skipped over unless GOTO-ed*/
	}

	// Unreachable
	assert(false);
}
