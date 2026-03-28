#include <ncurses.h>
#include <panel.h>

#include <assert.h>
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

struct RepaintReturn repaint_all(struct RepaintArray);

// https://tldp.org/HOWTO/NCURSES-Programming-HOWTO/helloworld.html
// https://github.com/mcdaniel/curses_tutorial
int main(void) {
    initscr(); // Init the ncurses system; init the terminal into curses mode. Just returns a pointer to stdscr
    cbreak(); // Disable line buffering; remove character processing (except for interrupts like Ctrl-C)
    keypad(stdscr, true); // Enable parsing escape sequences into function keys, arrow keys and similar
    noecho(); // Don't echo user input to the screen

    if (has_colors()) {
        start_color(); // Enable color functionality
        use_default_colors(); // Allow using the terminal's default background color (instead of black)

        init_pair(1, COLOR_WHITE, -1); // Define color pair 1 as white foreground and default background
        init_pair(2, COLOR_BLACK, COLOR_CYAN); // Define color pair 2 as black foreground, cyan background
    }

    struct RepaintReturn repaint_data = repaint_all(
        (struct RepaintArray) {} // Leave all null
    );
    int size_y = repaint_data.size_y;
    int size_x = repaint_data.size_x;
    WINDOW * central_win = repaint_data.winptrs[0];
    WINDOW * controls_win = repaint_data.winptrs[1];

    /*mmask_t newmask = BUTTON1_CLICKED;
    mousemask(newmask, nullptr); // Don't save old mouse mask*/

    int ch;
    while (true) {
        ch = wgetch(central_win); // Get character - wait for user input

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

            case '\x18': // Ctrl-X
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

// Supports up to 5 windows, but for now we only use two.
struct RepaintReturn repaint_all(struct RepaintArray wrapper) {
    for (int i = 0; i < 5; i++) {
        if (wrapper.winptrs[i] != nullptr) {
            delwin(wrapper.winptrs[i]);
        }
    }

    standend(); // Clear all attributes
    //clear();

    int size_y, size_x; // You can declare multiple vars of same type in one go in C
    getmaxyx(stdscr, size_y, size_x);
    
    // ------------------------------

    // size_y - 1 = 1 line from bottom (last line)
    WINDOW * controls_win = newwin(1, size_x, size_y - 1, 0); // nlines, ncols, begin_y, begin_x
    assert(controls_win != nullptr);

    if (has_colors()) {
        wattr_on(controls_win, COLOR_PAIR(2), nullptr);
        wattr_on(controls_win, A_BOLD, nullptr);
    }

    wprintw(controls_win, " ^X Quit");
    // Fill the rest of the line with spaces to paint it with the background color
    for (int i = getcurx(controls_win); i < size_x; i++) {
        waddch(controls_win, ' ');
    }

    wrefresh(controls_win);
    
    // ------------------------------

    // Leave 1 line at the bottom for controls
    WINDOW * central_win = newwin(size_y - 1, size_x, 0, 0); // nlines, ncols, begin_y, begin_x
    assert(central_win != nullptr);

    if (has_colors()) {
        wattr_on(central_win, COLOR_PAIR(1), nullptr);
    }

    box(central_win, 0, 0); // Draw a box around the central window with default line drawing characters
    int greet_start_x = (size_x / 2) - 17; // The greeting message is about 32 chars, so we start it 16 chars before the middle to center it
    mvwprintw(central_win, 0, greet_start_x, " Hello, world! Window size: %ix%i ", size_x, size_y); // Overwrites the box

    mvwprintw(central_win, 1, 2, ">>> ");

    wrefresh(central_win);
    
    // ------------------------------

    return (struct RepaintReturn) {
        .winptrs = {central_win, controls_win},
        .size_y = size_y,
        .size_x = size_x
    };
}
