#pragma once

#include "exchange/IMatchingStrategy.h"

namespace flower_exchange {

/**
 * Preserves the current price-time priority matching policy.
 */
class PriceTimePriorityStrategy : public IMatchingStrategy {
public:
    bool canMatch(const Order& incoming, const Order& resting) const override;
    double determineExecutionPrice(const Order& resting) const override;
};

}  // namespace flower_exchange
