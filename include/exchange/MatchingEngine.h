#pragma once

#include <memory>
#include <vector>

#include "exchange/IMatchingStrategy.h"
#include "model/ExecutionReport.h"
#include "model/Order.h"

namespace flower_exchange {

class OrderBookSide;
class TimeProvider;

/**
 * Owns price-time matching behavior behind a narrow interface.
 */
class MatchingEngine {
public:
    MatchingEngine();
    explicit MatchingEngine(std::unique_ptr<IMatchingStrategy> strategy);
    ~MatchingEngine();

    bool canMatch(const Order& incoming, const Order& resting) const;
    double determineExecutionPrice(const Order& resting) const;

    /**
     * Attempts to match an incoming order against the opposite book side.
     */
    std::vector<ExecutionReport> match(Order& incoming,
                                       OrderBookSide& oppositeSide,
                                       OrderBookSide& sameSide,
                                       const TimeProvider& timeProvider) const;

private:
    // MatchingEngine owns the active matching policy and delegates rule decisions to it.
    std::unique_ptr<IMatchingStrategy> strategy_;
};

}  // namespace flower_exchange
