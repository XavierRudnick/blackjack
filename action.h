#ifndef ACTION_H
#define ACTION_H

#include <iostream>
#include <cstdint>

enum class Action : uint8_t {
    Stand,
    Hit,
    Double,
    Split
};

std::ostream& operator<<(std::ostream& os, const Action action);

#endif
