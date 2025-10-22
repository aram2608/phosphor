#include "ui/ui.hpp"
#include "ui.hpp"

// UI Constructor
UI::UI() {
    // We need to load in our font
    title_font_ = LoadFont(
        "JetBrainsMono-2.304/fonts/ttf/JetBrainsMono-ExtraBoldItalic.ttf");
    text_font_ =
        LoadFont("JetBrainsMono-2.304/fonts/ttf/JetBrainsMono-Medium.ttf");
}

// Destructor - We need to offload the font resources
UI::~UI() {
    UnloadFont(title_font_);
    UnloadFont(text_font_);
}

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

// Method to draw the buffer contents onto the screen
void UI::draw_buffer(const char *str) const {
    DrawTextEx(text_font_, str, buffer_pos_, 20.0f, 2, text_color_);
}

// Method to draw the filename to the screen
void UI::draw_fn(const char *fn) const {
    DrawTextEx(title_font_, fn, fn_pos_, 30.0f, 2, title_color_);
}

// Method to draw the rename state to screen
void UI::draw_rename_fn(const char *fn) const {
    DrawTextEx(text_font_, "Renaming: ", rename_pos_, 25.0f, 2, title_color_);
    DrawTextEx(text_font_, fn, fn_pos_, 25.0f, 2, title_color_);
}

// We chose the color palette given the palette type
void UI::dispatch_palette() {
    // We need to create an alias for a pointer to one of the UI member
    // functions so we can make an array of the functions we dispatch to
    using ColorChooser = void (UI::*)();
    static std::array<ColorChooser, static_cast<std::size_t>(Palette::Count)>
        TABLE = {&UI::phosphor_green, &UI::phosphor_amber,
                 &UI::phosphor_blue,  &UI::phosphor_red,
                 &UI::phosphor_cyan,  &UI::phosphor_magenta,
                 &UI::phosphor_white};
    // We dispatch to the relevant function
    (this->*TABLE[static_cast<std::size_t>(palette_)])();
}

// Helper to choose green color palette
void UI::phosphor_green() {
    title_color_ = PhosphorGreen::LightGreen;
    ui_color_ = PhosphorGreen::SoftGreen;
    bg_color_ = PhosphorGreen::DarkBg;
    text_color_ = PhosphorGreen::DarkGreen;
}

// Helper to choose amber color palette
void UI::phosphor_amber() {
    title_color_ = PhosphorAmber::PaleAmber;
    ui_color_ = PhosphorAmber::SoftAmber;
    bg_color_ = PhosphorAmber::DarkBg;
    text_color_ = PhosphorAmber::LightAmber;
}

// Helper to choose blue color palette
void UI::phosphor_blue() {
    title_color_ = PhosphorBlue::SoftBlue;
    ui_color_ = PhosphorBlue::MidBlue;
    bg_color_ = PhosphorBlue::DarkBg;
    text_color_ = PhosphorBlue::DeepBlue;
}

// Helper to choose red color palette
void UI::phosphor_red() {
    title_color_ = PhosphorRed::BrightRed;
    ui_color_ = PhosphorRed::MidRed;
    bg_color_ = PhosphorRed::DarkBg;
    text_color_ = PhosphorRed::DarkCrimson;
}

// Helper to choose cyan color palette
void UI::phosphor_cyan() {
    title_color_ = PhosphorCyan::AquaGlow;
    ui_color_ = PhosphorCyan::MidTeal;
    bg_color_ = PhosphorCyan::DarkBg;
    text_color_ = PhosphorCyan::DeepOcean;
}

// Helper to choose magenta color palette
void UI::phosphor_magenta() {
    title_color_ = PhosphorMagenta::BrightFuchsia;
    ui_color_ = PhosphorMagenta::MidPink;
    bg_color_ = PhosphorMagenta::DarkBg;
    text_color_ = PhosphorMagenta::DeepViolet;
}

// Helper to choose white/monochrome color palette
void UI::phosphor_white() {
    title_color_ = PhosphorWhite::SoftWhite;
    ui_color_ = PhosphorWhite::LightGrey;
    bg_color_ = PhosphorWhite::DarkBg;
    text_color_ = PhosphorWhite::MidGrey;
}