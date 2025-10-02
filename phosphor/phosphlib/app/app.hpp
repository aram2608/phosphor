#ifndef APP_HPP
#define APP_HPP
#include <raylib.h>

// App class to contain resources used during execution of program
class App {
  public:
    // Constructor
    App();
    // Deconstructor
    ~App();
    // Main Logic to draw to scren
    void draw();
    // Main logic to update keyboard events
    void update();

    // Publically available resources
    Font font;

  private:
};

#endif