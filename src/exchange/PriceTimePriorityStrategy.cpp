#include "exchange/PriceTimePriorityStrategy.h"

namespace flower_exchange {

bool PriceTimePriorityStrategy::canMatch(const Order& incoming, const Order& resting) const {
    if (incoming.getSide() == resting.getSide()) {
        return false;
    }

    if (incoming.isBuy()) {
        // Buy orders cross when they can pay at least the resting sell price.
        return incoming.getPrice() >= resting.getPrice();
    }

    // Sell orders cross when they can accept at most the resting buy price.
    return incoming.getPrice() <= resting.getPrice();
}

double PriceTimePriorityStrategy::determineExecutionPrice(const Order& resting) const {
    // Preserve the current behavior: trade at the resting order price.
    return resting.getPrice();
}

}  // namespace flower_exchange
