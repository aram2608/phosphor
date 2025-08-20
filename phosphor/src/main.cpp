#include <iostream>
#include <raylib.h>
#include "app.hpp"

int main() {
    // Harded screen params with offset
    int width = 750;
    int height = 750;
    int offset = 50;

    // Config flags for window - dont want to worry about that logic yet for the UI
    // SetConfigFlags(FLAG_WINDOW_RESIZABLE);

    // Initialize window before loading GPU resources
    InitWindow(width + offset, height + offset, "phosphor");

    // Lock FPS to make the window less jumpy
    SetTargetFPS(60);

    // App UI
    App app = App();

    // Load font
    while(!WindowShouldClose()) {
        // Draw contents to screen
        app.draw();
    }

    // Close window following execution
    CloseWindow();
    return 0;
}