# InSem2Project

### Group Name : Titans
### Name : Tanvi Nakum
### Student ID : 202401262

# "Tetris"

## Table of Contents
- [Introduction](#introduction)
- [Game Components](#game-components)
  - [Tetrominoes](#tetrominoes)
  - [Game Board](#game-board)
  - [User Input and Movement](#user-input-and-movement)
- [Code Explanation](#code-explanation)
- [Controls](#controls)
- [Requirements](#requirements)
- [Compilation and Execution](#compilation-and-execution)
- [Screenshot](#screenshot)
- [Conclusion](#conclusion)

## Introduction
Tetris is a classic tile-matching puzzle video game. Players move tetrominoes, which move down the playing field, to fill horizontal lines. The completed lines disappear, granting the player points, and all other blocks move down the corresponding number of lines.

## Game Components
The game consists of several key components that contribute to its functionality and gameplay experience.

### Tetrominoes
The game features different shapes of blocks (tetrominoes) that fall from the top of the screen. The player can rotate and move these blocks to fit them into empty spaces. The tetrominoes include:

- I (Straight line)
- O (Square)
- T (T-shape)
- L and J (L-shaped blocks)
- S and Z (Zig-zag blocks)

### Game Board
The game board consists of a grid where the tetrominoes fall. It has a fixed width and height, and the objective is to fill horizontal lines to clear them and score points. The board detects when a block reaches the bottom or collides with another block, stopping its movement.

### User Input and Movement
The player controls the movement of the falling tetromino using keyboard inputs. The available movements include:
- Moving left or right
- Rotating the tetromino
- Dropping the tetromino faster

## Code Explanation
- Tetromino Class: Handles the creation, movement, and rotation of different tetromino shapes.
- GameBoard Class: Manages the game board, checks for line clears, and detects collisions.
- Game Loop: Runs continuously, updating the position of the falling piece, checking for input, and clearing full lines.
- Collision Detection: Ensures pieces do not move beyond the board boundaries or overlap with existing blocks.

## Controls
- Left Arrow (←): Move block left
- Right Arrow (→): Move block right
- Up Arrow (↑): Rotate block
- Down Arrow (↓): Drop block faster
- Spacebar: Instantly drop block
- P: Pause game

## Collision Detection
Collisions occur when a falling piece touches the bottom of the board or another placed piece. If a piece cannot move down, it locks in place, and a new piece spawns at the top.

## Score Tracking
Points are awarded when a full row of blocks is completed and cleared. The score increases with the number of rows cleared at once:
- 1 Line = 100 points
- 2 Lines = 300 points
- 3 Lines = 500 points
- 4 Lines = 800 points

## Game Loop and Logic
The game operates in a loop where:

1. A new piece spawns at the top.
2. The player moves or rotates the piece.
3. The piece falls over time.
4. If a row is completed, it is cleared.
5. If pieces stack up to the top, the game ends.

##  Requirements
To run this project, you need:
- A C++ compiler (e.g., g++, MinGW)
- A terminal
- Windows

## Compilation and Execution
To compile the program, use a C++ compiler like g++:

g++ InSem2Project.cpp -o InSem2Project

Run the executable:

./InSem2Project

## Screenshot
Below is a screenshot of the game:

![Tetris Game Screenshot](Screenshot%202025-03-28%20083857.png)
![Tetris Game Screenshot](Screenshot%202025-03-28%20083927.png)
![Tetris Game Screenshot](Screenshot%202025-03-28%20083946.png)

## Conclusion
The Tetris game is an exciting puzzle game that challenges the player’s problem-solving skills. This project implements game mechanics like gravity, collision detection, and scoring, making it a great learning experience in game development.

## License
This project is released under the MIT License, allowing for open-source usage and modification. Users are free to modify and distribute the game as long as proper credit is given to the original creator.


