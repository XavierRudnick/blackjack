#include "RPCStrategy.h"
#include <cmath>

RPCStrategy::RPCStrategy(float deck_size){
    num_decks_left = deck_size;
}

int RPCStrategy::getBetSize() {
    if (true_count < 1){
        return 25;
    }
    else if (true_count <= 1.5) {
        return 100;
    } 
    else if (true_count < 3.0) { 
        return 300;
    } 
    else if (true_count < 4.0) {
        return 500;
    } 
    else if (true_count < 5.0) {
        return 1000;
    } 
    else if (true_count < 6.0) {
        return 1600;
    } 
    else if (true_count < 7.0) {
        return 2000;
    } 
    else {
        return 2000;
    }

    // //xav bet spread
    // if (true_count < 1){
    //     return 5;
    // }
    // else if (true_count <= 1.5) {
    //     return 20;
    // } 
    // else if (true_count < 3.0) { 
    //     return 30;
    // } 
    // else if (true_count < 4.0) {
    //     return 50;
    // } 
    // else if (true_count < 5.0) {
    //     return 100;
    // } 
    // else if (true_count < 6.0) {
    //     return 160;
    // } 
    // else if (true_count < 7.0) {
    //     return 200;
    // } 
    // else {
    //     return 200;
    // }

}

void RPCStrategy::updateCount(Card card) {
    int score = card.getValue();

    switch (score)
    {
    case 2:
        running_count += 1;
        break;
    case 3:
        running_count += 2;
        break;
    case 4:
        running_count += 2;
        break;
    case 5:
        running_count += 2;
        break;
    case 6: 
        running_count += 2;
        break;
    case 7:
        running_count += 1;
        break;
    case 8:
        running_count += 0;
        break;
    case 9:         
        running_count += 0;
        break;
    case 10:
        running_count -= 2;
        break;
    case 11: //Ace
        running_count -= 2;
        break;
    
    default:
        break;
    }

    float raw = running_count / num_decks_left;
    true_count = raw;
    return;
}

void RPCStrategy::updateDeckSize(int num_cards_left){
    num_decks_left = static_cast<float>(num_cards_left) / 52.0f;

    if (num_decks_left > 0) {
        float raw = running_count / num_decks_left;
        true_count = raw;
    }
    return;
}

float RPCStrategy::getTrueCount() const{
    return true_count;
}

float RPCStrategy::getRunningCount() const{
    return running_count;
}

float RPCStrategy::getDecksLeft() const{
    return num_decks_left;
}

bool RPCStrategy::shouldAcceptInsurance() const{
    constexpr int insuranceThreshold = 3; //mathmatical point where insurance is profitable accoding to gemini
    if (true_count >= insuranceThreshold){
        return true;
    }
    return false;
}

Action RPCStrategy::shouldDeviatefromHard(int playerTotal, Rank dealerUpcard, float trueCount){
    int dealerValue = BasicStrategy::getIndex(dealerUpcard) + INDEX_OFFSET;

    switch (playerTotal) {
        case 16:
            if (dealerValue == 10 && trueCount > 0) {
                return Action::Stand;
            }
            break;
            
        case 15: 
            if (dealerValue == 10 && trueCount >= 4) {
                return Action::Stand;
            }
            break;


        case 12:
            if (dealerValue == 3 && trueCount >= 2) {
                return Action::Stand;
            }
            if (dealerValue == 2 && trueCount >= 3) {
                return Action::Stand;
            }

            break;

        case 10:
            if (dealerValue == 11 && trueCount >= 4){
                return Action::Double;
            }
            break;

        default: return Action::Skip;
    }
    return Action::Skip;
}

Action RPCStrategy::shouldDeviatefromSplit(Rank playerRank, Rank dealerUpcard, float trueCount){
    int dealerValue = BasicStrategy::getIndex(dealerUpcard) + INDEX_OFFSET;
    int playerValue = BasicStrategy::getIndex(playerRank) + INDEX_OFFSET;
    switch (playerValue) {
        
        // Commented out, very obvious counting cards when you split on tens    
        // case 10: 
        //     if (dealerValue == 5 && trueCount >= 5) {
        //         return Action::Split;
        //     }
        //     if (dealerValue == 4 && trueCount >= 6) {
        //         return Action::Split;
        //     }
        //     if (dealerValue == 6 && trueCount >= 4) {
        //         return Action::Split;
        //     }
        //     break;
        default: return Action::Skip; break;
    }
    return Action::Skip;
}

Action RPCStrategy::shouldSurrender(int playerTotal, Rank dealerUpcard, float trueCount){
    int dealerValue = BasicStrategy::getIndex(dealerUpcard) + INDEX_OFFSET;
    switch (playerTotal) {
        case 17:
            if (dealerValue == 11 && trueCount >= 0) {
                return Action::Surrender;
            }
            break;
        case 16:
            if (dealerValue == 10 && trueCount >= 0) {
                return Action::Surrender;
            }
            if (dealerValue == 11 && trueCount >= 3) {
                return Action::Surrender;
            }
            break;
        case 15:
            if (dealerValue == 10 && trueCount >= 0) {
                return Action::Surrender;
            }
            if (dealerValue == 11 && trueCount >= 1) {
                return Action::Surrender;
            }
            if (dealerValue == 9 && trueCount >= 2) {
                return Action::Surrender;
            }
            break;
        case 14:
            if (dealerValue == 11 && trueCount >= 3) {
                return Action::Surrender;
            }
            break;
        default: return Action::Skip; break;
    }
    return Action::Skip;
}

Action RPCStrategy::getHardHandAction(int playerTotal, Rank dealerUpcard, float trueCount) {
    constexpr int lowerBound = 5;
    constexpr int upperBound = 20;
    if (playerTotal < lowerBound) {
        return Action::Hit;
    }

    if (playerTotal > upperBound) {
        return Action::Stand;
    }
    
    int dealerIdx = BasicStrategy::getIndex(dealerUpcard);
    
    int playerIdx = playerTotal - lowerBound;  // Player total 5 maps to index 0 Since chart starts at 5
    Action deviation = shouldDeviatefromHard(playerTotal, dealerUpcard, trueCount);
    if (deviation != Action::Skip) {
        return deviation;
    }
    else{
        return BasicStrategy::hardTotalTable[playerIdx][dealerIdx];
    }
    
}

Action RPCStrategy::getSoftHandAction(int playerTotal, Rank dealerUpcard) {
    constexpr int lowerBound = 13;
    int dealerIdx = BasicStrategy::getIndex(dealerUpcard);
    int playerIdx = playerTotal - lowerBound;  // Soft 13 maps to index 0 Since chart starts at A,2
    
    Action action = BasicStrategy::softTotalTable[playerIdx][dealerIdx];

    return action;
}

Action RPCStrategy::getSplitAction(Rank playerSplitRank, Rank dealerUpcard, float trueCount) {
    int dealerIdx = BasicStrategy::getIndex(dealerUpcard);
    int pairIdx = BasicStrategy::getIndex(playerSplitRank);
    Action deviation = shouldDeviatefromSplit(playerSplitRank, dealerUpcard, trueCount);
    if (deviation != Action::Skip) {
        return deviation;
    }
    else{
        return BasicStrategy::splitTable[pairIdx][dealerIdx];
    }
}

void RPCStrategy::reset(int deckSize){
    running_count = 0;
    true_count = 0;
    num_decks_left = deckSize;
}

std::string RPCStrategy::getName() {
    return "RPCStrategy";
}