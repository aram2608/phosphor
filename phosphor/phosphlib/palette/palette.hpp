#ifndef PALETTE_HPP
#define PALETTE_HPP
#include <raylib.h>

enum class Pallete { Green, Amber, Count };

struct PhosphorGreen {
    static constexpr Color DarkBg = {0x0d, 0x0f, 0x0a, 255};
    static constexpr Color SoftMint = {0xb8, 0xff, 0xb8, 255};
    static constexpr Color BrightGreen = {0x5c, 0xff, 0x5c, 255};
    static constexpr Color AquaMint = {0x7e, 0xf9, 0xd8, 255};
    static constexpr Color LightGreen = {0xa0, 0xff, 0xa0, 255};
    static constexpr Color ForestMoss = {0x77, 0xc7, 0x77, 255};
    static constexpr Color LimeYellow = {0xcc, 0xff, 0x66, 255};
    static constexpr Color DeepFern = {0x1a, 0x2a, 0x16, 255};
};

struct PhosphorAmber {
    static constexpr Color DarkBg = {0x0b, 0x09, 0x07, 255};
    static constexpr Color SoftAmber = {0xff, 0xdd, 0x99, 255};
    static constexpr Color BrightAmber = {0xff, 0xb3, 0x47, 255};
    static constexpr Color LightAmber = {0xff, 0xd2, 0x7f, 255};
    static constexpr Color PaleAmber = {0xff, 0xc2, 0x66, 255};
    static constexpr Color AmberBrown = {0xb8, 0x8f, 0x5a, 255};
    static constexpr Color AmberYellow = {0xff, 0xf2, 0xb3, 255};
    static constexpr Color DeepAmber = {0x26, 0x1e, 0x12, 255};
};

#endif