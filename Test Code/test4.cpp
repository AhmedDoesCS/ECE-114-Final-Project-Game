#include <iostream>
#include <thread>
#include <chrono>
#include <algorithm> 

// --- PLATFORM-SPECIFIC HEADERS & IMPLEMENTATION SELECTION ---

#if defined(_WIN32) || defined(_WIN64)
    // Windows Headers
    #include <windows.h>
    #include <conio.h>
#else
    // Unix/Linux/macOS Headers
    #include <termios.h>
    #include <unistd.h>
    #include <stdio.h>
#endif


// --- CONSOLE ABSTRACTION CLASS DEFINITION ---

class Console {
private:
#if !(defined(_WIN32) || defined(_WIN64))
    static struct termios original_termios; 
#endif

public:
    static void Initialize();
    static void Shutdown();
    static void GoToXY(int x, int y);
    static void DrawChar(int x, int y, char c);
    static int GetLiveInput();
};


// --- CONSOLE IMPLEMENTATION (Windows) ---

#if defined(_WIN32) || defined(_WIN64)

void Console::Initialize() {
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
    cursorInfo.bVisible = FALSE; 
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
}

void Console::Shutdown() {
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
    cursorInfo.bVisible = TRUE; 
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
}

void Console::GoToXY(int x, int y) {
    COORD c = { (short)x, (short)y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
}

void Console::DrawChar(int x, int y, char c) {
    GoToXY(x, y);
    putc(c, stdout);
    fflush(stdout); 
}

int Console::GetLiveInput() {
    if (_kbhit()) { 
        return _getch(); 
    }
    return 0; 
}


// --- CONSOLE IMPLEMENTATION (Unix/Linux/macOS) ---

#else 

struct termios Console::original_termios;

void Console::Initialize() {
    struct termios t;
    tcgetattr(STDIN_FILENO, &original_termios);
    t = original_termios;

    t.c_lflag &= ~(ICANON | ECHO); 
    t.c_cc[VMIN] = 0; 
    t.c_cc[VTIME] = 0; 

    tcsetattr(STDIN_FILENO, TCSANOW, &t);
    
    printf("\033[2J\033[H"); 
}

void Console::Shutdown() {
    tcsetattr(STDIN_FILENO, TCSANOW, &original_termios); 
}

void Console::GoToXY(int x, int y) {
    printf("\033[%d;%dH", y + 1, x + 1);
}

void Console::DrawChar(int x, int y, char c) {
    GoToXY(x, y);
    putc(c, stdout);
    fflush(stdout); 
}

int Console::GetLiveInput() {
    return getchar();
}

#endif // End of platform-specific Console implementations


// --- UNIVERSAL GAME LOGIC ---

using namespace std;

// 1. DIMENSIONS: 60x40 (W x H)
const int WIDTH = 60;
const int HEIGHT = 40;

char player_char = '@';
int player_x = 5; 
int player_y = 5;
char map[HEIGHT][WIDTH];


// 2. URBAN MAP DATA (60x40) using only PLAIN ASCII
// Key: ' ' (Road/Path), '.' (Pavement), '+' (Building Corner), '|'/'--' (Building Wall), 'X' (Car), 'T' (Lamppost), '#' (Boundary)
const char* URBAN_MAP_DATA[] = {
"############################################################",
"#..........................................................#",
"#+++---------------------------+++-------------------------+#",
"#|                               |                       |.#",
"#|           . . . . . . . . . . . . . .             |.#",
"#|           . +-------------+ .             |.#",
"#|           . |      T      | .             |.#",
"#|           . |             | .             |.#",
"#|           . |      X      | .             |.#",
"#|           . |             | .             |.#",
"#|           . +-------------+ .             |.#",
"#|           . . . . . . . . . . . . . .             |.#",
"#+++---------------------------+++-------------------------+#",
"#                                                          #",
"#..........................................................#",
"############################################################",
"#..........................................................#",
"#                                                          #",
"#+++---------------------------+++-------------------------+#",
"#|                               |                       |.#",
"#|             . . . . . . . . . . . . .             |.#",
"#|             . +-------------+ .             |.#",
"#|             . |             | .             |.#",
"#|             . |             | .             |.#",
"#|             . |      T      | .             |.#",
"#|             . |             | .             |.#",
"#|             . |             | .             |.#",
"#|             . +-------------+ .             |.#",
"#|             . . . . . . . . . . . . .             |.#",
"#+++---------------------------+++-------------------------+#",
"#..........................................................#",
"############################################################",
"#..........................................................#",
"#                                                          #",
"#+++---------------------------+++-------------------------+#",
"#|                               |                       |.#",
"#|                   X           |                       |.#",
"#|                               |                       |.#",
"#|                               |                       |.#",
"#|                               |                       |.#",
"#|                               |                       |.#",
"#+++---------------------------+++-------------------------+#",
"#..........................................................#",
"############################################################"
};


// 3. InitializeMap implementation (using URBAN_MAP_DATA)
void InitializeMap() {
    for (int y = 0; y < HEIGHT; ++y) {
        for (int x = 0; x < WIDTH; ++x) {
            map[y][x] = URBAN_MAP_DATA[y][x];
        }
    }
    // Set player start position on traversable pavement
    player_x = 5;
    player_y = 5;
}

// 4. IsObstacle helper function (Updated for PLAIN ASCII map)
bool IsObstacle(char tile) {
    // Obstacle Characters: '#' (Boundary), '+', '|', '-' (Building/Structure), 'X' (Car), 'T' (Lamppost)
    return (tile == '#')  
        || (tile == '+')  
        || (tile == '|')
        || (tile == '-')
        || (tile == 'X')
        || (tile == 'T');
    // Space ' ' and '.' (Pavement) are traversable.
}


// Function to handle player input and movement
void UpdateGame(int key) {
    int next_x = player_x;
    int next_y = player_y;

    // 1. Calculate the intended next position
    switch (key) {
        case 'w': next_y = player_y - 1; break;
        case 's': next_y = player_y + 1; break;
        case 'a': next_x = player_x - 1; break;
        case 'd': next_x = player_x + 1; break;
        case 'q': Console::Shutdown(); exit(0); 
        default: return;
    }
    
    // 2. Check Map Boundaries
    if (next_x >= 0 && next_x < WIDTH && next_y >= 0 && next_y < HEIGHT) {
        
        // 3. Check for Obstacle Collision
        char next_tile = map[next_y][next_x];
        
        if (!IsObstacle(next_tile)) {
            // Erase old position
            Console::DrawChar(player_x, player_y, map[player_y][player_x]);
            
            // Move to new position
            player_x = next_x;
            player_y = next_y;
        }
    }
}

// Function to redraw the entire game state
void DrawGame() {
    Console::GoToXY(0, 0);

    // Draw the entire map quickly
    for (int y = 0; y < HEIGHT; ++y) {
        cout.write(map[y], WIDTH); 
        if (y < HEIGHT - 1) {
            cout << '\n';
        }
    }
    cout.flush(); 
    
    // Draw the player on top
    Console::DrawChar(player_x, player_y, player_char);
    
    // Draw the HUD
    Console::GoToXY(0, HEIGHT + 1);
    cout << "Press 'q' to exit. Use WASD to move. Theme: URBAN (60x40 - ASCII). Pos: (" << player_x << ", " << player_y << ")    ";
    cout.flush();
}

int main() {
    // NOTE: Ensure your terminal can display 60 columns and at least 42 rows.
    
    Console::Initialize(); 
    InitializeMap();

    while (true) {
        int key = Console::GetLiveInput();
        
        if (key != 0 && key != -1) { 
            UpdateGame(key);
        }

        DrawGame();
        
        // Control game speed
        this_thread::sleep_for(chrono::milliseconds(50)); 
    }

    Console::Shutdown(); 
    return 0;
}