#include "../include/editor.hpp"
#include "../vendor/cxxopts.hpp"
#include "../vendor/raylib.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <ostream>

// Helper function to decide if a file has contents
static inline std::string slurp_file(const std::filesystem::path &path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "File could not be opened." << std::endl;
        return "";
    }
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    file.close();
    return content;
}

int main(int argc, const char **argv) {
    // We create a new options object
    cxxopts::Options options("phosphor", "The phosphor text editor");

    // We add the options we want to parse
    options.add_options()("h,help", "Help message")(
        "f,file", "Path to file for editing", cxxopts::value<std::string>());

    // We need to catch any strange inputs
    options.allow_unrecognised_options();

    // We create an empty instance of a file path object
    std::filesystem::path file{};

    try {
        // We can now parse the arguments
        auto result = options.parse(argc, argv);

        // We print our help message and exit if help flag is passed
        if (result.count("help")) {
            std::cout << options.help() << std::endl;
            exit(0);
            // We extract the file name if passed in
        } else if (result.count("file")) {
            file = result["file"].as<std::string>();
        }

        // We retrieve unmatched arguments
        std::vector<std::string> unmatched_args = result.unmatched();

        if (!unmatched_args.empty()) {
            std::cerr << "Warning: Unrecognized arguments detected:"
                      << std::endl;
            for (const auto &arg : unmatched_args) {
                std::cerr << "  " << arg << std::endl;
            }
        }
    } catch (const cxxopts::exceptions::exception &e) {
        std::cerr << "Error parsing options: " << e.what() << std::endl;
        return 1;
    }

    std::string initial = file.empty() ? std::string{} : slurp_file(file);

    SetTraceLogLevel(LOG_ERROR);
    const int WIDTH = 1200;
    const int HEIGHT = 800;
    InitWindow(WIDTH, HEIGHT, "phosphor");

    SetTargetFPS(120);

    Editor editor{initial, file};

    while (!WindowShouldClose()) {
        editor.poll_input();
        BeginDrawing();
        editor.draw();
        EndDrawing();
    }

    CloseWindow();
    return 0;
}