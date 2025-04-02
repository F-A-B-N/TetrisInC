# Tetris in C 🎮

A classic Tetris implementation written in C for terminal environments using ncurses.

## Features ✨

- All 7 classic Tetromino pieces (I, J, L, O, S, T, Z)
- Smooth terminal rendering using ncurses
- Score system
- Next piece preview
- Collision detection
- Line clearing with scoring

## Requirements 📋

- GCC or other C compiler
- Ncurses library
- Linux/macOS terminal (or Windows with WSL)

## Installation ⚙️

### Linux/macOS

1. Clone the repository:
   ```bash
   git clone https://github.com/yourusername/TetrisInC.git
   cd TetrisInC

2. Compile with gcc:
   ```bash
   gcc -o tetris tetris.c -lncurses
3. Run the game:
   ```bash
   ./tetris
