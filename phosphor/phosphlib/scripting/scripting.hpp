#ifndef SCRIPTING_HPP
#define SCRIPTING_HPP

#include <cstdio>
#include <filesystem>
#include <iostream>
#include <raylib.h>
#include <string>
#include <unordered_map>

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
    ~ScriptingVM();

    // Method to load Lua file
    void load_init(const std::filesystem::path &path);

    // Methods to register Lua commands and bind Raylib keys
    void register_command(const std::string &name, int lua_fn_ref);
    // Method to bind keys
    void bind_key(int keycode, const std::string &cmd);

    // Method to loop up key in the key map
    bool has_key(int keycode) const;

    // Method to get the key to the stored commands
    const std::string *key_to_command(int keycode) const;

    // Method to run Lua command
    void run_command(const std::string &name);

    // Getter method to return the editor pointer
    Editor *owner() const { return owner_; }

  private:
    // Pointer to the Editor since it owns this class
    Editor *owner_;
    // Pointer to Lua VM instance
    lua_State *L_ = nullptr;
    // Map with commands
    std::unordered_map<std::string, int> lua_cmds_;
    // Map with keys and attached functions
    std::unordered_map<int, std::string> lua_keymap_;
};

#endif