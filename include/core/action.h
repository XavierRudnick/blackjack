#ifndef ACTION_H
#define ACTION_H

#include <iostream>
#include <cstdint>

enum class Action : int {
    Stand,
    Hit,
    Double,
    Split,
    Skip,
    Surrender
};

std::ostream& operator<<(std::ostream& os, const Action action);

#endif
