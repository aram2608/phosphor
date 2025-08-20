#include <iostream>
#include <raylib.h>
#include "app.hpp"

int main() {
    // Harded screen params with offset
    int width = 750;
    int height = 750;
    int offset = 50;

    // Initialize window before loading GPU resources
    InitWindow(width + offset, height + offset, "phosphor");

    // App UI
    App app = App();

    // Load font
    while(!WindowShouldClose()) {
        BeginDrawing();
            // Base UI - will probably change in the future, it burns my eyes a bit
            ClearBackground(Palette::DarkBg);
            DrawRectangleRoundedLinesEx({10, 10, 780, 780}, 0.18f, 20, 2, Palette::AquaMint);
            DrawLineEx({25, 60}, {775, 60}, 3, Palette::AquaMint);
            DrawTextEx(app.font, "phosphor", {40, 35}, 25.0f, 2, Palette::SoftMint);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}