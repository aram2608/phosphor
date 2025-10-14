#pragma once
#include <algorithm>
#include <cassert>
#include <cstring> // std::memmove
#include <stdexcept>
#include <string>
#include <vector>

class GapBuffer {
  public:
    using size_type = std::size_t;

    explicit GapBuffer(size_type initial_capacity = 64)
        : buf_(std::max<size_type>(initial_capacity, 1), '\0'), gap_begin_(0),
          gap_end_(buf_.size()) {}

    // Construct from string
    explicit GapBuffer(const std::string &s)
        : buf_(std::max<size_type>(s.size() * 2 + 16, 1), '\0') {
        // put text entirely before the gap
        std::memcpy(buf_.data(), s.data(), s.size());
        gap_begin_ = s.size();
        gap_end_ = buf_.size();
    }

    // current logical size (characters in buffer, excluding gap)
    size_type size() const noexcept { return buf_.size() - gap_size(); }

    // current cursor position (index in [0, size()])
    size_type cursor() const noexcept { return gap_begin_; }

    // get the whole text as std::string
    std::string str() const {
        std::string out;
        out.reserve(size());
        out.append(buf_.data(), gap_begin_);
        out.append(buf_.data() + gap_end_, buf_.size() - gap_end_);
        return out;
    }

    // set cursor (moves gap to pos)
    void set_cursor(size_type pos) {
        // We add a bounds check just in case
        if (pos > size()) {
            throw std::out_of_range("cursor out of range");
        }
        move_gap_to(pos);
    }

    // insert a single char at cursor
    void insert(char c) {
        ensure_gap(1);
        buf_[gap_begin_++] = c;
    }

    // insert a string (or arbitrary bytes) at cursor
    void insert(const char *s, size_type len) {
        ensure_gap(len);
        std::memcpy(buf_.data() + gap_begin_, s, len);
        gap_begin_ += len;
    }

    void insert(const std::string &s) { insert(s.data(), s.size()); }

    // delete k characters *ahead* of cursor (like Delete)
    size_type erase_forward(size_type k) {
        size_type r = right_len();
        size_type to_del = std::min(k, r);
        gap_end_ += to_del; // grow the gap into the right block
        return to_del;
    }

    // delete k characters *behind* cursor (like Backspace)
    size_type erase_back(size_type k) {
        size_type l = left_len();
        size_type to_del = std::min(k, l);
        gap_begin_ -= to_del; // grow the gap into the left block
        return to_del;
    }

    // Move cursor by signed delta (negative = left)
    void move_cursor(long long delta) {
        long long pos = static_cast<long long>(cursor());
        long long n = static_cast<long long>(size());
        // We clamp the pos + delta and our lower bound is 0 and uper bound is
        // the size of the buffer this prevent us from going out of bounds
        long long np = std::clamp(pos + delta, 0LL, n);
        set_cursor(static_cast<size_type>(np));
    }

    // Reserve additional free space in gap (useful to reduce reallocs)
    void reserve_gap(size_type extra) { ensure_gap(extra); }

  private:
    std::vector<char> buf_;
    size_type gap_begin_;
    size_type gap_end_;

    size_type gap_size() const noexcept { return gap_end_ - gap_begin_; }
    size_type left_len() const noexcept { return gap_begin_; }
    size_type right_len() const noexcept { return buf_.size() - gap_end_; }

    // Move gap so that gap_begin_ == pos
    void move_gap_to(size_type pos) {
        if (pos == gap_begin_)
            return;

        if (pos < gap_begin_) {
            // Move block [pos, gap_begin_) to end side just before gap_end_
            size_type count = gap_begin_ - pos;
            // Destination starts at gap_end_ - count
            std::memmove(buf_.data() + (gap_end_ - count), buf_.data() + pos,
                         count);
            gap_begin_ = pos;
            gap_end_ -= count;
        } else {
            // pos > gap_begin_: move block [gap_end_, gap_end_ + count) down to
            // gap_begin_
            size_type count = pos - gap_begin_;
            std::memmove(buf_.data() + gap_begin_, buf_.data() + gap_end_,
                         count);
            gap_begin_ += count;
            gap_end_ += count;
        }
        assert(gap_end_ >= gap_begin_);
    }

    // Ensure the gap has at least min_free capacity
    void ensure_gap(size_type min_free) {
        if (gap_size() >= min_free)
            return;

        size_type old_cap = buf_.size();
        size_type need = min_free - gap_size();
        size_type new_cap = std::max(old_cap * 2, old_cap + need + 32);

        std::vector<char> nb(new_cap, '\0');

        // left part size and right part size
        size_type L = left_len();
        size_type R = right_len();

        // Copy left block to beginning
        if (L)
            std::memcpy(nb.data(), buf_.data(), L);

        // New gap sits after left, with a bigger size
        size_type new_gap_begin = L;
        size_type new_gap_end = new_cap - R;

        // Copy right block to the end
        if (R) {
            std::memcpy(nb.data() + new_gap_end, buf_.data() + gap_end_, R);
        }

        buf_.swap(nb);
        gap_begin_ = new_gap_begin;
        gap_end_ = new_gap_end;
    }
};
