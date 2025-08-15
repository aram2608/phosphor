#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>
#include "ftxui/screen/color.hpp"
#include <iostream>

int main() {
    using namespace ftxui;

    // Creates our text box
    Element document = hbox(
        text("Hello world!") | border | color(Color::RGB(0x0d,0x0f,0x0a))
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