#include "editor/editor.hpp"
#include "palette/palette.hpp"

// Constructor for the Editor class
// We pass in the parsed contents and the file path
// We also initialize a vector of keys we want to poll for
Editor::Editor(std::string contents, std::filesystem::path file)
    : contents(contents), file(file),
      keys{KEY_LEFT,      KEY_RIGHT, KEY_UP,           KEY_DOWN,
           KEY_BACKSPACE, KEY_ENTER, KEY_LEFT_CONTROL, KEY_TAB} {
    // We load in the font from our assets file
    title_font = LoadFont(
        "JetBrainsMono-2.304/fonts/ttf/JetBrainsMono-ExtraBoldItalic.ttf");
    // We load the font from JetBrains
    text_font =
        LoadFont("JetBrainsMono-2.304/fonts/ttf/JetBrainsMono-Medium.ttf");
    // We bind the keymap in our initializer
    bind();
}

// Destructor for the Editor class
Editor::~Editor() { UnloadFont(text_font); }

// Function to draw editor contents to window
void Editor::draw() {
    // Base UI - will probably change in the future, it burns my eyes a bit
    // We clear the background every loop and display the dark background
    ClearBackground(PhosphorGreen::DarkBg);
    DrawRectangleRoundedLinesEx({10, 10, 780, 780}, 0.18f, 20, 2,
                                PhosphorGreen::SoftMint);
    DrawLineEx(Vector2{25, 60}, Vector2{775, 60}, 3, PhosphorGreen::SoftMint);
    DrawTextEx(title_font, "phosphor", Vector2{40, 25}, 25.0f, 2,
               PhosphorGreen::ForestMoss);
    // We simple pass in the preloaded font and the contents as a c string
    // TODO: calculate position given text size
    DrawTextEx(text_font, contents.c_str(), Vector2{100, 200}, 25.0f, 2,
               PhosphorGreen::ForestMoss);
}

// Function to poll for keyboard input
void Editor::poll_input() {
    // We listen for keyboard events and return the code point
    for (int code_point; (code_point = GetCharPressed()) != 0;) {
        // If the code is greater than 32 or a new line or a tab we process it
        if (code_point >= 32 || code_point == '\n' || code_point == '\t') {
            contents += code_point;
        }
    }

    // We iterate over the keys vector and dispatch the applicable method if
    //  encountered
    for (int k : keys)
        if (IsKeyDown(k)) {
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
    keymap[KEY_LEFT_CONTROL] = [](Editor &editor) { editor.handle_ctrl(); };
    keymap[KEY_TAB] = [](Editor &editor) { editor.handle_tab(); };
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
    // We create a static value to retain the last time an erase happened
    // this will ensure it persits even when the function goes out of scope
    static double last_erase_time = 0.0;
    // We create a little buffer to slow down erasing contents
    double erase_buffer = 0.15;
    double current_time = GetTime();

    // We only erase if enough time has passed since last erase and the contents
    // are not empty
    if (current_time - last_erase_time >= erase_buffer && !contents.empty()) {
        contents.pop_back();
        last_erase_time = current_time;
    }
}

void Editor::handle_enter() {
    // We make a little buffer to prevent spam new lines
    static double last_enter_time = 0.0;
    double enter_buffer = 0.15;
    double current_time = GetTime();

    // We only erase if enough time has passed since last erase and the contents
    // are not empty
    if (current_time - last_enter_time >= enter_buffer && !contents.empty()) {
        contents += "\n";
        last_enter_time = current_time;
    }
}

void Editor::handle_ctrl() {
    if (IsKeyPressed(KEY_S)) {
        std::cout << "Saving file." << std::endl;
        save();
    }
}

void Editor::handle_tab() {
    // We make a little buffer to prevent spam tabs
    static double last_tab_time = 0.0;
    double tab_buffer = 0.15;
    double current_time = GetTime();

    // We only erase if enough time has passed since last erase and the contents
    // are not empty
    if (current_time - last_tab_time >= tab_buffer && !contents.empty()) {
        contents += "\t";
        last_tab_time = current_time;
    }
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