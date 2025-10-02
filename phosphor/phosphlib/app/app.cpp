#include "app/app.hpp"
#include "app.hpp"
#include "palette/palette.hpp"

// Constructor for App class
App::App() {
    // We load in the font from our assets file
    font = LoadFont("assets/fonts/Debrosee-ALPnL.ttf");
}

// Deconstructor for App class
App::~App() {
    // We unload our font to prevent mem leaks
    UnloadFont(font);
}

// Funntion to draw our window and screen changes
void App::draw() {
    // Base UI - will probably change in the future, it burns my eyes a bit
    // We clear the background every loop and display the dark background
    ClearBackground(Palette::DarkBg);
    DrawRectangleRoundedLinesEx({10, 10, 780, 780}, 0.18f, 20, 2,
                                Palette::AquaMint);
    DrawLineEx({25, 60}, {775, 60}, 3, Palette::AquaMint);
    DrawTextEx(font, "phosphor", {40, 35}, 25.0f, 2, Palette::SoftMint);
}

void App::update() {}