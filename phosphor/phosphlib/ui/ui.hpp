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
    const Vector2 header_ln_strt_{10, 60};
    const Vector2 header_ln_end_{790, 60};
    void draw_ui() const;
    void draw_bg() const;
    void draw_header() const;
    void dispatch_palette();
    void phosphor_green();
    void phosphor_amber();
    void phosphor_blue();
    void phosphor_red();
    void phosphor_cyan();
    void phosphor_magenta();
    void phosphor_white();
    Font title_font_;
    Color title_color_{PhosphorGreen::LightGreen};
    Color text_color_{PhosphorGreen::DarkGreen};
    Color ui_color_{PhosphorGreen::SoftGreen};
    Color bg_color_{PhosphorGreen::DarkBg};
    Palette palette_{Palette::Green};
    const char *title_{"phosphor"};
};

#endif