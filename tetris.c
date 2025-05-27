#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h> // Required for srand and rand

// Board dimensions
#define BOARD_WIDTH 10
#define BOARD_HEIGHT 20

// --- Tetromino Shapes ---
// Shapes are defined for their 0-degree rotation state using flattened 1D arrays.
// The 'size' field in the Piece struct will determine how this 1D array is treated (e.g., as 2x2, 3x3, or 4x4).

// I_SHAPE (4x4):  0000
//                 1111
//                 0000
//                 0000
const int I_SHAPE_0_DATA[] = {
    0, 0, 0, 0,
    1, 1, 1, 1,
    0, 0, 0, 0,
    0, 0, 0, 0
};

// O_SHAPE (2x2):  11
//                 11
const int O_SHAPE_0_DATA[] = {
    1, 1,
    1, 1
};

// T_SHAPE (3x3):  010
//                 111
//                 000
const int T_SHAPE_0_DATA[] = {
    0, 1, 0,
    1, 1, 1,
    0, 0, 0
};

// S_SHAPE (3x3):  011
//                 110
//                 000
const int S_SHAPE_0_DATA[] = {
    0, 1, 1,
    1, 1, 0,
    0, 0, 0
};

// Z_SHAPE (3x3):  110
//                 011
//                 000
const int Z_SHAPE_0_DATA[] = {
    1, 1, 0,
    0, 1, 1,
    0, 0, 0
};

// J_SHAPE (3x3):  100
//                 111
//                 000
const int J_SHAPE_0_DATA[] = {
    1, 0, 0,
    1, 1, 1,
    0, 0, 0
};

// L_SHAPE (3x3):  001
//                 111
//                 000
const int L_SHAPE_0_DATA[] = {
    0, 0, 1,
    1, 1, 1,
    0, 0, 0
};

// --- Piece Structure ---
/**
 * @brief Represents a Tetris piece (tetromino).
 *
 * Stores the piece's current shape data (as a pointer to a flattened 1D array),
 * its size (dimension of the square matrix, e.g., 2 for 2x2 O-piece),
 * and its x, y position on the game board (top-left of the piece's matrix).
 */
typedef struct {
    const int *shape_data; // Pointer to the current rotation's shape data (flattened 1D array)
    int size;         // Dimension of the square matrix for the shape (e.g., 2 for O, 3 for T, 4 for I)
    int x;            // X position of the top-left of the piece matrix on the board
    int y;            // Y position of the top-left of the piece matrix on the board
    // Note: A 'rotation_state' (0-3) and 'piece_type' field would be needed for a full rotation system
    // that cycles through pre-defined rotated shapes.
} Piece;

// Game board: 0 = empty, 1 (or other non-zero) = filled by a locked piece
int game_board[BOARD_HEIGHT][BOARD_WIDTH];

// --- Function Declarations (Prototypes) ---
void init_board();
bool check_collision(const Piece *piece);
void rotate_piece_clockwise(Piece *piece); // Placeholder
void rotate_piece_counter_clockwise(Piece *piece); // Placeholder
void spawn_new_piece(Piece *current_piece);
bool move_piece_down(Piece *current_piece);
void move_piece_left(Piece *current_piece);
void move_piece_right(Piece *current_piece);
void lock_piece(const Piece *piece);
int clear_lines();
void run_game(); // Main game loop placeholder

// --- Function Implementations ---

/**
 * @brief Initializes the game board to be empty.
 * Sets all cells of the global `game_board` array to 0.
 */
void init_board() {
    for (int i = 0; i < BOARD_HEIGHT; i++) {
        for (int j = 0; j < BOARD_WIDTH; j++) {
            game_board[i][j] = 0;
        }
    }
}

/**
 * @brief Checks if the given piece collides with board boundaries or existing blocks.
 *
 * Iterates through the cells of the piece's shape matrix. If a cell in the piece is filled (non-zero),
 * it calculates the corresponding coordinates on the game board.
 * Collision occurs if these coordinates are:
 *  - Outside the left, right, or bottom boundaries of the board.
 *  - Overlapping with a cell on the board that is already filled (non-zero).
 * Note: Does not check for collision with the top boundary (y < 0), assuming pieces spawn at y >= 0.
 *
 * @param piece A pointer to the `Piece` structure to check.
 * @return `true` if there is a collision, `false` otherwise.
 */
bool check_collision(const Piece *piece) {
    for (int i = 0; i < piece->size; i++) { // Iterate through rows of the piece matrix
        for (int j = 0; j < piece->size; j++) { // Iterate through columns of the piece matrix
            if (piece->shape_data[i * piece->size + j]) { // Check only the filled parts of the piece
                int board_x = piece->x + j; // Piece's cell x-coordinate on the game board
                int board_y = piece->y + i; // Piece's cell y-coordinate on the game board

                // Check for out of bounds (left, right, bottom)
                if (board_x < 0 || board_x >= BOARD_WIDTH || board_y >= BOARD_HEIGHT) {
                    return true; // Collision with side walls or bottom
                }

                // Check for collision with existing blocks on the board
                // Ensure board_y is not negative before accessing game_board,
                // although pieces typically don't move above y=0 after spawning.
                if (board_y >= 0 && game_board[board_y][board_x]) {
                    return true; // Collision with another block
                }
            }
        }
    }
    return false; // No collision
}


/**
 * @brief Rotates the piece clockwise (Placeholder).
 *
 * A full implementation would:
 * 1. Determine the next rotation state's shape data (e.g., from pre-defined arrays for all rotations).
 * 2. Create a temporary copy of the piece with the new shape data.
 * 3. Call `check_collision` on this temporary piece.
 * 4. If no collision, update the original piece's `shape_data` (and potentially `size` if it changes).
 * 5. Implement "wall kicks" (slight movements if rotation is blocked near a wall/floor but would be valid if shifted).
 * This current version is a placeholder and does not change the piece's shape.
 *
 * @param piece Pointer to the `Piece` to rotate.
 */
void rotate_piece_clockwise(Piece *piece) {
    // This is a placeholder. True rotation requires:
    // 1. Knowing the piece type (I, T, L, etc.) to select the correct set of rotated shapes.
    // 2. Having all rotated shapes defined (e.g., L_SHAPE_90_DATA, L_SHAPE_180_DATA, etc.).
    // 3. A 'rotation_state' field in the Piece struct to know the current orientation.
    // 4. Logic to cycle piece->shape_data to the next pre-rotated array and update rotation_state.
    // 5. Collision check: if collision, revert to the previous shape_data and rotation_state.
    // (void)piece; // Suppress unused parameter warning for placeholder
    printf("Rotate clockwise (not implemented)\n");
}

/**
 * @brief Rotates the piece counter-clockwise (Placeholder).
 * Similar to `rotate_piece_clockwise`, this is a placeholder.
 * @param piece Pointer to the `Piece` to rotate.
 */
void rotate_piece_counter_clockwise(Piece *piece) {
    // (void)piece; // Suppress unused parameter warning for placeholder
    printf("Rotate counter-clockwise (not implemented)\n");
}

/**
 * @brief Spawns a new random tetromino at the top-center of the board.
 *
 * Randomly selects one of the 7 tetromino types, initializes its shape, size,
 * and starting position (top-center).
 * If the new piece collides immediately upon spawning (e.g., board is full at the top),
 * it indicates a game over condition.
 *
 * @param current_piece Pointer to the `Piece` structure to be updated with the new piece's data.
 */
void spawn_new_piece(Piece *current_piece) {
    // Note: srand() should be called once at the beginning of the game (e.g., in run_game or main).
    int piece_type = rand() % 7; // Random number between 0 and 6

    switch (piece_type) {
        case 0: // I-Piece
            current_piece->shape_data = I_SHAPE_0_DATA;
            current_piece->size = 4;
            break;
        case 1: // O-Piece
            current_piece->shape_data = O_SHAPE_0_DATA;
            current_piece->size = 2;
            break;
        case 2: // T-Piece
            current_piece->shape_data = T_SHAPE_0_DATA;
            current_piece->size = 3;
            break;
        case 3: // S-Piece
            current_piece->shape_data = S_SHAPE_0_DATA;
            current_piece->size = 3;
            break;
        case 4: // Z-Piece
            current_piece->shape_data = Z_SHAPE_0_DATA;
            current_piece->size = 3;
            break;
        case 5: // J-Piece
            current_piece->shape_data = J_SHAPE_0_DATA;
            current_piece->size = 3;
            break;
        case 6: // L-Piece
            current_piece->shape_data = L_SHAPE_0_DATA;
            current_piece->size = 3;
            break;
    }

    // Position new piece at top-center
    current_piece->x = BOARD_WIDTH / 2 - current_piece->size / 2;
    current_piece->y = 0; // Spawn at the very top row

    // Check for game over: if the new piece collides immediately.
    if (check_collision(current_piece)) {
        // This is a basic game over condition. A full game would handle this state.
        printf("GAME OVER - Collision on spawn\n");
        // In a real game, set a game over flag or trigger a game over sequence.
        // For now, the game might continue with this piece overlapping, which is not ideal.
        // Or, could re-initialize board: init_board(); spawn_new_piece(current_piece); to restart.
    }
}

/**
 * @brief Moves the current piece down by one unit if no collision occurs.
 *
 * Increments the piece's y-coordinate. If this results in a collision,
 * the move is reverted (y-coordinate is decremented), and the function returns false.
 *
 * @param current_piece Pointer to the `Piece` to move.
 * @return `true` if the piece was successfully moved down, `false` if it collided.
 */
bool move_piece_down(Piece *current_piece) {
    current_piece->y++;
    if (check_collision(current_piece)) {
        current_piece->y--; // Revert move due to collision
        return false;       // Cannot move down
    }
    return true; // Moved successfully
}

/**
 * @brief Moves the current piece left by one unit if no collision occurs.
 *
 * Decrements the piece's x-coordinate. If this results in a collision,
 * the move is reverted (x-coordinate is incremented).
 *
 * @param current_piece Pointer to the `Piece` to move.
 */
void move_piece_left(Piece *current_piece) {
    current_piece->x--;
    if (check_collision(current_piece)) {
        current_piece->x++; // Revert move due to collision
    }
}

/**
 * @brief Moves the current piece right by one unit if no collision occurs.
 *
 * Increments the piece's x-coordinate. If this results in a collision,
 * the move is reverted (x-coordinate is decremented).
 *
 * @param current_piece Pointer to the `Piece` to move.
 */
void move_piece_right(Piece *current_piece) {
    current_piece->x++;
    if (check_collision(current_piece)) {
        current_piece->x--; // Revert move due to collision
    }
}

/**
 * @brief Locks the current piece onto the game board.
 *
 * Iterates through the cells of the piece's shape matrix. If a cell in the piece is filled,
 * the corresponding cell on the `game_board` is marked as filled (e.g., set to 1).
 * This function should be called when a piece can no longer move down.
 *
 * @param piece Pointer to the `Piece` to lock onto the board.
 */
void lock_piece(const Piece *piece) {
    for (int i = 0; i < piece->size; i++) { // Iterate through rows of the piece matrix
        for (int j = 0; j < piece->size; j++) { // Iterate through columns of the piece matrix
            if (piece->shape_data[i * piece->size + j]) { // If this part of the piece is filled
                int board_x = piece->x + j;
                int board_y = piece->y + i;

                // Make sure the part of the piece is within board bounds before locking.
                // This check is important if a piece can somehow be partially out of bounds when lock_piece is called.
                // Typically, collision detection should prevent this, but it's a good safeguard.
                if (board_y >= 0 && board_y < BOARD_HEIGHT && board_x >= 0 && board_x < BOARD_WIDTH) {
                    game_board[board_y][board_x] = 1; // Mark the board cell as occupied
                }
            }
        }
    }
}

/**
 * @brief Checks for and clears any completed (full) lines on the board.
 *
 * Iterates through the board from bottom to top. If a line is full, it's cleared,
 * and all lines above it are shifted down by one unit.
 * The topmost line of the board is then cleared.
 *
 * @return The number of lines cleared.
 */
int clear_lines() {
    int lines_cleared = 0;
    for (int i = BOARD_HEIGHT - 1; i >= 0; i--) { // Iterate rows from bottom to top
        bool line_is_full = true;
        for (int j = 0; j < BOARD_WIDTH; j++) { // Iterate cells in the current row
            if (game_board[i][j] == 0) {
                line_is_full = false; // Found an empty cell, line is not full
                break;
            }
        }

        if (line_is_full) {
            lines_cleared++;
            // Shift all lines above the current cleared line down by one
            for (int k = i; k > 0; k--) { // Start from the cleared line index 'i'
                for (int j = 0; j < BOARD_WIDTH; j++) {
                    game_board[k][j] = game_board[k - 1][j]; // Move line k-1 down to line k
                }
            }
            // Clear the topmost line (row 0) as it's now effectively empty or garbage
            for (int j = 0; j < BOARD_WIDTH; j++) {
                game_board[0][j] = 0;
            }
            // Since lines were shifted down, the current row index 'i'
            // needs to be re-checked in case the new line 'i' (which was 'i-1') is also full.
            i++;
        }
    }
    return lines_cleared;
}

/**
 * @brief Placeholder for the main game loop function.
 *
 * This function would typically:
 * 1. Initialize the game (board, score, seed random generator).
 * 2. Spawn the first piece.
 * 3. Enter a loop that continues until a game over condition:
 *    a. Handle user input (move left/right, rotate, drop).
 *    b. Automatically move the current piece down at a set interval.
 *    c. If the piece cannot move down:
 *       i. Lock the piece onto the board (`lock_piece`).
 *       ii. Check for and clear completed lines (`clear_lines`), updating score.
 *       iii. Spawn a new piece (`spawn_new_piece`).
 *       iv. If the new piece causes an immediate collision, game over.
 *    d. Render the game state (board and current piece).
 *    e. Repeat.
 *
 * This function's full implementation will interact with `main.c` (for program entry,
 * input handling, and rendering if using a library like ncurses or SDL) and
 * potentially `tetris.h` (for shared definitions if this file were split).
 */
void run_game() {
    // Seed the random number generator once at the start of the game.
    srand(time(NULL));

    init_board(); // Initialize the game board to empty

    Piece current_piece;
    spawn_new_piece(&current_piece); // Spawn the first piece

    printf("run_game() called. Game initialized. Board empty, first piece spawned.\n");
    printf("Main game loop logic is conceptual and would be implemented here or in main.c.\n");

    // --- Conceptual Game Loop (Illustrative) ---
    // bool game_is_over = false;
    // time_t last_fall_time = time(NULL);
    // const double fall_interval_seconds = 0.5; // Piece falls every 0.5 seconds

    // while (!game_is_over) {
    //     // Handle user input (non-blocking check)
    //     // e.g., if (kbhit()) { char input = getch(); process_input(input, &current_piece); }

    //     // Automatic piece descent based on timer
    //     if (difftime(time(NULL), last_fall_time) >= fall_interval_seconds) {
    //         if (!move_piece_down(&current_piece)) {
    //             lock_piece(&current_piece);
    //             int lines = clear_lines();
    //             // Update score based on 'lines'
    //             spawn_new_piece(&current_piece);
    //             if (check_collision(&current_piece)) { // Check if new piece causes game over
    //                 printf("GAME OVER - New piece cannot be placed.\n");
    //                 game_is_over = true;
    //             }
    //         }
    //         last_fall_time = time(NULL);
    //     }

    //     // Render game (e.g., print_board_to_console(game_board, &current_piece))
    //     // Add a small delay to control game speed if not using timer-based events
    //     // e.g., usleep(10000); // 10ms
    // }

    // printf("Exiting game.\n");
}
```
