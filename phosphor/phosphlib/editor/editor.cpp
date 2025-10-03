#include "editor/editor.hpp"
#include "palette/palette.hpp"

// Constructor for the Editor class
// We pass in the parsed contetns and the file path
Editor::Editor(std::string contents, std::filesystem::path file)
    : contents(contents), file(file),
      keys{KEY_LEFT, KEY_RIGHT, KEY_UP, KEY_DOWN, KEY_BACKSPACE, KEY_ENTER} {
    // We load the font from JetBrains
    text_font =
        LoadFont("JetBrainsMono-2.304/fonts/ttf/JetBrainsMono-Medium.ttf");
    // We bind the keymap in our initializer
    bind();
}

// Destrcutor for the Editor class
Editor::~Editor() { UnloadFont(text_font); }

// Function to draw editor contents to window
void Editor::draw() {
    // We simple pass in the preloaded font and the contents as a c string
    // TODO: calculate position given text size
    DrawTextEx(text_font, contents.c_str(), Vector2{100, 200}, 25.0f, 2,
               PhosphorGreen::ForestMoss);
}

// Function to save changed buffer
bool Editor::save() {
    // We first make sure the file is not empty
    if (contents.empty()) {
        return false;
    }
    // We then create an output file stream with our file data
    std::ofstream out(file);
    // If the object was not created we return out
    if (!out) {
        return false;
    }
    // We copy the written contents into a new string and write out
    const std::string new_contents = contents;
    out.write(new_contents.data(), (std::streamsize)new_contents.size());
    // If successful we cast the object to a bool and return
    return (bool)out;
}

void Editor::poll_input() {
    // We listen for keyboard events and return the
    for (int code_point; (code_point = GetCharPressed()) != 0;) {
        // If the code is greater than 32 or a new line or a tab we process it
        if (code_point >= 32 || code_point == '\n' || code_point == '\t') {
            std::cout << code_point << std::endl;
        }
    }

    // We iterate over the keys vector and dispatch the applicable method if
    //  encountered
    for (int k : keys)
        if (IsKeyPressed(k)) {
            dispatch(k);
        }
}

// Helper function to bind the methods to our keymap
// This helps keep our constructor clean
void Editor::bind() {
    // We use a lambda to capture the dereferenced pointer to this, (ie *this)
    keymap[KEY_LEFT] = [](Editor &editor) { editor.handle_left_key(); };
    keymap[KEY_RIGHT] = [](Editor &editor) { editor.handle_right_key(); };
    keymap[KEY_UP] = [](Editor &editor) { editor.handle_up_key(); };
    keymap[KEY_DOWN] = [](Editor &editor) { editor.handle_down_key(); };
    keymap[KEY_BACKSPACE] = [](Editor &editor) { editor.handle_backspace(); };
    keymap[KEY_ENTER] = [](Editor &editor) { editor.handle_enter(); };
}

// Helper function to search for our method in the keymap
void Editor::dispatch(int key) {
    // We use the find method to return a map iterator
    auto elem = keymap.find(key);
    // We test if the object is in the map
    if (elem != keymap.end()) {
        // We then invoke the method and dereference the Editor instance
        elem->second(*this);
    }
}

void Editor::handle_left_key() { std::cout << "Left key pressed" << std::endl; }

void Editor::handle_right_key() {
    std::cout << "Right key pressed" << std::endl;
}

void Editor::handle_up_key() { std::cout << "Up key pressed" << std::endl; }

void Editor::handle_down_key() { std::cout << "Down key pressed" << std::endl; }

void Editor::handle_backspace() {
    std::cout << "Backspace pressed" << std::endl;
}

void Editor::handle_enter() { std::cout << "Enter pressed" << std::endl; }
