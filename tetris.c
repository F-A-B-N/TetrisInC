#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>

#define WIDTH 10
#define HEIGHT 20
#define PAD_X 5
#define PAD_Y 1

typedef struct {
    int x, y;
    int piece[4][4];
    int type;
} Tetromino;

char pieceList[7][2] = {"I", "O", "T", "L", "J", "S", "Z"};
int gameGrid[HEIGHT][WIDTH] = {0};
int score = 0;
int level = 1;
int linesCleared = 0;

int pieces[7][4][4] = {
    // I
    {{0,0,0,0},
     {1,1,1,1},
     {0,0,0,0},
     {0,0,0,0}},
    // O
    {{0,0,0,0},
     {0,1,1,0},
     {0,1,1,0},
     {0,0,0,0}},
    // T
    {{0,0,0,0},
     {1,1,1,0},
     {0,1,0,0},
     {0,0,0,0}},
    // L
    {{0,0,0,0},
     {1,1,1,0},
     {1,0,0,0},
     {0,0,0,0}},
    // J
    {{0,0,0,0},
     {1,1,1,0},
     {0,0,1,0},
     {0,0,0,0}},
    // S
    {{0,0,0,0},
     {0,1,1,0},
     {1,1,0,0},
     {0,0,0,0}},
    // Z
    {{0,0,0,0},
     {1,1,0,0},
     {0,1,1,0},
     {0,0,0,0}}
};

void initTetromino(Tetromino *t, int type) {
    t->type = type;
    t->x = WIDTH/2 - 2;
    t->y = 0;
    memcpy(t->piece, pieces[type], sizeof(t->piece));
}

void rotateMatrix(int matrix[4][4]) {
    int temp[4][4];
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            temp[j][3-i] = matrix[i][j];
        }
    }
    memcpy(matrix, temp, sizeof(temp));
}

int checkCollision(Tetromino *t) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (t->piece[i][j]) {
                int x = t->x + j;
                int y = t->y + i;
                
                if (x < 0 || x >= WIDTH || y >= HEIGHT)
                    return 1;
                
                if (y >= 0 && gameGrid[y][x])
                    return 1;
            }
        }
    }
    return 0;
}

void mergeTetromino(Tetromino *t) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (t->piece[i][j]) {
                int x = t->x + j;
                int y = t->y + i;
                if (y >= 0 && x >= 0 && x < WIDTH && y < HEIGHT) {
                    gameGrid[y][x] = t->type + 1;
                }
            }
        }
    }
}

int clearLines() {
    int lines = 0;
    for (int i = HEIGHT-1; i >= 0; i--) {
        int full = 1;
        for (int j = 0; j < WIDTH; j++) {
            if (!gameGrid[i][j]) {
                full = 0;
                break;
            }
        }
        
        if (full) {
            lines++;
            for (int k = i; k > 0; k--) {
                for (int j = 0; j < WIDTH; j++) {
                    gameGrid[k][j] = gameGrid[k-1][j];
                }
            }
            for (int j = 0; j < WIDTH; j++) {
                gameGrid[0][j] = 0;
            }
            i++;
        }
    }
    
    if (lines > 0) {
        linesCleared += lines;
        switch (lines) {
            case 1: score += 100 * level; break;
            case 2: score += 300 * level; break;
            case 3: score += 500 * level; break;
            case 4: score += 800 * level; break;
        }
        level = 1 + linesCleared / 10;
    }
    
    return lines;
}

void drawGame(Tetromino *current, Tetromino *next) {
    clear();
    
    for (int i = 0; i < HEIGHT+1; i++) {
        mvprintw(i+PAD_Y, PAD_X-1, "|");
        mvprintw(i+PAD_Y, PAD_X+WIDTH*2, "|");
    }
    for (int j = 0; j < WIDTH*2+2; j++) {
        mvprintw(HEIGHT+PAD_Y, PAD_X-1+j, "-");
    }

    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            if (gameGrid[i][j]) {
                attron(COLOR_PAIR(gameGrid[i][j]));
                mvprintw(i+PAD_Y, PAD_X+j*2, "[]");
                attroff(COLOR_PAIR(gameGrid[i][j]));
            } else {
                mvprintw(i+PAD_Y, PAD_X+j*2, "  ");
            }
        }
    }
    
    // Draw current piece with proper positioning
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (current->piece[i][j]) {
                int y = current->y + i;
                int x = current->x + j;
                if (y >= 0 && x >= 0 && x < WIDTH && y < HEIGHT) {
                    attron(COLOR_PAIR(current->type + 1));
                    mvprintw(y+PAD_Y, PAD_X+x*2, "[]");
                    attroff(COLOR_PAIR(current->type + 1));
                }
            }
        }
    }
    
    mvprintw(1, PAD_X+WIDTH*2+4, "Next:");
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (next->piece[i][j]) {
                attron(COLOR_PAIR(next->type + 1));
                mvprintw(3+i, PAD_X+WIDTH*2+4+j*2, "[]");
                attroff(COLOR_PAIR(next->type + 1));
            } else {
                mvprintw(3+i, PAD_X+WIDTH*2+4+j*2, "  ");
            }
        }
    }
    
    // Draw score info
    mvprintw(8, PAD_X+WIDTH*2+4, "Score: %d", score);
    mvprintw(9, PAD_X+WIDTH*2+4, "Level: %d", level);
    mvprintw(10, PAD_X+WIDTH*2+4, "Lines: %d", linesCleared);
    
    // Draw controls
    mvprintw(HEIGHT-6+PAD_Y, PAD_X+WIDTH*2+4,  "Controls:");
    mvprintw(HEIGHT-5+PAD_Y, PAD_X+WIDTH*2+4,  "Left:   <-");
    mvprintw(HEIGHT-4+PAD_Y, PAD_X+WIDTH*2+4,  "Right:  ->");
    mvprintw(HEIGHT-3+PAD_Y, PAD_X+WIDTH*2+4,  "Rotate: UP");
    mvprintw(HEIGHT-2+PAD_Y, PAD_X+WIDTH*2+4,  "Drop: DOWN");
    mvprintw(HEIGHT+-2+PAD_Y, PAD_X+WIDTH*2+4, "Quit:    Q");
    
    refresh();
}

int main() {
    srand(time(NULL));
    
    initscr();
    raw();
    keypad(stdscr, TRUE);
    noecho();
    curs_set(0);
    timeout(100);
    
    start_color();
    init_pair(1, COLOR_CYAN, COLOR_BLACK);
    init_pair(2, COLOR_YELLOW, COLOR_BLACK);
    init_pair(3, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(4, COLOR_WHITE, COLOR_BLACK);
    init_pair(5, COLOR_BLUE, COLOR_BLACK);
    init_pair(6, COLOR_RED, COLOR_BLACK);
    init_pair(7, COLOR_GREEN, COLOR_BLACK);
    
    Tetromino current, next;
    initTetromino(&current, rand() % 7);
    initTetromino(&next, rand() % 7);
    
    int run = 1;
    int tick = 0;
    int speed = 5;
    
    while (run) {
        int ch = getch();
        
        Tetromino temp = current;
        
        switch (ch) {
            case KEY_LEFT:
                temp.x--;
                if (!checkCollision(&temp)) current = temp;
                break;
            case KEY_RIGHT:
                temp.x++;
                if (!checkCollision(&temp)) current = temp;
                break;
            case KEY_DOWN:
                temp.y++;
                if (!checkCollision(&temp)) {
                    current = temp;
                } else {
                    mergeTetromino(&current);
                    clearLines();
                    current = next;
                    initTetromino(&next, rand() % 7);
                    if (checkCollision(&current)) {
                        run = 0;
                    }
                }
                tick = 0;
                break;
            case KEY_UP:
                rotateMatrix(temp.piece);
                if (!checkCollision(&temp)) current = temp;
                break;
            case 'q':
            case 'Q':
                run = 0;
                break;
        }
        
        if (++tick > (speed - level + 1)) {
            tick = 0;
            temp = current;
            temp.y++;
            
            if (!checkCollision(&temp)) {
                current = temp;
            } else {
                mergeTetromino(&current);
                clearLines();
                current = next;
                initTetromino(&next, rand() % 7);
                if (checkCollision(&current)) {
                    run = 0;
                }
            }
        }
        
        drawGame(&current, &next);
    }
    
    endwin();
    printf("Game Over! Final Score: %d\n", score);
    return 0;
}