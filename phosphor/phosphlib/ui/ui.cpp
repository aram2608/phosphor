#include "ui/ui.hpp"
#include "ui.hpp"

UI::UI() : title_("phosphor") {
    title_font_ = LoadFont(
        "JetBrainsMono-2.304/fonts/ttf/JetBrainsMono-ExtraBoldItalic.ttf");
    dispatch_pallete();
}

UI::~UI() { UnloadFont(title_font_); }

void UI::draw_ui() const {
    draw_bg();
    draw_header();
}

// Simple helper to draw background
void UI::draw_bg() const { ClearBackground(bg_color_); }

// Wrapper function to draw the header
void UI::draw_header() const {
    DrawRectangleRoundedLinesEx(frame_, 0.18f, 20, 2, ui_color_);
    DrawLineEx(header_ln_strt_, header_ln_end_, 3, ui_color_);
    DrawTextEx(title_font_, title_, title_pos_, 25.0f, 2, title_color_);
}

void UI::dispatch_pallete() {
    using ColorChooser = void (UI::*)();
    static std::array<ColorChooser, 2> TABLE = {&UI::phosphor_green,
                                                &UI::phosphor_amber};
    (this->*TABLE[static_cast<std::size_t>(pallete_)])();
}

// Helper to choose green color pallete
void UI::phosphor_green() {
    title_color_ = PhosphorGreen::ForestMoss;
    ui_color_ = PhosphorGreen::SoftMint;
    bg_color_ = PhosphorGreen::DarkBg;
    text_color_ = PhosphorGreen::ForestMoss;
}

// Helper to choose amber color pallete
void UI::phosphor_amber() {
    title_color_ = PhosphorAmber::PaleAmber;
    ui_color_ = PhosphorAmber::SoftAmber;
    bg_color_ = PhosphorAmber::DarkBg;
    text_color_ = PhosphorAmber::LightAmber;
}