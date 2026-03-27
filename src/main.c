#include <ncurses.h>
#include <panel.h>

#include <stdio.h>

WINDOW * draw_central_window(int win_size_y, int win_size_x);
void recentre_resize_window(WINDOW * win_ptr, int win_size_y, int win_size_x);

// https://tldp.org/HOWTO/NCURSES-Programming-HOWTO/helloworld.html
// https://github.com/mcdaniel/curses_tutorial
int main(void) {
    initscr(); // Init the ncurses system; init the terminal into curses mode. Just returns a pointer to stdscr
    cbreak(); // Disable line buffering; remove character processing (except for interrupts like Ctrl-C)
    keypad(stdscr, true); // Enable parsing escape sequences into function keys, arrow keys and similar
    noecho(); // Don't echo user input to the screen
    
    // Print the message at the current coords - default (y,x) = (0,0)
    printw("Hello, world! Press a non-letter key to exit.");
    refresh(); // Flush the internal buffer to the display

    int size_y, size_x; // You can declare multiple vars of same type in one go in C
    getmaxyx(stdscr, size_y, size_x);
    mvprintw(1, 0, "Window size: %ix%i", size_y, size_x);

    // Leave 5 chars from each side for the X coordinate
    auto central_win = draw_central_window(5, -10);

    int ch;
    while (true) {
        ch = getch(); // Get character - wait for user input

        switch (ch) {
            case KEY_RESIZE:
                getmaxyx(stdscr, size_y, size_x);

                move(1, 0);
                clrtoeol(); // Clear from cursor to end of line
                mvprintw(1, 0, "Window size: %ix%i", size_x, size_y);

                recentre_resize_window(central_win, 5, -10);

                // Need to break because else the switch acts just like a bunch of GOTOs and we would fall through
                break;

            // Warning: range expressions are non-standard
            case 'A' ... 'Z': // Emulates an OR, fall-through is desired
            case 'a' ... 'z':
                mvaddch(2, 0, ch);
                break;

            default:
                // Need a GOTO because else we couldn't break out of the infinite looop
                goto exit;
        };

        refresh();
        continue;
        exit: break; // Skipped over unless GOTO-ed
    }

    endwin(); // Switch back to normal terminal (calling refresh() or doupdate() after this would resume curses mode)
    fprintf(stderr, "Exiting...\n"); // Just test a clean exit
    return 0;
}

WINDOW * draw_central_window(int win_size_y, int win_size_x) {
    int size_y, size_x;
    getmaxyx(stdscr, size_y, size_x);

    if (win_size_y < 0)
        win_size_y = size_y + win_size_y; // Need addition because it's already negative
    if (win_size_x < 0)
        win_size_x = size_x + win_size_x;

    int win_topleft_y = (size_y - win_size_y) / 2;
    int win_topleft_x = (size_x - win_size_x) / 2;

    WINDOW * win_ptr = newwin(
        win_size_y, // nlines
        win_size_x, // ncols
        win_topleft_y, // begin_y
        win_topleft_x // begin_x
    );
    box(win_ptr, 0, 0); // Use default characters
    wrefresh(win_ptr);

    return win_ptr;
}

// TODO: Appears to be severely broken when window width changes
// Some more sporadic bugs when changing just heigth too
void recentre_resize_window(WINDOW * win_ptr, int win_size_y, int win_size_x) {
    int size_y, size_x;
    getmaxyx(stdscr, size_y, size_x);

    if (win_size_y < 0)
        win_size_y = size_y + win_size_y;
    if (win_size_x < 0)
        win_size_x = size_x + win_size_x;

    int win_topleft_y = (size_y - win_size_y) / 2;
    int win_topleft_x = (size_x - win_size_x) / 2;

    wclear(win_ptr);
    wrefresh(win_ptr);

    wresize(win_ptr, win_size_y, win_size_x);
    mvwin(win_ptr, win_topleft_y, win_topleft_x);

    box(win_ptr, 0, 0);
    wrefresh(win_ptr);
}
