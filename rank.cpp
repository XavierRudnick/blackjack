#include "rank.h"

std::ostream& operator<<(std::ostream& os, const Rank rank){

    switch(rank)
        {
            case Rank::Two:
                os << "2";
                break;
            case Rank::Three:
                os << "3";
                break;
            case Rank::Four:
                os << "4";
                break;
            case Rank::Five:
                os << "5";
                break;
            case Rank::Six:
                os << "6";
                break;
            case Rank::Seven:
                os << "7";
                break;
            case Rank::Eight:
                os << "8";
                break;
            case Rank::Nine:
                os << "9";
                break;
            case Rank::Ten:
                os << "10";
                break;
            case Rank::Jack:
                os << "J";
                break;
            case Rank::Queen:
                os << "Q";
                break;
            case Rank::King:
                os << "K";
                break;
            case Rank::Ace:
                os << "A";
                break;
        }
    return os;
}
