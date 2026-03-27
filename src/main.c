#include <ncurses.h>
#include <panel.h>

#include <stdio.h>

struct RepaintReturn {
    WINDOW * central_win;
    int size_y;
    int size_x;
};

WINDOW * draw_central_window(int win_size_y, int win_size_x);
struct RepaintReturn repaint_all(WINDOW * central_win);

// https://tldp.org/HOWTO/NCURSES-Programming-HOWTO/helloworld.html
// https://github.com/mcdaniel/curses_tutorial
int main(void) {
    initscr(); // Init the ncurses system; init the terminal into curses mode. Just returns a pointer to stdscr
    cbreak(); // Disable line buffering; remove character processing (except for interrupts like Ctrl-C)
    keypad(stdscr, true); // Enable parsing escape sequences into function keys, arrow keys and similar
    noecho(); // Don't echo user input to the screen

    struct RepaintReturn repaint_data = repaint_all(nullptr);
    int size_y = repaint_data.size_y;
    int size_x = repaint_data.size_x;
    WINDOW * central_win = repaint_data.central_win;

    mmask_t newmask = BUTTON1_CLICKED;
    mousemask(newmask, nullptr); // Don't save old mouse mask

    int ch;
    while (true) {
        ch = getch(); // Get character - wait for user input

        switch (ch) {
            case KEY_RESIZE:
                repaint_data = repaint_all(central_win);
                size_y = repaint_data.size_y;
                size_x = repaint_data.size_x;
                central_win = repaint_data.central_win;
                break;

            case KEY_MOUSE:
                MEVENT event;
                if (getmouse(&event) == OK) {
                    if (event.bstate & BUTTON1_CLICKED) {
                        move(3, 0);
                        clrtoeol();
                        printw("Mouse clicked at: %i %i", event.x, event.y);
                        refresh();
                    }
                }
                break;

            // Warning: range expressions are non-standard
            case 'A' ... 'Z': // Emulates an OR, fall-through is desired
            case 'a' ... 'z':
                mvaddch(2, 0, ch);

                // Need to break because else the switch acts just like a bunch of GOTOs and we would fall through
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

struct RepaintReturn repaint_all(WINDOW * central_win) {
    if (central_win != nullptr)
        delwin(central_win);

    clear();
    // Print the message at the current coords - default (y,x) = (0,0)
    mvprintw(0, 0, "Hello, world! Press a non-letter key to exit.");
    refresh(); // Flush the internal buffer to the display

    int size_y, size_x; // You can declare multiple vars of same type in one go in C
    getmaxyx(stdscr, size_y, size_x);
    mvprintw(1, 0, "Window size: %ix%i", size_y, size_x);

    // Leave 5 chars from each side for the X coordinate
    central_win = draw_central_window(5, -10);
    refresh();

    return (struct RepaintReturn) {
        .central_win = central_win,
        .size_y = size_y,
        .size_x = size_x
    };
}
