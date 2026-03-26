#include <ncurses.h>
#include <stdio.h>

//#include "lib.h"

// https://tldp.org/HOWTO/NCURSES-Programming-HOWTO/helloworld.html
int main(void) {
    initscr(); // Init the ncurses system; init the terminal into curses mode. Just returns a pointer to stdscr
    cbreak(); // Disable line buffering; remove character processing (except for interrupts like Ctrl-C)
    keypad(stdscr, TRUE); // Enable parsing escape sequences into function keys, arrow keys and similar
    noecho(); // Don't echo user input to the screen
    
    printw("Hello, world! Press any key to exit."); // Print the message at the current coords - default (y,x) = (0,0)
    refresh(); // Flush the internal buffer to the display

    int ch = getch(); // Get character - wait for user input

    endwin(); // Switch back to normal terminal (calling refresh() or doupdate() after this would resume curses mode)
    delwin(stdscr); // Free the memory allocated for the window
    fprintf(stderr, "Exiting...\n"); // Just test a clean exit
    return 0;
}
