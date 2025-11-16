// The Saga of Rocky: The Golden Floret
// ECE 114 - Fall 2025
// Ahmed Ahmed
// Universal Libraries
#include "consoleGameEngine.h"
#include "menues.h"
#include "scenes.h"
using namespace std;

// Constant Definitions
int player_x = WIDTH / 2;
int player_y = HEIGHT / 2;
char map[HEIGHT][WIDTH];
char player_c = '^';

// Function declarations
// Mechanics
void initializeConsole();
void initializeMap();
void initializeGardenMap();
void updateGame(int key);
void drawGame();
bool IsObstacle(char tile);
void introductionCinematic();

// Menues
bool titleScreen();

// ##### MAIN GAME LOOP ###############################################################################################

int main(int argc, char const *argv[])
{
    initializeConsole(); // Set up console to eliminate flicker
    bool titleOption = titleScreen();

    if (titleOption)
    {
        introductionCinematic();
        initializeMap(); // Set up the starting point of the map

        // Game loop that runs as long as the character continues to play
        while (true)
        {

            int userInput = getLiveInput();

            if (userInput != 0 && userInput != -1)
            {
                updateGame(userInput);
            }

            drawGame();
            this_thread::sleep_for(chrono::milliseconds(5));
        }
    }
    system("cls");
    return 0;
}

// ####################################################################################################################

// Function definitions


void initializeConsole()
{
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
}

void initializeMap()
{
    system("cls");
    for (int y = 0; y < HEIGHT; y++)
    {
        for (int x = 0; x < WIDTH; x++)
        {
            if (x == 0 || x == WIDTH - 1 || y == 0 || y == HEIGHT - 1)
            {
                map[y][x] = '#';
            }
            else
            {
                map[y][x] = ' ';
            }
        }
    }
}

void drawGame()
{
    goToXY(0, 0);
    for (int y = 0; y < HEIGHT; ++y)
    {
        // cout.write is faster than repeated << operators for raw char arrays.
        cout.write(map[y], WIDTH);
        if (y < HEIGHT - 1)
        {
            cout << '\n'; // Print a newline after each row, except the last.
        }
    }
    cout.flush();

    drawChar(player_x, player_y, player_c);
    goToXY(0, HEIGHT + 1);
    cout.flush();
}



void updateGame(int key)
{
    drawChar(player_x, player_y, map[player_y][player_x]);
    switch (key)
    {
    case 'w':
        player_y = max(1, player_y - 1);
        break;
    case 's':
        player_y = min(HEIGHT - 2, player_y + 1);
        break;
    case 'a':
        player_x = max(1, player_x - 1);
        break;
    case 'd':
        player_x = min(WIDTH - 2, player_x + 1);
        break;
    }
}


// ------------------------------- SCENES ------------------------------------------------------
void introductionCinematic() {
    system("cls");
    scene1.play();
    this_thread::sleep_for(chrono::seconds(1));
}