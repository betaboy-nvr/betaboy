#ifndef TETRIS_H
#define TETRIS_H

#include <stdlib.h> // For general utilities like rand(), srand() - though typically used in .c
#include <stdbool.h> // For the bool type
#include <time.h>   // For time() - typically used in .c for srand()

// --- Constant Definitions ---

/**
 * @brief Width of the Tetris game board in cells.
 */
#define BOARD_WIDTH 10

/**
 * @brief Height of the Tetris game board in cells.
 */
#define BOARD_HEIGHT 20

/**
 * @brief Dimension of the square matrix used to define piece shapes (e.g., 4 for a 4x4 matrix).
 * All tetromino shapes can be represented within a 4x4 grid.
 */
#define PIECE_MATRIX_DIM 4


// --- Struct Definitions ---

/**
 * @brief Represents a Tetris piece (tetromino).
 *
 * Stores the piece's current top-left position (x, y) on the board,
 * its shape as a 2D array, its type (an identifier for I, O, T, etc.),
 * and its current rotation state.
 */
typedef struct {
    int x;      /**< The x-coordinate of the top-left corner of the piece's bounding box on the board. */
    int y;      /**< The y-coordinate of the top-left corner of the piece's bounding box on the board. */
    int shape[PIECE_MATRIX_DIM][PIECE_MATRIX_DIM]; /**< 2D array representing the piece's current shape. */
    int type;   /**< Identifier for the piece type (e.g., 0 for I, 1 for O, etc.). Allows access to base shapes for rotation. */
    int rotation; /**< Current rotation state (e.g., 0, 1, 2, 3). */
} Piece;


// --- Global Variable Declarations ---

/**
 * @brief The main game board.
 * Declared as extern, defined in tetris.c.
 * 0 represents an empty cell, non-zero represents a filled cell.
 */
extern int game_board[BOARD_HEIGHT][BOARD_WIDTH];


// --- Function Prototypes ---

/**
 * @brief Initializes the game board to be empty.
 */
void init_board(void);

/**
 * @brief Spawns a new random piece at the top-center of the board.
 * Updates the given Piece struct with the new piece's data.
 * @param current_piece Pointer to the Piece struct to be populated.
 */
void spawn_new_piece(Piece *current_piece);

/**
 * @brief Checks if the given piece collides with board boundaries or existing blocks.
 * @param piece Pointer to the Piece to check (const as it's not modified).
 * @return true if there is a collision, false otherwise.
 */
bool check_collision(const Piece *piece);

/**
 * @brief Locks the given piece onto the game board.
 * Copies the piece's shape into the game_board at its current position.
 * @param piece Pointer to the Piece to lock (const as it's not modified).
 */
void lock_piece(const Piece *piece);

/**
 * @brief Clears any completed lines on the board and shifts lines above down.
 * @return The number of lines cleared.
 */
int clear_lines(void);

/**
 * @brief Moves the current piece down by one unit if possible.
 * @param current_piece Pointer to the Piece to move.
 * @return true if the piece was moved successfully, false if it collided.
 */
bool move_piece_down(Piece *current_piece);

/**
 * @brief Moves the current piece left by one unit if possible.
 * @param current_piece Pointer to the Piece to move.
 */
void move_piece_left(Piece *current_piece);

/**
 * @brief Moves the current piece right by one unit if possible.
 * @param current_piece Pointer to the Piece to move.
 */
void move_piece_right(Piece *current_piece);

/**
 * @brief Rotates the current piece clockwise, if possible.
 * Handles collision checks for rotation and potential wall kicks (simplified).
 * @param current_piece Pointer to the Piece to rotate.
 */
void rotate_piece_clockwise(Piece *current_piece);

/**
 * @brief Rotates the current piece counter-clockwise, if possible.
 * Handles collision checks for rotation and potential wall kicks (simplified).
 * @param current_piece Pointer to the Piece to rotate.
 */
void rotate_piece_counter_clockwise(Piece *current_piece);

/**
 * @brief Runs the main game loop and logic.
 * Intended to be called from main.c to start and manage the game.
 */
void run_game(void);

#endif // TETRIS_H
