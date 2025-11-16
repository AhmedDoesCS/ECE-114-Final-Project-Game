#include <iostream>          // Standard input/output stream (used primarily for std::cout).
#include <thread>            // Used for std::this_thread::sleep_for for controlling the frame rate.
#include <chrono>            // Used with <thread> to define the duration for sleeping (e.g., milliseconds).
#include <algorithm>         // Used for std::max and std::min to enforce boundary checks (keep player in bounds).

// --- PLATFORM-SPECIFIC HEADERS & IMPLEMENTATION SELECTION ---

// Check if the target system is Windows (32-bit or 64-bit).
#if defined(_WIN32) || defined(_WIN64)
    // Windows Headers: These are only compiled if the target is Windows.
    #include <windows.h>     // Core Windows API header; contains functions for console manipulation (SetConsoleCursorPosition, GetStdHandle).
    #include <conio.h>       // Contains _kbhit() and _getch() for non-blocking, non-echoing keyboard input.
#else
    // Unix/Linux/macOS Headers: These are only compiled if the target is NOT Windows.
    #include <termios.h>     // Contains termios structure and functions (tcgetattr, tcsetattr) for managing terminal I/O settings.
    #include <unistd.h>      // Contains STDIN_FILENO and other POSIX utilities.
    #include <stdio.h>       // Contains standard I/O functions like printf (for ANSI codes) and putc/getchar.
#endif


// --- CONSOLE ABSTRACTION CLASS (The Core Wrapper) ---

class Console {
private:
#if !(defined(_WIN32) || defined(_WIN64))
    // Unix-specific: This static member declaration is ONLY compiled for Unix.
    // It is CRITICAL for storing the terminal's initial, normal settings.
    // This allows us to restore the terminal after the game exits.
    static struct termios original_termios; 
#endif

public:
    // Universal interface declarations. The implementation changes per platform.
    static void Initialize();      // Sets up the console (e.g., non-blocking input, hides cursor).
    static void Shutdown();        // Cleans up and restores original console settings.
    static void GoToXY(int x, int y); // Moves the drawing cursor to a specific coordinate.
    static void DrawChar(int x, int y, char c); // Draws a single character at a specific coordinate.
    static int GetLiveInput();     // Gets a character immediately, without waiting for the 'Enter' key.
};


// --- CONSOLE IMPLEMENTATION (Windows) ---

// Start conditional compilation block for Windows.
#if defined(_WIN32) || defined(_WIN64)

void Console::Initialize() {
    // Hide the console cursor for a cleaner, non-flickering look.
    CONSOLE_CURSOR_INFO cursorInfo; // Windows structure to define cursor appearance.
    GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo); // Retrieve current settings.
    cursorInfo.bVisible = FALSE; // Set the visibility flag to FALSE.
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo); // Apply the new settings.
}

void Console::Shutdown() {
    // Restore the cursor visibility when the program exits. (Good console etiquette.)
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
    cursorInfo.bVisible = TRUE; // Set the visibility flag back to TRUE.
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
}

void Console::GoToXY(int x, int y) {
    COORD c = { (short)x, (short)y }; // COORD is a Windows structure for (x, y) console coordinates. Casts x/y to short.
    // GetStdHandle(STD_OUTPUT_HANDLE) gets the handle (reference) to the current console screen buffer.
    // SetConsoleCursorPosition moves the Windows cursor to the specified coordinates (c).
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c); 
}

void Console::DrawChar(int x, int y, char c) {
    GoToXY(x, y); // Move the cursor to the exact spot.
    putc(c, stdout); // Write the character 'c' to the console at the current cursor position.
    fflush(stdout); // Force the C++ output buffer to immediately display the character on the screen (reduces perceived latency).
}

int Console::GetLiveInput() {
    if (_kbhit()) { // _kbhit() checks the console input buffer. It returns non-zero if a key is pressed (non-blocking).
        return _getch(); // _getch() reads the key press immediately without waiting for 'Enter' and without echoing the character.
    }
    return 0; // If no key was pressed, return 0 (or any defined sentinel value) so the game loop can continue.
}


// --- CONSOLE IMPLEMENTATION (Unix/Linux/macOS) ---

// Start of the alternative platform implementation.
#else 

struct termios Console::original_termios; // Static member definition: Allocates memory for the original_termios struct.

void Console::Initialize() {
    struct termios t; // Local termios structure for temporary modification.
    tcgetattr(STDIN_FILENO, &original_termios); // Get the current terminal attributes (blocking, echoing) and save them to original_termios.
    t = original_termios; // Copy the original settings to 't' for modification.

    // c_lflag is the local flag field in termios.
    // ICANON (Canonical mode) is normally ON (requires Enter). We turn it OFF.
    // ECHO (Echo input) is normally ON (shows what you type). We turn it OFF.
    t.c_lflag &= ~(ICANON | ECHO); 
    
    // c_cc is the control characters array. Used here for non-blocking read.
    t.c_cc[VMIN] = 0;  // VMIN=0: Read returns immediately, even if no character is available.
    t.c_cc[VTIME] = 0; // VTIME=0: No timeout delay. Together with VMIN=0, this achieves non-blocking input.

    tcsetattr(STDIN_FILENO, TCSANOW, &t); // Apply the new modified settings immediately (TCSANOW).
    
    // ANSI escape codes: Standard sequence for terminal control.
    // \033[2J : Clears the entire screen.
    // \033[H : Moves the cursor to the home position (1, 1). This ensures the redraw starts from the top.
    printf("\033[2J\033[H"); 
}

void Console::Shutdown() {
    // CRITICAL: Restore the original terminal settings saved during Initialize(). 
    // This ensures the user's terminal is returned to normal blocking mode after the game quits.
    tcsetattr(STDIN_FILENO, TCSANOW, &original_termios);
}

void Console::GoToXY(int x, int y) {
    // ANSI escape code: \033[<ROW>;<COL>H.
    // Terminal rows/cols are 1-based, while our logic (x, y) is 0-based.
    // We add 1 to both x and y to translate to the 1-based ANSI coordinate system.
    printf("\033[%d;%dH", y + 1, x + 1);
}

void Console::DrawChar(int x, int y, char c) {
    GoToXY(x, y); // Move cursor to the target position.
    putc(c, stdout); // Write the character to the console.
    fflush(stdout); // Force the output buffer to be written to the terminal immediately.
}

int Console::GetLiveInput() {
    // Since the terminal is in non-canonical mode with VMIN=0, getchar() reads immediately.
    // If no key is pressed, it returns -1.
    return getchar();
}

#endif // End of Unix platform-specific code


// --- UNIVERSAL GAME LOGIC ---

using namespace std; // Use the standard namespace throughout the universal game logic.

const int WIDTH = 160;    // Define the game screen width in characters.
const int HEIGHT = 40;   // Define the game screen height in characters.
char player_char = '@';  // The ASCII character used to represent the player.

int player_x = WIDTH / 2; // Initial player X position (center).
int player_y = HEIGHT / 2; // Initial player Y position (center).
char map[HEIGHT][WIDTH]; // 2D array representing the game map/level data.


void InitializeMap() {
    // Populate the initial map array.
    for (int y = 0; y < HEIGHT; ++y) {
        for (int x = 0; x < WIDTH; ++x) {
            // Check for boundaries to draw walls.
            if (x == 0 || x == WIDTH - 1 || y == 0 || y == HEIGHT - 1) {
                map[y][x] = '#'; // Wall character.
            } else {
                map[y][x] = ' '; // Floor character.
            }
        }
    }
}

void UpdateGame(int key) {
    // CRITICAL STEP: Erase player's old position *before* moving.
    // We redraw the map tile that was underneath the player.
    Console::DrawChar(player_x, player_y, map[player_y][player_x]);

    // Handle Movement based on the keypress value.
    switch (key) {
        case 'w': case 72 : 
            // Move up (decrease y). std::max(1, ...) prevents moving past the top wall (y=0).
            player_y = max(1, player_y - 1); 
            break;
        case 's': case 80:  
            // Move down (increase y). std::min(HEIGHT - 2, ...) prevents moving past the bottom wall (y=HEIGHT-1).
            player_y = min(HEIGHT - 2, player_y + 1); 
            break;
        case 'a': case 75:  
            // Move left (decrease x). std::max(1, ...) prevents moving past the left wall (x=0).
            player_x = max(1, player_x - 1); 
            break;
        case 'd': case 77: 
            // Move right (increase x). std::min(WIDTH - 2, ...) prevents moving past the right wall (x=WIDTH-1).
            player_x = min(WIDTH - 2, player_x + 1); 
            break;
        case 'q': 
            Console::Shutdown(); // Call the platform-specific cleanup function.
            exit(0); // Terminate the program cleanly.
    }
}

void DrawGame() {
    // The core of flicker reduction: Move cursor to top-left (0, 0)
    // to overwrite the previous frame data instantly.
    Console::GoToXY(0, 0);

    // Redraw the entire map buffer (can be optimized for large maps, but fast for this size).
    for (int y = 0; y < HEIGHT; ++y) {
        // cout.write is faster than repeated << operators for raw char arrays.
        cout.write(map[y], WIDTH); 
        if (y < HEIGHT - 1) {
            cout << '\n'; // Print a newline after each row, except the last.
        }
    }
    cout.flush(); // Force the entire map buffer to be written to the screen.
    
    // Draw the player character at its new, updated position.
    Console::DrawChar(player_x, player_y, player_char);
    
    // Draw HUD (Head-Up Display).
    Console::GoToXY(0, HEIGHT + 1); // Move cursor to the line below the game map.
    // Print the HUD message, padding with spaces to clear any old text from the previous line.
    cout << "Press 'q' to exit. Use WASD to move. Current Pos: (" << player_x << ", " << player_y << ")    ";
    cout.flush(); // Flush to display HUD immediately.
}

int main() {
    Console::Initialize(); // Perform all necessary platform-specific console setup.
    InitializeMap();       // Set up the initial game world layout.

    // Game Loop: The heart of the program.
    while (true) {
        int key = Console::GetLiveInput(); // Check for a non-blocking keypress.
        
        // Key check: Unix returns -1/0 when no key is pressed. Windows returns 0.
        // We only call UpdateGame if a meaningful key was pressed.
        if (key != 0 && key != -1) { 
            UpdateGame(key);
        }

        DrawGame(); // Redraw the entire game screen (map, player, HUD).
        
        // Frame rate control: Pause the loop for 50 milliseconds (20 FPS).
        this_thread::sleep_for(chrono::milliseconds(5)); 
    }

    Console::Shutdown(); // Will only be reached if the loop breaks unexpectedly (not with 'q').
    return 0; // Standard program exit code.
}