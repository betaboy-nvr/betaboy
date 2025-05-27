#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h> // For usleep

#include "tetris.h"

// --- Game Settings ---
#define INITIAL_FALL_DELAY_MS 500 // Initial delay for piece falling (milliseconds)
#define FALL_DELAY_DECREMENT_MS 10 // How much to decrease delay per level/score threshold
#define MIN_FALL_DELAY_MS 100     // Minimum fall delay
#define LINES_PER_LEVEL 10        // Number of lines to clear to increase speed (level up)

// --- Function Prototypes for main.c ---
void init_ncurses();
void draw_board(WINDOW *win, const int board[BOARD_HEIGHT][BOARD_WIDTH]);
void draw_piece(WINDOW *win, const Piece *piece);
void draw_game_info(WINDOW *win, int score, int lines_cleared, int level);
void show_game_over(WINDOW *win);

// Global window for the game
WINDOW *game_win;

int main() {
    // --- Initialization ---
    srand(time(NULL)); // Seed random number generator
    init_ncurses();    // Initialize ncurses

    init_board(); // Initialize the game board (from tetris.c)

    Piece current_piece;
    spawn_new_piece(&current_piece); // Spawn the first piece (from tetris.c)
                                     // Assumes spawn_new_piece populates current_piece correctly
                                     // according to tetris.h's Piece struct.

    // Game state variables
    bool game_over = false;
    int score = 0;
    int lines_cleared_total = 0;
    int level = 1;
    long fall_delay_ms = INITIAL_FALL_DELAY_MS;
    long fall_timer = 0; // Accumulates time towards next fall
    struct timespec last_loop_time, current_loop_time;
    long loop_delta_ms;

    // Initial check for game over on first spawn (though spawn_new_piece might handle this)
    if (check_collision(&current_piece)) {
        game_over = true;
    }

    clock_gettime(CLOCK_MONOTONIC_RAW, &last_loop_time);

    // --- Game Loop ---
    while (!game_over) {
        // Calculate time delta for this loop iteration
        clock_gettime(CLOCK_MONOTONIC_RAW, &current_loop_time);
        loop_delta_ms = (current_loop_time.tv_sec - last_loop_time.tv_sec) * 1000 +
                        (current_loop_time.tv_nsec - last_loop_time.tv_nsec) / 1000000;
        last_loop_time = current_loop_time;

        fall_timer += loop_delta_ms;

        // --- Input Handling ---
        int ch = getch(); // Non-blocking due to nodelay(stdscr, TRUE)
        switch (ch) {
            case KEY_LEFT:
                move_piece_left(&current_piece);
                break;
            case KEY_RIGHT:
                move_piece_right(&current_piece);
                break;
            case KEY_DOWN: // Move piece down faster
                if (move_piece_down(&current_piece)) {
                    score += 1; // Small score for manual drop
                } else {
                    // If move_piece_down fails, it means it hit something.
                    // So, lock it, clear lines, and spawn new.
                    lock_piece(&current_piece);
                    int lines = clear_lines();
                    if (lines > 0) {
                        lines_cleared_total += lines;
                        score += lines * lines * 100; // Score based on lines cleared (e.g., Tetris scoring)
                        if (lines_cleared_total / LINES_PER_LEVEL >= (unsigned int)level) {
                            level++;
                            fall_delay_ms = INITIAL_FALL_DELAY_MS - (level -1) * FALL_DELAY_DECREMENT_MS;
                            if (fall_delay_ms < MIN_FALL_DELAY_MS) {
                                fall_delay_ms = MIN_FALL_DELAY_MS;
                            }
                        }
                    }
                    spawn_new_piece(&current_piece);
                    if (check_collision(&current_piece)) {
                        game_over = true;
                    }
                }
                fall_timer = 0; // Reset fall timer after manual move
                break;
            case KEY_UP: // Rotate clockwise
            case 'r':
            case 'R':
                rotate_piece_clockwise(&current_piece);
                break;
            // Add case for counter-clockwise rotation if desired, e.g., 'e'
            // case 'e':
            //    rotate_piece_counter_clockwise(&current_piece);
            //    break;
            case 'q': // Quit game
            case 'Q':
                game_over = true;
                break;
        }

        // --- Update Game State: Automatic Piece Falling ---
        if (fall_timer >= fall_delay_ms) {
            if (!move_piece_down(&current_piece)) {
                lock_piece(&current_piece);
                int lines = clear_lines();
                if (lines > 0) {
                    lines_cleared_total += lines;
                    score += lines * lines * 100; // Example scoring
                     if (lines_cleared_total / LINES_PER_LEVEL >= (unsigned int)level) {
                        level++;
                        fall_delay_ms = INITIAL_FALL_DELAY_MS - (level-1) * FALL_DELAY_DECREMENT_MS;
                        if (fall_delay_ms < MIN_FALL_DELAY_MS) {
                            fall_delay_ms = MIN_FALL_DELAY_MS;
                        }
                    }
                }
                spawn_new_piece(&current_piece);
                if (check_collision(&current_piece)) {
                    game_over = true; // Game over if new piece collides
                }
            }
            fall_timer = 0; // Reset timer
        }

        if (game_over) break; // Exit loop immediately if game over condition met

        // --- Rendering ---
        erase(); // Clear the whole screen (stdscr)

        // Draw game elements in the game window
        // Box around the game window (optional, for better UI)
        box(game_win, 0, 0);

        draw_board(game_win, game_board);
        draw_piece(game_win, &current_piece);
        draw_game_info(game_win, score, lines_cleared_total, level);

        wrefresh(game_win); // Refresh the game window
        refresh(); // Refresh stdscr if other elements are drawn directly on it (less common with a sub-window)

        // --- Timing ---
        // The loop already uses a dynamic delay based on loop_delta_ms and fall_timer.
        // A small fixed delay can prevent CPU hogging if game logic is very fast.
        usleep(10000); // e.g., 10ms delay. Adjust as needed.
    }

    // --- Game Over ---
    show_game_over(game_win);
    nodelay(stdscr, FALSE); // Make getch() blocking again to wait for key press
    getch();                // Wait for a key press

    // --- Cleanup ---
    delwin(game_win); // Delete the game window
    endwin();         // Restore terminal settings
    printf("Game Over! Final Score: %d, Lines: %d, Level: %d\n", score, lines_cleared_total, level);

    return 0;
}

/**
 * @brief Initializes ncurses settings for the game.
 */
void init_ncurses() {
    initscr();            // Start curses mode
    cbreak();             // Line buffering disabled, Pass on ever char
    noecho();             // Don't echo() while we do getch
    keypad(stdscr, TRUE); // Enable Fx keys, arrows, etc.
    nodelay(stdscr, TRUE);// Make getch() non-blocking
    curs_set(0);          // Make cursor invisible

    // Create a window for the game board area
    // Dimensions: BOARD_HEIGHT + 2 for borders, BOARD_WIDTH*2 + 2 for borders (since chars are taller than wide)
    // Position: Centered or at a fixed location
    int starty = (LINES - (BOARD_HEIGHT + 2)) / 2; // LINES is ncurses global for terminal height
    int startx = (COLS - (BOARD_WIDTH * 2 + 2)) / 2; // COLS is ncurses global for terminal width
    if (starty < 0) starty = 0;
    if (startx < 0) startx = 0;

    game_win = newwin(BOARD_HEIGHT + 2, BOARD_WIDTH * 2 + 2, starty, startx);
    // keypad(game_win, TRUE); // Enable keypad for the subwindow as well if needed
    // nodelay(game_win, TRUE); // Enable non-blocking input for the subwindow
}

/**
 * @brief Draws the game board borders and filled cells.
 * @param win The ncurses window to draw in.
 * @param board The game board data.
 */
void draw_board(WINDOW *win, const int board[BOARD_HEIGHT][BOARD_WIDTH]) {
    // wborder(win, '|', '|', '-', '-', '+', '+', '+', '+'); // Draw a border around the window

    for (int i = 0; i < BOARD_HEIGHT; ++i) {
        for (int j = 0; j < BOARD_WIDTH; ++j) {
            if (board[i][j]) {
                mvwprintw(win, i + 1, j * 2 + 1, "[]"); // Draw block (use *2 for width)
            } else {
                mvwprintw(win, i + 1, j * 2 + 1, " ."); // Draw empty space (optional)
            }
        }
    }
}

/**
 * @brief Draws the currently falling piece on the board.
 * @param win The ncurses window to draw in.
 * @param piece Pointer to the current piece.
 */
void draw_piece(WINDOW *win, const Piece *piece) {
    // Assuming piece->shape is PIECE_MATRIX_DIM x PIECE_MATRIX_DIM (e.g. 4x4)
    for (int i = 0; i < PIECE_MATRIX_DIM; ++i) {
        for (int j = 0; j < PIECE_MATRIX_DIM; ++j) {
            if (piece->shape[i][j]) {
                // Draw the block, offset by piece's x, y position
                // Add 1 to x and y for border offset in game_win
                mvwprintw(win, piece->y + i + 1, (piece->x + j) * 2 + 1, "[]");
            }
        }
    }
}

/**
 * @brief Draws game information like score, lines, level.
 * @param win The ncurses window where info should be drawn (could be game_win or another window).
 * @param score Current score.
 * @param lines_cleared Total lines cleared.
 * @param level Current game level.
 */
void draw_game_info(WINDOW *win, int score, int lines_cleared, int level) {
    // Example: Draw info outside the main game_win or at the bottom of it if space allows.
    // For this example, let's draw it on stdscr, above or next to game_win.
    // Or, if game_win is large enough, draw inside it.
    // For simplicity, drawing on stdscr at fixed positions:
    mvprintw(1, 2, "Score: %d", score);
    mvprintw(2, 2, "Lines: %d", lines_cleared_total);
    mvprintw(3, 2, "Level: %d", level);
    // If using a dedicated game_win, you might draw relative to that window:
    // mvwprintw(win, BOARD_HEIGHT + 1, 2, "Score: %d", score); // Example if space below board in game_win
}

/**
 * @brief Displays the "Game Over" message.
 * @param win The ncurses window to display the message in.
 */
void show_game_over(WINDOW *win) {
    // Clear a central part of the window or use a new window
    // For simplicity, printing in the middle of the game_win
    int height, width;
    getmaxyx(win, height, width); // Get dimensions of the window

    // Clear window content before showing game over
    // werase(win); // or wclear(win)
    // box(win, 0, 0); // Redraw box if erased

    mvwprintw(win, height / 2 - 1, (width - 10) / 2, "GAME OVER");
    mvwprintw(win, height / 2, (width - 22) / 2, "Press any key to exit...");
    wrefresh(win);
}

// Note: The `run_game()` function from tetris.c is not explicitly called here.
// The main game loop and ncurses handling are implemented directly in `main()`.
// This implies that `tetris.c`'s `run_game()` might be simplified or removed if it
// was intended to contain similar ncurses logic. If `run_game()` was purely for
// non-UI game logic sequence, then `main()` has effectively taken over that role
// with UI integration.

// This code assumes that the Piece struct in tetris.h is:
// typedef struct {
//     int x, y;
//     int shape[PIECE_MATRIX_DIM][PIECE_MATRIX_DIM]; // PIECE_MATRIX_DIM = 4
//     int type;
//     int rotation;
// } Piece;
// And that functions in tetris.c (spawn_new_piece, check_collision, lock_piece, rotate_piece_*)
// are compatible with this struct. Specifically, `spawn_new_piece` must initialize `piece.shape`,
// `piece.type`, and `piece.rotation`. `check_collision` and `lock_piece` must use `piece.shape`.
// `rotate_piece_*` must modify `piece.shape` (and potentially `piece.rotation`).
```
