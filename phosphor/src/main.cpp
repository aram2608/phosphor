#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>
#include "ftxui/screen/color.hpp"
#include <iostream>

int main() {
    using namespace ftxui;

    // Creates our text box
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

    /*
    Creates the screen:
        Width parameter to fit full terminal size
        Height parameter fits text
    */
    Screen screen = Screen::Create(
    Dimension::Full(),
    Dimension::Fit(document)
    );

    // Render and print to screen
    Render(screen, document);
    screen.Print();

    // Create new line and flush buffer
    std::cout << '\n' << std::endl;
    return 0;
}