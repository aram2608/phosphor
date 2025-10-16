#include "scripting/scripting.hpp"
#include "editor/editor.hpp"

// clang-format off
    #define PUT(KEYSYM) \
        #KEYSYM, KEYSYM
// clang-format on

// ScriptingVM constructor - we pass in a ptr to the editor instance
ScriptingVM::ScriptingVM(Editor *owner) : owner_(owner) {
    // We load a lead library set
    lua.open_libraries(sol::lib::base, sol::lib::math, sol::lib::table);
    // We can now push our API to the VM
    push_api(lua);
}

// Method to load files and run the undrelying Lua script
void ScriptingVM::load_init(const std::filesystem::path &path) {
    // We use a safe method to catch errors and not crash the editor
    auto result = lua.safe_script_file(path, &sol::script_pass_on_error);
    // We error check and report any issues
    if (!result.valid()) {
        sol::error err = result;
        std::cerr << "Lua error: " << err.what() << '\n';
    }
}

// Method containing the main logic to push our API
void ScriptingVM::push_api(sol::state &L) {
    // We store an enum type with the bit mask of our MOD keys
    // this will lets us create custom key binds
    L.new_enum("Mod", "NONE", Mod::MOD_NONE, "CTRL", Mod::MOD_CTRL, "SHIFT",
               Mod::MOD_SHIFT, "ALT", Mod::MOD_ALT, "SUPER", Mod::MOD_SUPER);

    // We create a table to store useful keys to override
    L["keys"] = L.create_table_with(
        // Mod keys
        PUT(KEY_LEFT_ALT), PUT(KEY_RIGHT_ALT), PUT(KEY_LEFT_CONTROL),
        PUT(KEY_LEFT_SUPER), PUT(KEY_RIGHT_SUPER), PUT(KEY_LEFT_SHIFT),
        PUT(KEY_RIGHT_SHIFT),
        // Movement keys
        PUT(KEY_UP), PUT(KEY_DOWN), PUT(KEY_LEFT), PUT(KEY_RIGHT),
        // Accessory keys
        PUT(KEY_ENTER), PUT(KEY_TAB), PUT(KEY_SPACE), PUT(KEY_DELETE),
        PUT(KEY_HOME), PUT(KEY_F1), PUT(KEY_F2), PUT(KEY_F3), PUT(KEY_F4),
        PUT(KEY_F5), PUT(KEY_F6), PUT(KEY_F7), PUT(KEY_F8), PUT(KEY_F9),
        PUT(KEY_F10), PUT(KEY_F11), PUT(KEY_F12),
        // Alphabet
        PUT(KEY_A), PUT(KEY_B), PUT(KEY_C), PUT(KEY_D), PUT(KEY_E), PUT(KEY_F),
        PUT(KEY_G), PUT(KEY_H), PUT(KEY_I), PUT(KEY_J), PUT(KEY_K), PUT(KEY_L),
        PUT(KEY_M), PUT(KEY_N), PUT(KEY_O), PUT(KEY_P), PUT(KEY_Q), PUT(KEY_R),
        PUT(KEY_S), PUT(KEY_T), PUT(KEY_U), PUT(KEY_V), PUT(KEY_W), PUT(KEY_X),
        PUT(KEY_Y), PUT(KEY_Z));

    // We expose the editor's functions to Lua by defining a new user type
    // Everytime Lua sees a reference to Editor, it will default to these
    // methods under the alias "Editor"
    L.new_usertype<Editor>("Editor", "insert_text", &Editor::insert_text);

    // We can create a method to register commands to the editors keymap
    L["register_command"] = [this](int key, Mod m, sol::function f) {
        // We create a keychord object so we can store it in the method map
        KeyChord chord{key, m};

        // We create a protected function as it is safer
        sol::protected_function pf = f;

        // We then capture a view into the state to keep sol from complaining
        sol::state_view sv(lua);

        // We need to cast the sol command into a void(Editor&) so we can store
        // it in the editor's method map
        // Because our reference is name ed, that is the name that must be used
        // in any Lua script
        Command cmd = [pf, sv](Editor &ed) mutable {
            // We pass in a reference to the editor using a reference wrapper
            // so that we can reassign it safely and view the editor in memory
            // This way we modify the existing Editor and not a copy made in Lua
            auto result = pf(std::ref(ed));
            // We do some validation to ensure the script doesnt crash the
            // editor
            if (!result.valid()) {
                sol::error err = result;
                std::cerr << "[Lua error] " << err.what() << '\n';
            }
        };

        // We can now push the chord into the map using mov semantics
        // This overrides any previously defined commands so be wary of what you
        // bind
        owner_->chordmap_[chord] = std::move(cmd);
    };
}
