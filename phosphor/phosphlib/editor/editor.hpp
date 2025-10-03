#ifndef EDITOR_HPP
#define EDITOR_HPP

#include "input/input.hpp"

#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <raylib.h>
#include <string>
#include <unordered_map>
#include <vector>

// Forward declaration
class Editor;

/*
 * We create an alias for any function that takes a reference to our editor
 * and returns void
 * This will be useful when we create our method map
 */
using Action = std::function<void(Editor &)>;

class Editor {
  public:
    Editor(std::string contents, std::filesystem::path file);
    ~Editor();
    void draw();
    void poll_input();

  private:
    bool save();
    void bind();
    void dispatch(int key);
    void handle_left_key();
    void handle_right_key();
    void handle_up_key();
    void handle_down_key();
    void handle_backspace();
    void handle_enter();
    // We use a hashmap for faster lookups than a map,
    // C++ has a built in hash function for primitives that is quite good
    // so we don't have to make our own
    std::unordered_map<int, Action> keymap;
    const std::vector<int> keys;
    std::filesystem::path file;
    Font text_font;
    std::string contents;
};

#endif