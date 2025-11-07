#include <iostream>
#include <random>
#include <vector>

using namespace std;

enum class Rank : uint8_t{
    Two = 0,    // enum auto fills in the rest of the vals +1 each time
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

enum class Suit : uint8_t{
    Spades = 0,
    Clubs,
    Hearts,
    Diamonds
}

class Card{
    private:
        const Rank rank_;
        const Suit suit_;
    public:
        Card(Rank rank, Suit suit){
            rank_ = rank;
            suit_ = suit;
        }

        Rank getRank(){
            return rank_;
        }

        Suit getSuit(){
            return suit_;
        }
}

class Hand{
    private:
        std::vector<Card> hand;


    public:
        Hand(Card first, Card second){
            hand.emplace_back(first);
            hand.emplace_back(second);
        }

        int getScoreHard(){
            int score = 0;

            for (Card val : hand){

                Rank rank = val.getRank();
                if (rank == Rank::Ace){
                    score += 11;
                }
                else if (rank == Rank::Jack || rank == Rank::Queen || rank == Rank::King){
                    score += 10;
                }
                else{
                    score = static_cast<int>(rank) + 2
                }

            }
            return score;
        }

        int isSoftHand(){
            int score = 0;
            for (Card val : hand){

                Rank rank = val.getRank();
                if (rank == Rank::Ace){
                    score += 1;
                }
                else if (rank == Rank::Jack || rank == Rank::Queen || rank == Rank::King){
                    score += 10;
                }
                else{
                    score = static_cast<int>(rank) + 2
                }

            }
        }



}
 
class Deck{
    private:
        const int card_val[13] = {2,3,4,5,6,7,8,9,10,10,10,11};
        int deck_size;
        int card_counts[13] = {0};
        const uint8_t NUM_RANK = 13;
        const uint8_t NUM_SUIT = 4;
        std::vector<Card> deck;
        std::mt19937 rand;

    public:
        Deck(int deck_size){
            deck.reserve(num_decks * 52); // Pre-allocate memory to avoid reallocations

            for(int i = 0; i < deck_size; i++){
                for(int rank = Two; rank < NUM_RANK; rank++){
                    for(int suit = Spades; suit < NUM_SUIT; suit++){
                        deck.emplace_back(Card(static_cast<Rank>(rank),static_cast<Suit>(suit))) // use emplace back to construct in place, o(1)
                    }
                }
            }

            rand.seed(std::random_device{}());
            std::shuffle(deck.begin(), deck.end(), rand);
        }

        void deal(){
            Card first = deck.back();
            deck.pop_back();
            Card second = deck.back();
            deck.pop_back();
            Hand(first, second);
        }
}


int main(){
    return 0;
}