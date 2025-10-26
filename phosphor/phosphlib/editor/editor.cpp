#include "editor/editor.hpp"
#include "palette/palette.hpp"

// Helper method to decide if a mod key is currently applied
static inline Mod current_mods() {
    Mod m = MOD_NONE;
    if (IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)) {
        m |= MOD_CTRL;
    }
    if (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)) {
        m |= MOD_SHIFT;
    }
    if (IsKeyDown(KEY_LEFT_ALT) || IsKeyDown(KEY_RIGHT_ALT)) {
        m |= MOD_ALT;
    }
    if (IsKeyDown(KEY_LEFT_SUPER) || IsKeyDown(KEY_RIGHT_SUPER)) {
        m |= MOD_SUPER;
    }
    return m;
}

// Constructor for the Editor class
// We pass in the parsed contents and the file path
// We also initialize a vector of keys we want to poll for
Editor::Editor(std::string contents, std::filesystem::path file)
    : buffer_(contents), contents_(contents), file_(file), vm_(this) {
    // We bind the keymap in our initializer
    bind();
    vm_.load_init(std::filesystem::path("init.lua"));
    state_ = EditingState::Editing;
}

// Function to draw editor contents to window
void Editor::draw() {
    // We draw the main UI components
    ui_.draw_ui();
    // We use the ui helper function to draw the buffer
    ui_.draw_buffer(buffer_.c_str());
    // If we are editing we display the file name
    if (state_ == EditingState::Editing) {
        ui_.draw_fn(file_.c_str());
        // If we are renaming we need to display the new name to the screen
    } else if (state_ == EditingState::Renaming) {
        ui_.draw_rename_fn(new_name_.c_str());
    }
}

// Function to poll for keyboard input
void Editor::poll_input() {
    // We make an alias to a function pointer for a member function that returns
    // a void
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        move_to_mouse(GetMousePosition());
    }

    using IO = void (Editor::*)();
    // We make an array scoped to this function that stores the functions we
    // want to poll for, we make it static so that the table is remembered
    // across calls
    static std::array<IO, static_cast<size_t>(EditingState::Count)> TABLE = {
        &Editor::editing,
        &Editor::name_file,
    };
    // We then cast our state into a size_t so we can index the correct
    // method
    (this->*TABLE[static_cast<size_t>(state_)])();
}

// Wrapper method for inserting characters to the buffer
// Useful for exposing insertion capabilites for Lua extension
void Editor::insert_text(std::string text) { buffer_.insert(text); }

// Helper exposed to the Lua VM for picking color palettes
void Editor::pick_palette(const int palette) {
    // We need to make sure the provided integer actually corresponds to one
    // of the available palettes so we need to bounds check
    if (palette > static_cast<int>(Palette::Count) - 1 || palette < 0) {
        std::cout << "Invalid palette" << std::endl;
        return;
    }
    // If it is in bounds we can cast it to the correct enum and assign
    ui_.palette_ = static_cast<Palette>(palette);
    // Afterwards we need to dispatch to the correct pallete type
    ui_.dispatch_palette();
}

// Helper exposed to Lua API for toggling between palletes
void Editor::toggle_palette() {
    // We need to define a static int to make sure it is remembered between
    // calls
    static int palette_idx = 0;
    // We need to bounds check so we can reset back to the first palette after
    // we reach the maximum number
    if (palette_idx < static_cast<int>(Palette::Count) - 1) {
        pick_palette(++palette_idx);
    } else {
        palette_idx = 0;
        pick_palette(palette_idx);
    }
}

// Function to rename files
void Editor::name_file() {
    // We listen for keyboard events and return the code point
    for (int code_point; (code_point = GetCharPressed()) != 0;) {
        // If the code is greater than 32 or a new line or a tab we process
        // it
        if (code_point >= 32 || code_point == '\n' || code_point == '\t') {
            new_name_ += code_point;
        }
    }
    // After we hit enter we save the new name and return to an editing state
    if (IsKeyPressed(KEY_ENTER)) {
        file_ = new_name_;
        new_name_.clear();
        save();
        state_ = EditingState::Editing;
        // We need to be able to erase characters from the new name so
        // we pop back teh value if he new name string is not empty
    } else if (IsKeyPressed(KEY_BACKSPACE)) {
        if (!new_name_.empty()) {
            new_name_.pop_back();
        }
    }
}

// Function to handle editting logic
void Editor::editing() {
    // We listen for keyboard events and return the code point
    for (int cp; (cp = GetCharPressed()) != 0;) {
        // I will think of something later but for now, the catch below prevents
        // shifted characters so we'll just jump over it for now
        if (current_mods() & MOD_SHIFT) {
            goto shifted;
        }
        // We prevents Ctrl+S from inserting 's' or other combos
        if (current_mods() != MOD_NONE) {
            continue;
        }
    shifted:
        // Otherwise can insert the character into the buffer
        if (cp >= 32 || cp == '\n' || cp == '\t') {
            buffer_.insert(cp);
        }
    }

    // We listen for which Key is pressed and create a chord object
    for (int key; (key = GetKeyPressed()) != 0;) {
        // We create the object with the current key and whether we are
        // pressing a modifying key, ie Ctrl or Super
        KeyChord chord{key, current_mods()};
        // We then loop up the chord in the chordmap
        if (auto it = chordmap_.find(chord); it != chordmap_.end()) {
            // If we find it we dispatch the method and return to listening
            // for events
            it->second(*this);
            continue;
        }
    }
}

// Helper function to bind the methods to our keymap
// This helps keep our constructor clean
void Editor::bind() {
    chordmap_[{KEY_S, MOD_CTRL}] = [](Editor &e) { e.save(); };
    chordmap_[{KEY_LEFT, MOD_NONE}] = [](Editor &e) { e.move_left(); };
    chordmap_[{KEY_RIGHT, MOD_NONE}] = [](Editor &e) { e.move_right(); };
    chordmap_[{KEY_TAB, MOD_NONE}] = [](Editor &e) { e.tab(); };
    chordmap_[{KEY_ENTER, MOD_NONE}] = [](Editor &e) { e.enter(); };
    chordmap_[{KEY_BACKSPACE, MOD_NONE}] = [](Editor &e) { e.backspace(); };
    chordmap_[{KEY_UP, MOD_NONE}] = [](Editor &e) { e.move_up(); };
    chordmap_[{KEY_DOWN, MOD_NONE}] = [](Editor &e) { e.move_down(); };
    chordmap_[{KEY_V, MOD_CTRL}] = [](Editor &e) { e.paste(); };
}

// Method to move the cursor left
void Editor::move_left() { buffer_.move_cursor(-1); }

// Method to move the cursor right
void Editor::move_right() { buffer_.move_cursor(1); }

// TODO: Implement up moves
void Editor::move_up() { std::cout << "Up key pressed" << std::endl; }

// TODO: Implement down moves
void Editor::move_down() { std::cout << "Down key pressed" << std::endl; }

// Method to handle backspace, we simply erase back one char
void Editor::backspace() { buffer_.erase_back(1); }

// Method to handle enter, we simply push a new line
void Editor::enter() { buffer_.insert('\n'); }

// Method to handle tab, we simply push a tab
// can be problematic for Python so maybe need to offer a 4 space tab too
void Editor::tab() { buffer_.insert('\t'); }

// Method to paste clip board contents
void Editor::paste() {
    // We need to make sure the contents are not empty
    if (std::string contents = GetClipboardText(); !contents.empty()) {
        buffer_.insert(contents);
    }
}

void Editor::move_to_mouse(Vector2 mouse_pos) {
    // We test if the click is above the text
    if (mouse_pos.y < ui_.buffer_pos_.y) {
        // We simply move to the start of the text
        int index = buffer_.size();
        buffer_.move_cursor(-index);
    }

    // TODO: Figure out how to transform this to a logical positon below text
    // Currently the text is only measure by the actual number of characters
    // without accounting for new lines so the position is off by a lot
    if (mouse_pos.y > MeasureTextEx(ui_.text_font_, buffer_.c_str(),
                                    ui_.text_size_, ui_.text_spacing_)
                          .y) {
        std::cout << "Below text" << std::endl;
    }

    // We test if the click is to the left of the text
    if (mouse_pos.x < ui_.buffer_pos_.x) {
        std::cout << "Right of text" << std::endl;
    }
}

// Function to save changed buffer
void Editor::save() {
    // We first test if we were given a file path
    if (file_.empty()) {
        // If not we change to a renaming state
        state_ = EditingState::Renaming;
        return;
    }

    // We then make sure the buffer is not empty
    if (buffer_.empty()) {
        // If it's empty we return out
        return;
    }

    // We then create an output file stream with our file data
    std::ofstream out(file_);
    // If the object was not created we return out
    if (!out) {
        return;
    }

    // We copy the written contents into a new string and write out
    const std::string new_contents = buffer_.str();
    out.write(new_contents.data(), (std::streamsize)new_contents.size());
}