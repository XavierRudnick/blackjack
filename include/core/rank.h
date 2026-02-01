#ifndef RANK_H
#define RANK_H

#include <iostream>
#include <cstdint>

enum class Rank : uint8_t{
    Two,       // 0: enum auto fills in the rest of the vals +1 each time
    Three,     // 1
    Four,      // 2
    Five,      // 3
    Six,       // 4
    Seven,     // 5
    Eight,     // 6
    Nine,      // 7
    Ten,       // 8
    Jack,      // 9
    Queen,     // 10
    King,      // 11
    Ace         // 12
};

std::ostream& operator<<(std::ostream& os, const Rank rank);

#endif
