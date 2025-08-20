#include "editor.hpp"

// Main editor logic
Editor::Editor() {
    size_t caret = 0;
    size_t goal_col = 0;
    size_t top_line = 0;
    std::filesystem::path file;

    explicit Editor(std::string_view initial, std::filesystem::path p = {})
        : buf(initial), file(std::move(p)) {
        rebuild_index();
    }

    void rebuild_index() {
        li.rebuild(buf.str());
        if (caret > buf.size()) caret = buf.size();
    }

    // Function to insert text into buffer
    void insert_text(std::string_view s) {
        buf.insert(caret, s);
        caret += s.size();
        rebuild_index();
    }

    // Function to keep text visible
    void keep_visible(int height, int margin = 2) {
        size_t line = li.line_of(caret);
        if (line < top_line + margin) {
            top_line = (line > (size_t)margin) ? line - margin : 0;
        }
        if (line >= top_line + height - margin) {
            top_line = line - (height - margin - 1);
        }
    }

    // Function to erase text from buffer
    void backspace() {
        if (caret == 0) return;
        std::string s = buf.str();
        size_t prev = utf8::prev_cp(s, caret);
        size_t len = caret - prev;
        buf.erase(prev, len);
        caret = prev;
        rebuild_index();
    }
};