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

int enemy_move_throttle = 5; // Enemy moves only once every 5 game loops (frames)
int enemy_frame_counter = 0; // Counts frames until next enemy move

// Function declarations
// Mechanics
void initializeMap();
void initializeGardenMap();
void updateGame(int key);
void drawGame();
void UpdateNPCs();

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
        InitializeNPCs();

        // Game loop that runs as long as the character continues to play
        while (true)
        {

            int userInput = getLiveInput();

            if (userInput != 0 && userInput != -1)
            {
                updateGame(userInput);
            }
            if (enemy_frame_counter >= enemy_move_throttle)
            { // <--- CHECK IF COUNTER MET THROTTLE
                UpdateNPCs();
                enemy_frame_counter = 0; // Reset the counter after movement
            }
            else
            {
                enemy_frame_counter++; // Increment the counter every loop iteration
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



void initializeMap()
{
    system("cls");
    for (int y = 0; y < HEIGHT; y++)
    {

        for (int x = 0; x < WIDTH; ++x)
        {
            map[y][x] = GARDEN_MAP_DATA[y][x];
        }
    }
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

    for (int i = 0; i < MAX_ENEMIES; ++i)
    {
        if (enemies[i].is_alive)
        {
            drawChar(enemies[i].x, enemies[i].y, enemies[i].character);
        }
    }

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

void UpdateNPCs()
{
    for (int i = 0; i < MAX_ENEMIES; ++i)
    {
        if (!enemies[i].is_alive)
            continue;

        int next_x = enemies[i].x;
        int next_y = enemies[i].y;

        // Simple A.I.: Move one step closer to the player on the x-axis or y-axis.
        if (abs(player_x - enemies[i].x) > abs(player_y - enemies[i].y))
        {
            // Move horizontally
            next_x += (player_x > enemies[i].x) ? 1 : -1;
        }
        else
        {
            // Move vertically
            next_y += (player_y > enemies[i].y) ? 1 : -1;
        }

        // 1. Check for Map Boundaries/Obstacles (using your existing IsObstacle)
        if (next_x >= 0 && next_x < WIDTH && next_y >= 0 && next_y < HEIGHT && !isObstacle(map[next_y][next_x]))
        {

            // Clear the old enemy position on the screen/map array
            drawChar(enemies[i].x, enemies[i].y, map[enemies[i].y][enemies[i].x]);

            // Move the enemy
            enemies[i].x = next_x;
            enemies[i].y = next_y;
        }

        // 2. Check for Threat (Collision with Player)
        if (enemies[i].x == player_x && enemies[i].y == player_y)
        {
            // GAME OVER logic goes here!
            goToXY(WIDTH / 2 - 5, HEIGHT / 2);
            cout << "!!! GAME OVER !!!";
            // Implement exit or reset
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