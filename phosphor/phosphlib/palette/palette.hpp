#ifndef PALETTE_HPP
#define PALETTE_HPP
#include <raylib.h>

enum class Palette { Green, Amber, Blue, Red, Cyan, Magenta, White, Count };

struct PhosphorGreen {
    static constexpr Color DarkBg = {0x0d, 0x0f, 0x0a, 255};
    static constexpr Color SoftGreen = {0xb8, 0xff, 0xb8, 255};
    static constexpr Color DarkGreen = {0x77, 0xc7, 0x77, 255};
    static constexpr Color LightGreen = {0xae, 0xe8, 0xae, 255};
};

struct PhosphorAmber {
    static constexpr Color DarkBg = {0x0b, 0x09, 0x07, 255};
    static constexpr Color SoftAmber = {0xff, 0xdd, 0x99, 255};
    static constexpr Color LightAmber = {0xff, 0xd2, 0x7f, 255};
    static constexpr Color PaleAmber = {0xff, 0xc2, 0x66, 255};
};

struct PhosphorBlue {
    static constexpr Color DarkBg = {0x0a, 0x0a, 0x14, 255};
    static constexpr Color SoftBlue = {0xbb, 0xbb, 0xff, 255};
    static constexpr Color MidBlue = {0x77, 0x77, 0xcc, 255};
    static constexpr Color DeepBlue = {0x44, 0x44, 0x99, 255};
};

struct PhosphorRed {
    static constexpr Color DarkBg = {0x14, 0x0a, 0x0a, 255};
    static constexpr Color BrightRed = {0xff, 0x88, 0x88, 255};
    static constexpr Color MidRed = {0xcc, 0x44, 0x44, 255};
    static constexpr Color DarkCrimson = {0x99, 0x22, 0x22, 255};
};

struct PhosphorCyan {
    static constexpr Color DarkBg = {0x0a, 0x10, 0x10, 255};
    static constexpr Color AquaGlow = {0x88, 0xff, 0xff, 255};
    static constexpr Color MidTeal = {0x44, 0xbb, 0xbb, 255};
    static constexpr Color DeepOcean = {0x22, 0x88, 0x88, 255};
};

struct PhosphorMagenta {
    static constexpr Color DarkBg = {0x0c, 0x07, 0x0c, 255};
    static constexpr Color BrightFuchsia = {0xff, 0x99, 0xff, 255};
    static constexpr Color MidPink = {0xcc, 0x66, 0xcc, 255};
    static constexpr Color DeepViolet = {0x99, 0x33, 0x99, 255};
};

struct PhosphorWhite {
    static constexpr Color DarkBg = {0x0d, 0x0d, 0x0d, 255};
    static constexpr Color SoftWhite = {0xee, 0xee, 0xee, 255};
    static constexpr Color LightGrey = {0xaa, 0xaa, 0xaa, 255};
    static constexpr Color MidGrey = {0x77, 0x77, 0x77, 255};
};

#endif