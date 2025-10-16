#ifndef UI_HPP
#define UI_HPP

#include "palette/palette.hpp"
#include <array>

#include <raylib.h>

struct UI {
    UI();
    ~UI();
    const Rectangle frame_{10, 10, 780, 780};
    const Vector2 title_pos_{40, 25};
    const Vector2 fn_pos_{600, 25};
    const Vector2 header_ln_strt_{25, 60};
    const Vector2 header_ln_end_{775, 60};
    void draw_ui() const;
    void draw_bg() const;
    void draw_header() const;
    bool choose_pallete();
    void dispatch_pallete();
    void phosphor_green();
    void phosphor_amber();
    Font title_font_;
    Color title_color_;
    Color text_color_;
    Color ui_color_;
    Color bg_color_;
    Pallete pallete_{Pallete::Green};
    const char *title_;
};

#endif