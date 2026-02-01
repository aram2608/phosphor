#ifndef KEYCHORDS_HPP
#define KEYCHORDS_HPP

#include <cstdint>
#include <functional>
#include <unordered_map>

// We create a small enum that inherits from the uint_8
// This lets us create a bit mask for assinging modifer keys
// We use the << to bitshift in order to create our flags
enum Mod : uint8_t {
    // 0000
    MOD_NONE = 0,
    // 0001
    MOD_CTRL = 1 << 0,
    // 0010
    MOD_SHIFT = 1 << 1,
    // 0100
    MOD_ALT = 1 << 2,
    // 1000
    MOD_SUPER = 1 << 3,
};

// We need to overload the bitwise operators so we can
// Perform bitwise ops using our custom class

// Overload to | our bit mask
// 0100, 0010, then 0110 is kept and both bits are turned on
inline Mod operator|(Mod a, Mod b) { return Mod(uint8_t(a) | uint8_t(b)); }

// Overload to |= our bit mask
// Same as above but it modifies the left hand value
inline Mod &operator|=(Mod &a, Mod b) {
    a = a | b;
    return a;
}

// Overload to keep bits that are 1 in both operands
// 0101 0100, only 0100 is kept
inline Mod operator&(Mod a, Mod b) { return Mod(uint8_t(a) & uint8_t(b)); }

// Same as above but modifies left hand value
inline Mod &operator&=(Mod &a, Mod b) {
    a = a & b;
    return a;
}

// Overload to keep bits that are 1 in only one operand
// 0110, 0100, then 0010 is kept, only if exactly one side has 1
inline Mod operator^(Mod a, Mod b) { return Mod(uint8_t(a) ^ uint8_t(b)); }

// Same as above but modifies left hand value
inline Mod &operator^=(Mod &a, Mod b) {
    a = a ^ b;
    return a;
}

// Overload to flip bits
// 1111 becomes 0000
inline Mod operator~(Mod a) { return Mod(~uint8_t(a)); }

struct KeyChord {
    int key;
    Mod mods;
    bool operator==(const KeyChord &o) const;
};

struct KeyChordHash {
    std::size_t operator()(const KeyChord &c) const;
};

#endif