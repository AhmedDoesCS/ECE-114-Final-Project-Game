#pragma once
#include "consoleGameEngine.h"
// Add to your global definitions
struct NPC {
    int x;
    int y;
    char character = 'E'; // E for Enemy
    bool is_alive = true;
};

// Global NPC array (or vector, for C++ flexibility)
const int MAX_ENEMIES = 3;
NPC enemies[MAX_ENEMIES];

void InitializeNPCs() {
    // Example placement on traversable terrain (e.g., grass '.')
    enemies[0] = {10, 10, 'E', true};
    enemies[1] = {50, 20, 'E', true};
    enemies[2] = {120, 35, 'E', true};
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

    if (tile == 'B') {
        return true; 
    }

    return false; // Return false for all other traversable characters
}