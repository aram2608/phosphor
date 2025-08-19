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