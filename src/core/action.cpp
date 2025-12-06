#include "action.h"

std::ostream& operator<<(std::ostream& os, const Action action){

    switch(action)
        {
            case Action::Hit:
                os << "Hit";
                break;
            case Action::Stand:
                os << "Stand";
                break;
            case Action::Double:
                os << "Double";
                break;
            case Action::Split:
                os << "Split";
                break;
            case Action::Skip:
                os << "Skip";
                break;
            case Action::Surrender:
                os << "Surrender";
                break;
        }
    return os;
}
