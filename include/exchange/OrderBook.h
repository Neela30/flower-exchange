#pragma once

#include <string>
#include <vector>

#include "exchange/OrderBookSide.h"
#include "model/ExecutionReport.h"

namespace flower_exchange {

class MatchingEngine;
class TimeProvider;

/**
 * Aggregates both sides of one instrument book and delegates matching work.
 */
class OrderBook {
public:
    OrderBook(std::string instrument, MatchingEngine& matchingEngine);
    ~OrderBook();

    std::vector<ExecutionReport> processOrder(Order order, const TimeProvider& timeProvider);

    const std::string& getInstrument() const;
    OrderBookSide& getBuySide();
    OrderBookSide& getSellSide();
    const OrderBookSide& getBuySide() const;
    const OrderBookSide& getSellSide() const;

private:
    std::string instrument_;
    OrderBookSide buySide_;
    OrderBookSide sellSide_;
    MatchingEngine& matchingEngine_;
};

}  // namespace flower_exchange
