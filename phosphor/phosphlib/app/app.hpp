#ifndef APP_HPP
#define APP_HPP
#include <raylib.h>
#include <string>

// App class to contain resources used during execution of program
class App {
  public:
    // Constructor
    App(std::string contents);
    // Deconstructor
    ~App();
    // Main Logic to draw to scren
    void draw();
    // Main logic to update keyboard events
    void update();

    // Publically available resources
    Font title_font;
    Font text_font;
    std::string contents;

  private:
};

#endif