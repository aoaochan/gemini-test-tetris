#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <conio.h>
#include <time.h>

#define BOARD_WIDTH 10
#define BOARD_HEIGHT 20
#define EMPTY_CELL ' '
#define FILLED_CELL '#'
#define GHOST_CELL '.'

// --- Structs ---
typedef struct {
    int x, y;
    int shape[4][4];
} Piece;

// --- Global Variables ---
char board[BOARD_HEIGHT][BOARD_WIDTH];
int score = 0;
int gameOver = 0;
Piece currentPiece;
const int tetrominoes[7][4][4] = {
    {{0,1,0,0}, {1,1,1,0}, {0,0,0,0}, {0,0,0,0}}, // T
    {{0,1,1,0}, {0,1,1,0}, {0,0,0,0}, {0,0,0,0}}, // O
    {{0,0,1,0}, {1,1,1,0}, {0,0,0,0}, {0,0,0,0}}, // L
    {{1,0,0,0}, {1,1,1,0}, {0,0,0,0}, {0,0,0,0}}, // J
    {{1,1,0,0}, {0,1,1,0}, {0,0,0,0}, {0,0,0,0}}, // S
    {{0,1,1,0}, {1,1,0,0}, {0,0,0,0}, {0,0,0,0}}, // Z
    {{1,1,1,1}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}}  // I
};

// --- Function Prototypes ---
void setCursorPosition(int x, int y);
void initializeBoard(void);
int checkCollision(int newX, int newY, int shape[4][4]);
void calculateGhostPosition(Piece* ghost);
void drawBoard(void);
void spawnNewPiece(void);
void lockPiece(void);
void clearLines(void);
void rotatePiece(void);
void hardDrop(void);
void handleInput(void);
void updateGame(void);

// --- Function Definitions ---
void setCursorPosition(int x, int y) {
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void initializeBoard(void) {
    for (int i = 0; i < BOARD_HEIGHT; i++) {
        for (int j = 0; j < BOARD_WIDTH; j++) {
            board[i][j] = EMPTY_CELL;
        }
    }
}

void calculateGhostPosition(Piece* ghost) {
    *ghost = currentPiece;
    while (!checkCollision(ghost->x, ghost->y + 1, ghost->shape)) {
        ghost->y++;
    }
}

void drawBoard(void) {
    setCursorPosition(0, 0);
    printf("Score: %d\n", score);
    printf("+");
    for (int j = 0; j < BOARD_WIDTH; j++) printf("--");
    printf("+\n");

    char tempBoard[BOARD_HEIGHT][BOARD_WIDTH];
    memcpy(tempBoard, board, sizeof(board));

    Piece ghostPiece;
    calculateGhostPosition(&ghostPiece);
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (ghostPiece.shape[i][j]) {
                if (ghostPiece.y + i >= 0) {
                   tempBoard[ghostPiece.y + i][ghostPiece.x + j] = GHOST_CELL;
                }
            }
        }
    }

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (currentPiece.shape[i][j]) {
                if (currentPiece.y + i >= 0) {
                   tempBoard[currentPiece.y + i][currentPiece.x + j] = FILLED_CELL;
                }
            }
        }
    }

    for (int i = 0; i < BOARD_HEIGHT; i++) {
        printf("|");
        for (int j = 0; j < BOARD_WIDTH; j++) {
            if (tempBoard[i][j] == FILLED_CELL) {
                printf("##");
            } else if (tempBoard[i][j] == GHOST_CELL) {
                printf("::");
            } else {
                printf("  ");
            }
        }
        printf("|\n");
    }

    printf("+");
    for (int j = 0; j < BOARD_WIDTH; j++) printf("--");
    printf("+\n");
    printf("Controls: A(Left), D(Right), S(Down), W(Rotate), Q(Hard Drop)\n");
}

void spawnNewPiece(void) {
    srand(time(0));
    int pieceIndex = rand() % 7;
    currentPiece.x = BOARD_WIDTH / 2 - 2;
    currentPiece.y = 0;
    memcpy(currentPiece.shape, tetrominoes[pieceIndex], sizeof(currentPiece.shape));
}

int checkCollision(int newX, int newY, int shape[4][4]) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (shape[i][j]) {
                int boardX = newX + j;
                int boardY = newY + i;
                if (boardX < 0 || boardX >= BOARD_WIDTH || boardY >= BOARD_HEIGHT) {
                    return 1;
                }
                if (boardY >= 0 && board[boardY][boardX] != EMPTY_CELL) {
                    return 1;
                }
            }
        }
    }
    return 0;
}

void lockPiece(void) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (currentPiece.shape[i][j]) {
                int boardY = currentPiece.y + i;
                int boardX = currentPiece.x + j;
                if (boardY >= 0) {
                    board[boardY][boardX] = FILLED_CELL;
                }
            }
        }
    }
}

void clearLines(void) {
    int linesCleared = 0;
    for (int i = BOARD_HEIGHT - 1; i >= 0; i--) {
        int fullLine = 1;
        for (int j = 0; j < BOARD_WIDTH; j++) {
            if (board[i][j] == EMPTY_CELL) {
                fullLine = 0;
                break;
            }
        }
        if (fullLine) {
            linesCleared++;
            for (int k = i; k > 0; k--) {
                memcpy(board[k], board[k - 1], sizeof(board[0]));
            }
            memset(board[0], EMPTY_CELL, sizeof(board[0]));
            i++;
        }
    }
    score += linesCleared * linesCleared * 100;
}

void rotatePiece(void) {
    int rotatedShape[4][4] = {0};
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (currentPiece.shape[i][j]) {
                rotatedShape[j][3 - i] = currentPiece.shape[i][j];
            }
        }
    }
    if (!checkCollision(currentPiece.x, currentPiece.y, rotatedShape)) {
        memcpy(currentPiece.shape, rotatedShape, sizeof(currentPiece.shape));
    }
}

void hardDrop(void) {
    while (!checkCollision(currentPiece.x, currentPiece.y + 1, currentPiece.shape)) {
        currentPiece.y++;
    }
    lockPiece();
    clearLines();
    spawnNewPiece();
    if (checkCollision(currentPiece.x, currentPiece.y, currentPiece.shape)) {
        gameOver = 1;
    }
}

void handleInput(void) {
    if (_kbhit()) {
        char key = _getch();
        switch (key) {
            case 'a': case 'A':
                if (!checkCollision(currentPiece.x - 1, currentPiece.y, currentPiece.shape)) currentPiece.x--;
                break;
            case 'd': case 'D':
                if (!checkCollision(currentPiece.x + 1, currentPiece.y, currentPiece.shape)) currentPiece.x++;
                break;
            case 's': case 'S':
                if (!checkCollision(currentPiece.x, currentPiece.y + 1, currentPiece.shape)) currentPiece.y++;
                break;
            case 'w': case 'W':
                rotatePiece();
                break;
            case 'q': case 'Q':
                hardDrop();
                drawBoard(); // Force redraw
                break;
        }
    }
}

void updateGame(void) {
    if (!checkCollision(currentPiece.x, currentPiece.y + 1, currentPiece.shape)) {
        currentPiece.y++;
    } else {
        lockPiece();
        clearLines();
        spawnNewPiece();
        if (checkCollision(currentPiece.x, currentPiece.y, currentPiece.shape)) {
            gameOver = 1;
        }
    }
}

int main(void) {
    initializeBoard();
    spawnNewPiece();

    int frameCount = 0;
    int fallSpeed = 30;

    while (!gameOver) {
        handleInput();

        if (frameCount % fallSpeed == 0) {
             updateGame();
        }

        drawBoard();
        Sleep(1000 / 60);
        frameCount++;
    }

    setCursorPosition(0, 23);
    printf("Game Over!\n");
    printf("Final Score: %d\n", score);

    return 0;
}