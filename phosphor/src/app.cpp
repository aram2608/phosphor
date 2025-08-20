#include "app.hpp"

// Constructor to load in resources
App::App() {
    font = LoadFont("assets/fonts/Debrosee-ALPnL.ttf");
}

// Deconstructor to offload resources
App::~App() {
    UnloadFont(font);
}