#include <ftxui/dom/elements.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/screen/color.hpp>
#include <iostream>
#include <fstream>

using namespace ftxui;

// Simple helper function to display time
char* calc_time() {
    // Calculate time
    time_t now = time(0);
    char* dt = ctime(&now);
    return dt;
}

// Helper function to decide if a file has contents
static std::string slurp_or_empty(const std::filesystem::path& p) {
    std::ifstream in(p, std::ios::binary);
    if (!in) return {};
    std::ostringstream ss;
    ss << in.rdbuf();
    return ss.str();
}

int main() {
    // Creates an interactive screen, compiler deducts typing
    auto screen = ScreenInteractive::Fullscreen();

    // Creates the renderer for interactive components
    auto renderer = Renderer([&] {

        // Heading
        auto heading = hbox(
            text(" phosphor ") | color(Color::RGB(0x6d, 0x3a, 0x5a)) | border
        );

        // Keep track of time
        auto time = hbox(
            text(calc_time()) | color(Color::RGB(0x6d, 0x3a, 0x5a)) | border
        );

        // Set the name of input file, line number, and controls
        std::string status;
        status += "  |  ";
        status += "  |  Ctrl+A:Save  Ctrl+R:Quit";
        auto status_line = hbox({ text(status) }) | color(Color::RGB(0x6d, 0x3a, 0x5a)) | border;

        // top hud
        auto hud = gridbox({
            {heading, time, status_line}
        });

        // placeholder for color options
        auto options = gridbox({
            {
                text("0x0d,0x0f,0x0a") | color(Color(0x0d,0x0f,0x0a)) | border,
                text("0xb8,0xff,0xb8") | color(Color(0xb8,0xff,0xb8)) | border,
                text("0x5c,0xff,0x5c") | color(Color(0x5c,0xff,0x5c)) | border,
                text("0x7e,0xf9,0xd8") | color(Color(0x7e,0xf9,0xd8)) | border,
                text("0xa0,0xff,0xa0") | color(Color(0xa0,0xff,0xa0)) | border,
                text("0x77,0xc7,0x77") | color(Color(0x77,0xc7,0x77)) | border,
                text("0xcc,0xff,0x66") | color(Color(0xcc,0xff,0x66)) | border,
                text("0x1a,0x2a,0x16") | color(Color(0x1a,0x2a,0x16)) | border
            }
        });

        // Return the rendered editor
        return vbox(
            hud | bgcolor(Color(0x0d,0x0f,0x0a)),
            options | bgcolor(Color(0x0d,0x0f,0x0a))
            //content | bgcolor(Color(0x0d,0x0f,0x0a))
        );
    });

    // Catches events for interactive changes to program
    auto catcher = CatchEvent(renderer, [&](Event event) {

        // Handle I/O editor actions
        if (event == Event::Backspace)  { std::cout << "Backspace" << std::endl;    return true; }
        if (event == Event::ArrowLeft)  { std::cout << "Arrow Left" << std::endl;   return true; }
        if (event == Event::ArrowRight) { std::cout << "Arrow Right" << std::endl;  return true; }
        if (event == Event::ArrowUp)    { std::cout << "Arrow Up" << std::endl;     return true; }
        if (event == Event::ArrowDown)  { std::cout << "Arrow Down" << std::endl;   return true; }
        if (event == Event::Home)       { std::cout << "Home" << std::endl;         return true; }
        if (event == Event::End)        { std::cout << "End" << std::endl;          return true; }
        if (event == Event::Return)     { std::cout << "Return" << std::endl;       return true; }

        // ctrl-r to quit editor
        if (event == Event::Special({18})) {
            screen.Exit();
            return true;
        }

        // Handle text input
        if (event.is_character()) {
            char32_t ch = event.character()[0];
            if (ch >= 0x20) {
                std::cout << ch << std::endl;
                return true;
            }
        }
        
        // ctrl-a to save 
        if (event == Event::Special({1})) {std::cout << "Saved file" << std::endl; return true; }
        return false;
    });

    // The main loop to print to screen after catching events
    screen.Loop(catcher);
    return 0;
}