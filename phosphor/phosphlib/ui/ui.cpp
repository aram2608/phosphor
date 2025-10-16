#include "ui/ui.hpp"

// UI Constructor
UI::UI() {
    // We need to load in our font
    title_font_ = LoadFont(
        "JetBrainsMono-2.304/fonts/ttf/JetBrainsMono-ExtraBoldItalic.ttf");
}

// Destructor - We need to offload the font resources
UI::~UI() { UnloadFont(title_font_); }

// Wrapper method to draw UI components
void UI::draw_ui() const {
    draw_bg();
    draw_header();
}

// Simple helper to draw background
void UI::draw_bg() const { ClearBackground(bg_color_); }

// Wrapper function to draw the header
void UI::draw_header() const {
    DrawRectangleRoundedLinesEx(frame_, 0.05f, 20, 2, ui_color_);
    DrawLineEx(header_ln_strt_, header_ln_end_, 3, ui_color_);
    DrawTextEx(title_font_, title_, title_pos_, 30.0f, 2, title_color_);
}

// We chose the color pallete given the pallete type
void UI::dispatch_pallete() {
    // We need to create an alias for a pointer to one of the UI member
    // functions so we can make an array of the functions we dispatch to
    using ColorChooser = void (UI::*)();
    static std::array<ColorChooser, 7> TABLE = {
        &UI::phosphor_green, &UI::phosphor_amber, &UI::phosphor_blue,
        &UI::phosphor_red,   &UI::phosphor_cyan,  &UI::phosphor_magenta,
        &UI::phosphor_white};
    // We dispatch to the relevant function
    (this->*TABLE[static_cast<std::size_t>(pallete_)])();
}

// Helper to choose green color pallete
void UI::phosphor_green() {
    title_color_ = PhosphorGreen::LightGreen;
    ui_color_ = PhosphorGreen::SoftGreen;
    bg_color_ = PhosphorGreen::DarkBg;
    text_color_ = PhosphorGreen::DarkGreen;
}

// Helper to choose amber color pallete
void UI::phosphor_amber() {
    title_color_ = PhosphorAmber::PaleAmber;
    ui_color_ = PhosphorAmber::SoftAmber;
    bg_color_ = PhosphorAmber::DarkBg;
    text_color_ = PhosphorAmber::LightAmber;
}

// Helper to choose blue color pallete
void UI::phosphor_blue() {
    title_color_ = PhosphorBlue::SoftBlue;
    ui_color_ = PhosphorBlue::MidBlue;
    bg_color_ = PhosphorBlue::DarkBg;
    text_color_ = PhosphorBlue::DeepBlue;
}

// Helper to choose red color pallete
void UI::phosphor_red() {
    title_color_ = PhosphorRed::BrightRed;
    ui_color_ = PhosphorRed::MidRed;
    bg_color_ = PhosphorRed::DarkBg;
    text_color_ = PhosphorRed::DarkCrimson;
}

// Helper to choose cyan color pallete
void UI::phosphor_cyan() {
    title_color_ = PhosphorCyan::AquaGlow;
    ui_color_ = PhosphorCyan::MidTeal;
    bg_color_ = PhosphorCyan::DarkBg;
    text_color_ = PhosphorCyan::DeepOcean;
}

// Helper to choose magenta color pallete
void UI::phosphor_magenta() {
    title_color_ = PhosphorMagenta::BrightFuchsia;
    ui_color_ = PhosphorMagenta::MidPink;
    bg_color_ = PhosphorMagenta::DarkBg;
    text_color_ = PhosphorMagenta::DeepViolet;
}

// Helper to choose white/monochrome color pallete
void UI::phosphor_white() {
    title_color_ = PhosphorWhite::SoftWhite;
    ui_color_ = PhosphorWhite::LightGrey;
    bg_color_ = PhosphorWhite::DarkBg;
    text_color_ = PhosphorWhite::MidGrey;
}