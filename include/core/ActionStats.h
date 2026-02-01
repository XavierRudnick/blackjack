#ifndef ACTIONSTATS_H
#define ACTIONSTATS_H

#include <cmath>
struct ActionStats {
    int handsPlayed = 0;  // number of hands
    int splitsPlayed = 0; // number of times split occurred
    double totalPayout = 0.0;       // total net payout across all hands
    double totalMoneyWagered = 0.0; // total dollars wagered across all hands
    double mean = 0.0;              // running EV per dollar wagered
    double M2 = 0.0;                // running squared deviation (per dollar)

    void addResult(double net, double wagered) {
        if (wagered <= 0.0) {
            return;
        }

        handsPlayed++;
        totalPayout += net;
        totalMoneyWagered += wagered;

        // Weighted Welford update using wagered as frequency weight
        const double value = net / wagered;
        const double prevMean = mean;
        const double totalWeight = totalMoneyWagered;
        mean = prevMean + (wagered / totalWeight) * (value - prevMean);
        M2 += wagered * (value - prevMean) * (value - mean);
    }

    void addResult(double net) {
        addResult(net, 1.0);
    }

    void addInsuranceLose(double loss = .5){
        addResult(-loss, loss);
    }

    void timesSplit() {
        splitsPlayed++;
    }

    double getEV() const {
        return mean;
    }

    // double getSplitEV() const {
    //     if (splitsPlayed == 0) return 0.0;
    //     return totalPayout / splitsPlayed;
    // }

    double getVariance() const {
        return (totalMoneyWagered > 0.0) ? (M2 / totalMoneyWagered) : 0.0;   // population variance per dollar
    }

    double getStdDev() const {
        return std::sqrt(getVariance());
    }

    double getStdError() const {
        return (totalMoneyWagered > 0.0) ? (getStdDev() / std::sqrt(totalMoneyWagered)) : 0.0;
    }
};


struct DecisionPoint {
    ActionStats hitStats;
    ActionStats standStats;
    ActionStats doubleStats;
    ActionStats splitStats;
    ActionStats surrenderStats;
    ActionStats insuranceAcceptStats;
    ActionStats insuranceDeclineStats;
};

#endif // ACTIONSTATS_H
