#ifndef EDITOR_HPP
#define EDITOR_HPP

#include "gap_buffer/gap_buffer.hpp"
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
 */
using Action = std::function<void(Editor &)>;
using Command = std::function<void(Editor &)>;

// DO NOT TOUCH THE ORDER OF THIS - THE STATE MANAGER WILL BREAK
enum class EditingState { Editing, Renaming, Count };

// We create a small enum that inherits from the uint_8
// This lets us create a bit mask for assinging modifer keys
enum Mod : std::uint8_t {
    MOD_NONE = 0,
    MOD_CTRL = 1 << 0,
    MOD_SHIFT = 1 << 1,
    MOD_ALT = 1 << 2,
    MOD_SUPER = 1 << 3,
};
// We need to overload the bitwise | or operator so we can
// Perform bitwise ops using our custom class
inline Mod operator|(Mod a, Mod b) {
    return Mod(std::uint8_t(a) | std::uint8_t(b));
}

// We create a small POD struct to store the key type and whether
// it is modified or not
struct KeyChord {
    int key;
    Mod mods;
    // We need to overload the equality operator so we can test for the
    // presence of a given key in our keymap
    bool operator==(const KeyChord &o) const {
        return key == o.key && mods == o.mods;
    }
};

// Since we are stuffing everything into a hash map, we need to create a hashing
// functor for the KeyChords struct
struct KeyChordHash {
    std::size_t operator()(const KeyChord &c) const {
        // The internal hashing funtions used for ints are quite good so we can
        // simply use them on the undrelying enums
        // The ^ XOR bitwise operator is standard for combining two hash values
        // and we bit shift once to the left since it reduces hash collisions
        // apparently
        return std::hash<int>{}(c.key) ^ (std::hash<int>{}(int(c.mods)) << 1);
    }
};

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
    void move_left();
    void move_right();
    void handle_up_key();
    void handle_down_key();
    void backspace();
    void enter();
    void tab();
    ScriptingVM *vm_ = nullptr;
    GapBuffer buffer_;
    std::unordered_map<KeyChord, Command, KeyChordHash> chordmap_;
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