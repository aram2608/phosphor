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
// We need to overload the bitwise | or operator so we can
// Perform bitwise ops using our custom class
inline Mod operator|(Mod a, Mod b) {
    return Mod(std::uint8_t(a) | std::uint8_t(b));
}

// We do the same for the |= operator
inline Mod &operator|=(Mod &a, Mod b) {
    a = Mod(std::uint8_t(a) | std::uint8_t(b));
    return a;
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