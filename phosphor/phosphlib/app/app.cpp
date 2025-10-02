#include "app/app.hpp"
#include "app.hpp"
#include "palette/palette.hpp"

// Constructor for App class
App::App(std::string contents, std::filesystem::path file)
    : contents(contents), file(file), editor(contents, file) {
    // We load in the font from our assets file
    title_font = LoadFont(
        "JetBrainsMono-2.304/fonts/ttf/JetBrainsMono-ExtraBoldItalic.ttf");
    text_font =
        LoadFont("JetBrainsMono-2.304/fonts/ttf/JetBrainsMono-Medium.ttf");
}

// Deconstructor for App class
App::~App() {
    // We unload our font to prevent mem leaks
    UnloadFont(title_font);
}

// Funntion to draw our window and screen changes
void App::draw() {
    // Base UI - will probably change in the future, it burns my eyes a bit
    // We clear the background every loop and display the dark background
    ClearBackground(PhosphorGreen::DarkBg);
    DrawRectangleRoundedLinesEx({10, 10, 780, 780}, 0.18f, 20, 2,
                                PhosphorGreen::SoftMint);
    DrawLineEx(Vector2{25, 60}, Vector2{775, 60}, 3, PhosphorGreen::SoftMint);
    DrawTextEx(title_font, "phosphor", Vector2{40, 25}, 25.0f, 2,
               PhosphorGreen::ForestMoss);
    editor.draw();
}

// Function update window after keyboard events
void App::update() { editor.update(); }