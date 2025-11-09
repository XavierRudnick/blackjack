#include <iostream>
#include <random>
#include <vector>
#include <algorithm>
#include <optional>

using namespace std;

enum class Rank : uint8_t{
    Two,    // enum auto fills in the rest of the vals +1 each time
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
    Spades ,
    Clubs,
    Hearts,
    Diamonds
};

ostream& operator<<(ostream& os, const Rank rank){

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

ostream& operator<<(ostream& os, const Suit suit){

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

class Card{
    private:
        Rank rank_ = Rank::Ace;
        Suit suit_ = Suit::Spades;
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
};

class Hand{
    private:
        vector<Card> hand;

    public:
        Hand(pair <Card,Card> cards){
            hand.emplace_back(cards.first);
            hand.emplace_back(cards.second);
            
        }

        Hand(Card card){
            hand.emplace_back(card);
        }

        Card get_second_card(){
            return hand.back();
        }

        void pop_second_card(){
            hand.pop_back();
            return;
        }

        void show_cards(){
            for (Card val : hand){
                cout << val.getRank() << " " << val.getSuit() << endl;
            }
            cout << "value: " << getScoreHard() << endl;
        }

        void peek_dealer(){
            cout << hand.front().getRank() << " " << hand.front().getSuit() << endl;
        }

        bool check_can_split(){
            if (hand.size() == 2 && hand.front().getRank() == hand.back().getRank()){
                return true;
            }
            return false;
        } 
        
        void addCard(Card card){
            hand.emplace_back(card);
        }

        bool check_over(){
            if (getScoreHard() > 21){
                return true;
            }
            return false;
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
                    score += static_cast<int>(rank) + 2;
                }

            }
            return score;
        }

        int getScoreSoft(){
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
                    score = static_cast<int>(rank) + 2;
                }

            }
            return score;
        }

        bool isSoftHand(){
            for (Card val : hand){
                Rank rank = val.getRank();
                if (rank == Rank::Ace){
                    return true;
                }
            }
        }
};
 
class Deck{
    private:
        const uint8_t NUM_RANK = 13;
        const uint8_t NUM_SUIT = 4;
        vector<Card> deck;
        mt19937 rand;

    public:
        int deck_size;
        Deck(uint8_t deck_size){
            deck.reserve(deck_size * 52); // Pre-allocate memory to avoid reallocations

            for(int i = 0; i < deck_size; i++){
                for(int rank = static_cast<int>(Rank::Two); rank < NUM_RANK; rank++){
                    for(int suit = static_cast<int>(Suit::Spades); suit < NUM_SUIT; suit++){
                        deck.emplace_back(Card(static_cast<Rank>(rank),static_cast<Suit>(suit))); // use emplace back to construct in place, o(1)
                    }
                }
            }

            rand.seed(std::random_device{}());
            shuffle(deck.begin(), deck.end(), rand);
        }

        pair <Card,Card> deal(){
            Card first = deck.back();
            deck.pop_back();
            Card second = deck.back();
            deck.pop_back();
            return {first,second};
        }

        Card dealOne(){
            Card first = deck.back();
            deck.pop_back();
            return first;
        }

        Card hit(){
            Card val = deck.back();
            deck.pop_back();
            return val;
        }

        // void show_cards(){
        //     for (Card card : deck){
        //         cout << card.getRank() << " " << card.getSuit() << endl;
        //     }
        // }

        int getSize(){
            return deck.size();
        }
};

struct Engine{
    optional<Deck> deck;
    const uint8_t deck_count;

    Engine(uint8_t deck_size) : deck_count(deck_size){
        deck.emplace(deck_size); // creates deck in optinal deck variable, avoid creating unnecissary constructor
        runner();
    }

    void runner(){
        //cout << deck.getSize();
        cout << "starting a " << deck_count << " deck game!" << endl;
        while (deck->getSize() > deck_count * 13){//deck_size is number of 52 card decks, so like 4 decks, reshuffle when 25% of deck cards left.
            cout << "========================================" << endl;
            Hand dealer = draw_cards();
            Hand user = draw_cards();
            print_dealer_hand(dealer);
            vector<Hand> hands = user_play(user);

        }    
    }

    vector<Hand> user_play(Hand& user){
        vector<Hand> hands;
        hands.reserve(4);
        user_play_hand(user, hands);
        return hands;
    }

    void user_play_hand(Hand& user, vector<Hand>& hands){
        int choice;
        bool game_over = false;
        print_hand(user);

        while(!game_over){
            cin >> choice;
            switch(choice)
            {
                case 0:
                    print_hand(user);
                    game_over = true;
                    hands.emplace_back(user);
                    break;
                case 1:
                    user.addCard(deck->hit());
                    if (user.check_over()) {game_over = true;}
                    print_hand(user);
                    break;
                case 2:
                    user.addCard(deck->hit());
                    print_hand(user);
                    game_over = true;
                    break;
                case 3:
                    if (user.check_can_split()){
                        Hand user2 = Hand(user.get_second_card());
                        user.pop_second_card();
                        user.addCard(deck->hit());
                        user2.addCard(deck->hit());

                        user_play_hand(user2,hands);
                        print_hand(user);
                    }
                    break;
            }
        }
    }


    void print_initial(Hand dealer, Hand user){
        peek_dealer(dealer);
        print_hand(user);
    }

    void print_state(Hand dealer, Hand user){
        print_dealer_hand(dealer);
        print_hand(user);
    }

    void print_hand(Hand user){
        cout << "Your card" << endl;
        user.show_cards();
        cout << "0: Stand, 1: Hit, 2: Double, 3: Stand" << endl;
        cout << endl;
    }

    void print_dealer_hand(Hand dealer){
        cout << "Dealer card" << endl;
        dealer.show_cards();
        cout << endl;
    }

    void peek_dealer(Hand dealer){
        cout << "Dealer card" << endl;
        dealer.peek_dealer();
        cout << endl;
    }

    Hand draw_cards(){
        Hand hand = Hand(deck->deal());
        return hand;
    }

    void dealer_draw(Hand& dealer){
        while (dealer.getScoreHard() <= 17){
            dealer.addCard(deck->hit());
            cout << "Dealer card"<< endl;
            dealer.show_cards();
            cout << endl;
        }
    }

};


int main(){
    Engine(1);
    return 0;
}