
#include "CountingStrategy.h"

// Default deviations: do nothing special for counting; override in derived classes.
Action CountingStrategy::shouldDeviatefromHard(int /*playerTotal*/, Rank /*dealerUpcard*/, float /*true_count*/) {
	return Action::Skip;
}

Action CountingStrategy::shouldDeviatefromSplit(Rank /*playerSplitRank*/, Rank /*dealerUpcard*/, float /*true_count*/) {
	return Action::Skip;
}

Action CountingStrategy::shouldSurrender(int /*playerTotal*/, Rank /*dealerUpcard*/, float /*true_count*/) {
	return Action::Skip;
}
