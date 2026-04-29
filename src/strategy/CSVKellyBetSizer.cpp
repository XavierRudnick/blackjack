#include "CSVKellyBetSizer.h"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>

namespace {

struct EvRow {
    float trueCount;
    double handsPlayed;
    double evPerDollar;
    double stdErrorPerDollar;
};

// Trim trailing CR/whitespace (CSV files written on either platform).
std::string rtrim(std::string s) {
    while (!s.empty() && (s.back() == '\r' || s.back() == '\n' || s.back() == ' ' || s.back() == '\t')) {
        s.pop_back();
    }
    return s;
}

std::vector<std::string> splitCsvLine(const std::string& line) {
    std::vector<std::string> out;
    std::string field;
    std::stringstream ss(line);
    while (std::getline(ss, field, ',')) {
        out.push_back(field);
    }
    return out;
}

int findColumn(const std::vector<std::string>& header, const std::string& name) {
    for (std::size_t i = 0; i < header.size(); ++i) {
        if (header[i] == name) return static_cast<int>(i);
    }
    return -1;
}

}  // namespace

CSVKellyBetSizer::CSVKellyBetSizer(std::unique_ptr<CountingStrategy> inner,
                                   const std::string& evCsvPath,
                                   double startBankroll,
                                   float kellyFraction)
    : inner_(std::move(inner)),
      B0_(startBankroll),
      kelly_(kellyFraction),
      tcMinScaled_(0),
      tcMaxScaled_(0) {

    std::ifstream in(evCsvPath);
    if (!in) {
        throw std::runtime_error("CSVKellyBetSizer: failed to open EV CSV: " + evCsvPath);
    }

    std::string line;
    if (!std::getline(in, line)) {
        throw std::runtime_error("CSVKellyBetSizer: EV CSV is empty: " + evCsvPath);
    }
    auto header = splitCsvLine(rtrim(line));
    int idxTc = findColumn(header, "TrueCount");
    int idxHands = findColumn(header, "HandsPlayed");
    int idxEv = findColumn(header, "EVPerDollar");
    int idxSe = findColumn(header, "StdErrorPerDollar");
    if (idxTc < 0 || idxHands < 0 || idxEv < 0 || idxSe < 0) {
        throw std::runtime_error(
            "CSVKellyBetSizer: EV CSV missing one of TrueCount/HandsPlayed/EVPerDollar/StdErrorPerDollar: " +
            evCsvPath);
    }

    std::vector<EvRow> rows;
    rows.reserve(128);
    while (std::getline(in, line)) {
        line = rtrim(line);
        if (line.empty()) continue;
        auto fields = splitCsvLine(line);
        int needed = std::max({idxTc, idxHands, idxEv, idxSe});
        if (static_cast<int>(fields.size()) <= needed) continue;
        EvRow r;
        try {
            r.trueCount = std::stof(fields[idxTc]);
            r.handsPlayed = std::stod(fields[idxHands]);
            r.evPerDollar = std::stod(fields[idxEv]);
            r.stdErrorPerDollar = std::stod(fields[idxSe]);
        } catch (const std::exception&) {
            continue;  // skip malformed rows
        }
        rows.push_back(r);
    }

    if (rows.empty()) {
        throw std::runtime_error("CSVKellyBetSizer: no usable rows in EV CSV: " + evCsvPath);
    }

    // Establish TC bucket span.
    int minScaled = std::numeric_limits<int>::max();
    int maxScaled = std::numeric_limits<int>::min();
    for (const auto& r : rows) {
        int s = static_cast<int>(std::lround(r.trueCount / TC_STEP));
        if (s < minScaled) minScaled = s;
        if (s > maxScaled) maxScaled = s;
    }
    tcMinScaled_ = minScaled;
    tcMaxScaled_ = maxScaled;
    const int span = tcMaxScaled_ - tcMinScaled_ + 1;
    bucketBet_.assign(span, MIN_BET);

    // Precompute the integer bet for every TC bucket present in the CSV.
    // Mirrors KellySizer.bet_vec / _quantize_to_base_units in the Python sizer.
    const double minBet = static_cast<double>(MIN_BET);
    const double maxBet = static_cast<double>(MAX_BET);
    const int kMax = std::max(1, static_cast<int>(std::floor(maxBet / minBet)));

    for (const auto& r : rows) {
        int s = static_cast<int>(std::lround(r.trueCount / TC_STEP));
        int idx = s - tcMinScaled_;
        const double sd = r.stdErrorPerDollar;
        const double var = (sd * sd) * r.handsPlayed;  // (StdErr * sqrt(N))^2

        if (!(var > 0.0) || !(r.evPerDollar > 0.0)) {
            bucketBet_[idx] = MIN_BET;
            continue;
        }

        double raw = B0_ * static_cast<double>(kelly_) * r.evPerDollar / var;
        if (!std::isfinite(raw)) {
            bucketBet_[idx] = MIN_BET;
            continue;
        }
        double clipped = std::min(std::max(raw, minBet), maxBet);
        int k = static_cast<int>(std::lround(clipped / minBet));
        if (k < 1) k = 1;
        if (k > kMax) k = kMax;
        bucketBet_[idx] = k * MIN_BET;
    }

    // Diagnostic: print the integer bets at TC +1..+10 so the user can
    // eyeball-match the Python summary block.
    std::ostringstream banner;
    banner << "[CSVKellyBetSizer] Loaded " << evCsvPath << "\n"
           << "  B0=$" << static_cast<long long>(B0_)
           << "  k=" << kelly_
           << "  TC range [" << (tcMinScaled_ * TC_STEP) << " .. " << (tcMaxScaled_ * TC_STEP) << "]\n"
           << "  Bet by TC: ";
    for (int tc = 1; tc <= 10; ++tc) {
        banner << "TC+" << tc << "=$" << betForTrueCount(static_cast<float>(tc));
        if (tc < 10) banner << "  ";
    }
    banner << "\n";
    std::cout << banner.str();
}

int CSVKellyBetSizer::getBetSize() {
    const float tc = inner_->getBettingTrueCount();
    return betForTrueCount(tc);
}

void CSVKellyBetSizer::setUnitSize(float /*kellyFraction*/) {
    // No-op: B0 and kelly are fixed at construction. This is the whole point
    // of the fixed-B0 sizer — we ignore Engine's per-shoe kelly refresh.
}

int CSVKellyBetSizer::betForTrueCount(float trueCount) const {
    int s = static_cast<int>(std::lround(trueCount / TC_STEP));
    int idx = s - tcMinScaled_;
    if (idx < 0 || idx >= static_cast<int>(bucketBet_.size())) {
        return MIN_BET;
    }
    return bucketBet_[idx];
}
