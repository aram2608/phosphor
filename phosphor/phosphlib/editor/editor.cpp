#include "editor/editor.hpp"
#include "editor.hpp"
#include "palette/palette.hpp"

// Constructor for the Editor class
// We pass in the parsed contetns and the file path
Editor::Editor(std::string contents, std::filesystem::path file)
    : contents(contents), file(file) {
    // We load the font from JetBrains
    text_font =
        LoadFont("JetBrainsMono-2.304/fonts/ttf/JetBrainsMono-Medium.ttf");
}

// Destrcutor for the Editor class
Editor::~Editor() { UnloadFont(text_font); }

// Function to draw editor contents to window
void Editor::draw() {
    DrawTextEx(text_font, contents.c_str(), Vector2{200, 200}, 25.0f, 2,
               PhosphorGreen::ForestMoss);
}

// Function to save changed buffer
bool Editor::save() {
    if (file.empty()) {
        return false;
    }
    std::ofstream out(file);
    if (!out) {
        return false;
    }
    const std::string new_contents = contents;
    out.write(new_contents.data(), (std::streamsize)new_contents.size());
    return (bool)out;
}

void Editor::update() {
    keymap();
}

void Editor::keymap() {
    if (IsKeyPressed(KEY_A)) {
        contents += "a";
    }
    if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyDown(KEY_S)) {
        std::cout << "clicked" << std::endl;
        if (save()) {
            std::cout << "should close" << std::endl;
        }
    }
}
