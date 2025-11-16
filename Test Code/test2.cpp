#include <iostream>     // Includes the standard input/output stream library, needed for std::cout and std::cin
#include <vector>       // Includes the vector container library, used to store sequences of strings representing frames and titles
#include <thread>       // Includes threading support, needed for sleep functionality to create animation timing
#include <chrono>       // Includes chrono library, used for specifying durations for sleeps in milliseconds

// Function: clearScreen
// Purpose: Clears the terminal screen so that the next frame of the animation can be drawn in the same place.
// This creates the illusion of movement in a terminal (since terminals do not automatically refresh).
void clearScreen() {
#ifdef _WIN32                 // Checks if the program is being compiled on a Windows system
    system("cls");           // Calls the system command "cls" to clear the terminal screen on Windows
#else                         // Otherwise (assume Unix/Linux/macOS)
    system("clear");         // Calls the system command "clear" to clear the terminal screen on Unix/Linux/macOS
#endif
}

int main() {                  // Main function: entry point for the program
    // -------------------------
    // Define Rocky's hover/flap frames
    // -------------------------
    // Each frame is a vector of strings, each string representing a line in the ASCII art.
    // There are 3 frames to simulate wings flapping and Rocky slightly hovering up and down.
    std::vector<std::vector<std::string>> frames = {
        { "       *    __     __   *        ",   // Line 1: top sparkles and wings
          "      * <(o^o)>(o^o)>  *       ",   // Line 2: Rocky's eyes and wing tips with sparkles
          "       \\*| | | | |/*            ",   // Line 3: body stripes and wing sparkles
          "        \\  *   *  /             ",   // Line 4: body middle with floating sparkles
          "         '   *   '              " }, // Line 5: tail/stinger with sparkles

        { "       *                   *     ",   // Line 1: blank top space, wings mid-flap with outer sparkles
          "        __     __       *        ",   // Line 2: Rocky mid-body, wing tips start to move
          "      <(o^o)>(o^o)>    *  *     ",   // Line 3: Rocky's face and eyes, slight sparkles around wings
          "        \\*| | | |/*             ",   // Line 4: body stripes with sparkles
          "         \\  *   *  /            ",   // Line 5: middle body sparkles
          "          '   *   '              " }, // Line 6: tail/stinger sparkles

        { "       *    __     __   *        ",   // Line 1: back to high hover, wings down
          "      * <(o^o)>(o^o)>  *       ",   // Line 2: Rocky's eyes with sparkles
          "       / |*| | | |*| \\           ",   // Line 3: wings folded with sparkles along edges
          "        \\  *   *  /             ",   // Line 4: body mid-section with sparkles
          "         '   *   '              " }  // Line 5: tail/stinger with sparkles
    };

    // -------------------------
    // Define Rocky's victory landing sequence frames
    // -------------------------
    // These frames show Rocky landing on the Golden Floret: wings folded slightly, Floret glows brighter.
    std::vector<std::vector<std::string>> victoryFrames = {
        { "       *    __     __   *        ",
          "      * <(o^o)>(o^o)>  *       ",
          "       / |   | |   | \\          ",
          "        \\  *   *  /             ",
          "         '   *   '              " },

        { "       *      __     __  *       ",
          "      *   <(o^o)>(o^o)>  *      ",
          "        /   | | | | |   \\       ",
          "         \\   *   *   /          ",
          "          '   *   '             " },

        { "            __     __            ",
          "         <(o^o)>(o^o)>          ",
          "           | | | | |            ",
          "           *   *   *            ",
          "            '   *   '           " }
    };

    // -------------------------
    // Define the title banner
    // -------------------------
    // The ASCII title of the game is stored as a vector of strings, each line being a string.
    std::vector<std::string> title = {
        "  ____  _            ____           _        _____           _ _       ",
        " / ___|| | ___   ___|  _ \\ ___  ___| |_     |  ___|__   ___ | | | ___  ",
        " \\___ \\| |/ _ \\ / _ \\ |_) / _ \\/ __| __|    | |_ / _ \\ / _ \\| | |/ _ \\ ",
        "  ___) | | (_) |  __/  _ <  __/\\__ \\ |_     |  _| (_) | (_) | | |  __/ ",
        " |____/|_|\\___/ \\___|_| \\_\\___||___/\\__|    |_|  \\___/ \\___/|_|_|\\___|  ",
        "                                                                         ",
        "                ~ The Saga of Rocky: The Golden Floret ~               "
    };

    // -------------------------
    // Floret sparkles positions per frame
    // -------------------------
    // Each vector<int> represents horizontal positions of sparkles in the last line of the title for a given frame.
    std::vector<std::vector<int>> sparkles = {
        {23, 27, 35}, // frame 1 sparkles
        {24, 26, 34}, // frame 2 sparkles
        {23, 29, 32}  // frame 3 sparkles
    };

    // -------------------------
    // Dandelion seed positions per frame
    // -------------------------
    // Each vector<int> represents horizontal positions of floating seeds below the title. These simulate particles rising from the Floret.
    std::vector<std::vector<int>> seeds = {
        {24, 28, 30}, // frame 1
        {25, 27, 31}, // frame 2
        {23, 26, 30}  // frame 3
    };

    // -------------------------
    // Main hover/flap intro loop
    // -------------------------
    // Loop a fixed number of times to repeat the hovering/flapping intro before the victory sequence.
    for (int loop = 0; loop < 8; ++loop) { // 8 cycles of flapping
        for (size_t i = 0; i < frames.size(); ++i) {  // Iterate over the 3 hover/flap frames
            clearScreen();                              // Clear the terminal to render the new frame

            // Print Rocky's ASCII art line by line
            for (const auto &line : frames[i])
                std::cout << line << "\n";             // Each line is printed followed by a newline

            // Print the title banner line by line
            for (size_t t = 0; t < title.size(); ++t) {
                std::string line = title[t];            // Copy the title line to modify for sparkles
                if (t == title.size() - 1) {            // Last line of title is where Floret sparkles appear
                    for (int pos : sparkles[i])        // For each sparkle position in this frame
                        if (pos < line.size())         // Safety check to avoid out-of-bounds
                            line[pos] = '*';          // Replace the character with a sparkle '*'
                }
                std::cout << line << "\n";             // Print the modified line
            }

            // Print the floating seed line below the title
            std::string seedLine(40, ' ');              // Start with 40 spaces
            for (int pos : seeds[i])                     // Place seeds ('.') in the specified horizontal positions
                if (pos < seedLine.size()) 
                    seedLine[pos] = '.';
            std::cout << seedLine << "\n";             // Print the seed line

            std::this_thread::sleep_for(std::chrono::milliseconds(125)); // Wait 125ms to create visible animation frame timing
        }
    }

    // -------------------------
    // Victory landing sequence
    // -------------------------
    // Rocky lands on the Floret; wings fold; Floret glows brighter.
    for (size_t i = 0; i < victoryFrames.size(); ++i) {
        clearScreen();                                  // Clear terminal for the next frame

        // Print Rocky landing
        for (const auto &line : victoryFrames[i])
            std::cout << line << "\n";

        // Print title with sparkles for landing frames
        for (size_t t = 0; t < title.size(); ++t) {
            std::string line = title[t];               // Copy line for modification
            if (t == title.size() - 1) {               // Last line for Floret sparkles
                for (int pos : sparkles[i % sparkles.size()])
                    if (pos < line.size()) 
                        line[pos] = '*';
            }
            std::cout << line << "\n";
        }

        // Print floating seeds below the title
        std::string seedLine(40, ' ');
        for (int pos : seeds[i % seeds.size()])
            if (pos < seedLine.size()) 
                seedLine[pos] = '.';
        std::cout << seedLine << "\n";

        std::this_thread::sleep_for(std::chrono::milliseconds(125)); // Wait for animation timing
    }

    // -------------------------
    // Pause and wait for player input
    // -------------------------
    // This allows the player to press Enter to begin the game after watching the intro cinematic.
    std::cout << "\nPress Enter to begin the quest...";
    std::cin.get();                                     // Waits for Enter key press

    return 0;                                           // Exit main function and terminate the program
}
