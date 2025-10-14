#include "../phosphlib/editor/editor.hpp"
#include <cxxopts.hpp>
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

int main(int argc, const char **argv) {
    // We create a new options object
    cxxopts::Options options("phosphor", "The phosphor text editor");

    // We add the options we want to parse
    options.add_options()("h,help", "Help message")(
        "f,file", "Path to file for editing", cxxopts::value<std::string>());

    // We can now parse the arguments
    auto result = options.parse(argc, argv);

    // We create an empty instance of a file path object
    std::filesystem::path file{};

    // We print our help message and exit if help flag is passed
    if (result.count("help")) {
        std::cout << options.help() << std::endl;
        exit(0);
        // We extract the file name if passed in
    } else if (result.count("file")) {
        file = result["file"].as<std::string>();
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