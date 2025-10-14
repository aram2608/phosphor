#ifndef UI_HPP
#define UI_HPP

#include "palette/palette.hpp"

#include <raylib.h>

struct UI {
    UI();
    ~UI();
    const Rectangle frame_{10, 10, 780, 780};
    const Vector2 title_pos_{40, 25};
    const Vector2 fn_pos_{600, 25};
    const Vector2 header_ln_strt_{25, 60};
    const Vector2 header_ln_end_{775, 60};
    const void draw_ui();
    const void draw_bg();
    const void draw_header();
    Font title_font_;
    const char *title_;
};

#endif