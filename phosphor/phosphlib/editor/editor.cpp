#include "editor/editor.hpp"
#include "editor.hpp"
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
    // We load the font from JetBrains
    text_font_ =
        LoadFont("JetBrainsMono-2.304/fonts/ttf/JetBrainsMono-Medium.ttf");
    // We bind the keymap in our initializer
    bind();
    vm_.load_init(std::filesystem::path("init.lua"));
    state_ = EditingState::Editing;
}

// Destructor for the Editor class
Editor::~Editor() { UnloadFont(text_font_); }

// Function to draw editor contents to window
void Editor::draw() {
    // We draw the main UI components
    ui_.draw_ui();
    DrawTextEx(text_font_, buffer_.c_str(), Vector2{60, 70}, 25.0f, 2,
               ui_.text_color_);
    if (state_ == EditingState::Editing) {
        DrawTextEx(ui_.title_font_, file_.c_str(), ui_.fn_pos_, 30.0f, 2,
                   ui_.title_color_);
    } else if (state_ == EditingState::Renaming) {
        DrawTextEx(text_font_, "Renaming: ", Vector2{400, 25}, 25.0f, 2,
                   ui_.title_color_);
        DrawTextEx(text_font_, new_name_.c_str(), Vector2{550, 25}, 25.0f, 2,
                   ui_.title_color_);
    }
}

// Function to poll for keyboard input
void Editor::poll_input() {
    // We make an alias to a function pointer for a member function that returns
    // a void
    using IO = void (Editor::*)();
    // We make an array scoped to this function that stores the functions we
    // want to poll for
    static constexpr std::array<IO, 2> TABLE = {
        &Editor::editing,
        &Editor::name_file,
    };
    // We then cast our state into a size_t so we can index the correct
    // method
    (this->*TABLE[static_cast<std::size_t>(state_)])();
}

// Wrapper method for inserting characters to the buffer
// Useful for exposing inseration capabilites for Lua extensions
void Editor::insert_text(const std::string &text) {
    // We loop through each character and insert into buffer
    for (char c : text) {
        buffer_.insert(c);
    }
}

// Helper exposed to the Lua VM for picking color palletes
void Editor::pick_pallete(const int pallete) {
    // We need to make sure the provided integer actually corresponds to one
    // of the available palletes so we need to bounds check
    if (pallete > static_cast<int>(Pallete::Count) - 1 || pallete < 0) {
        std::cout << "Invalid pallete" << std::endl;
        return;
    }
    // If it is in bounds we can cast it to the correct enum and assign
    ui_.pallete_ = static_cast<Pallete>(pallete);
    // Afterwards we need to dispatch to the correct pallete type
    ui_.dispatch_pallete();
}

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
        state_ = EditingState::Editing;
        // We need to be able to erase characters from the new name so
        // we pop back teh value if he new name string is not empty
    } else if (IsKeyPressed(KEY_BACKSPACE)) {
        if (!new_name_.empty()) {
            new_name_.pop_back();
        }
    }
}

void Editor::editing() {
    // We listen for keyboard events and return the code point
    for (int cp; (cp = GetCharPressed()) != 0;) {
        // We prevents Ctrl+S from inserting 's' or other combos
        if (current_mods() != MOD_NONE) {
            continue;
        }
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
}

void Editor::move_left() { buffer_.move_cursor(-1); }

void Editor::move_right() { buffer_.move_cursor(1); }

void Editor::move_up() { std::cout << "Up key pressed" << std::endl; }

void Editor::move_down() { std::cout << "Down key pressed" << std::endl; }

void Editor::backspace() {
    // We create a static value to retain the last time an erase happened
    // this will ensure it persits even when the function goes out of scope
    static double last_erase_time = 0.0;
    // We create a little buffer to slow down erasing contents
    double erase_buffer = 0.15;
    double current_time = GetTime();

    // We only erase if enough time has passed since last erase and the contents
    // are not empty
    if (current_time - last_erase_time >= erase_buffer && !buffer_.empty()) {
        buffer_.erase_back(1);
        last_erase_time = current_time;
    }
}

void Editor::enter() {
    // We make a little buffer to prevent spam new lines
    static double last_enter_time = 0.0;
    double enter_buffer = 0.15;
    double current_time = GetTime();

    // We only erase if enough time has passed since last erase and the contents
    // are not empty
    if (current_time - last_enter_time >= enter_buffer) {
        buffer_.insert('\n');
        last_enter_time = current_time;
    }
}

void Editor::tab() {
    // We make a little buffer to prevent spam tabs
    static double last_tab_time = 0.0;
    double tab_buffer = 0.15;
    double current_time = GetTime();

    // We only erase if enough time has passed since last erase and the buffer
    // are not empty
    if (current_time - last_tab_time >= tab_buffer) {
        buffer_.insert('\t');
        last_tab_time = current_time;
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