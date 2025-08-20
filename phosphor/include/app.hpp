#pragma once
#include <raylib.h>

// Namespace for color pallete
namespace Palette {
    constexpr Color DarkBg      = { 0x0d, 0x0f, 0x0a, 255 };
    constexpr Color SoftMint    = { 0xb8, 0xff, 0xb8, 255 };
    constexpr Color BrightGreen = { 0x5c, 0xff, 0x5c, 255 };
    constexpr Color AquaMint    = { 0x7e, 0xf9, 0xd8, 255 };
    constexpr Color LightGreen  = { 0xa0, 0xff, 0xa0, 255 };
    constexpr Color ForestMoss  = { 0x77, 0xc7, 0x77, 255 };
    constexpr Color LimeYellow  = { 0xcc, 0xff, 0x66, 255 };
    constexpr Color DeepFern    = { 0x1a, 0x2a, 0x16, 255 };
}

// App class to contain resources used during exectution of program
class App {
    public:
        // Constructor
        App();
        // Deconstructor
        ~App();

        // Publically available resources
        Font font;
    private:
};