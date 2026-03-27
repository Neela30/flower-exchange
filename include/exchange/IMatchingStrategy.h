#pragma once

#include "model/Order.h"

namespace flower_exchange {

/**
 * Defines the policy used by MatchingEngine to decide whether orders match
 * and which execution price should be used.
 */
class IMatchingStrategy {
public:
    virtual ~IMatchingStrategy() = default;

    virtual bool canMatch(const Order& incoming, const Order& resting) const = 0;
    virtual double determineExecutionPrice(const Order& resting) const = 0;
};

}  // namespace flower_exchange
