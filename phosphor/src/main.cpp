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
        // Draw contents to screen
        app.draw();
    }

    // Close window following execution
    CloseWindow();
    return 0;
}