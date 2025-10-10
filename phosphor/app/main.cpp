#include "../phosphlib/editor/editor.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <ostream>
#include <raylib.h>

// Helper function to decide if a file has contents
static std::string slurp_file(const std::filesystem::path &path) {
    // We create an ifstream object
    std::ifstream file(path);
    // If the file cannot be opened we return an empty string
    if (!file.is_open()) {
        std::cout << "File could not be opened." << std::endl;
        return "";
    }
    // Read entire file into string using iterators
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    // We can then close the file and return the contents
    file.close();
    return content;
}

int main(int argc, const char *argv[]) {
    // We create an instance of a file path object
    std::filesystem::path file{};

    // If the number of args is greater than one, we assume a file was given
    if (argc > 1) {
        file = argv[1];
    }

    // We try and read the file, if it is empty we simply return an empty string
    // otherwise we try and slurp its contents
    std::string initial = file.empty() ? std::string{} : slurp_file(file);

    // Harded screen params with offset
    const int WIDTH = 800;
    const int HEIGHT = 800;

    // Config flags for window - dont want to worry about that logic yet for the
    // UI SetConfigFlags(FLAG_WINDOW_RESIZABLE);

    // Initialize window before loading GPU resources
    InitWindow(WIDTH, HEIGHT, "phosphor");

    // Lock FPS to make the window less jumpy
    SetTargetFPS(120);

    // We create our editor instance
    Editor editor = Editor(initial, file);

    // Load font
    while (!WindowShouldClose()) {
        // We poll keyboard input
        editor.poll_input();
        // Draw contents to screen
        BeginDrawing();
        // We draw updated contents
        editor.draw();
        EndDrawing();
    }

    // Close window following execution
    CloseWindow();
    return 0;
}