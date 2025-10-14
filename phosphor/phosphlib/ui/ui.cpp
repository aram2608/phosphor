#include "ui/ui.hpp"
#include "ui.hpp"

UI::UI() : title_("phsophor") {
    title_font_ = LoadFont(
        "JetBrainsMono-2.304/fonts/ttf/JetBrainsMono-ExtraBoldItalic.ttf");
}

const void UI::draw_ui() {
    draw_bg();
    draw_header();
}

// Simple helper to draw background
const void UI::draw_bg() { ClearBackground(PhosphorGreen::DarkBg); }

const void UI::draw_header() {
    DrawRectangleRoundedLinesEx(frame_, 0.18f, 20, 2, PhosphorGreen::SoftMint);
    DrawLineEx(header_ln_strt_, header_ln_end_, 3, PhosphorGreen::SoftMint);
    DrawTextEx(title_font_, title_, title_pos_, 25.0f, 2,
               PhosphorGreen::ForestMoss);
}