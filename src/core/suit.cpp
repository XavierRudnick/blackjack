#include "suit.h"

std::ostream& operator<<(std::ostream& os, const Suit suit){

    switch(suit)
        {
            case Suit::Spades:
                os << "Spades";
                break;
            case Suit::Clubs:
                os << "Clubs";
                break;
            case Suit::Diamonds:
                os << "Diamonds";
                break;
            case Suit::Hearts:
                os << "Hearts";
                break;
        }
    return os;
}
