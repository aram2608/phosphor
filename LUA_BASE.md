# Lua Base API Usage
---

I switched to sol2 but I am keeping this here for future reference

``` C++
#include "scripting/scripting.hpp"
#include "editor/editor.hpp"

// We define a single raw pointer so C can access our instance without a this *
static ScriptingVM *G = nullptr;

// For a standard lua command we write static int l_foo_bar(lua_State* L)
// The idenitifer is just to keep the style consistent and the parameter
// is the call stack we are interacting with
// The int return type refers to how many values are left on the Lua stack

// The model goes like this
// 1: Check the stack (ie: luaL_check* / lua_tonumber, etc) and use indices
// starting at one 2: Return values to Lua using lua_push* and return the count
// # when a value is left on the stack, it is usually popped later

static inline void push_keys_table(lua_State *L) {
    // We make a new table
    // we will store it as editor.keys
    lua_newtable(L);
    // I hate macros but in this case its useful for mass subsitutions for our
    // keys of interest
    // clang-format off
    #define PUT(KEYSYM) \
        lua_pushinteger(L, KEYSYM); \
        lua_setfield(L, -2, #KEYSYM)

        PUT(KEY_H);
        PUT(KEY_I);
        PUT(KEY_S);
        PUT(KEY_TAB);
        PUT(KEY_ENTER);
        PUT(KEY_LEFT_SHIFT);

    #undef PUT
    // clang-format on
}

static int l_register_command(lua_State *L) {
    // We use check string to get the first argument and return as a c-str
    const char *name = luaL_checkstring(L, 1);
    // We then ensure the arg 2 is a function
    luaL_checktype(L, 2, LUA_TFUNCTION);
    // We can then push the same reference back onto the stack so we can pop it
    lua_pushvalue(L, 2);
    // We now get a reference to the function and push it to the C registry so
    // we can access it later
    int ref = luaL_ref(L, LUA_REGISTRYINDEX);
    // We can now pass that refernce over to our hashmap with its identifier
    G->register_command(name, ref);
    return 0;
}

// Method to bind keys to commands
static int l_bind_key(lua_State *L) {
    // We get the integer keycode
    int keycode = (int)luaL_checkinteger(L, 1);
    // We get the string representation of the command name
    const char *cmd = luaL_checkstring(L, 2);
    // We then bind it to the keymap so we can call it later
    G->bind_key(keycode, cmd);
    return 0;
}

static int l_hello_world(lua_State *L) {
    std::cout << "Hello world!" << std::endl;
    return 0;
}

// Method to insert text to buffer
static int l_insert_text(lua_State *L) {
    // We forward declare a size_t for the size of the string
    std::size_t n;
    // We then use checklstring to return the string along with its length
    const char *s = luaL_checklstring(L, 1, &n);
    // We can then reconstruct the string using the length
    G->owner()->insert_text(std::string{s, n});
    return 0;
}

// Helper method to create a new editor table to expose
// the Editors functions
static void push_editor_api(lua_State *L) {
    // Places a new {} on the stack
    lua_newtable(L);

    // We push our functions and set the respective fields
    // -1 is the table we pushed and -2 is the table underneat

    lua_pushcfunction(L, l_register_command);
    lua_setfield(L, -2, "register_command");

    lua_pushcfunction(L, l_bind_key);
    lua_setfield(L, -2, "bind_key");

    lua_pushcfunction(L, l_hello_world);
    lua_setfield(L, -2, "hello_world");

    lua_pushcfunction(L, l_insert_text);
    lua_setfield(L, -2, "insert_text");

    // We expose the Raylib keys to Lua for easy of access and so we dont
    // have to memorize the key codes for each one
    push_keys_table(L);
    lua_setfield(L, -2, "keys");

    // We add the table globally
    lua_setglobal(L, "editor");
}

ScriptingVM::ScriptingVM(Editor *owner) : owner_(owner) {
    G = this;
    L_ = luaL_newstate();
    // We load in a lean library set for Lua
    luaL_requiref(L_, "_G", luaopen_base, 1);
    lua_pop(L_, 1);
    luaL_requiref(L_, LUA_STRLIBNAME, luaopen_string, 1);
    lua_pop(L_, 1);
    luaL_requiref(L_, LUA_TABLIBNAME, luaopen_table, 1);
    lua_pop(L_, 1);
    luaL_requiref(L_, LUA_MATHLIBNAME, luaopen_math, 1);
    lua_pop(L_, 1);

    push_editor_api(L_);
}

ScriptingVM::~ScriptingVM() {
    // We need to loop over the refernces in the cmds map and unrefernce them
    for (auto &[name, ref] : lua_cmds_) {
        luaL_unref(L_, LUA_REGISTRYINDEX, ref);
    }
    // We can the close the Lua VM
    lua_close(L_);
}

// Method to initialize Lua VM from an input file
void ScriptingVM::load_init(const std::filesystem::path &path) {
    // We try and load our file and return out if not successful
    if (!std::filesystem::exists(path)) {
        return;
    }
    // We load the file from the path after converting it to a c str
    // We also use a protected call to report an error so we do not crash
    // the editor during execution
    if (luaL_loadfile(L_, path.string().c_str()) || lua_pcall(L_, 0, 0, 0)) {
        std::fprintf(stderr, "[lua] %s error: %s\n", path.string().c_str(),
                     lua_tostring(L_, -1));
        // We pop the error
        lua_pop(L_, 1);
    }
}

void ScriptingVM::register_command(const std::string &name, int lua_fn_ref) {
    // We clean up in case the command already exists as a precaution
    if (auto it = lua_cmds_.find(name); it != lua_cmds_.end()) {
        // We unrefernce the function
        luaL_unref(L_, LUA_REGISTRYINDEX, it->second);
    }
    // We then assignm the name of the command to its refernce
    lua_cmds_[name] = lua_fn_ref;
}

// Helper method to bind keys for commands
void ScriptingVM::bind_key(int keycode, const std::string &cmd) {
    lua_keymap_[keycode] = cmd;
}

// Helper to make sure a key exists
bool ScriptingVM::has_key(int keycode) const {
    return lua_keymap_.find(keycode) != lua_keymap_.end();
}

const std::string *ScriptingVM::key_to_command(int keycode) const {
    auto it = lua_keymap_.find(keycode);
    return it == lua_keymap_.end() ? nullptr : &it->second;
}

// Function that acts as the main orchestrator for running saved Lua commands
void ScriptingVM::run_command(const std::string &name) {
    // We first fine the command name
    auto it = lua_cmds_.find(name);
    // If it does not exist we return out
    if (it == lua_cmds_.end()) {
        std::fprintf(stderr, "[lua] unknown command: %s\n", name.c_str());
        return;
    }
    // We get our function off the registry and push it onto the stack
    lua_rawgeti(L_, LUA_REGISTRYINDEX, it->second);
    // We can now invoke it and listen for errors
    // If we get one we can report it and pop the error off
    if (lua_pcall(L_, 0, 0, 0) != LUA_OK) {
        std::fprintf(stderr, "[lua] cmd '%s' err: %s\n", name.c_str(),
                     lua_tostring(L_, -1));
        lua_pop(L_, 1);
    }
}

```