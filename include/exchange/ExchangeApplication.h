#pragma once

#include <atomic>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "common/IdGenerator.h"
#include "exchange/Exchange.h"
#include "exchange/Validator.h"
#include "io/TimeProvider.h"
#include "model/ExecutionReport.h"
#include "model/Order.h"

namespace flower_exchange
{

    class IMatchingStrategy;
    class ITimeProvider;

    /**
     * Exchange-side application service that assigns metadata, validates, and delegates processing.
     */
    class ExchangeApplication
    {
    public:
        ExchangeApplication(std::shared_ptr<ITimeProvider> timeProvider = nullptr,
                            std::unique_ptr<IMatchingStrategy> matchingStrategy = nullptr,
                            OrderBookFactory orderBookFactory = OrderBookFactory());
        ~ExchangeApplication();

        // Assigns exchange metadata, validates, and routes one order.
        std::vector<ExecutionReport> submitOrder(Order order);

        // Builds a rejected execution report for a failed order.
        ExecutionReport createRejectedReport(const Order &order, const std::string &reason) const;

    private:
        Validator validator_;
        Exchange exchange_;
        IdGenerator idGenerator_;
        std::shared_ptr<ITimeProvider> timeProvider_;
        std::atomic<std::uint64_t> nextSequenceNumber_;
    };

} // namespace flower_exchange
