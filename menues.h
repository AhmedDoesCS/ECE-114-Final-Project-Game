#pragma once
#include "consoleGameEngine.h"


bool titleScreen()
{
    system("cls");
    for (int y = 0; y < HEIGHT; y++)
    {
        for (int x = 0; x < WIDTH; x++)
        {
            if (x == 0 || x == WIDTH - 1 || y == 0 || y == HEIGHT - 1)
            {
                drawChar(x, y, '*');
            }
        }
    }

    int x = 35;
    int y = 10;

    goToXY(x, y);
    std::cout << R"(  ________            _____                            ____   ____             __        )" << std::endl;
    goToXY(x, ++y);
    std::cout << R"( /_  __/ /_  ___     / ___/____ _____ _____ _   ____  / __/  / __ \____  _____/ /____  __)" << std::endl;
    goToXY(x, ++y);
    std::cout << R"(  / / / __ \/ _ \    \__ \/ __ `/ __ `/ __ `/  / __ \/ /_   / /_/ / __ \/ ___/ //_/ / / /)" << std::endl;
    goToXY(x, ++y);
    std::cout << R"( / / / / / /  __/   ___/ / /_/ / /_/ / /_/ /  / /_/ / __/  / _, _/ /_/ / /__/ ,< / /_/ / )" << std::endl;
    goToXY(x, ++y);
    std::cout << R"(/_/ /_/ /_/\___/   /____/\__,_/\__, /\__,_/   \____/_/    /_/ |_|\____/\___/_/|_|\__, /  )" << std::endl;
    goToXY(x, ++y);
    std::cout << R"(                              /____/                                            /____/   )" << std::endl;
    goToXY(x, ++y);
    std::cout << R"( )" << std::endl;
    goToXY(x, ++y);
    std::cout << R"(   ___       ___     __   __        __   ___          ___       __   __   ___ ___ )" << std::endl;
    goToXY(x, ++y);
    std::cout << R"(    |  |__| |__     / _` /  \ |    |  \ |__  |\ |    |__  |    /  \ |__) |__   |  )" << std::endl;
    goToXY(x, ++y);
    std::cout << R"(    |  |  | |___    \__> \__/ |___ |__/ |___ | \|    |    |___ \__/ |  \ |___  |  )" << std::endl;
    goToXY(x, ++y);
    std::cout << R"()";
    goToXY(x, ++y);

    x = 63;
    y = 25;

    goToXY(x, y);
    std::cout << ">> Press spacebar to begin";
    goToXY(x, y + 1);
    std::cout << ">> Press q to quit";

    while (true)
    {
        int choice = getLiveInput();

        if (choice == 32)
        {
            return true;
        }
        else if (choice == 113)
        {
            return false;
        }
    }
}