// retro_theme.hpp

/*
Theme idea i got from chatGPT, a retro style green or amber
hope they work
*/

#pragma once
#include <ftxui/dom/elements.hpp>
using namespace ftxui;

struct RetroTheme {
  Color bg, fg, kw, str, num, com, cursor, sel_bg;
};

inline RetroTheme GreenPhosphor() {
  return {
    Color::RGB(0x0d,0x0f,0x0a),
    Color::RGB(0xb8,0xff,0xb8),
    Color::RGB(0x5c,0xff,0x5c),
    Color::RGB(0x7e,0xf9,0xd8),
    Color::RGB(0xa0,0xff,0xa0),
    Color::RGB(0x77,0xc7,0x77),
    Color::RGB(0xcc,0xff,0x66),
    Color::RGB(0x1a,0x2a,0x16),
  };
}

inline RetroTheme AmberTerminal() {
  return {
    Color::RGB(0x0b,0x09,0x07),
    Color::RGB(0xff,0xdd,0x99),
    Color::RGB(0xff,0xb3,0x47),
    Color::RGB(0xff,0xd2,0x7f),
    Color::RGB(0xff,0xc2,0x66),
    Color::RGB(0xb8,0x8f,0x5a),
    Color::RGB(0xff,0xf2,0xb3),
    Color::RGB(0x26,0x1e,0x12),
  };
}

// Simple token kinds for demo
enum class Tok { Plain, Keyword, String, Number, Comment };

// Style helper
inline Decorator deco_for(Tok t, const RetroTheme& T) {
  switch (t) {
    case Tok::Keyword: return color(T.kw) | bold;
    case Tok::String:  return color(T.str);
    case Tok::Number:  return color(T.num);
    case Tok::Comment: return color(T.com) | dim;
    default:           return color(T.fg);
  }
}

// Example line renderer (replace with your tokenizer)
inline Element render_line(const std::vector<std::pair<Tok,std::string>>& parts,
                           const RetroTheme& T) {
  std::vector<Element> segs;
  segs.reserve(parts.size());
  for (auto& [tok, text] : parts)
    segs.push_back(text | deco_for(tok, T));
  return hbox(std::move(segs));
}

// Background & selection wrapper
inline Element editor_frame(Element content, const RetroTheme& T, bool selected=false) {
  auto bgcol = selected ? bgcolor(T.sel_bg) : bgcolor(T.bg);
  return window(text(""), content) | bgcol; // borderless feel
}

// Cursor decorator (block cursor)
inline Element cursor_overlay(int x, const RetroTheme& T) {
  // Draw a 1-char block. Integrate with your layout to place at (x, row).
  return text(" ") | bgcolor(T.cursor);
}
