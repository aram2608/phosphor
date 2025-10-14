#ifndef EDITOR_HPP
#define EDITOR_HPP

#include "gap_buffer/gap_buffer.hpp"
#include "ui/ui.hpp"

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

enum class EditingState { Editing, Saved, Renaming };

class Editor {
  public:
    Editor(std::string contents, std::filesystem::path file);
    ~Editor();
    void draw();
    void poll_input();

  private:
    void name_file();
    void editing();
    void save();
    void bind();
    void dispatch(int key);
    void handle_left_key();
    void handle_right_key();
    void handle_up_key();
    void handle_down_key();
    void handle_backspace();
    void handle_enter();
    void handle_ctrl();
    void handle_tab();
    // We use a hashmap for faster lookups than a map,
    // C++ has a built in hash function for primitives that is quite good
    // so we don't have to make our own
    GapBuffer buffer_;
    std::unordered_map<int, Action> keymap_;
    const std::vector<int> keys_;
    std::filesystem::path file_;
    Font text_font_;
    std::string contents_;
    std::string new_name_{};
    EditingState state_;
    UI ui_;
};

#endif