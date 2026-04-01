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
#include <alloca.h>

static inline size_t process_command(const char* command, unsigned char (*buffer)[4096]);

int worker(void* untyped_args) {

	// ------------------------------ UNPACK ARGS

	assert(untyped_args != nullptr);
	struct WorkerArgs* args = untyped_args;

	int worker_rx = args->worker_rx;
	int worker_tx = args->worker_tx;

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
			return 1; // Read error
		} else if (bytes_read == 0) {
			// EOF - the frontend has closed the write end of the pipe
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

					return 2; // Write error

				// Technically encompasses the previous condition
				} else if (bytes_written != bytes_read) {
					// This should never happen with pipes, but just in case
					return 2; // Write error
				}

				break;
			}

			case TASK: {
				WorkerMessage* msg = (WorkerMessage*) buf;
				assert(msg->len == bytes_read - sizeof(WorkerMessage)); // The length field should match the actual data length

				char* string = alloca(msg->len + 1);
				string[msg->len] = 0;
				memcpy(string, msg->data, msg->len);

				size_t out_len = process_command(string, &buf);

				size_t reply_size = sizeof(WorkerMessage) + out_len;
				WorkerMessage* reply = alloca(reply_size);
				memset(reply, 0, reply_size);

				reply->type = DONE;
				reply->len = out_len;
				memcpy(reply->data, &buf, out_len);

				auto bytes_written = write(worker_tx, reply, reply_size);
                if (reply_size != (unsigned long)bytes_written) return 2;

				break;
			}
			
			default: {
				WorkerMessage* msg = alloca(sizeof(WorkerMessage) + 16);
				memset(msg, 0, sizeof(WorkerMessage) + 16);
				msg->type = ERR;
				msg->len = 16;
				memcpy(msg->data, "Invalid command", 16);

				auto bytes_written = write(worker_tx, &msg, bytes_read);

				if (bytes_written != sizeof(WorkerMessage)) {
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

// Returns length of buffer written
static inline size_t process_command(const char* command, unsigned char (*buffer)[4096]) {
	assert(command != nullptr);
	assert(buffer != nullptr);

	if (strcmp(command, "lipsum") == 0 || strcmp(command, "lorem ipsum") == 0) {
		const char* lipsum = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Pellentesque elementum id ligula vitae dictum. Nunc urna metus, iaculis quis tempor ac, bibendum sed dui. In quis ornare risus. Maecenas ultricies a justo sed suscipit. Vivamus suscipit ipsum vel elit tristique rhoncus. Quisque suscipit augue ac quam congue laoreet. Sed rutrum at purus lobortis rhoncus. Aliquam varius turpis eros, eu venenatis ante consequat quis. Aenean non ligula pharetra, ullamcorper mi ut, bibendum velit. Vivamus vitae bibendum ligula. Sed feugiat luctus tristique. Nulla sollicitudin pretium dignissim. Aenean nec ipsum ut neque ultrices tincidunt. In hac habitasse platea dictumst. Donec fermentum molestie rhoncus. Duis elementum nisl quis sodales auctor. Vestibulum non sapien quam. Maecenas id massa nunc. Mauris varius euismod porta. Pellentesque id turpis vehicula, rutrum urna id, dapibus ante. Mauris sollicitudin elit vestibulum, porttitor urna ornare, dictum velit. Duis cursus, nibh eget ullamcorper lobortis, justo ipsum volutpat ipsum, at rhoncus sapien tellus non tortor. Sed dolor libero, dapibus sed sodales at, dapibus ut leo. Etiam ut urna vulputate tortor ultricies elementum. Maecenas lobortis commodo erat, non blandit nisi mollis at.\n\nFusce turpis sapien, pharetra nec eros sit amet, porta placerat turpis. Donec vitae est sit amet mauris facilisis auctor et nec orci. Suspendisse efficitur eleifend leo, quis dignissim metus lacinia sit amet. Praesent at dolor ultricies mi bibendum hendrerit. Duis finibus sagittis orci ut tempus.Sed porttitor quam velit, sit amet pellentesque arcu auctor id. Etiam eget dui lacinia, gravida enim sit amet, gravida est. Ut non euismod sapien. Phasellus efficitur lorem vel massa congue, vitae iaculis urna aliquet. Mauris et elit in ex lobortis porttitor vel id orci. Aenean fermentum, ipsum vel sagittis tincidunt, neque nibh posuere est, ut semper tortor magna vitae diam. Vivamus et gravida mauris, a tempor lectus. Suspendisse semper fringilla libero, sed pretium arcu malesuada interdum. Nullam scelerisque, urna dapibus pellentesque cursus, erat purus pellentesque massa, a luctus risus mi eget magna. Duis eros felis, tempor ac neque a, consectetur tempus tellus. Mauris accumsan molestie viverra. Quisque dui ligula, consectetur sed est id, ullamcorper vestibulum arcu. Curabitur dapibus molestie elit, sed scelerisque erat egestas sit amet.\n\nVestibulum ornare lorem mi, sit amet blandit lectus semper nec. Duis dignissim aliquet leo, quis hendrerit mi tincidunt ac. Pellentesque egestas in lorem in gravida. Vivamus maximus finibus nulla, cursus ultrices dui consequat quis. In erat metus, accumsan sit amet justo in, pretium venenatis tortor. Morbi vitae erat ac purus tincidunt porta. Praesent vehicula nisl non bibendum pharetra. Orci varius natoque penatibus et magnis dis parturient montes, nascetur ridiculus mus. Proin eu erat odio. Morbi eget massa quam. Praesent varius, mauris vitae interdum feugiat, quam felis tempus sem, sed pretium urna magna id diam. Nullam dictum dolor porttitor mollis bibendum. Sed tristique nisi varius augue suscipit, quis finibus leo aliquet.\n\nSed venenatis, lectus quis commodo sagittis, mi nulla volutpat enim, id ullamcorper dolor eros sed augue. Sed vitae venenatis augue, non ultrices diam. Aliquam a egestas est, sit amet faucibus dolor. Sed vel malesuada dui. Nulla eu auctor massa, a aliquam ligula. Donec pellentesque augue ut mi maximus pellentesque. In tempor ornare sapien, a mattis risus facilisis consectetur. Nam ullamcorper, erat vitae feugiat ullamcorper, tellus mauris tempor ipsum, sed ultrices diam nunc vel eros. Lorem ipsum dolor sit amet, consectetur adipiscing elit. Cras posuere nisl in nibh venenatis, id ullamcorper dolor suscipit. Aenean et ante eget tellus hendrerit ultricies eget eget ipsum. Ut venenatis pretium erat, molestie sollicitudin odio. Nam vel velit feugiat, egestas eros id, accumsan nisl. Pellentesque sagittis purus nisi, et dignissim diam commodo in. Proin quis porta ante.";
		memcpy(buffer, lipsum, 4008);
		return 4008;
	} else {
		size_t command_len = strlen(command);
		assert(command_len + 1 <= 4096);
		memcpy(buffer, command, command_len + 1); // strlen() omits null terminator
		return command_len + 1; // Don't actually return the null terminator, just write it into the buffer
	}
}
