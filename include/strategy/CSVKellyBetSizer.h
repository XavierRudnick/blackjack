#ifndef CSVKELLYBETSIZER_H
#define CSVKELLYBETSIZER_H

#include <memory>
#include <string>
#include <vector>

#include "CountingStrategy.h"

// Decorator that replaces the wrapped strategy's getBetSize() with a fixed-B0
// μ/σ² Kelly sizer driven by an EV-per-TC CSV. Mirrors the Python sizer in
// ai_analysis_scripts/bankroll_replay_kelly.py (KellySizer / KellyEVTable).
//
// All other CountingStrategy virtuals forward to the inner strategy, so count
// updates, deviations, insurance, and naming behave exactly as before.
class CSVKellyBetSizer : public CountingStrategy {
public:
    // Loads the CSV at construction and precomputes the integer bet for every
    // TC bucket in the file. B0 and kellyFraction are captured here and never
    // re-read — fixed-B0 sizing.
    CSVKellyBetSizer(std::unique_ptr<CountingStrategy> inner,
                     const std::string& evCsvPath,
                     double startBankroll,
                     float kellyFraction);

    int getBetSize() override;
    void setUnitSize(float kellyFraction) override;

    void updateCount(Card card) override { inner_->updateCount(card); }
    void updateDeckSize(int num_cards_left) override { inner_->updateDeckSize(num_cards_left); }
    void reset(int deckSize) override { inner_->reset(deckSize); }

    float getTrueCount() const override { return inner_->getTrueCount(); }
    float getBettingTrueCount() const override { return inner_->getBettingTrueCount(); }
    float getDecksLeft() const override { return inner_->getDecksLeft(); }
    float getRunningCount() const override { return inner_->getRunningCount(); }

    bool shouldAcceptInsurance() const override { return inner_->shouldAcceptInsurance(); }

    Action shouldDeviatefromHard(int playerTotal, Rank dealerUpcard, float trueCount) override {
        return inner_->shouldDeviatefromHard(playerTotal, dealerUpcard, trueCount);
    }
    Action shouldDeviatefromSplit(Rank playerSplitRank, Rank dealerUpcard, float trueCount) override {
        return inner_->shouldDeviatefromSplit(playerSplitRank, dealerUpcard, trueCount);
    }
    Action shouldSurrender(int playerTotal, Rank dealerUpcard, float trueCount) override {
        return inner_->shouldSurrender(playerTotal, dealerUpcard, trueCount);
    }
    Action getHardHandAction(int playerTotal, Rank dealerUpcard, float trueCount) override {
        return inner_->getHardHandAction(playerTotal, dealerUpcard, trueCount);
    }
    Action getSoftHandAction(int playerTotal, Rank dealerUpcard) override {
        return inner_->getSoftHandAction(playerTotal, dealerUpcard);
    }
    Action getSplitAction(Rank playerSplitRank, Rank dealerUpcard, float trueCount) override {
        return inner_->getSplitAction(playerSplitRank, dealerUpcard, trueCount);
    }

    std::string getName() override { return inner_->getName(); }

    // Diagnostics: integer bet at integer TCs +1..+10 (matches Python summary).
    int betForTrueCount(float trueCount) const;

    ~CSVKellyBetSizer() override = default;

private:
    static constexpr float TC_STEP = 0.5f;

    std::unique_ptr<CountingStrategy> inner_;
    double B0_;
    float kelly_;
    int tcMinScaled_;            // min(round(TC / TC_STEP)) seen in CSV
    int tcMaxScaled_;            // max(round(TC / TC_STEP)) seen in CSV
    std::vector<int> bucketBet_; // precomputed bet (already clipped + rounded), MIN_BET if missing
};

#endif
