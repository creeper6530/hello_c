#include <ncurses.h>
#include <panel.h>

#include <assert.h>
#include <stdio.h>

void repaint_all(void);

// Globals
static WINDOW * central_win = nullptr;
static WINDOW * controls_win = nullptr;

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
        
        init_pair(2, COLOR_BLACK, COLOR_CYAN); // Define color pair 2 as black foreground, cyan background
    }

    repaint_all();

    /*mmask_t newmask = BUTTON1_CLICKED;
    mousemask(newmask, nullptr); // Don't save old mouse mask*/

    int ch;
    while (true) {
        ch = wgetch(central_win); // Get character - wait for user input

        switch (ch) {
            case KEY_RESIZE:
                repaint_all();
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
void repaint_all(void) {
    if (central_win != nullptr) {
        delwin(central_win);
        central_win = nullptr; // Destroy the old dangling pointer
    }
    if (controls_win != nullptr) {
        delwin(controls_win);
        controls_win = nullptr;
    }

    standend(); // Clear all attributes
    //clear();

    int size_y, size_x; // You can declare multiple vars of same type in one go in C
    getmaxyx(stdscr, size_y, size_x);
    
    // ------------------------------

    // size_y - 1 = 1 line from bottom (last line)
    controls_win = newwin(1, size_x, size_y - 1, 0); // nlines, ncols, begin_y, begin_x
    assert(controls_win != nullptr); // We do not handle the error.

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
    central_win = newwin(size_y - 1, size_x, 0, 0); // nlines, ncols, begin_y, begin_x
    assert(central_win != nullptr);

    box(central_win, 0, 0); // Draw a box around the central window with default line drawing characters
    int greet_start_x = (size_x / 2) - 17; // The greeting message is about 32 chars, so we start it 16 chars before the middle to center it
    mvwprintw(central_win, 0, greet_start_x, " Hello, world! Window size: %ix%i ", size_x, size_y); // Overwrites the box

    mvwprintw(central_win, 1, 2, ">>> ");

    wrefresh(central_win);
}
