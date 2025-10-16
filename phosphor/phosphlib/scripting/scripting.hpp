#ifndef SCRIPTING_HPP
#define SCRIPTING_HPP

#include "keychords/keychords.hpp"
#include "palette/palette.hpp"

#include <cstdio>
#include <filesystem>
#include <iostream>
#include <raylib.h>
#include <string>
#include <unordered_map>
#include <utility>

#include <sol/sol.hpp>
extern "C" {
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
}

// Forward declaration
class Editor;

class ScriptingVM {
    friend class Editor;

  public:
    explicit ScriptingVM(Editor *owner);

    // Method to load Lua file
    void load_init(const std::filesystem::path &path);

    void push_api(sol::state &L);

  private:
    // Pointer to the Editor since it owns this class
    Editor *owner_;
    sol::state lua_;
    std::unordered_map<int, int> keys_;
    std::unordered_map<KeyChord, sol::function, KeyChordHash> chordmap_;
};

#endif