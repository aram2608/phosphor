#ifndef EDITOR_HPP
#define EDITOR_HPP

#include "gap_buffer/gap_buffer.hpp"
#include "keychords/keychords.hpp"
#include "scripting/scripting.hpp"
#include "ui/ui.hpp"

#include <array>
#include <cstdint>
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
 * Both of these aliases will work because we don't capture anything
 * and non capture lambdas become a function ptr
 */
using Command = std::function<void(Editor &)>;
// using Command = void(*)(Editor&);

// DO NOT TOUCH THE ORDER OF THIS - THE STATE MANAGER WILL BREAK
enum class EditingState { Editing, Renaming, Count };

class Editor {
    friend class ScriptingVM;

  public:
    // Constructor for the editor class
    Editor(std::string contents, std::filesystem::path file);
    // Main method to draw to window
    void draw();
    // Main logic to poll for keyboard events
    void poll_input();
    // Method for inserting text that we expose to the Lua API
    void insert_text(std::string text);
    // Method for picking a palette exposed to the Lua API
    void pick_palette(const int palette);
    // Method for toggling between palette options exposed to the Lua API
    void toggle_palette();
    void backspace();
    void enter();
    void tab();
    void paste();

  private:
    void name_file();
    void editing();
    void save();
    void bind();
    void move_left();
    void move_right();
    void move_up();
    void move_down();
    void move_to_mouse(Vector2 mouse_pos);
    ScriptingVM vm_;
    GapBuffer buffer_;
    std::unordered_map<KeyChord, Command, KeyChordHash> chordmap_;
    std::filesystem::path file_;
    std::string contents_;
    std::string new_name_{};
    EditingState state_;
    UI ui_;
};

#endif