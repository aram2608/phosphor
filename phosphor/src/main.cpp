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

using namespace ftxui;

// UTF-8 helper functions
namespace utf8 {
    inline size_t cp_len_forward(const std::string& s, size_t i) {
        if (i >= s.size()) return 0;
        unsigned char c = static_cast<unsigned char>(s[i]);
        if ((c & 0x80) == 0) return 1;
        if ((c & 0xE0) == 0xC0) return 2;
        if ((c & 0xF0) == 0xE0) return 3;
        if ((c & 0xF8) == 0xF0) return 4;
        return 1;
    }
    inline size_t prev_cp(const std::string& s, size_t i) {
        if (i == 0) return 0;
        size_t j = i - 1;
        int back = 0;
        while (j > 0 && back < 3) {
            unsigned char c = static_cast<unsigned char>(s[j]);
            if ((c & 0xC0) != 0x80) break;
            --j; ++back;
        }
        return j;
    }
    inline size_t next_cp(const std::string& s, size_t i) {
        if (i >= s.size()) return s.size();
        return i + cp_len_forward(s, i);
    }
}

// Line index for viewport
struct LineIndex {
    std::vector<size_t> line_starts{0};

    void rebuild(const std::string& s) {
        line_starts.clear();
        line_starts.push_back(0);
        for (size_t i = 0; i < s.size(); ++i)
            if (s[i] == '\n') line_starts.push_back(i + 1);
    }

    size_t line_of(size_t byte) const {
        size_t lo = 0, hi = line_starts.size();
        while (lo + 1 < hi) {
            size_t mid = (lo + hi) / 2;
            if (line_starts[mid] <= byte) lo = mid;
            else hi = mid;
        }
        return lo;
    }

    size_t column_of(const std::string& s, size_t byte) const {
        size_t line = line_of(byte);
        size_t start = line_starts[line];
        size_t col = 0;
        for (size_t i = start; i < byte && i < s.size();) {
            i = utf8::next_cp(s, i);
            ++col;
        }
        return col;
    }

    size_t byte_at_col(const std::string& s, size_t line, size_t col) const {
        if (line >= line_starts.size()) return s.size();
        size_t i = line_starts[line];
        size_t c = 0;
        while (i < s.size() && s[i] != '\n' && c < col) {
            i = utf8::next_cp(s, i);
            ++c;
        }
        return i;
    }

    size_t line_count() const { return line_starts.size(); }
};

class GapBuffer {
public:
    GapBuffer() = default;

    explicit GapBuffer(std::string_view s) {
        buf_.assign(s.begin(), s.end());
        // create a small initial gap at end
        const size_t gap = 1024;
        buf_.insert(buf_.end(), gap, '\0');
        gap_lo_ = s.size();
        gap_hi_ = s.size() + gap;
    }

    size_t size() const { return buf_.size() - gap_size(); }

    // logical byte -> physical index in buf_
    size_t phys(size_t pos) const {
        if (pos < gap_lo_) return pos;
        return pos + gap_size();
    }

    char at(size_t pos) const {
        if (pos > size()) throw std::out_of_range("at()");
        return buf_[phys(pos)];
    }

    void insert(size_t pos, std::string_view s) {
        move_gap(pos);
        ensure_gap(s.size());
        std::copy(s.begin(), s.end(), buf_.begin() + gap_lo_);
        gap_lo_ += s.size();
    }

    void erase(size_t pos, size_t len) {
        if (pos > size()) return;
        len = std::min(len, size() - pos);
        move_gap(pos);
        // delete forward from gap_hi_, effectively enlarging gap
        gap_hi_ += len;
    }

    std::string str() const {
        std::string out;
        out.reserve(size());
        out.append(buf_.begin(), buf_.begin() + gap_lo_);
        out.append(buf_.begin() + gap_hi_, buf_.end());
        return out;
    }

private:
    std::vector<char> buf_;
    size_t gap_lo_ = 0;
    size_t gap_hi_ = 0;

    size_t gap_size() const { return gap_hi_ - gap_lo_; }

    void ensure_gap(size_t need) {
        if (gap_size() >= need) return;
        // grow by 2x strategy
        size_t new_gap = std::max(need, gap_size() * 2 + 1024);
        std::vector<char> newbuf;
        newbuf.reserve(size() + new_gap);
        // left
        newbuf.insert(newbuf.end(), buf_.begin(), buf_.begin() + gap_lo_);
        // new gap
        newbuf.insert(newbuf.end(), new_gap, '\0');
        size_t new_gap_lo = gap_lo_;
        size_t new_gap_hi = new_gap_lo + new_gap;
        // right
        newbuf.insert(newbuf.end(), buf_.begin() + gap_hi_, buf_.end());
        buf_.swap(newbuf);
        gap_lo_ = new_gap_lo;
        gap_hi_ = new_gap_hi;
    }

    void move_gap(size_t pos) {
        if (pos == gap_lo_) return;
        if (pos < gap_lo_) {
            size_t move = gap_lo_ - pos;
            // move left: shift chars left of gap into right side
            std::move_backward(buf_.begin() + pos,
                            buf_.begin() + gap_lo_,
                            buf_.begin() + gap_hi_);
            gap_lo_ -= move;
            gap_hi_ -= move;
        } else { // pos > gap_lo_
            size_t move = pos - gap_lo_;
            // move right: shift right side into left side
            std::move(buf_.begin() + gap_hi_,
                    buf_.begin() + gap_hi_ + move,
                    buf_.begin() + gap_lo_);
            gap_lo_ += move;
            gap_hi_ += move;
        }
    }
};

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

    void insert_text(std::string_view s) {
        buf.insert(caret, s);
        caret += s.size();
        rebuild_index();
    }

    void backspace() {
        if (caret == 0) return;
        std::string s = buf.str();
        size_t prev = utf8::prev_cp(s, caret);
        size_t len = caret - prev;
        buf.erase(prev, len);
        caret = prev;
        rebuild_index();
    }

    void del() {
        if (caret >= buf.size()) return;
        std::string s = buf.str();
        size_t next = utf8::next_cp(s, caret);
        size_t len = next - caret;
        buf.erase(caret, len);
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

// Simple helper function to display time
char* calc_time() {
    // Calculate time
    time_t now = time(0);
    char* dt = ctime(&now);
    return dt;
}

// Helper function to decide if a file has contents
static std::string slurp_or_empty(const std::filesystem::path& p) {
    std::ifstream in(p, std::ios::binary);
    if (!in) return {};
    std::ostringstream ss;
    ss << in.rdbuf();
    return ss.str();
}

int main(int argc, const char* argv[]) {
    // Load in file from command line
    std::filesystem::path file;
    if (argc > 1) file = argv[1];
    std::string initial = file.empty() ? std::string{} : slurp_or_empty(file);

    // Creates instance of Editor struct
    Editor editor(initial, file);

    // Creates an interactive screen, compiler deducts typing
    auto screen = ScreenInteractive::Fullscreen();

    // Creates the renderer for interactive components
    auto renderer = Renderer([&] {
        // Use screen size to compute viewport
        int total_h = screen.dimy();
        int total_w = screen.dimx();
        int view_height = std::max(1, total_h - 4);

        // Load contents
        std::string contents = editor.buf.str();
        editor.keep_visible(view_height);

        // Calculate lines in contents
        std::vector<Element> lines;
        size_t start_line = editor.top_line;
        size_t end_line = editor.top_line + (size_t)view_height;

        for (size_t line = start_line; line < end_line && line < editor.li.line_count(); ++line) {
            size_t start = editor.li.line_starts[line];
            size_t stop  = (line + 1 < editor.li.line_count()) ? editor.li.line_starts[line + 1] : contents.size();
            std::string_view sv(&contents[start], stop - start);
            if (!sv.empty() && sv.back() == '\n') sv.remove_suffix(1);
            lines.push_back(text(std::string(sv)));
        }
        if (lines.empty()) lines.push_back(text(""));

        size_t caret_line = editor.li.line_of(editor.caret);
        size_t caret_col  = editor.li.column_of(contents, editor.caret);

        // Creates the text boxes
        auto content = hbox(
            std::move(lines)
        );

        // Heading
        auto heading = hbox(
            text(" phosphor ") | color(Color::RGB(0x0d,0x0f,0x0a)) | border | flex
        );

        // Keep track of time
        auto time = hbox(
            text(calc_time()) | color(Color::RGB(0x0d,0x0f,0x0a)) | border | flex
        );

        std::string status;
        status += editor.file.empty() ? "[No Name]" : editor.file.string();
        status += "  |  ";
        status += "Ln " + std::to_string(caret_line + 1) + ", Col " + std::to_string(caret_col + 1);
        status += "  |  F2:Save  Ctrl+Q:Quit";

        auto status_line = hbox({ text(status) }) | border;

        // Return the rendered editor
        return gridbox({
        {heading, time, status_line},
        {content}
        });
    });

    // Catches events for interactive changes to program
    auto catcher = CatchEvent(renderer, [&](Event event) {
        if (event.is_character()) {
            char32_t ch = event.character()[0];
            if (ch == '\r') return true;
            if (ch == '\n') { editor.insert_text("\n"); return true; }
            if (ch == '\t') { editor.insert_text("\t"); return true; }
            if (ch >= 0x20) {
                editor.insert_text(event.character());
                return true;
            }
        }
        if (event == Event::Backspace) { editor.backspace(); return true; }
        if (event == Event::Delete)    { editor.del();       return true; }

        if (event == Event::ArrowLeft)  { editor.move_left();  return true; }
        if (event == Event::ArrowRight) { editor.move_right(); return true; }
        if (event == Event::ArrowUp)    { editor.move_up();    return true; }
        if (event == Event::ArrowDown)  { editor.move_down();  return true; }
        if (event == Event::Home)       { editor.move_home();  return true; }
        if (event == Event::End)        { editor.move_end();   return true; }

        // ctrl-q to quit editor - maps to ASCII code 17
        if (event == Event::Special({17})) {
            screen.Exit();
            return true;
        }
        // ctrl-x to save - maps to ASCII code 24
        if (event == Event::Special({24})) {
            if (!editor.file.empty()) {
                editor.save();
            }
            return true;
        }
        return false;
    });

    // The main loop to print to screen after catching events
    screen.Loop(catcher);
    return 0;
}