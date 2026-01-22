#ifndef ACTIONSTATS_H
#define ACTIONSTATS_H

#include <cmath>
struct ActionStats {
    int handsPlayed = 0;  // number of hands
    int splitsPlayed = 0; // number of times split occurred
    double totalPayout = 0;  // total payout across all hands
    double mean = 0.0;    // running EV
    double M2 = 0.0;      // running squared deviation

    void addResult(double x) {
        handsPlayed++;
        totalPayout += x;

        double delta = x - mean;
        mean += delta / handsPlayed;

        double delta2 = x - mean;
        M2 += delta * delta2;
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
        return (handsPlayed > 1) ? (M2 / handsPlayed) : 0.0;   // population variance
    }

    double getStdDev() const {
        return std::sqrt(getVariance());
    }

    double getStdError() const {
        return getStdDev() / std::sqrt(handsPlayed);
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