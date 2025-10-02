#ifndef APP_HPP
#define APP_HPP

#include "editor/editor.hpp"
#include <filesystem>
#include <raylib.h>
#include <string>

// App class to contain resources used during execution of program
class App {
  public:
    // Constructor
    App(std::string contents, std::filesystem::path file);
    // Deconstructor
    ~App();
    // Main Logic to draw to scren
    void draw();
    // Main logic to update keyboard events
    void update();

    // Publically available resources
    Editor editor;
    Font title_font;
    Font text_font;
    std::string contents;
    std::filesystem::path file;

  private:
};

#endif