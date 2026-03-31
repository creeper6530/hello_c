// Just for our IDE, the Makefile should define it for us automatically
#ifndef _GNU_SOURCE
#define _GNU_SOURCE // For pipe2 and O_DIRECT
#endif

#include "backend.h"

#include <ncurses.h>
//#include <panel.h>
#include <assert.h>
#include <string.h> // For memset

#include <fcntl.h> // For O_DIRECT
#include <unistd.h> // For pipe2, read, write, close
//#include <errno.h>
#include <alloca.h>

#include <threads.h>
#include <stdatomic.h>

// cdecl.org : declare buffer as pointer to array of char
static void repaint_all(char (*buffer)[]);

// Globals
static WINDOW * central_win = nullptr;
static WINDOW * controls_win = nullptr;
static atomic_bool worker_running;

// https://tldp.org/HOWTO/NCURSES-Programming-HOWTO/helloworld.html
// https://github.com/mcdaniel/curses_tutorial
int main(void) {

    // ------------------------------ INIT BACKEND

    // fd[0] - read end; fd[1] - write end
    // File descriptor pair for backend-to-frontend
    int btf_fds[2];
    assert(pipe2(btf_fds, O_DIRECT) == 0);

    // File descriptor pair for frontend-to-backend
    int ftb_fds[2];
    assert(pipe2(ftb_fds, O_DIRECT) == 0);

    int frontend_rx = btf_fds[0]; // fd to read from
    int frontend_tx = ftb_fds[1]; // fd to write to

    atomic_init(&worker_running, true); // Initialize the worker_running flag to true
    struct WorkerArgs args = {
        .worker_tx = btf_fds[1],
        .worker_rx = ftb_fds[0],
        .worker_running = &worker_running
    };
    thrd_t thread;
    assert(thrd_create(&thread, worker, &args) == thrd_success);

    // ------------------------------ INIT NCURSES

    initscr(); // Init the ncurses system; init the terminal into curses mode. Just returns a pointer to stdscr
    cbreak(); // Disable line buffering; remove character processing (except for interrupts like Ctrl-C)
    noecho(); // Don't echo user input to the screen

    if (has_colors()) {
        start_color(); // Enable color functionality
        use_default_colors(); // Allow using the terminal's default background color (instead of black)
        
        init_pair(2, COLOR_BLACK, COLOR_CYAN); // Define color pair 2 as black foreground, cyan background
    }

    repaint_all(nullptr);
    int size_y, size_x; // You can declare multiple vars of same type in one go in C
    getmaxyx(central_win, size_y, size_x);

    /*mmask_t newmask = BUTTON1_CLICKED;
    mousemask(newmask, nullptr); // Don't save old mouse mask*/

    // ------------------------------ MAIN LOOP

    char input_buf[150] = {}; // Zero-inited input buffer (150 chars incl. nullterm should suffice)
    int input_buf_len = 0;
    int ch;

    while (true) {
        ch = wgetch(central_win); // Get character - wait for user input

        switch (ch) {
            case KEY_RESIZE:
                //flash();
                repaint_all(&input_buf);
                getmaxyx(central_win, size_y, size_x);

                break;

            /*case KEY_MOUSE:
                MEVENT event;
                if (getmouse(&event) == OK) {
                    if (event.bstate & BUTTON1_CLICKED) {
                        move(2, 1);
                        //clrtoeol();
                        printw("Mouse clicked at: %i %i     ", event.x, event.y);
                        refresh();
                    }
                }
                break;*/

            /*case 0x18: // Ctrl-X - eXit*/
            case KEY_F(10):
                // Need a GOTO because else we couldn't break out of the infinite looop
                goto exit;

            /*case 0x08: // Ctrl-Bksp
            case 0x0C: // Ctrl-L - cLear screen*/
            case KEY_F(8):
                memset(input_buf, 0, sizeof input_buf);
                input_buf_len = 0;
                
                ungetch(KEY_RESIZE); // Enqueues a resize signal to be processed on the next iteration
                break;

            case KEY_BACKSPACE:
                if (input_buf_len == 0) break;

                int curr_x = getcurx(central_win);

                mvwaddch(central_win, 1, --curr_x, ' ');
                wmove(central_win, 1, curr_x);
                input_buf[--input_buf_len] = 0;

                wrefresh(central_win);
                break;

            // GCC extension
            case 'A' ... 'Z':
            case 'a' ... 'z':
            case '0' ... '9':
            case ' ':
                // Cast input_buf_len to signed int to avoid compiler warning
                if ((signed int)input_buf_len == size_x - 8) break;

                waddch(central_win, ch);
                input_buf[input_buf_len++] = (unsigned char)ch;

                wrefresh(central_win);
                break;

            // Who knows which char does Enter key send, so we accept both
            case '\r':
            case '\n':
                // Resets the screen back to normal temporarily so as not to garble possible crash messages
                // TODO: Remove once ASan is happy - possibly will stay here for a while more...
                endwin();

                //{
                    auto tx_msg_size = sizeof(WorkerMessage) + input_buf_len; // sizeof omits the flexible array member
                    WorkerMessage* tx_msg = alloca(tx_msg_size);
                    memset(tx_msg, 0, tx_msg_size); // Zero-initialise the memory
                    
                    tx_msg->type = 0x01; // ECHO
                    tx_msg->len = input_buf_len;
                    memcpy(tx_msg->data, input_buf, input_buf_len); // Copy the input buffer into the message's data field

                    auto bytes_written = write(frontend_tx, tx_msg, tx_msg_size);
                    assert(tx_msg_size == (unsigned long)bytes_written); // Crash on error
                //} // Drop the alloca-allocated message

                WorkerMessage* rx_msg;
                {
                    unsigned char read_buf[4096] = {}; // Allocate a giant buffer statically
                    WorkerMessage* read_buf_cast = (WorkerMessage*) read_buf;
                    auto bytes_read = read(frontend_rx, &read_buf, sizeof read_buf);

                    auto rx_len = sizeof(WorkerMessage) + read_buf_cast->len;
                    assert(rx_len == (unsigned long)bytes_read);

                    rx_msg = alloca(rx_len); // Allocate smaller buffer dynamically
                    memset(rx_msg, 0, rx_len); // Zero-initialise the memory

                    rx_msg->type = read_buf_cast->type;
                    rx_msg->len = read_buf_cast->len;
                    memcpy(rx_msg->data, read_buf_cast->data, read_buf_cast->len);
                } // Drop the giant buffer

                //assert(memcmp(tx_msg, read_buf_cast, sizeof(WorkerMessage) + input_buf_len) == 0);
                assert(memcmp(tx_msg, rx_msg, sizeof(WorkerMessage) + input_buf_len) == 0);

                // Return control over screen back to ncurses
                doupdate();

                break;

            default: // Nothing, just fall through to the continue
        };

        continue;
        exit: break; // Skipped over unless GOTO-ed
    }

    // ------------------------------ CLEANUP AND EXIT

    endwin(); // Switch back to normal terminal (calling refresh() or doupdate() after this would resume curses mode)
    puts("Exiting...");

    // Close the file descriptors to signal EOF to the worker thread and let it exit cleanly
    close(frontend_tx);

    int worker_exit_code;
    thrd_join(thread, &worker_exit_code); // Wait for the worker thread to finish before exiting the program
    assert(worker_exit_code == 0); // The worker should exit cleanly with code 0
    return 0;
}

#define CONTROLS_STR " F8 Clear input | F10 Quit"

// Parameter is a character buffer to be written as input after redraw.
static void repaint_all(char (*buffer)[]) {

    // ------------------------------ CLEAR OLD WINDOWS

    if (central_win != nullptr) {
        delwin(central_win);
        central_win = nullptr; // Destroy the old dangling pointer
    }
    if (controls_win != nullptr) {
        delwin(controls_win);
        controls_win = nullptr;
    }

    // GET TERMINAL SIZES
    int size_y, size_x;
    getmaxyx(stdscr, size_y, size_x);
    
    // ------------------------------ INIT CONTROLS WINDOW

    // size_y - 1 = 1 line from bottom (last line)
    controls_win = newwin(1, size_x, size_y - 1, 0); // nlines, ncols, begin_y, begin_x
    assert(controls_win != nullptr); // We do not handle the error.

    if (has_colors()) {
        wattr_on(controls_win, COLOR_PAIR(2), nullptr);
        wattr_on(controls_win, A_BOLD, nullptr);
    }

    wprintw(controls_win, CONTROLS_STR);
    // Fill the rest of the line with spaces to paint it with the background color
    for (int i = getcurx(controls_win); i < size_x; i++) {
        waddch(controls_win, ' ');
    }

    wrefresh(controls_win);
    
    // ------------------------------ INIT CENTRAL WINDOW

    // Leave 1 line at the bottom for controls
    central_win = newwin(size_y - 1, size_x, 0, 0); // nlines, ncols, begin_y, begin_x
    assert(central_win != nullptr);

    box(central_win, 0, 0); // Draw a box around the central window with default line drawing characters
    int greet_start_x = (size_x / 2) - 17; // The greeting message is about 32 chars, so we start it 16 chars before the middle to center it
    mvwprintw(central_win, 0, greet_start_x, " Hello, world! Window size: %ix%i ", size_x, size_y); // Overwrites the box

    keypad(central_win, true); // Enable parsing escape sequences into function keys, arrow keys and similar
    mvwprintw(central_win, 1, 2, ">>> ");
    if (buffer != nullptr)
        wprintw(central_win, *buffer);

    wrefresh(central_win);
}
