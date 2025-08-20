#include <iostream>
#include <raylib.h>
#include "palette.hpp"

int main() {
    // Harded screen params with offset
    int width = 750;
    int height = 750;
    int offset = 50;
    InitWindow(width + offset, height + offset, "phosphor");

    while(!WindowShouldClose()) {
        BeginDrawing();
            ClearBackground(Palette::DarkBg);
            DrawRectangleRoundedLinesEx({10, 10, 780, 780}, 0.18f, 20, 2, Palette::BrightGreen);
            DrawLineEx({25, 60}, {775, 60}, 3, Palette::BrightGreen);
            DrawTextEx();
        
        EndDrawing();
    }

    CloseWindow();
    return 0;
}