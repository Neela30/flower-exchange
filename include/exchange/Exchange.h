#pragma once

#include <map>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <string>
#include <vector>

#include "exchange/MatchingEngine.h"
#include "exchange/OrderBook.h"
#include "exchange/OrderBookFactory.h"
#include "model/ExecutionReport.h"
#include "model/Order.h"

namespace flower_exchange
{

    class IMatchingStrategy;
    class ITimeProvider;

    /**
     * Routes validated orders to the correct order book and owns the matching engine instance.
     */
    class Exchange
    {
    public:
        explicit Exchange(OrderBookFactory orderBookFactory = OrderBookFactory(),
                          std::unique_ptr<IMatchingStrategy> matchingStrategy = nullptr);
        ~Exchange();

        // Creates one order book per configured instrument.
        void initializeBooks();

        // Routes one order to its instrument book.
        std::vector<ExecutionReport> processOrder(Order order, const ITimeProvider &timeProvider);

    private:
        mutable std::shared_mutex orderBooksIndexMutex_;
        std::map<std::string, OrderBook> orderBooks_;
        std::map<std::string, std::mutex> orderBookMutexes_;
        MatchingEngine matchingEngine_;
        OrderBookFactory orderBookFactory_;
    };

} // namespace flower_exchange
