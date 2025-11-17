// The Saga of Rocky: The Golden Floret
// ECE 114 - Fall 2025
// Ahmed Ahmed
// Universal Libraries
#include "consoleGameEngine.h"
#include "mechanics.h"
#include "menues.h"
#include "scenes.h"
#include "maps.h"
using namespace std;

// Constant Definitions
int player_x = WIDTH / 2;
int player_y = HEIGHT / 2;
char map[HEIGHT][WIDTH];
char player_c = 'V';

// Function declarations
// Mechanics
void initializeConsole();
void initializeMap();
void initializeGardenMap();
void updateGame(int key);
void drawGame();
bool isObstacle(char tile);

// -------------------------------- SCENES ------------------------------------------------------
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
        // for (int x = 0; x < WIDTH; x++)
        // {
        //     if (x == 0 || x == WIDTH - 1 || y == 0 || y == HEIGHT - 1)
        //     {
        //         map[y][x] = '#';
        //     }
        //     else
        //     {
        //         map[y][x] = ' ';
        //     }
        // }

        for (int x = 0; x < WIDTH; ++x)
        {
            map[y][x] = GARDEN_MAP_DATA[y][x];
        }
    }
}

bool isObstacle(char tile)
{
    if (tile == '#')
    {
        return true;
    }

    if (tile == '|'     // Pond wall/Trunk
        || tile == '-'  // Pond wall
        || tile == '~'  // Water
        || tile == '/'  // Water edge/Pond curve
        || tile == '\\' // Water edge/Pond curve
        || tile == '{'  // Pond side
        || tile == '}'  // Pond side
        || tile == '('  // Tree part
        || tile == ')'  // Tree part
        || tile == '_') // Pond/structure top
    {
        return true;
    }

    return false; // Return false for all other traversable characters
}

void drawGame()
{
    goToXY(0, 0);
    for (int y = 0; y < HEIGHT; y++)
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
    int next_x = player_x;
    int next_y = player_y;

    switch (key)
    {
    case 'w':
        next_y = player_y - 1;
        break;
    case 's':
        next_y = player_y + 1;
        break;
    case 'a':
        next_x = player_x - 1;
        break;
    case 'd':
        next_x = player_x + 1;
        break;
    }

    if (next_x >= 0 && next_x < WIDTH && next_y >= 0 && next_y < HEIGHT)
    {
        char nextTile = map[next_y][next_x];
        if (!isObstacle(nextTile))
        {
            drawChar(player_x, player_y, map[player_y][player_x]);
            player_x = next_x;
            player_y = next_y;
        }
    }
}

// ------------------------------- SCENES ------------------------------------------------------
void introductionCinematic()
{
    system("cls");
    scene1.play();
    this_thread::sleep_for(chrono::seconds(1));
}