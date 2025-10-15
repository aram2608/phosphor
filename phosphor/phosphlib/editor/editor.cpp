#include "editor/editor.hpp"
#include "editor.hpp"
#include "palette/palette.hpp"

// Constructor for the Editor class
// We pass in the parsed contents and the file path
// We also initialize a vector of keys we want to poll for
Editor::Editor(std::string contents, std::filesystem::path file)
    : buffer_(contents), contents_(contents), file_(file),
      keys_{KEY_LEFT,      KEY_RIGHT, KEY_UP,           KEY_DOWN,
            KEY_BACKSPACE, KEY_ENTER, KEY_LEFT_CONTROL, KEY_TAB} {
    // We load the font from JetBrains
    text_font_ =
        LoadFont("JetBrainsMono-2.304/fonts/ttf/JetBrainsMono-Medium.ttf");
    // We bind the keymap in our initializer
    bind();
    vm_ = new ScriptingVM(this);
    vm_->load_init(std::filesystem::path("init.lua"));
    for (auto &[key, cmd] : vm_->lua_keymap_) {
        keys_.push_back(key);
    }
    state_ = EditingState::Editing;
}

// Destructor for the Editor class
Editor::~Editor() { UnloadFont(text_font_); }

// Function to draw editor contents to window
void Editor::draw() {
    ui_.draw_ui();
    // TODO: calculate position given text size
    DrawTextEx(text_font_, buffer_.c_str(), Vector2{100, 70}, 25.0f, 2,
               PhosphorGreen::ForestMoss);
    if (state_ == EditingState::Editing) {
        DrawTextEx(text_font_, file_.c_str(), ui_.fn_pos_, 25.0f, 2,
                   PhosphorGreen::ForestMoss);
    } else if (state_ == EditingState::Renaming) {
        DrawTextEx(text_font_, "Renaming: ", Vector2{400, 25}, 25.0f, 2,
                   PhosphorGreen::ForestMoss);
        DrawTextEx(text_font_, new_name_.c_str(), Vector2{550, 25}, 25.0f, 2,
                   PhosphorGreen::ForestMoss);
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

void Editor::insert_text(const std::string &text) {
    for (unsigned char c : text) {
        buffer_.insert(c);
    }
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
    } else if (IsKeyPressed(KEY_BACKSPACE)) {
        if (!new_name_.empty()) {
            new_name_.pop_back();
        }
    }
}

void Editor::editing() {
    // We listen for keyboard events and return the code point
    for (int code_point; (code_point = GetCharPressed()) != 0;) {
        // If the code is greater than 32 or a new line or a tab we process
        // it
        if (code_point >= 32 || code_point == '\n' || code_point == '\t') {
            // contents_ += code_point;
            buffer_.insert(code_point);
        }
    }

    // We iterate over the keys vector and dispatch the applicable method if
    //  encountered
    for (int k : keys_) {
        if (IsKeyDown(k)) {
            dispatch(k);
        }
    }
}

// Helper function to search for our method in the keymap
void Editor::dispatch(int key) {
    // We use the find method to return a map iterator
    auto elem = keymap_.find(key);
    // We test if the object is in the map
    if (elem != keymap_.end()) {
        // We then invoke the method and dereference the Editor instance
        elem->second(*this);
    }

    if (vm_ && vm_->has_key(key)) {
        if (auto name = vm_->key_to_command(key)) {
            vm_->run_command(*name);
        }
        return;
    }
}

// Helper function to bind the methods to our keymap
// This helps keep our constructor clean
void Editor::bind() {
    // We use a lambda to capture the dereferenced pointer to this, (ie *this)
    keymap_[KEY_LEFT] = [](Editor &editor) { editor.handle_left_key(); };
    keymap_[KEY_RIGHT] = [](Editor &editor) { editor.handle_right_key(); };
    keymap_[KEY_UP] = [](Editor &editor) { editor.handle_up_key(); };
    keymap_[KEY_DOWN] = [](Editor &editor) { editor.handle_down_key(); };
    keymap_[KEY_BACKSPACE] = [](Editor &editor) { editor.handle_backspace(); };
    keymap_[KEY_ENTER] = [](Editor &editor) { editor.handle_enter(); };
    keymap_[KEY_LEFT_CONTROL] = [](Editor &editor) { editor.handle_ctrl(); };
    keymap_[KEY_TAB] = [](Editor &editor) { editor.handle_tab(); };
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
    if (current_time - last_erase_time >= erase_buffer && !buffer_.empty()) {
        buffer_.erase_back(1);
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
    if (current_time - last_enter_time >= enter_buffer) {
        buffer_.insert('\n');
        last_enter_time = current_time;
    }
}

void Editor::handle_ctrl() {
    if (IsKeyPressed(KEY_S)) {
        save();
    }
}

void Editor::handle_tab() {
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