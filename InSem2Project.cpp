#include <iostream>
#include <vector>
#include <conio.h>
#include <windows.h>
#include <ctime>
#include <fstream> // For file operations

using namespace std;

const int WIDTH = 10;
const int HEIGHT = 20;
const char EMPTY = '.';
const char BLOCK = 'O'; // Block symbol changed to 'O'

vector<vector<char>> grid(HEIGHT, vector<char>(WIDTH, EMPTY));
int score = 0;
int highScore = 0;  // Variable to store the high score

struct Tetromino {
    vector<pair<int, int>> shape;
    int x, y;
    int color;
};

Tetromino currentPiece;

vector<vector<pair<int, int>>> tetrominoes = {
    {{0, 0}, {1, 0}, {2, 0}, {3, 0}}, // I
    {{0, 0}, {1, 0}, {0, 1}, {1, 1}}, // O
    {{0, 0}, {1, 0}, {2, 0}, {1, 1}}, // T
    {{0, 0}, {1, 0}, {1, 1}, {2, 1}}, // S
    {{1, 0}, {2, 0}, {0, 1}, {1, 1}}, // Z
    {{0, 0}, {0, 1}, {1, 1}, {2, 1}}, // L
    {{2, 0}, {0, 1}, {1, 1}, {2, 1}}  // J
};

vector<int> colors = {9, 14, 13, 10, 12, 6, 11}; // Blue, yellow, purple, green, red, orange, cyan

// Declare the function before it's used
void gameOverScreen();
void loadHighScore();
void saveHighScore();

void drawGrid() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD cursorPosition = {0, 0};
    SetConsoleCursorPosition(hConsole, cursorPosition);

    CONSOLE_SCREEN_BUFFER_INFO csbi;
    int consoleWidth = 80;
    if (GetConsoleScreenBufferInfo(hConsole, &csbi)) {
        consoleWidth = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    }
    int padding = (consoleWidth - (WIDTH * 2 + 2)) / 2; // Adjust for border width
    string pad(padding, ' ');

    cout << "\n\n";
    cout << pad << "Score: " << score << "   High Score: " << highScore << "\n\n";

    vector<vector<char>> tempGrid = grid;
    for (auto &block : currentPiece.shape) {
        tempGrid[currentPiece.y + block.second][currentPiece.x + block.first] = BLOCK;
    }

    // Top border
    cout << pad << "+";
    for (int i = 0; i < WIDTH * 2; i++) cout << "-";
    cout << "+\n";

    for (int i = 0; i < HEIGHT; i++) {
        cout << pad << "|"; // Left border
        for (int j = 0; j < WIDTH; j++) {
            if (tempGrid[i][j] == BLOCK) {
                SetConsoleTextAttribute(hConsole, currentPiece.color);
                cout << BLOCK << " "; // Using 'O' as block
                SetConsoleTextAttribute(hConsole, 7); // Reset to default
            } else {
                cout << tempGrid[i][j] << " ";
            }
        }
        cout << "|\n"; // Right border
    }

    // Bottom border
    cout << pad << "+";
    for (int i = 0; i < WIDTH * 2; i++) cout << "-";
    cout << "+\n";
}

bool isCollision(int dx, int dy) {
    for (auto &block : currentPiece.shape) {
        int newX = currentPiece.x + block.first + dx;
        int newY = currentPiece.y + block.second + dy;
        if (newX < 0 || newX >= WIDTH || newY >= HEIGHT || grid[newY][newX] == BLOCK) {
            return true;
        }
    }
    return false;
}

void clearFullRows() {
    for (int i = 0; i < HEIGHT; i++) {
        bool fullRow = true;
        for (int j = 0; j < WIDTH; j++) {
            if (grid[i][j] == EMPTY) {
                fullRow = false;
                break;
            }
        }
        if (fullRow) {
            for (int k = i; k > 0; k--) {
                grid[k] = grid[k - 1];
            }
            grid[0] = vector<char>(WIDTH, EMPTY);
            score += 10; // Increase score when a row is cleared
        }
    }
}

void lockPiece() {
    for (auto &block : currentPiece.shape) {
        grid[currentPiece.y + block.second][currentPiece.x + block.first] = BLOCK;
    }
    clearFullRows();
}

void rotatePiece() {
    vector<pair<int, int>> rotatedShape;
    for (auto &block : currentPiece.shape) {
        rotatedShape.push_back({-block.second, block.first});
    }
    Tetromino rotatedPiece = {rotatedShape, currentPiece.x, currentPiece.y};
    bool collision = false;
    for (auto &block : rotatedPiece.shape) {
        int newX = rotatedPiece.x + block.first;
        int newY = rotatedPiece.y + block.second;
        if (newX < 0 || newX >= WIDTH || newY >= HEIGHT || grid[newY][newX] == BLOCK) {
            collision = true;
            break;
        }
    }
    if (!collision) {
        currentPiece.shape = rotatedPiece.shape;
    }
}

void spawnPiece() {
    int index = rand() % tetrominoes.size();
    currentPiece.shape = tetrominoes[index];
    currentPiece.x = WIDTH / 2 - 1;
    currentPiece.y = 0;
    currentPiece.color = colors[index];
    if (isCollision(0, 0)) {
        gameOverScreen();  // Calling game over when the new piece collides
    }
}

void movePiece(int dx, int dy) {
    if (!isCollision(dx, dy)) {
        currentPiece.x += dx;
        currentPiece.y += dy;
    } else if (dy > 0) {
        lockPiece();
        spawnPiece();
    }
}

void hardDrop() {
    while (!isCollision(0, 1)) {
        currentPiece.y++;
    }
    lockPiece();  // Lock the piece once it reaches the bottom
    spawnPiece();  // Spawn a new piece
}

void gameOverScreen() {
    // Update the high score if the current score is higher
    if (score > highScore) {
        highScore = score;
        saveHighScore();  // Save high score to file
    }

    system("cls");
    string pad(10, ' ');
    cout << pad << "GAME OVER\n";
    cout << pad << "-----------\n";
    cout << pad << "Final Score: " << score << "\n";
    cout << pad << "High Score: " << highScore << "\n";
    cout << pad << "Press 'R' to Restart or 'ESC' to Exit\n";

    while (true) {
        char key = _getch();
        if (key == 'r' || key == 'R') {
            score = 0;
            grid = vector<vector<char>>(HEIGHT, vector<char>(WIDTH, EMPTY));  // Reset the grid
            spawnPiece();
            return;
        } else if (key == 27) { // ESC key to exit
            exit(0);
        }
    }
}

// Function to load high score from a file
void loadHighScore() {
    ifstream inputFile("highscore.txt");
    if (inputFile) {
        inputFile >> highScore;
    }
    inputFile.close();
}

// Function to save the current high score to a file
void saveHighScore() {
    ofstream outputFile("highscore.txt");
    if (outputFile) {
        outputFile << highScore;
    }
    outputFile.close();
}

void drawStartMenu() {
    system("cls");
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    int consoleWidth = 80;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
        consoleWidth = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    }
    int padding = (consoleWidth - 30) / 2;
    string pad(padding, ' ');

    cout << pad << "TETRIS GAME\n";
    cout << pad << "------------------------\n";
    cout << pad << "Instructions:\n";
    cout << pad << "- Use 'L' to move left\n";
    cout << pad << "- Use 'R' to move right\n";
    cout << pad << "- Use 'S' to speed up descent\n";
    cout << pad << "- Use 'W' to rotate the piece\n";
    cout << pad << "- Use 'Space' for Hard Drop\n";
    cout << pad << "- Press 'ESC' to exit during gameplay\n";
    cout << pad << "------------------------\n";
    cout << pad << "1. Start Game\n";
    cout << pad << "2. Exit\n";
    cout << pad << "High Score: " << highScore << "\n";
    cout << pad << "Select an option: ";
}

void gameLoop() {
    spawnPiece();
    int levelThreshold = 100;  // Level threshold, after which speed increases
    while (true) {
        drawGrid();
        
        // Adjust falling speed based on score
        int fallingSpeed = 200 - (score / levelThreshold) * 30;
        if (fallingSpeed < 50) fallingSpeed = 50; // Minimum speed

        if (_kbhit()) {
            char key = _getch();
            if (key == 27) {  // ESC key for exit
                cout << "\nExiting game...";
                exit(0);
            } else if (key == 'l') movePiece(-1, 0);
            else if (key == 'r') movePiece(1, 0);
            else if (key == 's') movePiece(0, 1);
            else if (key == 'w') rotatePiece();
            else if (key == 32) hardDrop();  // Space key for hard drop
        }

        movePiece(0, 1);
        Sleep(fallingSpeed);
    }
}

int main() {
    srand(time(0));
    loadHighScore();  // Load the high score from file when the game starts

    while (true) {
        drawStartMenu();
        char choice = _getch();
        if (choice == '1') {
            system("cls");
            gameLoop();
        } else if (choice == '2') {
            cout << "\nExiting game...";
            return 0;
        }
    }
}
