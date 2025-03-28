#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <conio.h>
#include <windows.h>
#include <fstream>
#include <algorithm>

using namespace std;

// Game constants
const int WIDTH = 10;
const int HEIGHT = 20;
const int BLOCK_SIZE = 4;
const int NEXT_PIECES_COUNT = 3;
const int INITIAL_DROP_DELAY = 500;
const int DROP_ACCELERATION = 50; // 50ms faster per level
const int MIN_DROP_DELAY = 100;
const int SCORE_PER_LEVEL = 1000;

// Tetromino shapes
const int SHAPES[7][4][4] = {
    {{0,0,0,0}, {1,1,1,1}, {0,0,0,0}, {0,0,0,0}}, // I
    {{0,0,0,0}, {0,1,1,0}, {0,1,1,0}, {0,0,0,0}}, // O
    {{0,0,0,0}, {0,1,0,0}, {1,1,1,0}, {0,0,0,0}}, // T
    {{0,0,0,0}, {0,1,1,0}, {1,1,0,0}, {0,0,0,0}}, // S
    {{0,0,0,0}, {1,1,0,0}, {0,1,1,0}, {0,0,0,0}}, // Z
    {{0,0,0,0}, {1,0,0,0}, {1,1,1,0}, {0,0,0,0}}, // J
    {{0,0,0,0}, {0,0,1,0}, {1,1,1,0}, {0,0,0,0}}  // L
};

//colors
enum ConsoleColor {
    BLACK = 0, BLUE = 1, GREEN = 2, CYAN = 3, RED = 4, 
    MAGENTA = 5, BROWN = 6, LIGHTGRAY = 7, DARKGRAY = 8,
    LIGHTBLUE = 9, LIGHTGREEN = 10, LIGHTCYAN = 11, 
    LIGHTRED = 12, LIGHTMAGENTA = 13, YELLOW = 14, WHITE = 15
};

class Tetromino {
public:
    int shape[BLOCK_SIZE][BLOCK_SIZE];
    int x, y;
    int type;
    ConsoleColor color;

    Tetromino() {
        type = rand() % 7;
        for (int i = 0; i < BLOCK_SIZE; i++)
            for (int j = 0; j < BLOCK_SIZE; j++)
                shape[i][j] = SHAPES[type][i][j];
        
        x = WIDTH / 2 - BLOCK_SIZE / 2;
        y = 0;
        
        switch(type) {
            case 0: color = CYAN; break;
            case 1: color = YELLOW; break;
            case 2: color = MAGENTA; break;
            case 3: color = GREEN; break;
            case 4: color = RED; break;
            case 5: color = BLUE; break;
            case 6: color = BROWN; break;
        }
    }

    Tetromino(const Tetromino& other) {
        type = other.type;
        for (int i = 0; i < BLOCK_SIZE; i++)
            for (int j = 0; j < BLOCK_SIZE; j++)
                shape[i][j] = other.shape[i][j];
        x = other.x;
        y = other.y;
        color = other.color;
    }

    void rotate() {
        int temp[BLOCK_SIZE][BLOCK_SIZE];
        for (int i = 0; i < BLOCK_SIZE; i++)
            for (int j = 0; j < BLOCK_SIZE; j++)
                temp[j][BLOCK_SIZE-1-i] = shape[i][j];
        
        for (int i = 0; i < BLOCK_SIZE; i++)
            for (int j = 0; j < BLOCK_SIZE; j++)
                shape[i][j] = temp[i][j];
    }
};

class TetrisGame {
private:
    vector<vector<int>> grid;
    vector<vector<ConsoleColor>> colorGrid;
    Tetromino currentPiece;
    vector<Tetromino> nextPieces;
    Tetromino* heldPiece = nullptr;
    bool canHold = true;
    int score = 0;
    int highScore = 0;
    int level = 1;
    int linesCleared = 0;
    bool paused = false;
    bool gameOver = false;
    bool showStartMenu = true;
    int dropDelay = INITIAL_DROP_DELAY;
    int lastDropTime = 0;
    bool softDropActive = false;

    void setColor(ConsoleColor text, ConsoleColor background = BLACK) {
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(hConsole, (WORD)((background << 4) | text));
    }

    void resetColor() {
        setColor(WHITE, BLACK);
    }

    void gotoxy(int x, int y) {
        COORD coord;
        coord.X = x;
        coord.Y = y;
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
    }

    void clearScreen() {
        system("cls");
    }

    void loadHighScore() {
        ifstream inputFile("highscore.txt");
        if (inputFile) {
            inputFile >> highScore;
        }
        inputFile.close();
    }

    void saveHighScore() {
        ofstream outputFile("highscore.txt");
        if (outputFile) {
            outputFile << highScore;
        }
        outputFile.close();
    }

    int calculateHardDropPosition() {
        int dropDistance = 0;
        while (isValidMove(currentPiece.x, currentPiece.y + dropDistance + 1)) {
            dropDistance++;
        }
        return dropDistance;
    }

    bool isValidMove(int newX, int newY) {
        for (int i = 0; i < BLOCK_SIZE; i++) {
            for (int j = 0; j < BLOCK_SIZE; j++) {
                if (currentPiece.shape[i][j]) {
                    int gridX = newX + j;
                    int gridY = newY + i;
                    
                    if (gridX < 0 || gridX >= WIDTH || gridY >= HEIGHT || 
                        (gridY >= 0 && grid[gridY][gridX])) {
                        return false;
                    }
                }
            }
        }
        return true;
    }

    void mergePiece() {
        for (int i = 0; i < BLOCK_SIZE; i++) {
            for (int j = 0; j < BLOCK_SIZE; j++) {
                if (currentPiece.shape[i][j]) {
                    grid[currentPiece.y + i][currentPiece.x + j] = 1;
                    colorGrid[currentPiece.y + i][currentPiece.x + j] = currentPiece.color;
                }
            }
        }
    }

    void clearLines() {
        int linesClearedThisTurn = 0;
        for (int i = HEIGHT - 1; i >= 0; i--) {
            bool full = true;
            for (int j = 0; j < WIDTH; j++) {
                if (!grid[i][j]) {
                    full = false;
                    break;
                }
            }
            
            if (full) {
                grid.erase(grid.begin() + i);
                grid.insert(grid.begin(), vector<int>(WIDTH, 0));
                colorGrid.erase(colorGrid.begin() + i);
                colorGrid.insert(colorGrid.begin(), vector<ConsoleColor>(WIDTH, BLACK));
                linesClearedThisTurn++;
            }
        }
        
        // Update score only if lines were cleared
        if (linesClearedThisTurn > 0) {
            switch(linesClearedThisTurn) {
                case 1: score += 100 * level; break;
                case 2: score += 300 * level; break;
                case 3: score += 500 * level; break;
                case 4: score += 800 * level; break;
            }
            
            linesCleared += linesClearedThisTurn;
            level = max(1, linesCleared / 10 + 1);
            dropDelay = max(MIN_DROP_DELAY, INITIAL_DROP_DELAY - (level - 1) * DROP_ACCELERATION);
            
            if (score > highScore) {
                highScore = score;
            }
        }
    }

    void generateNextPieces() {
        while (nextPieces.size() < NEXT_PIECES_COUNT) {
            nextPieces.push_back(Tetromino());
        }
    }

    void holdCurrentPiece() {
        if (!canHold) return;
        
        if (heldPiece == nullptr) {
            heldPiece = new Tetromino(currentPiece);
            getNewPiece();
        } else {
            Tetromino temp = currentPiece;
            currentPiece = *heldPiece;
            *heldPiece = temp;
            currentPiece.x = WIDTH / 2 - BLOCK_SIZE / 2;
            currentPiece.y = 0;
        }
        canHold = false;
    }

    void getNewPiece() {
        if (nextPieces.empty()) {
            currentPiece = Tetromino();
            generateNextPieces();
        } else {
            currentPiece = nextPieces[0];
            nextPieces.erase(nextPieces.begin());
            generateNextPieces();
        }
        currentPiece.x = WIDTH / 2 - BLOCK_SIZE / 2;
        currentPiece.y = 0;
        canHold = true;
    }

    void drawStartMenu() {
        clearScreen();
        
        gotoxy(15, 5);
        setColor(YELLOW);
        cout << "THE TETRIS GAME";
        resetColor();
        
        gotoxy(15, 6);
        cout << "------------------------";
        
        gotoxy(15, 8);
        setColor(LIGHTCYAN);
        cout << "Instructions:";
        resetColor();
        
        gotoxy(15, 9);
        cout << "- Use LEFT/RIGHT or A/D to move";
        gotoxy(15, 10);
        cout << "- Use DOWN or S to speed up";
        gotoxy(15, 11);
        cout << "- Use UP or W to rotate";
        gotoxy(15, 12);
        cout << "- Use H for Hard Drop";
        gotoxy(15, 13);
        cout << "- Use G to hold piece";
        gotoxy(15, 14);
        cout << "- Press P to pause";
        gotoxy(15, 15);
        cout << "- Press Q to quit";
        
        gotoxy(15, 16);
        cout << "------------------------";
        
        gotoxy(15, 17);
        setColor(LIGHTGREEN);
        cout << "High Score: " << highScore;
        resetColor();
        
        gotoxy(15, 19);
        setColor(LIGHTMAGENTA);
        cout << "Press any key to start...";
        resetColor();
        
        while (!_kbhit()) {
            Sleep(100);
        }
        _getch();
        showStartMenu = false;
    }

    void drawGameOver() {
        clearScreen();
        
        gotoxy(15, 5);
        setColor(RED);
        cout << "GAME OVER !";
        resetColor();
        
        gotoxy(15, 6);
        cout << "------------------------";
        
        gotoxy(15, 7);
        cout << "Final Score: " << score;
        gotoxy(15, 8);
        cout << "High Score: " << highScore;
        gotoxy(15, 9);
        cout << "Level: " << level;
        gotoxy(15, 10);
        cout << "Lines: " << linesCleared;
        
        gotoxy(15, 11);
        cout << "------------------------";
        
        gotoxy(15, 12);
        cout << "Press R to restart";
        gotoxy(15, 13);
        cout << "Press Q to quit";
        
        while (true) {
            if (_kbhit()) {
                char ch = _getch();
                if (ch == 'r' || ch == 'R') {
                    //To Reset game
                    grid = vector<vector<int>>(HEIGHT, vector<int>(WIDTH, 0));
                    colorGrid = vector<vector<ConsoleColor>>(HEIGHT, vector<ConsoleColor>(WIDTH, BLACK));
                    score = 0;
                    level = 1;
                    linesCleared = 0;
                    dropDelay = INITIAL_DROP_DELAY;
                    gameOver = false;
                    generateNextPieces();
                    getNewPiece();
                    break;
                } else if (ch == 'q' || ch == 'Q') {
                    gameOver = true;
                    break;
                }
            }
            Sleep(100);
        }
    }

    void drawGame() {
        clearScreen();
        
        gotoxy(0, 0);
        setColor(LIGHTGREEN);
        cout << "Score: " << score;
        gotoxy(0, 1);
        cout << "High Score: " << highScore;
        gotoxy(0, 2);
        cout << "Level: " << level;
        gotoxy(0, 3);
        cout << "Lines: " << linesCleared;
        resetColor();
        
        //game boundary
        gotoxy(0, 5);
        setColor(WHITE);
        cout << "+";
        for (int j = 0; j < WIDTH; j++) cout << "--";
        cout << "+";
        resetColor();

        //grid
        for (int i = 0; i < HEIGHT; i++) {
            gotoxy(0, 6 + i);
            setColor(WHITE);
            cout << "|";
            resetColor();
            
            for (int j = 0; j < WIDTH; j++) {
                if (grid[i][j]) {
                    setColor(colorGrid[i][j]);
                    cout << "[]";
                    resetColor();
                } else {
                    cout << " .";
                }
            }
            
            setColor(WHITE);
            cout << "|";
            resetColor();
        }

        //bottom boundary
        gotoxy(0, 6 + HEIGHT);
        setColor(WHITE);
        cout << "+";
        for (int j = 0; j < WIDTH; j++) cout << "--";
        cout << "+";
        resetColor();

        //current piece
        for (int i = 0; i < BLOCK_SIZE; i++) {
            for (int j = 0; j < BLOCK_SIZE; j++) {
                if (currentPiece.shape[i][j] && currentPiece.y + i >= 0) {
                    gotoxy(1 + (currentPiece.x + j) * 2, 6 + currentPiece.y + i);
                    setColor(currentPiece.color);
                    cout << "[]";
                    resetColor();
                }
            }
        }

        //next pieces
        gotoxy(WIDTH * 2 + 5, 5);
        setColor(LIGHTCYAN);
        cout << "Next:";
        resetColor();
        
        for (int p = 0; p < NEXT_PIECES_COUNT; p++) {
            if (p < nextPieces.size()) {
                for (int i = 0; i < BLOCK_SIZE; i++) {
                    gotoxy(WIDTH * 2 + 5, 6 + p * 5 + i);
                    for (int j = 0; j < BLOCK_SIZE; j++) {
                        if (nextPieces[p].shape[i][j]) {
                            setColor(nextPieces[p].color);
                            cout << "[]";
                            resetColor();
                        } else {
                            cout << "  ";
                        }
                    }
                }
            }
        }

        //held piece
        gotoxy(WIDTH * 2 + 5, 20);
        setColor(LIGHTMAGENTA);
        cout << "Held:";
        resetColor();
        
        if (heldPiece != nullptr) {
            for (int i = 0; i < BLOCK_SIZE; i++) {
                gotoxy(WIDTH * 2 + 5, 21 + i);
                for (int j = 0; j < BLOCK_SIZE; j++) {
                    if (heldPiece->shape[i][j]) {
                        setColor(heldPiece->color);
                        cout << "[]";
                        resetColor();
                    } else {
                        cout << "  ";
                    }
                }
            }
        }

        //controls
        gotoxy(WIDTH * 2 + 5, 26);
        setColor(YELLOW);
        cout << "Controls:";
        resetColor();
        
        gotoxy(WIDTH * 2 + 5, 27);
        cout << "Left:  <- or A";
        gotoxy(WIDTH * 2 + 5, 28);
        cout << "Right: -> or D";
        gotoxy(WIDTH * 2 + 5, 29);
        cout << "Rotate: ^ or W";
        gotoxy(WIDTH * 2 + 5, 30);
        cout << "Drop:   v or S";
        gotoxy(WIDTH * 2 + 5, 31);
        cout << "Hard Drop: H";
        gotoxy(WIDTH * 2 + 5, 32);
        cout << "Hold: G";
        gotoxy(WIDTH * 2 + 5, 33);
        cout << "Pause: P";
        gotoxy(WIDTH * 2 + 5, 34);
        cout << "Quit: Q";

        //game over or paused message
        if (gameOver) {
            gotoxy(WIDTH, HEIGHT / 2 + 3);
            setColor(RED);
            cout << "GAME OVER";
            resetColor();
        } else if (paused) {
            gotoxy(WIDTH, HEIGHT / 2 + 3);
            setColor(YELLOW);
            cout << "PAUSED";
            gotoxy(WIDTH - 4, HEIGHT / 2 + 4);
            cout << "Press P to resume";
            resetColor();
        }
    }

    void handleInput() {
        if (_kbhit()) {
            int ch = _getch();
            
            //arrow keys
            if (ch == 0 || ch == 224) {
                ch = _getch();
                
                int newX = currentPiece.x, newY = currentPiece.y;
                
                switch(ch) {
                    case 75: newX--; break; // Left arrow
                    case 77: newX++; break; // Right arrow
                    case 80: newY++; softDropActive = true; break; // Down arrow
                    case 72: { // Up arrow (rotate)
                        Tetromino temp = currentPiece;
                        temp.rotate();
                        if (isValidMove(temp.x, temp.y)) {
                            currentPiece.rotate();
                        }
                        break;
                    }
                }
                
                if (!paused && isValidMove(newX, newY)) {
                    currentPiece.x = newX;
                    currentPiece.y = newY;
                }
            }
            else { //regular keys
                int newX = currentPiece.x, newY = currentPiece.y;
                
                switch(tolower(ch)) {
                    case 'a': newX--; break;
                    case 'd': newX++; break;
                    case 's': newY++; softDropActive = true; break;
                    case 'w': { //To Rotate
                        Tetromino temp = currentPiece;
                        temp.rotate();
                        if (isValidMove(temp.x, temp.y)) {
                            currentPiece.rotate();
                        }
                        break;
                    }
                    case 'h': { //To Hard drop
                        int dropDistance = calculateHardDropPosition();
                        currentPiece.y += dropDistance;
                        mergePiece();
                        clearLines();
                        getNewPiece();
                        if (!isValidMove(currentPiece.x, currentPiece.y)) {
                            gameOver = true;
                        }
                        break;
                    }
                    case 'g': holdCurrentPiece(); break;
                    case 'p': paused = !paused; break;
                    case 'q': gameOver = true; break;
                }
                
                if (!paused && tolower(ch) != 'h' && isValidMove(newX, newY)) {
                    currentPiece.x = newX;
                    currentPiece.y = newY;
                }
            }
        }
        else {
            softDropActive = false;
        }
    }

    bool update() {
        if (gameOver) return false;
        if (paused) return true;
        
        int currentTime = GetTickCount();
        
        // automatic dropping
        if (currentTime - lastDropTime > (softDropActive ? dropDelay / 10 : dropDelay)) {
            lastDropTime = currentTime;
            
            if (isValidMove(currentPiece.x, currentPiece.y + 1)) {
                currentPiece.y++;
                return true;
            } else {
                mergePiece();
                clearLines();
                getNewPiece();
                if (!isValidMove(currentPiece.x, currentPiece.y)) {
                    gameOver = true;
                    saveHighScore();
                }
                return !gameOver;
            }
        }
        return true;
    }

public:
    TetrisGame() : grid(HEIGHT, vector<int>(WIDTH, 0)), 
                 colorGrid(HEIGHT, vector<ConsoleColor>(WIDTH, BLACK)) {
        srand(time(0));
        loadHighScore();
    }

    ~TetrisGame() {
        if (heldPiece) delete heldPiece;
        saveHighScore();
    }

    void run() {
        //To Set console window size
        system("mode con cols=60 lines=40");
        
        //To Hide cursor
        HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
        CONSOLE_CURSOR_INFO cursorInfo;
        GetConsoleCursorInfo(out, &cursorInfo);
        cursorInfo.bVisible = false;
        SetConsoleCursorInfo(out, &cursorInfo);
        
        lastDropTime = GetTickCount();
        
        while (true) {
            if (showStartMenu) {
                drawStartMenu();
                generateNextPieces();
                getNewPiece();
                continue;
            }
            
            if (gameOver) {
                drawGameOver();
                if (gameOver) break; //(quit)
                continue;
            }
            
            drawGame();
            handleInput();
            if (!update() && !gameOver) {
                gameOver = true;
                saveHighScore();
            }
            Sleep(500);
        }
    }
};

int main() {
    TetrisGame game;
    game.run();
    return 0;
}