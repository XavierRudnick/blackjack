#ifndef RANK_H
#define RANK_H

#include <iostream>
#include <cstdint>

enum class Rank : uint8_t{
    Two,       // enum auto fills in the rest of the vals +1 each time
    Three,
    Four,
    Five,
    Six,
    Seven,
    Eight,
    Nine,
    Ten,
    Jack,      
    Queen,
    King,
    Ace         
};

std::ostream& operator<<(std::ostream& os, const Rank rank);

#endif
