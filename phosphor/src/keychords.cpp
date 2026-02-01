#include "../include/keychords.hpp"

bool KeyChord::operator==(const KeyChord &o) const {
    return key == o.key && mods == o.mods;
}

std::size_t KeyChordHash::operator()(const KeyChord &c) const {
    return std::hash<int>{}(c.key) ^ (std::hash<int>{}(int(c.mods)) << 1);
}