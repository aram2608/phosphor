#include <iostream>
#include <raylib.h>
#include "palette.hpp"

int main() {
    // Harded screen params with offset
    int width = 750;
    int height = 750;
    int offset = 50;
    InitWindow(width + offset, height + offset, "phosphor");

    // Load font
    Font font = LoadFont("assets/fonts/Debrosee-ALPnL.ttf");

    while(!WindowShouldClose()) {
        BeginDrawing();
            // Base UI - will probably change in the future, it burns my eyes a bit
            ClearBackground(Palette::DarkBg);
            DrawRectangleRoundedLinesEx({10, 10, 780, 780}, 0.18f, 20, 2, Palette::AquaMint);
            DrawLineEx({25, 60}, {775, 60}, 3, Palette::AquaMint);
            DrawTextEx(font, "phosphor", {40, 35}, 25.0f, 2, Palette::SoftMint);
        EndDrawing();
    }

    // Unload resources to prevent mem leaks
    UnloadFont(font);
    CloseWindow();
    return 0;
}