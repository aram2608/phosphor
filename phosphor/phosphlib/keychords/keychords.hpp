#ifndef KEYCHORDS_HPP
#define KEYCHORDS_HPP

#include <cstdint>
#include <functional>

// We create a small enum that inherits from the uint_8
// This lets us create a bit mask for assinging modifer keys
// We use the << to bitshift in order to create our flags
enum Mod : std::uint8_t {
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
inline Mod operator|(Mod a, Mod b) {
    return Mod(std::uint8_t(a) | std::uint8_t(b));
}

// Overload to |= our bit mask
// Same as above but it modifies the left hand value
inline Mod& operator|=(Mod& a, Mod b) {
    a = a | b;
    return a;
}

// Overload to keep bits that are 1 in both operands
// 0101 0100, only 0100 is kept
inline Mod operator&(Mod a, Mod b) {
    return Mod(std::uint8_t(a) & std::uint8_t(b));
}

// Same as above but modifies left hand value
inline Mod& operator&=(Mod& a, Mod b) {
    a = a & b;
    return a;
}

// Overload to keep bits that are 1 in only one operand
// 0110, 0100, then 0010 is kept, only if exactly one side has 1
inline Mod operator^(Mod a, Mod b) {
    return Mod(std::uint8_t(a) ^ std::uint8_t(b));
}

// Same as above but modifies left hand value
inline Mod& operator^=(Mod& a, Mod b) {
    a = a ^ b;
    return a;
}

// Overload to flip bits
// 1111 becomes 0000
inline Mod operator~(Mod a) {
    return Mod(~std::uint8_t(a));
}

// We create a small POD struct to store the key type and whether
// it is modified or not
struct KeyChord {
    int key;
    Mod mods;
    // We need to overload the equality operator so we can test for the
    // presence of a given key in our keymap
    bool operator==(const KeyChord &o) const {
        return key == o.key && mods == o.mods;
    }
};

// Since we are stuffing everything into a hash map, we need to create a hashing
// functor for the KeyChords struct
struct KeyChordHash {
    std::size_t operator()(const KeyChord &c) const {
        // The internal hashing funtions used for ints are quite good so we can
        // simply use them on the undrelying enums
        // The ^ XOR bitwise operator is standard for combining two hash values
        // and we bit shift once to the left since it reduces hash collisions
        // apparently
        return std::hash<int>{}(c.key) ^ (std::hash<int>{}(int(c.mods)) << 1);
    }
};

#endif