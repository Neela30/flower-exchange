#pragma once

#include <map>
#include <string>
#include <vector>

#include "exchange/MatchingEngine.h"
#include "exchange/OrderBook.h"
#include "model/ExecutionReport.h"

namespace flower_exchange {

class TimeProvider;

/**
 * Routes validated orders to the correct order book and owns the matching engine instance.
 */
class Exchange {
public:
    Exchange();
    ~Exchange();

    // Creates one order book per configured instrument.
    void initializeBooks();

    // Routes one order to its instrument book.
    std::vector<ExecutionReport> processOrder(Order order, const TimeProvider& timeProvider);

private:
    std::map<std::string, OrderBook> orderBooks_;
    MatchingEngine matchingEngine_;
};

}  // namespace flower_exchange