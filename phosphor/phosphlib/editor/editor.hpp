#ifndef EDITOR_HPP
#define EDITOR_HPP

#include "gap_buffer/gap_buffer.hpp"
#include "scripting/scripting.hpp"
#include "ui/ui.hpp"

#include <array>
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

// DO NOT TOUCH THE ORDER OF THIS - THE STATE MANAGER WILL BREAK
enum class EditingState { Editing, Renaming, Count };

class Editor {
  public:
    // Constructor for the editor class
    Editor(std::string contents, std::filesystem::path file);
    // Destructor for the editor class
    ~Editor();
    // Main method to draw to window
    void draw();
    // Main logic to poll for keyboard events
    void poll_input();
    void insert_text(const std::string &text);

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
    ScriptingVM *vm_ = nullptr;
    GapBuffer buffer_;
    std::unordered_map<int, Action> keymap_;
    std::vector<int> keys_;
    std::filesystem::path file_;
    Font text_font_;
    std::string contents_;
    std::string new_name_{};
    EditingState state_;
    UI ui_;
};

#endif