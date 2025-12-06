#ifndef SUIT_H
#define SUIT_H

#include <iostream>
#include <cstdint>

enum class Suit : uint8_t{
    Spades ,
    Clubs,
    Hearts,
    Diamonds
};

std::ostream& operator<<(std::ostream& os, const Suit suit);

#endif
