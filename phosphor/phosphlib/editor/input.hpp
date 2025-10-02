#ifndef INPUT_HPP
#define INPUT_HPP

#include <cstdint>

// Enum class to contain the separate input types
enum class CommandType {
    InsertChar,
    BackSpace,
    MoveLeft,
    MoveRight,
    MoveUp,
    MoveDown,
    Save,
    Open,
    NewLine,
};

// Data type to represent the commands given
struct Command {
    CommandType type;
    uint32_t ch;
};

#endif