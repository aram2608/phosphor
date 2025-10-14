#ifndef GAP_BUFFER_HPP
#define GAP_BUFFER_HPP

#include <algorithm>
#include <cstring>
#include <string>
#include <vector>

class GapBuffer {
  public:
    explicit GapBuffer(std::size_t start_capacity = 64);

    explicit GapBuffer(const std::string &start_string);

    std::size_t cursor() const noexcept;
    void set_cursor(std::size_t pos);
    void move_cursor(long long delta);
    std::size_t size() const noexcept;
    bool empty() const noexcept;
    std::string str() const;

    const char *c_str() const;
    void insert(char c);
    void erase_back(std::size_t num_chars);

  private:
    std::vector<char> buf_;
    std::size_t gap_begin_;
    std::size_t gap_end_;

    std::size_t gap_size() const noexcept;
    std::size_t left_len() const noexcept;
    std::size_t right_len() const noexcept;
    void ensure_gap(std::size_t want);
    void move_gap_to(std::size_t pos);
    mutable std::string cached_str_;
    mutable bool cache_valid_{false};
};

#endif