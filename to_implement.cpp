#include <ftxui/dom/elements.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/event.hpp>
#include "ftxui/screen/color.hpp"

#include <ctime>
#include <fstream>
#include <string>
#include <vector>
#include <optional>
#include <sstream>
#include <filesystem>
#include <vector>
#include <string>
#include <string_view>
#include <stdexcept>
#include <algorithm>

#include <iostream>

// Main editor logic
struct Editor {
    GapBuffer buf;
    LineIndex li;
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

    void move_left() {
        if (caret == 0) return;
        std::string s = buf.str();
        caret = utf8::prev_cp(s, caret);
        goal_col = li.column_of(s, caret);
    }

    void move_right() {
        if (caret >= buf.size()) return;
        std::string s = buf.str();
        caret = utf8::next_cp(s, caret);
        goal_col = li.column_of(s, caret);
    }

    void move_home() {
        std::string s = buf.str();
        size_t line = li.line_of(caret);
        caret = li.byte_at_col(s, line, 0);
        goal_col = 0;
    }

    void move_end() {
        std::string s = buf.str();
        size_t line = li.line_of(caret);
        size_t i = li.byte_at_col(s, line, SIZE_MAX / 2);
        while (i < s.size() && s[i] != '\n') i = utf8::next_cp(s, i);
        caret = i;
        goal_col = li.column_of(s, caret);
    }

    void move_up() {
        std::string s = buf.str();
        size_t line = li.line_of(caret);
        if (line == 0) return;
        if (goal_col == SIZE_MAX) goal_col = li.column_of(s, caret);
        caret = li.byte_at_col(s, line - 1, goal_col);
        if (line - 1 < top_line) top_line = line - 1;
    }

    void move_down() {
        std::string s = buf.str();
        size_t line = li.line_of(caret);
        if (line + 1 >= li.line_count()) return;
        if (goal_col == SIZE_MAX) goal_col = li.column_of(s, caret);
        caret = li.byte_at_col(s, line + 1, goal_col);
    }

    void keep_visible(int height, int margin = 2) {
        size_t line = li.line_of(caret);
        if (line < top_line + margin) {
            top_line = (line > (size_t)margin) ? line - margin : 0;
        }
        if (line >= top_line + height - margin) {
            top_line = line - (height - margin - 1);
        }
    }

    bool save() const {
        if (file.empty()) return false;
        std::ofstream out(file, std::ios::binary);
        if (!out) return false;
        const auto s = buf.str();
        out.write(s.data(), (std::streamsize)s.size());
        return (bool)out;
    }
};