#ifndef HANDTRACE_H
#define HANDTRACE_H

#include <string>
#include <cstdint>
#include <cmath>

// outcome_code values:
//   0 = loss
//   1 = push
//   2 = win
//   3 = blackjack_win
//   4 = double_win
//   5 = double_loss
//   6 = split_net_win
//   7 = split_net_loss
struct HandTrace {
    uint64_t shoe_id = 0;
    std::string ruleset_id;
    std::string strategy_id;

    int   hand_number_in_shoe       = 0;
    float shoe_progress_before_hand = 0.0f;

    float true_count_bucket = 0.0f;  // 0.5-increment bucket

    double total_money_committed = 0.0;
    double net_profit            = 0.0;

    uint8_t outcome_code = 0;

    int  split_count     = 0;
    int  double_count    = 0;
    bool insurance_taken = false;
    bool surrendered     = false;
};

// Bucket TC to nearest 0.5 increment
inline float bucketTC(float tc) {
    return std::round(tc * 2.0f) / 2.0f;
}

// Bucket shoe progress to nearest 5%
inline float bucketShoeProgress(float p) {
    return std::round(p * 20.0f) / 20.0f;
}

// Two-decimal rounding so stored fraction does not carry binary float noise (e.g. 0.650000000005)
inline float roundShoeProgress(float p) {
    return std::round(p * 100.0f) / 100.0f;
}

uint8_t computeHandOutcomeCode(const HandTrace& t);

#endif
