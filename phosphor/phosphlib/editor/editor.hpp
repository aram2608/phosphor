#ifndef EDITOR_HPP
#define EDITOR_HPP

#include "editor/input.hpp"

#include <filesystem>
#include <fstream>
#include <raylib.h>
#include <string>

#include <iostream>

class Editor {
  public:
    Editor(std::string contents, std::filesystem::path file);
    ~Editor();
    void draw();
    bool save();
    void update();
    void keymap();

    std::filesystem::path file;
    Font text_font;
    std::string contents;
};

#endif