#include "FixedEngine.h"
#include "Deck.h"
#include "Engine.h"
#include "ActionStats.h"
#include "MonteCarloScenario.h"

#include <fstream>
#include <iomanip>
#include <filesystem>
#include <limits>
#include <cmath>

namespace {

int maxHandsPlayed(const DecisionPoint& decisionPoint) {
    int handsPlayed = decisionPoint.hitStats.handsPlayed;
    if (handsPlayed == 0) {
        handsPlayed = decisionPoint.standStats.handsPlayed;
    }
    if (handsPlayed == 0) {
        handsPlayed = decisionPoint.doubleStats.handsPlayed;
    }
    if (handsPlayed == 0) {
        handsPlayed = decisionPoint.splitStats.handsPlayed;
    }
    if (handsPlayed == 0) {
        handsPlayed = decisionPoint.surrenderStats.handsPlayed;
    }
    if (handsPlayed == 0) {
        handsPlayed = decisionPoint.insuranceAcceptStats.handsPlayed;
    }
    if (handsPlayed == 0) {
        handsPlayed = decisionPoint.insuranceDeclineStats.handsPlayed;
    }
    return handsPlayed;
}

bool decisionPointHasSamples(const DecisionPoint& dp) {
    return dp.hitStats.handsPlayed > 0 || dp.standStats.handsPlayed > 0 || dp.doubleStats.handsPlayed > 0 ||
           dp.splitStats.handsPlayed > 0 || dp.surrenderStats.handsPlayed > 0 ||
           dp.insuranceAcceptStats.handsPlayed > 0 || dp.insuranceDeclineStats.handsPlayed > 0;
}

void writeCsvRowsForTable(std::ostream& out, const EVTable& table) {
    out << "UserValue,DealerValue,IsSoft,TrueCount,"
        << "Hit EV,Hit Variance,Stand EV,Stand Variance,Double EV,Double Variance,Split EV,Split Variance,Surrender EV,Surrender Variance,Insurance Accept EV,Insurance Accept Variance,Insurance Decline EV,Insurance Decline Variance,Hands Played"
        << '\n';

    for (int s = 0; s < 2; ++s) {
        for (int p = 0; p < EV_P_RANGE; ++p) {
            for (int d = 0; d < EV_D_RANGE; ++d) {
                for (int tc = 0; tc < EV_TC_STEPS; ++tc) {
                    const int idx = ((s * EV_P_RANGE + p) * EV_D_RANGE + d) * EV_TC_STEPS + tc;
                    const DecisionPoint& decisionPoint = table[static_cast<std::size_t>(idx)];
                    if (!decisionPointHasSamples(decisionPoint)) {
                        continue;
                    }
                    const int playerTotal = p + EV_P_MIN;
                    const int dealerUp = d + 2;
                    const bool isSoft = (s == 1);
                    const float trueCount = (tc - EV_TC_OFFSET) * 0.5f;
                    const int handsPlayed = maxHandsPlayed(decisionPoint);

                    out << playerTotal << ',' << dealerUp << ',' << (isSoft ? 1 : 0) << ','
                        << std::setprecision(std::numeric_limits<float>::max_digits10) << std::defaultfloat
                        << trueCount << ',' << std::fixed << std::setprecision(6)
                        << decisionPoint.hitStats.getEV() << ',' << decisionPoint.hitStats.getVariance() << ','
                        << decisionPoint.standStats.getEV() << ',' << decisionPoint.standStats.getVariance() << ','
                        << decisionPoint.doubleStats.getEV() << ',' << decisionPoint.doubleStats.getVariance() << ','
                        << decisionPoint.splitStats.getEV() << ',' << decisionPoint.splitStats.getVariance() << ','
                        << decisionPoint.surrenderStats.getEV() << ',' << decisionPoint.surrenderStats.getVariance()
                        << ',' << decisionPoint.insuranceAcceptStats.getEV() << ','
                        << decisionPoint.insuranceAcceptStats.getVariance() << ','
                        << decisionPoint.insuranceDeclineStats.getEV() << ','
                        << decisionPoint.insuranceDeclineStats.getVariance() << ',' << handsPlayed << '\n';
                }
            }
        }
    }
}

} // namespace

FixedEngine::FixedEngine() : EVresults_(std::make_unique<EVTable>()) {
    handsBuffer_.reserve(5);
}

FixedEngine::FixedEngine(std::vector<Action> monteCarloActions, const GameConfig& gameConfig)
    : monteCarloActions(std::move(monteCarloActions)), EVresults_(std::make_unique<EVTable>()), config(gameConfig) {
    handsBuffer_.reserve(5);
}

FixedEngine::FixedEngine(std::vector<Action> monteCarloActions, const EVTable& evIn, const GameConfig& gameConfig)
    : monteCarloActions(std::move(monteCarloActions)),
      EVresults_(std::make_unique<EVTable>(evIn)),
      config(gameConfig) {
    handsBuffer_.reserve(5);
}

void FixedEngine::calculateEV(Player& player, Deck& deck, Hand& dealer, Hand& user, float trueCount,
                              std::pair<int, int> cardValues, bool isSoft) {

    std::vector<Card> deckCards = deck.getDeckCards();

    for (Action forcedAction : monteCarloActions) {
        handsBuffer_.clear();
        Hand simDealer = dealer;
        Hand simUser = user;

        playForcedHand(player, deck, simDealer, simUser, handsBuffer_, forcedAction, false, false, trueCount);
        Hand evalDealer = simDealer;
        evaluateHand(deck, evalDealer, handsBuffer_, trueCount, forcedAction, cardValues, isSoft, simUser.getBetSize());

        deck.restoreDeckCards(deckCards);
    }

    deck.restoreDeckCards(deckCards);
}

void FixedEngine::calculateEVForScenario(Player& player, Deck& deck, Hand& dealer, Hand& user, float trueCount,
                                         std::pair<int, int> cardValues, bool isSoft,
                                         const MonteCarloScenario& scenario) {
    for (Action forcedAction : scenario.actions) {
        handsBuffer_.clear();
        Hand simDealer = dealer;
        Hand simUser = user;
        Deck simDeck = deck.clone();

        playForcedHand(player, simDeck, simDealer, simUser, handsBuffer_, forcedAction, false, false, trueCount);
        Hand evalDealer = simDealer;
        evaluateHandForScenario(simDeck, evalDealer, handsBuffer_, trueCount, forcedAction, cardValues, isSoft,
                                simUser.getBetSize(), scenario.name);
    }
}

void FixedEngine::playForcedHand(Player& player, Deck& deck, Hand& dealer, Hand& user, std::vector<Hand>& hands,
                                 Action forcedAction, bool has_split_aces, bool has_split, float trueCount) {
    bool game_over = false;
    int i = 0;
    Action action = Action::Skip;
    while (!game_over) {
        if (i == 0 && forcedAction != Action::Skip) {
            action = forcedAction;
        } else {
            action = player.getAction(user, dealer, trueCount);
        }

        switch (action) {
        case Action::Stand:
            game_over = standHandler(user, hands);
            break;
        case Action::Hit:
            game_over = hitHandler(deck, user, hands);
            break;
        case Action::Double:
            game_over = doubleHandler(deck, user, hands, has_split);
            break;
        case Action::Split:
            game_over = splitHandler(player, deck, user, dealer, hands, has_split, has_split_aces, trueCount);
            break;
        case Action::InsuranceAccept:
            game_over = InsuranceHandler(player, deck, user, dealer, hands, trueCount);
            break;
        case Action::InsuranceDecline:
            game_over = InsuranceHandler(player, deck, user, dealer, hands, trueCount);
            break;
        case Action::Surrender:
            game_over = surrenderHandler(user, hands);
            break;
        case Action::Skip:
            break;
        }
        ++i;
    }
}

bool FixedEngine::standHandler(Hand& user, std::vector<Hand>& hands) {
    hands.emplace_back(user);
    return true;
}

bool FixedEngine::hitHandler(Deck& deck, Hand& user, std::vector<Hand>& hands) {
    Card c = deck.hit();
    user.addCard(c);

    if (user.checkOver()) {
        hands.emplace_back(user);
        return true;
    }

    return false;
}

bool FixedEngine::surrenderHandler(Hand& user, std::vector<Hand>& hands) {
    hands.emplace_back(user);
    return true;
}

bool FixedEngine::InsuranceHandler(Player& /*player*/, Deck& /*deck*/, Hand& user, Hand& dealer,
                                   std::vector<Hand>& hands, float /*trueCount*/) {
    bool dealerHasBlackjack = dealer.dealerHiddenTen();

    if (dealerHasBlackjack) {
        hands.emplace_back(user);
        return true;
    }
    return false;
}

bool FixedEngine::doubleHandler(Deck& deck, Hand& user, std::vector<Hand>& hands, bool has_split) {
    if (has_split && !config.doubleAfterSplitAllowed) {
        Card card = deck.hit();
        user.addCard(card);
        return false;
    }
    user.doubleBet();
    Card card = deck.hit();
    user.addCard(card);

    hands.emplace_back(user);
    return true;
}

bool FixedEngine::splitHandler(Player& player, Deck& deck, Hand& user, Hand& dealer, std::vector<Hand>& hands,
                               bool /*has_split*/, bool has_split_aces, float trueCount) {
    bool splitting_aces = (user.peekFrontCard() == Rank::Ace);

    if (!user.checkCanSplit()) {
        return false;
    }

    if (splitting_aces && has_split_aces && !config.allowReSplitAces) {
        hands.emplace_back(user);
        return true;
    }

    Hand user2 = Hand(user.getLastCard(), user.getBetSize());
    user.popLastCard();

    Card card1 = deck.hit();
    user.addCard(card1);

    Card card2 = deck.hit();
    user2.addCard(card2);

    if (splitting_aces) {
        if (user.isAces() && config.allowReSplitAces) {
            splitHandler(player, deck, user, dealer, hands, true, true, trueCount);
        } else {
            hands.emplace_back(user);
        }

        if (user2.isAces() && config.allowReSplitAces) {
            splitHandler(player, deck, user2, dealer, hands, true, true, trueCount);
        } else {
            hands.emplace_back(user2);
        }
        return true;
    }

    playForcedHand(player, deck, dealer, user, hands, Action::Skip, has_split_aces || splitting_aces, true, trueCount);
    playForcedHand(player, deck, dealer, user2, hands, Action::Skip, has_split_aces || splitting_aces, true, trueCount);
    return true;
}

void FixedEngine::evaluateHand(Deck& deck, Hand& dealer, std::vector<Hand>& hands, float trueCount,
                               Action forcedAction, std::pair<int, int> cardValues, bool isSoft, int baseBet) {
    const int idx = evIdx(cardValues.first, cardValues.second, isSoft, trueCount);
    DecisionPoint& decisionPoint = (*EVresults_)[static_cast<std::size_t>(idx)];
    evaluateHandCore(deck, dealer, hands, forcedAction, baseBet, decisionPoint);
}

void FixedEngine::evaluateHandForScenario(Deck& deck, Hand& dealer, std::vector<Hand>& hands, float trueCount,
                                          Action forcedAction, std::pair<int, int> cardValues, bool isSoft, int baseBet,
                                          const std::string& scenarioName) {
    const int idx = evIdx(cardValues.first, cardValues.second, isSoft, trueCount);
    DecisionPoint& decisionPoint = scenarioResults[scenarioName][static_cast<std::size_t>(idx)];
    evaluateHandCore(deck, dealer, hands, forcedAction, baseBet, decisionPoint);
}

void FixedEngine::evaluateHandCore(Deck& deck, Hand& dealer, std::vector<Hand>& hands, Action forcedAction, int baseBet,
                                   DecisionPoint& decisionPoint) {

    if (forcedAction == Action::Split) {
        double splitPayout = 0.0;

        int dealerScore = 0;

        for (Hand& hand : hands) {
            if (hand.getFinalScore() != 0) {
                dealer_draw(deck, dealer);
                dealerScore = dealer.getFinalScore();
                break;
            }
        }

        for (Hand& hand : hands) {
            int userScore = hand.getFinalScore();
            float result = 1.0f;

            if (dealerScore > userScore) {
                result *= -1.0f;
            } else if (dealerScore < userScore) {
                result *= 1.0f;
            } else if (dealerScore == 0 && userScore == 0) {
                result *= -1.0f;
            } else {
                result *= 0.0f;
            }

            double betMultiplier = 1.0;
            if (baseBet > 0) {
                betMultiplier = static_cast<double>(hand.getBetSize()) / static_cast<double>(baseBet);
            }
            splitPayout += result * betMultiplier;
        }

        decisionPoint.splitStats.addResult(splitPayout);
        return;
    }

    for (Hand& hand : hands) {
        int userScore = hand.getFinalScore();

        if (hand.isBlackjack() && !dealer.isBlackjack() && hands.size() == 1) {
            if (forcedAction == Action::InsuranceAccept) {
                decisionPoint.insuranceAcceptStats.addResult(1.0f);
                return;
            }
            if (forcedAction == Action::InsuranceDecline) {
                decisionPoint.insuranceDeclineStats.addResult(1.5f);
                return;
            }
            decisionPoint.standStats.addResult(1.5f);
            return;
        }

        if (userScore != 0) {
            dealer_draw(deck, dealer);
        }

        int dealerScore = dealer.getFinalScore();
        float result = 1.0f;

        if (dealerScore > userScore) {
            result *= -1.0f;
        } else if (dealerScore < userScore) {
            result *= 1.0f;
        } else if (dealerScore == 0 && userScore == 0) {
            result *= -1.0f;
        } else {
            result *= 0.0f;
        }

        if (forcedAction == Action::Hit) {
            decisionPoint.hitStats.addResult(result);
        } else if (forcedAction == Action::Stand) {
            decisionPoint.standStats.addResult(result);
        } else if (forcedAction == Action::Double) {
            decisionPoint.doubleStats.addResult(result * 2.0f);
        } else if (forcedAction == Action::Surrender) {
            decisionPoint.surrenderStats.addResult(-0.5f);
        } else if (forcedAction == Action::InsuranceAccept) {
            if (dealer.isBlackjack() && hand.isBlackjack() && hands.size() == 1) {
                decisionPoint.insuranceAcceptStats.addResult(1.0f);
            } else if (dealer.isBlackjack() && !hand.isBlackjack()) {
                decisionPoint.insuranceAcceptStats.addResult(0.0f);
            } else {
                decisionPoint.insuranceAcceptStats.addResult(result - 0.5f);
            }
        } else if (forcedAction == Action::InsuranceDecline) {
            decisionPoint.insuranceDeclineStats.addResult(result);
        }
    }
}

void FixedEngine::dealer_draw(Deck& deck, Hand& dealer) {
    bool isSoft17 = dealer.isSoft17();
    int score = dealer.getScore();

    if (score > 17 || (score == 17 && !isSoft17) || (isSoft17 && !config.dealerHitsSoft17)) {
        return;
    }
    while (!dealer.isDealerOver() || (dealer.isSoft17() && config.dealerHitsSoft17)) {
        if (deck.getSize() < 1) {
            throw std::runtime_error("Not enough cards to draw initial hand 271");
        }

        Card c = deck.hit();
        dealer.addCard(c);
    }
}

void FixedEngine::savetoCSVResults(const std::string& filename) const {
    std::filesystem::path outPath(filename);
    if (outPath.has_parent_path()) {
        std::error_code ec;
        std::filesystem::create_directories(outPath.parent_path(), ec);
        if (ec) {
            std::cerr << "Failed to create directory " << outPath.parent_path().string() << ": " << ec.message()
                      << std::endl;
            return;
        }
    }

    std::ofstream out(outPath, std::ios::out | std::ios::trunc);
    if (!out.is_open()) {
        std::cerr << "Failed to open " << outPath.string() << " for writing results." << std::endl;
        return;
    }

    writeCsvRowsForTable(out, *EVresults_);
}

void FixedEngine::merge(const FixedEngine& other) {
    auto accumulate = [](ActionStats& dst, const ActionStats& src) {
        dst.splitsPlayed += src.splitsPlayed;
        dst.totalPayout += src.totalPayout;
        dst.handsPlayed += src.handsPlayed;

        const double dstWeight = dst.totalMoneyWagered;
        const double srcWeight = src.totalMoneyWagered;

        if (srcWeight <= 0.0) {
            return;
        }

        if (dstWeight <= 0.0) {
            dst.totalMoneyWagered = srcWeight;
            dst.mean = src.mean;
            dst.M2 = src.M2;
            return;
        }

        const double totalWeight = dstWeight + srcWeight;
        const double delta = src.mean - dst.mean;

        dst.mean = (dstWeight * dst.mean + srcWeight * src.mean) / totalWeight;
        dst.M2 = dst.M2 + src.M2 + delta * delta * dstWeight * srcWeight / totalWeight;
        dst.totalMoneyWagered = totalWeight;
    };

    auto mergeTable = [&accumulate](EVTable& dst, const EVTable& src) {
        for (std::size_t i = 0; i < EV_TABLE_SLOTS; ++i) {
            DecisionPoint& cur = dst[i];
            const DecisionPoint& oth = src[i];
            accumulate(cur.hitStats, oth.hitStats);
            accumulate(cur.standStats, oth.standStats);
            accumulate(cur.doubleStats, oth.doubleStats);
            accumulate(cur.splitStats, oth.splitStats);
            accumulate(cur.surrenderStats, oth.surrenderStats);
            accumulate(cur.insuranceAcceptStats, oth.insuranceAcceptStats);
            accumulate(cur.insuranceDeclineStats, oth.insuranceDeclineStats);
        }
    };

    mergeTable(*EVresults_, *other.EVresults_);

    for (const auto& entry : other.scenarioResults) {
        mergeTable(scenarioResults[entry.first], entry.second);
    }
}

const EVTable& FixedEngine::getResults() const {
    return *EVresults_;
}

bool FixedEngine::hasAnyLegacyResults() const {
    for (const DecisionPoint& d : *EVresults_) {
        if (decisionPointHasSamples(d)) {
            return true;
        }
    }
    return false;
}

const EVTable& FixedEngine::getScenarioResults(const std::string& scenarioName) const {
    static const EVTable empty{};
    auto it = scenarioResults.find(scenarioName);
    if (it == scenarioResults.end()) {
        return empty;
    }
    return it->second;
}

std::vector<std::string> FixedEngine::getScenarioNames() const {
    std::vector<std::string> names;
    names.reserve(scenarioResults.size());
    for (const auto& entry : scenarioResults) {
        names.push_back(entry.first);
    }
    return names;
}

void FixedEngine::saveScenarioResults(const std::string& scenarioName, const std::string& baseFilename) const {
    auto it = scenarioResults.find(scenarioName);
    if (it == scenarioResults.end()) {
        std::cerr << "No results found for scenario: " << scenarioName << std::endl;
        return;
    }

    std::filesystem::path outPath(baseFilename);
    if (outPath.has_parent_path()) {
        std::error_code ec;
        std::filesystem::create_directories(outPath.parent_path(), ec);
        if (ec) {
            std::cerr << "Failed to create directory " << outPath.parent_path().string() << ": " << ec.message()
                      << std::endl;
            return;
        }
    }

    std::ofstream out(outPath, std::ios::out | std::ios::trunc);
    if (!out.is_open()) {
        std::cerr << "Failed to open " << outPath.string() << " for writing results." << std::endl;
        return;
    }

    writeCsvRowsForTable(out, it->second);
}
