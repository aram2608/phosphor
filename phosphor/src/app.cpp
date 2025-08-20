#include "app.hpp"

// Constructor to load in resources
App::App() {
    font = LoadFont("assets/fonts/Debrosee-ALPnL.ttf");
}

// Deconstructor to offload resources
App::~App() {
    UnloadFont(font);
}

void App::draw() {
    BeginDrawing();
        // Base UI - will probably change in the future, it burns my eyes a bit
        ClearBackground(Palette::DarkBg);
        DrawRectangleRoundedLinesEx({10, 10, 780, 780}, 0.18f, 20, 2, Palette::AquaMint);
        DrawLineEx({25, 60}, {775, 60}, 3, Palette::AquaMint);
        DrawTextEx(font, "phosphor", {40, 35}, 25.0f, 2, Palette::SoftMint);
    EndDrawing();
}
