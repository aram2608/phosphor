#include "gap_buffer/gap_buffer.hpp"

GapBuffer::GapBuffer(size_t start_capacity)
    // We initialize our buffer with the starting capacity and fill it with null
    // characters
    // We also set the start of the gap and end of the gap
    : buf_(std::max<size_t>(start_capacity, 1), '\0'), gap_begin_(0),
      gap_end_(buf_.size()) {}

GapBuffer::GapBuffer(const std::string &start_string)
    /*
     * We initialize our buffer with the size of the string * 2 and plus 16 more
     * chars If we pass in "hello" we then get 5 * 2 + 16, so that gives us a 27
     * char sized buffer with 5 characters on the left and 21 unused chars in
     * our gap with no characters on the right side of the gap
     *          'h' 'e' 'l' 'l''o' -------THIS IS THE GAP----------
     */
    : buf_(std::max<size_t>(start_string.size() * 2 + 16, 1), '\0') {
    // We place the data at the start of the buffer using memcpy
    // void* memcpy(void* destination, const void* src, std::size_t num_bytes);
    std::memcpy(buf_.data(), start_string.data(), start_string.size());
    // We then set the start of the gap to the size of the string
    gap_begin_ = start_string.size();
    // The gap size is equal to the entire buffer
    gap_end_ = buf_.size();
}

// Basic helper to get the cursor position at the start of the gap
size_t GapBuffer::cursor() const noexcept { return gap_begin_; }

// Method to set the cursor position
void GapBuffer::set_cursor(size_t pos) {
    // We need to make sure the position is in bounds
    if (pos > size()) {
        throw std::out_of_range("cursor out of range");
    }
    // We can then move the position
    move_gap_to(pos);
}

// Method to move the cursor
void GapBuffer::move_cursor(long long delta) {
    // We get our cursors position
    long long pos = static_cast<long long>(cursor());
    // We get the limit of our buffer
    long long limit = static_cast<long long>(size());
    // We clamp the pos + delta and our lower bound is 0 and upper bound is
    // the size of the buffer this prevent us from going out of bounds
    long long new_pos = std::clamp(pos + delta, 0LL, limit);
    set_cursor(static_cast<size_t>(new_pos));
}

// Method to return the size of the buffer contents
size_t GapBuffer::size() const noexcept { return buf_.size() - gap_size(); }

// Method to test if the container is empty
bool GapBuffer::empty() const noexcept { return size() == 0; }

// Method to convert the contents to a string
std::string GapBuffer::str() const {
    // We forward declare the out string
    std::string out;
    // We reserve enough space given the size of contents
    out.reserve(size());
    // We then append the contents, we pass in a pointer to the data
    // and the beginning of the gap as the size of the substring we want
    out.append(buf_.data(), gap_begin_);
    // We then append the rest of the contents, we need to pass in a pointer
    // to the underlying data and do some pointer arithmetic to get the end of
    // the gap and substring out the contents after the gap
    out.append(buf_.data() + gap_end_, buf_.size() - gap_end_);
    return out;
}

// Function to return a C-string for compatability with C APIs
const char *GapBuffer::c_str() const {
    // We need to ensure the previously cached string is valid
    if (!cache_valid_) {
        // If it is not, we need to recompute it
        compute_cache();
    }
    // Only then can we return the c str
    return cached_str_.c_str();
}

// Method to insert a single character
void GapBuffer::insert(char c) {
    // We need to make sure we have room for at least one char
    ensure_gap(1);
    // We then append the character and increment the start of the gap forward
    buf_[gap_begin_++] = c;
    // Since an edit was made we must rebuild the cached string
    cache_valid_ = false;
}

// Method to insert entire strings
void GapBuffer::insert(std::string str) {
    // This is just a wrapper of the single char method
    // We loop over each character and insert one by one
    // Not the most efficient but it'll for for now
    for (char c : str) {
        insert(c);
    }
}

void GapBuffer::erase_back(size_t num_chars) {
    // We get the left left and do a small calculation to see
    // how much to delete
    size_t left = left_len();
    size_t to_del = std::min(num_chars, left);
    // We grow the gap into the left block to delete
    gap_begin_ -= to_del;
    cache_valid_ = false;
}

// Method to return the size of the gap in the buffer
size_t GapBuffer::gap_size() const noexcept { return gap_end_ - gap_begin_; }

// Method to return the left side of the buffer
size_t GapBuffer::left_len() const noexcept { return gap_begin_; }

// Method to return the right side of the buffer
size_t GapBuffer::right_len() const noexcept { return buf_.size() - gap_end_; }

void GapBuffer::ensure_gap(size_t want) {
    // We test if our gap size is big enough and return if it is
    if (gap_size() >= want) {
        return;
    }

    // We store our old cap to calc the new cap
    size_t old_cap = buf_.size();
    // We calc how much space we need
    size_t need = want - gap_size();
    /*
     * We calc the new gap given the old cap and how much we need
     * We choose the max value between a simple doubling or adding our need
     * to old cap + 32, its abritrary but should give us enough space for most
     * editing processes
     */
    size_t new_cap = std::max(old_cap * 2, old_cap + need + 32);

    // We then make a tempory new buffer
    std::vector<char> new_buf(new_cap, '\0');

    // We then need to get the size of the left and right sides
    size_t left = left_len();
    size_t right = right_len();

    // We copy the left side to the beginning of the buffer
    if (left) {
        // Our destination is the new buffers data and we pass in the current
        // buffer data and the size of the left side
        std::memcpy(new_buf.data(), buf_.data(), left);
    }

    // We save the new gap begnning and end
    size_t new_gap_begin = left;
    size_t new_gap_end = new_cap - right;

    // We copy the right most data to the end of the buffer
    if (right) {
        // Our destination is the new buffer but we point up to the new gap end
        // and we pass in the current buffers data up to the end of the gap
        // the size is the length of the right most
        std::memcpy(new_buf.data() + new_gap_end, buf_.data() + gap_end_,
                    right);
    }

    // We use the swap member method to transfer over contents from the new
    // buffer to the member variable
    buf_.swap(new_buf);
    // We assign the member variables to the new gap beginning and end
    gap_begin_ = new_gap_begin;
    gap_end_ = new_gap_end;
    // We need to recalculate the cache
    cache_valid_ = false;
}

void GapBuffer::move_gap_to(size_t pos) {
    // If the position is at the beginning we don't move
    if (pos == gap_begin_) {
        return;
    }
    // We test if the position is less than the beginning
    if (pos < gap_begin_) {
        // Move block [pos, gap_begin_) to end side just before gap_end_
        size_t count = gap_begin_ - pos;
        // Destination starts at gap_end_ - count
        std::memmove(buf_.data() + (gap_end_ - count), buf_.data() + pos,
                     count);
        gap_begin_ = pos;
        gap_end_ -= count;
    } else {
        // pos > gap_begin_: move block [gap_end_, gap_end_ + count) down to
        // gap_begin_
        size_t count = pos - gap_begin_;
        std::memmove(buf_.data() + gap_begin_, buf_.data() + gap_end_, count);
        gap_begin_ += count;
        gap_end_ += count;
    }
    cache_valid_ = false;
}

// Helper method to cache the buffer contents into a string
void GapBuffer::compute_cache() const {
    // We empty the contents to start from scratch
    cached_str_.clear();
    // We reserve enough size given the size of the buffer
    cached_str_.reserve(size());
    // We then append the buffered contents to the left of the gap
    cached_str_.append(buf_.data(), gap_begin_);
    // We do the same for the right side of the gap
    cached_str_.append(buf_.data() + gap_end_, buf_.size() - gap_end_);
    // We have to null terminate the string
    cached_str_.push_back('\0');
    // Since our string is reconstructed this string is valid
    cache_valid_ = true;
}
