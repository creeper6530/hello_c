#include <ncurses.h>
#include <panel.h>

#include <stdio.h>

// Need to wrap the array in a struct to be able to pass it as a parameter
struct RepaintArray {
    WINDOW * winptrs[5]; // Support up to 5 window pointers
};
struct RepaintReturn {
    WINDOW * winptrs[5];
    int size_y;
    int size_x;
};

WINDOW * draw_central_window(int, int);
struct RepaintReturn repaint_all(struct RepaintArray);

// https://tldp.org/HOWTO/NCURSES-Programming-HOWTO/helloworld.html
// https://github.com/mcdaniel/curses_tutorial
int main(void) {
    initscr(); // Init the ncurses system; init the terminal into curses mode. Just returns a pointer to stdscr
    cbreak(); // Disable line buffering; remove character processing (except for interrupts like Ctrl-C)
    keypad(stdscr, true); // Enable parsing escape sequences into function keys, arrow keys and similar
    noecho(); // Don't echo user input to the screen

    curs_set(0); // Hide the cursor

    if (has_colors() == false) {
        endwin();
        fprintf(stderr, "Terminal does not support colors.\n");
        return 1;
    };
    start_color();
    init_pair(1, COLOR_WHITE, COLOR_BLACK); // For normal text
    init_pair(2, COLOR_WHITE, COLOR_CYAN); // For controls line

    struct RepaintReturn repaint_data = repaint_all(
        (struct RepaintArray) {} // Leave all null
    );
    int size_y = repaint_data.size_y;
    int size_x = repaint_data.size_x;
    WINDOW * central_win = repaint_data.winptrs[0];
    WINDOW * controls_win = repaint_data.winptrs[1];

    mmask_t newmask = BUTTON1_CLICKED;
    mousemask(newmask, nullptr); // Don't save old mouse mask

    int ch;
    while (true) {
        ch = getch(); // Get character - wait for user input

        switch (ch) {
            case KEY_RESIZE:
                repaint_data = repaint_all(
                    (struct RepaintArray) {
                        .winptrs = {central_win, controls_win} // Leave the rest null
                    }
                );
                size_y = repaint_data.size_y;
                size_x = repaint_data.size_x;
                central_win = repaint_data.winptrs[0];
                controls_win = repaint_data.winptrs[1];
                break;

            case KEY_MOUSE:
                MEVENT event;
                if (getmouse(&event) == OK) {
                    if (event.bstate & BUTTON1_CLICKED) {
                        move(2, 0);
                        clrtoeol();
                        printw("Mouse clicked at: %i %i", event.x, event.y);
                        refresh();
                    }
                }
                break;

            case 'Q':
            case 'q':
                // Need a GOTO because else we couldn't break out of the infinite looop
                goto exit;

            default: // Nothing, just fall through to the continue
        };

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

// Supports up to 5 windows, but for now we only use two.
struct RepaintReturn repaint_all(struct RepaintArray wrapper) {
    for (int i = 0; i < 5; i++) {
        if (wrapper.winptrs[i] != nullptr) {
            delwin(wrapper.winptrs[i]);
        }
    }

    standend(); // Clear all attributes
    use_default_colors();

    clear();
    // Print the message at the current coords - default (y,x) = (0,0)
    mvprintw(0, 0, "Hello, world!");
    refresh(); // Flush the internal buffer to the display

    int size_y, size_x; // You can declare multiple vars of same type in one go in C
    getmaxyx(stdscr, size_y, size_x);
    mvprintw(1, 0, "Window size: %ix%i", size_y, size_x);

    // Leave 5 chars from each side for the X coordinate
    WINDOW * central_win = draw_central_window(5, -10);

    WINDOW * controls_win = newwin(1, size_x, size_y - 2, 0); // nlines, ncols, begin_y, begin_x
    wattrset(controls_win, A_BOLD | COLOR_PAIR(2)); // Set color for controls line
    wprintw(controls_win, " Q)uit");

    for (int i = getcurx(controls_win); i < size_x; i++) {
        waddch(controls_win, ' '); // Fill the rest of the line with spaces to clear it
    }

    wrefresh(controls_win);

    refresh();

    return (struct RepaintReturn) {
        .winptrs = {central_win, controls_win},
        .size_y = size_y,
        .size_x = size_x
    };
}
