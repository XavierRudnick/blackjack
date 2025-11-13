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
        int bet_size_;
    public:
        Hand(pair <Card,Card> cards, int bet_size){
            hand.emplace_back(cards.first);
            hand.emplace_back(cards.second);
            bet_size_ = bet_size;
        }

        Hand(Card card,int bet_size){
            hand.emplace_back(card);
            bet_size_ = bet_size;
        }

        int getBetSize(){
            return bet_size_;
        }

        void doubleBet(){
            bet_size_ *= 2;
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
        //        cout <<
              //  val.getRank() << " " << val.getSuit() << endl;
            }
        //    cout << "value: " << getScoreHard() << endl;
            if (isSoftHand()){
         //       cout << "soft value: " << getScoreSoft() << endl;
            }
        }

        void dealer_show_cards(){
         //   cout << "Dealer card" << endl;
            for (Card val : hand){
                cout <<
                val.getRank() << " " << val.getSuit() << endl;
            }
        //    cout << "value: " << getScoreHard() << endl;
       //     cout << endl;
        }

        void peek_dealer(){
         //   cout << hand.front().getRank() << " " << hand.front().getSuit() << endl;
            //cout << "value: " << getScoreHard() << endl;
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
            if (getScoreHard() > 21 && getScoreSoft() > 21){
                return true;
            }
            return false;
        }

        int getScoreHard(){
            int score = 0;
            bool ace_appeared = false;
            for (Card val : hand){

                Rank rank = val.getRank();
                if (rank == Rank::Ace){
                    if (!ace_appeared) {ace_appeared = true; score += 11;}
                    else {score += 1;}
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

        int getFinalScore(){
            int hard = getScoreHard();
            int soft = getScoreSoft();

        
            if (hard > 21 && soft <= 21){
                return soft;
            }
            else if (soft > 21){
                return 0;
            }
            else{
                return hard;
            }
            

        }

        int getDealerScore(){
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

        bool isDealerOver(){
            if (getDealerScore() >= 17){
                return true;
            }
            return false;
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
                    score += static_cast<int>(rank) + 2;
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
            return false;
        }

};

class HiLoStrategy { //in docs note deck size is counted 100% accuratly in half size increments
    private:
        int true_count = 0;
        float num_decks_left;
    public:
        HiLoStrategy(float deck_size){
            num_decks_left = deck_size;
        }

        int getBetSize() const {
            if (true_count <= 1.5) {
                return 1;
            } 
            else if (true_count < 3.0) { 
                return 2;
            } 
            else if (true_count < 4.0) {
                return 3;
            } 
            else if (true_count < 5.0) {
                return 5;
            } 
            else {
                return static_cast<int>((true_count * 2.0) - 2.0);
            }
        }

        void updateCount(Card card) {
            Rank rank = card.getRank();
            int score = static_cast<int>(rank) + 2;
            int running_count = 0;

            if (score <= 6){
                running_count += 1;
            }
            else if (score >= 10){
                running_count -= 1;
            }
            true_count += running_count / num_decks_left; 
            return;
        }

        void updateDeckSize(int num_cards_left){
            float decks_left_unrounded = num_cards_left / 52; 
            decks_left_unrounded *= 2.0;
            float decks_left_rounded = round(decks_left_unrounded);
            decks_left_rounded = decks_left_rounded / 2;
            num_decks_left = decks_left_rounded;
            return; //this allows us to round to nearest .5
        }

        int getCount(){
            return true_count;
        }
};

template <typename Strategy>
class Deck{
    private:
        const uint8_t NUM_RANK = 13;
        const uint8_t NUM_SUIT = 4;
        const uint8_t NUM_CARDS_IN_DECK = 52;
        vector<Card> deck;
        mt19937 rand;
        Strategy strategy;

    public:
        Deck(uint8_t deck_size,Strategy strategy_input) : strategy(move(strategy_input)){
            deck.reserve(deck_size * NUM_CARDS_IN_DECK); // Pre-allocate memory to avoid reallocations

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
            strategy.updateCount(first);
            strategy.updateCount(second);
            return {first,second};
        }

        Card dealOne(){
            Card first = deck.back();
            deck.pop_back();
            strategy.updateCount(first);
            return first;
        }

        Card hit(){
            Card val = deck.back();
            deck.pop_back();
            strategy.updateCount(val);
            return val;
        }

        int getSize(){
            return deck.size();
        }

        Strategy getStrategy(){
            return strategy;
        }

        int getBetSize(){
            return strategy.getBetSize();
        }
};

template <typename Strategy>
struct Engine{
    optional<Deck<Strategy>> deck;
    const uint8_t number_of_decks;

    Engine(uint8_t deck_size,Strategy strategy) : number_of_decks(deck_size){
        deck.emplace(deck_size,move(strategy)); // creates deck in optinal deck variable, avoid creating unnecissary constructor
        runner();
    }

    void runner(){
       // cout << "starting a " << number_of_decks << " deck game!" << endl;
        int total = 100;
        while (deck->getSize() > number_of_decks * 13){//deck_size is number of 52 card decks, so like 4 decks, reshuffle when 25% of deck cards left.
        //    cout << "========================================" << endl;
            deck->getStrategy().updateDeckSize(deck->getSize()); 
            int betSize = deck->getBetSize();
            Hand dealer = draw_cards();
            Hand user = draw_cards(betSize);
            //implement insurance here
            peek_dealer(dealer);
            vector<Hand> hands = user_play(user);
            dealer_draw(dealer);
            
            int dealer_score = dealer.getDealerScore();

            for (Hand hand : hands){
                int score = hand.getFinalScore();
                if (dealer_score > score){
                    total -= hand.getBetSize();
                }
                else if (dealer_score < score){
                    total += hand.getBetSize();
                }
            }

           // cout << "total : " << total << endl;
           // cout << "true count : " << deck->getStrategy().getCount() << endl;
            //add hand evaluator to distribute wins
        }    
    }

    vector<Hand> user_play(Hand& user){
        vector<Hand> hands;
        hands.reserve(4); //unnessesary but fuck it I do what I want
        user_play_hand(user, hands);
        return hands;
    }

    void user_play_hand(Hand& user, vector<Hand>& hands){//need to implement strategy for here
        int choice;
        bool game_over = false;
        print_hand(user);

        while(!game_over){
        //    cin >> choice;
        //    cout << endl;
            switch(choice)
            {
                case 0:
                    print_hand(user);
                    game_over = true;
                    hands.emplace_back(user);
                    break;
                case 1:
                    user.addCard(deck->hit());
                    if (user.check_over()) {game_over = true; hands.emplace_back(user);}
                    print_hand(user);
                    break;
                case 2:
                    user.doubleBet();
                    user.addCard(deck->hit());
                    print_hand(user);
                    game_over = true;
                    hands.emplace_back(user);
                    break;
                case 3:
                    if (user.check_can_split()){
                        Hand user2 = Hand(user.get_second_card(),user.getBetSize());
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
        dealer.dealer_show_cards();
        print_hand(user);
    }

    void print_hand(Hand user){
        //cout << "Your card" << endl;
        user.show_cards();
        //cout << "0: Stand, 1: Hit, 2: Double, 3: Stand" << endl;
        //cout << endl;
    }

    void peek_dealer(Hand dealer){
       // cout << "Dealer card" << endl;
        dealer.peek_dealer();
      //  cout << endl;
    }

    Hand draw_cards(int betSize = 0){
        Hand hand = Hand(deck->deal(),betSize);
        return hand;
    }

    void dealer_draw(Hand& dealer){
        dealer.dealer_show_cards();
        if (dealer.isDealerOver()){
            return;
        }
        while (!dealer.isDealerOver()){
            dealer.addCard(deck->hit());
            dealer.dealer_show_cards();
        }
        return;
    }

};


int main(){
    int num_decks_used = 1;
    HiLoStrategy hilo = HiLoStrategy(num_decks_used);
    Engine(num_decks_used, hilo);
    return 0;
}