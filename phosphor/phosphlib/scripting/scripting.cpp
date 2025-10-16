#include "scripting/scripting.hpp"
#include "editor/editor.hpp"

// clang-format off
    #define PUT(KEYSYM) \
        #KEYSYM, KEYSYM
// clang-format on

ScriptingVM::ScriptingVM(Editor *owner) : owner_(owner) {
    lua.open_libraries(sol::lib::base, sol::lib::math, sol::lib::table);
    push_api(lua);
}

void ScriptingVM::load_init(const std::filesystem::path &path) {
    auto result = lua.safe_script_file(path, &sol::script_pass_on_error);
    if (!result.valid()) {
        sol::error err = result;
        std::cerr << "Lua error: " << err.what() << '\n';
    }
}

void ScriptingVM::push_api(sol::state &L) {
    L.new_enum("Mod", "NONE", Mod::MOD_NONE, "CTRL", Mod::MOD_CTRL, "SHIFT",
               Mod::MOD_SHIFT, "ALT", Mod::MOD_ALT, "SUPER", Mod::MOD_SUPER);

    // We create a table to store useful keys to override
    L["keys"] = L.create_table_with(PUT(KEY_UP), PUT(KEY_DOWN), PUT(KEY_LEFT),
                                    PUT(KEY_RIGHT), PUT(KEY_LEFT_ALT),
                                    PUT(KEY_LEFT_CONTROL), PUT(KEY_A),
                                    PUT(KEY_S), PUT(KEY_D), PUT(KEY_W));

    // We expose the editor's functions to Lua
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
        Command cmd = [pf, sv](Editor &ed) mutable {
            // We pass in a reference to the editor using a reference wrapper
            // so that we can reassign it safely
            auto result = pf(std::ref(ed));
            // We do some validaion to ensure the script doesnt crash the editor
            if (!result.valid()) {
                sol::error err = result;
                std::cerr << "[Lua error] " << err.what() << '\n';
            }
        };

        // We can now push the chord into the map using mov semantics
        owner_->chordmap_[chord] = std::move(cmd);
    };
}
