#include <ncurses.h>

//#include "lib.h"

// https://tldp.org/HOWTO/NCURSES-Programming-HOWTO/helloworld.html
int main(void) {
    initscr(); // Init the ncurses system; init the terminal into curses mode. 
    
    printw("Hello, world! Press any key to exit."); // Print the message at the current coords - default (y,x) = (0,0)
    refresh(); // Flush the internal buffer to the display

    getch(); // Get character - wait for user input

    endwin(); // Deinit ncurses; switch back to normal terminal
    return 0;
}
