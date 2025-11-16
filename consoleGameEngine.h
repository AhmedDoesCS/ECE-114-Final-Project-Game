#pragma once
#include <iostream>
#include <thread>
#include <chrono>
#include <algorithm>
#include "maps.h"

#define WIDTH 160
#define HEIGHT 40

// Platform-specific libraries
#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#include <conio.h>
#endif

void goToXY(int x, int y)
{
    COORD c = {
        (short)x, (short)y};

    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
}

void drawChar(int x, int y, char c)
{
    goToXY(x, y);
    putc(c, stdout);
    fflush(stdout);
}

int getLiveInput()
{
    if (_kbhit())
    {
        return _getch();
    }
    return 0;
}
