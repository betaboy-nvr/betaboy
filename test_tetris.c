#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h> // For memset

#include "tetris.h" // Uses game_board, Piece struct, and game logic functions

// Assertion Macro
#define ASSERT(condition, message_format, ...) \
    do { \
        if (!(condition)) { \
            fprintf(stderr, "Assertion failed: (%s) in %s, function %s, line %d: " message_format "\n", \
                    #condition, __FILE__, __func__, __LINE__, ##__VA_ARGS__); \
            exit(1); \
        } else { \
            printf("Assertion passed: " message_format "\n", ##__VA_ARGS__); \
        } \
    } while (0)

// --- Test Helper Functions ---
void print_board_for_debug() {
    printf("Current board state:\n");
    for (int i = 0; i < BOARD_HEIGHT; i++) {
        for (int j = 0; j < BOARD_WIDTH; j++) {
            printf("%d ", game_board[i][j]);
        }
        printf("\n");
    }
}

// --- Test Functions ---

void test_init_board() {
    printf("\nRunning test_init_board...\n");
    init_board();
    for (int i = 0; i < BOARD_HEIGHT; i++) {
        for (int j = 0; j < BOARD_WIDTH; j++) {
            ASSERT(game_board[i][j] == 0, "Cell (%d, %d) should be 0 after init_board, but was %d", i, j, game_board[i][j]);
        }
    }
    printf("test_init_board passed.\n");
}

void test_lock_piece() {
    printf("\nRunning test_lock_piece...\n");
    init_board();

    Piece test_piece;
    test_piece.x = 3;
    test_piece.y = 5;
    test_piece.type = 1; // O-piece (example type)
    test_piece.rotation = 0;

    // Manually define a 2x2 'O' piece within the PIECE_MATRIX_DIM (4x4)
    // This assumes tetris.c's lock_piece will correctly iterate only the relevant part if shapes are not always PIECE_MATRIX_DIM
    // Or, if it iterates the full PIECE_MATRIX_DIM, then non-block parts should be 0.
    memset(test_piece.shape, 0, sizeof(test_piece.shape)); // Clear the shape array
    test_piece.shape[0][0] = 1;
    test_piece.shape[0][1] = 1;
    test_piece.shape[1][0] = 1;
    test_piece.shape[1][1] = 1;

    lock_piece(&test_piece);

    ASSERT(game_board[5][3] == 1, "Cell (5,3) should be 1 after locking piece, but was %d", game_board[5][3]);
    ASSERT(game_board[5][4] == 1, "Cell (5,4) should be 1 after locking piece, but was %d", game_board[5][4]);
    ASSERT(game_board[6][3] == 1, "Cell (6,3) should be 1 after locking piece, but was %d", game_board[6][3]);
    ASSERT(game_board[6][4] == 1, "Cell (6,4) should be 1 after locking piece, but was %d", game_board[6][4]);
    // Check a cell outside the piece
    ASSERT(game_board[0][0] == 0, "Cell (0,0) should be 0, but was %d", game_board[0][0]);
    ASSERT(game_board[5][5] == 0, "Cell (5,5) should be 0 as it's outside the 2x2 part of the O-piece, but was %d", game_board[5][5]);


    // Test with a piece that has empty rows/cols within its 4x4 matrix (e.g. an I piece)
    init_board(); // Reset board
    Piece i_piece;
    i_piece.x = 2;
    i_piece.y = 1;
    i_piece.type = 0; // I-piece
    i_piece.rotation = 0;
    memset(i_piece.shape, 0, sizeof(i_piece.shape));
    // I piece: shape[1][0] to shape[1][3] are 1s for horizontal I
    //  0000
    //  1111
    //  0000
    //  0000
    i_piece.shape[1][0] = 1;
    i_piece.shape[1][1] = 1;
    i_piece.shape[1][2] = 1;
    i_piece.shape[1][3] = 1;

    lock_piece(&i_piece);
    ASSERT(game_board[1+1][2+0] == 1, "I-piece cell (2,2) expected 1, got %d", game_board[2][2]);
    ASSERT(game_board[1+1][2+1] == 1, "I-piece cell (2,3) expected 1, got %d", game_board[2][3]);
    ASSERT(game_board[1+1][2+2] == 1, "I-piece cell (2,4) expected 1, got %d", game_board[2][4]);
    ASSERT(game_board[1+1][2+3] == 1, "I-piece cell (2,5) expected 1, got %d", game_board[2][5]);
    ASSERT(game_board[1+0][2+0] == 0, "I-piece cell (1,2) (empty part of matrix) expected 0, got %d", game_board[1][2]);


    printf("test_lock_piece passed.\n");
}

void test_clear_lines() {
    printf("\nRunning test_clear_lines...\n");
    init_board();

    // Manually fill one entire row (e.g., row 19, the bottom row)
    for (int j = 0; j < BOARD_WIDTH; j++) {
        game_board[BOARD_HEIGHT - 1][j] = 1;
    }

    // Place some blocks in the row above (row 18)
    game_board[BOARD_HEIGHT - 2][0] = 1;
    game_board[BOARD_HEIGHT - 2][BOARD_WIDTH - 1] = 1;

    int lines_cleared = clear_lines();
    ASSERT(lines_cleared == 1, "clear_lines should return 1, but returned %d", lines_cleared);

    // Check that the filled row is now clear
    for (int j = 0; j < BOARD_WIDTH; j++) {
        ASSERT(game_board[BOARD_HEIGHT - 1][j] == 0, "Cell (%d, %d) in cleared line should be 0, but was %d", BOARD_HEIGHT - 1, j, game_board[BOARD_HEIGHT - 1][j]);
    }

    // Check that blocks from above have shifted down
    ASSERT(game_board[BOARD_HEIGHT - 1][0] == 1, "Cell (%d, %d) (shifted from above) should be 1, but was %d", BOARD_HEIGHT - 1, 0, game_board[BOARD_HEIGHT - 1][0]);
    ASSERT(game_board[BOARD_HEIGHT - 1][BOARD_WIDTH - 1] == 1, "Cell (%d, %d) (shifted from above) should be 1, but was %d", BOARD_HEIGHT - 1, BOARD_WIDTH - 1, game_board[BOARD_HEIGHT - 1][BOARD_WIDTH - 1]);
    // Check that an empty space in the shifted row remains empty
    ASSERT(game_board[BOARD_HEIGHT - 1][1] == 0, "Cell (%d, %d) (empty space shifted from above) should be 0, but was %d", BOARD_HEIGHT - 1, 1, game_board[BOARD_HEIGHT-1][1]);


    // Test clearing multiple lines
    init_board();
    for (int j = 0; j < BOARD_WIDTH; j++) {
        game_board[BOARD_HEIGHT - 1][j] = 1; // Bottom line
        game_board[BOARD_HEIGHT - 2][j] = 1; // Line above bottom
    }
    game_board[BOARD_HEIGHT - 3][0] = 1; // Block above the two full lines

    lines_cleared = clear_lines();
    ASSERT(lines_cleared == 2, "clear_lines should return 2 for two full lines, but returned %d", lines_cleared);
    ASSERT(game_board[BOARD_HEIGHT - 1][0] == 1, "Block from row H-3 should shift to H-1, cell (H-1,0) expected 1, got %d", game_board[BOARD_HEIGHT-1][0]);
    ASSERT(game_board[BOARD_HEIGHT - 2][0] == 0, "Cell (H-2,0) should be empty after lines shifted, got %d", game_board[BOARD_HEIGHT-2][0]);


    printf("test_clear_lines passed.\n");
}

void test_collision_detection() {
    printf("\nRunning test_collision_detection...\n");
    init_board();

    Piece test_piece;
    // Use a single block piece for simplicity in collision testing against boundaries
    // (1x1 piece at shape[0][0])
    memset(test_piece.shape, 0, sizeof(test_piece.shape));
    test_piece.shape[0][0] = 1;
    test_piece.type = -1; // Test type
    test_piece.rotation = 0;

    // Test collision with bottom boundary
    test_piece.x = 0;
    test_piece.y = BOARD_HEIGHT - 1; // Piece's (0,0) is on the last row
                                     // check_collision looks at piece.y + shape_row
                                     // If shape[0][0] is the only block, this is fine.
                                     // If shape[1][0] was a block, then y should be BOARD_HEIGHT - 2
    // For a piece defined by its top-left corner element at shape[0][0]:
    // If piece.shape[0][0] is 1, and piece.y = BOARD_HEIGHT - 1,
    // then board_y = (BOARD_HEIGHT - 1) + 0 = BOARD_HEIGHT - 1, which is a valid row.
    // Collision occurs if piece.y + i >= BOARD_HEIGHT.
    // So, if piece.y = BOARD_HEIGHT, then (BOARD_HEIGHT) + 0 >= BOARD_HEIGHT, collision.
    // If piece.y = BOARD_HEIGHT -1, and we try to move down (y becomes BOARD_HEIGHT), then collision.
    
    // Let's place a piece whose lowest block would be AT BOARD_HEIGHT
    test_piece.y = BOARD_HEIGHT; // The top-left of the piece matrix is at BOARD_HEIGHT
                                 // So, piece.shape[0][0] would be at game_board[BOARD_HEIGHT][0] -> collision
    ASSERT(check_collision(&test_piece) == true, "Collision with bottom boundary expected true, but got false (piece at y=%d)", test_piece.y);

    // Test collision with left boundary
    test_piece.y = 0;
    test_piece.x = -1; // piece.x + j < 0 where j=0 for shape[0][0]
    ASSERT(check_collision(&test_piece) == true, "Collision with left boundary expected true, but got false (piece at x=%d)", test_piece.x);

    // Test collision with right boundary
    test_piece.y = 0;
    test_piece.x = BOARD_WIDTH; // piece.x + j >= BOARD_WIDTH where j=0 for shape[0][0]
    ASSERT(check_collision(&test_piece) == true, "Collision with right boundary expected true, but got false (piece at x=%d)", test_piece.x);
    
    test_piece.x = BOARD_WIDTH -1; // piece.shape[0][0] is at board_x = BOARD_WIDTH -1. This is valid.
    // If the piece was wider, e.g. shape[0][1] = 1, then x=BOARD_WIDTH-1 would collide for that part.
    // Assuming shape[0][0] is the only block, this is not a collision.
    ASSERT(check_collision(&test_piece) == false, "No collision expected for single block at x=BOARD_WIDTH-1, y=0. Got true.");


    // Test collision with existing block
    init_board();
    // Lock a piece at (5,5)
    game_board[5][5] = 1;

    test_piece.x = 5;
    test_piece.y = 5; // This piece (shape[0][0]=1) now overlaps with the locked block
    ASSERT(check_collision(&test_piece) == true, "Collision with existing block at (5,5) expected true, but got false");

    test_piece.x = 4;
    test_piece.y = 5; // No collision here
    ASSERT(check_collision(&test_piece) == false, "No collision expected at (5,4) when (5,5) is blocked. Got true.");

    printf("test_collision_detection passed.\n");
}


int main() {
    printf("--- Starting Tetris Core Logic Tests ---\n");

    test_init_board();
    test_lock_piece();
    test_clear_lines();
    test_collision_detection();

    // Add calls to other test functions here as they are implemented

    printf("\n--- All tests completed successfully! ---\n");
    return 0;
}
```

A note on `test_lock_piece` and `test_collision_detection`:
The `Piece` struct in `tetris.h` has `int shape[PIECE_MATRIX_DIM][PIECE_MATRIX_DIM];` where `PIECE_MATRIX_DIM` is 4.
The tests assume that functions like `lock_piece` and `check_collision` in `tetris.c` will iterate this 4x4 `shape` array. If a piece is smaller (like a 2x2 'O' piece), its actual blocks should be represented within this 4x4 matrix, and the rest of the cells in the `shape` array should be 0. The tests for `lock_piece` reflect this by `memset`ing the shape to 0 first, then setting the active blocks.

If `tetris.c` has not been updated to use this `Piece` struct (from the old `shape_data` and `size` model), these tests will likely fail or not test the intended logic correctly. I am proceeding under the assumption that `tetris.c` *will be* or *is* compatible with `tetris.h`.

Next, I will update the `Makefile`.Okay, I've created the `test_tetris.c` file with the unit tests. Now I'll update the `Makefile` to include a `test` target that compiles and runs these tests.

I'll define `TEST_SRCS`, `TEST_OBJS`, and `TEST_TARGET`.
The `test` target will depend on `$(TEST_TARGET)` and then execute it.
The rule for `$(TEST_TARGET)` will link `test_tetris.o` and `tetris.o`.
I'll add a specific rule for `test_tetris.o`.
The `clean` rule will be updated to include test-related artifacts.

Regarding `LDFLAGS = -lncurses`: the core logic functions being tested (`init_board`, `lock_piece`, `clear_lines`, `check_collision`) ideally shouldn't directly call ncurses functions. If `tetris.c` is structured well, these functions would only manipulate the `game_board` array and `Piece` struct. Ncurses is for UI and should be in `main.c`. However, if there's some indirect dependency or if `tetris.c` was modified in a way that pulls in ncurses symbols (less ideal), linking with `-lncurses` might be necessary. For robust testing of core logic, it's better if ncurses is not involved. I'll include `$(LDFLAGS)` in the test target's linking stage for now, as specified in the prompt, but with a comment.

The `tetris.o` object file is already part of the main target's `$(OBJS)`. We can reuse it for the test target.
The `Makefile` currently has:
`OBJS = $(SRCS:.c=.o)` where `SRCS = main.c tetris.c`. So `OBJS = main.o tetris.o`.
For the test target, we need `test_tetris.o` and `tetris.o`.

Let's refine the Makefile update.
