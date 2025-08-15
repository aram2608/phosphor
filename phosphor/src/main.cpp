#include <ftxui/dom/elements.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/event.hpp>
#include "ftxui/screen/color.hpp"
#include <iostream>
#include <filesystem>

int main() {
    using namespace ftxui;

    // Creates the text boxes
    Element document = hbox(
        text("Hello world!") | color(Color::RGB(0x0d,0x0f,0x0a)) | border,
        text("Hello world!") | color(Color::RGB(0xb8,0xff,0xb8)) | border,
        text("Hello world!") | color(Color::RGB(0x5c,0xff,0x5c)) | border,
        text("Hello world!") | color(Color::RGB(0x7e,0xf9,0xd8)) | border,
        text("Hello world!") | color(Color::RGB(0xa0,0xff,0xa0)) | border,
        text("Hello world!") | color(Color::RGB(0x77,0xc7,0x77)) | border,
        text("Hello world!") | color(Color::RGB(0xcc,0xff,0x66)) | border,
        text("Hello world!") | color(Color::RGB(0x1a,0x2a,0x16)) | border
    );

    // Creates an interactive screen, compiler deducts typing
    auto screen = ScreenInteractive::Fullscreen();

    // Creates the renderer for interactive components
    auto renderer = Renderer([&] {
    return document;
    });

    // Catches events for interactive changes to program
    auto catcher = CatchEvent(renderer, [&](Event e) {
        if (e == Event::ArrowUp) {
            screen.Exit();
            return true;
        }
        return false;
    });

    // The main loop to print to screen after catching events
    screen.Loop(catcher);
    return 0;
}