#ifndef UI_HPP
#define UI_HPP

#include "palette.hpp"
#include <array>
#include <iostream>
#include <vector>

#include "../vendor/raylib.h"

struct UI {
    UI();
    ~UI();
    const Rectangle frame_{10, 10, 1180, 780};
    const Vector2 title_pos_{40, 25};
    const Vector2 fn_pos_{900, 25};
    const Vector2 header_ln_strt_{10, 60};
    const Vector2 header_ln_end_{1190, 60};
    const Vector2 rename_pos_{700, 25};
    const Vector2 buffer_pos_{60, 70};
    const float line_idx_xpos_{15};
    const float text_size_{20.0f};
    const float header_size_{30.0f};
    const float text_spacing_{2.0f};
    void draw_ui() const;
    void draw_bg() const;
    void draw_header() const;
    void draw_buffer(const char *str) const;
    void draw_fn(const char *fn) const;
    void draw_rename_fn(const char *fn) const;
    void draw_cursor() const;
    void dispatch_palette();
    void phosphor_green() noexcept;
    void phosphor_amber() noexcept;
    void phosphor_blue() noexcept;
    void phosphor_red() noexcept;
    void phosphor_cyan() noexcept;
    void phosphor_magenta() noexcept;
    void phosphor_white() noexcept;
    Font title_font_;
    Font text_font_;
    Color title_color_{PhosphorGreen::LightGreen};
    Color text_color_{PhosphorGreen::DarkGreen};
    Color ui_color_{PhosphorGreen::SoftGreen};
    Color bg_color_{PhosphorGreen::DarkBg};
    Palette palette_{Palette::Green};
    const char *title_{"phosphor\0"};
};

#endif