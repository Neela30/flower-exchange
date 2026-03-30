#include "exchange/ExchangeApplication.h"

#include <memory>
#include <utility>

#include "exchange/PriceTimePriorityStrategy.h"

namespace flower_exchange
{

    ExchangeApplication::ExchangeApplication(std::shared_ptr<ITimeProvider> timeProvider,
                                             std::unique_ptr<IMatchingStrategy> matchingStrategy,
                                             OrderBookFactory orderBookFactory)
        : validator_(),
          exchange_(std::move(orderBookFactory),
                    matchingStrategy ? std::move(matchingStrategy)
                                     : std::make_unique<PriceTimePriorityStrategy>()),
          idGenerator_(),
          timeProvider_(timeProvider ? std::move(timeProvider)
                                     : std::make_shared<TimeProvider>()),
          nextSequenceNumber_(1) {}

    ExchangeApplication::~ExchangeApplication() = default;

    std::vector<ExecutionReport> ExchangeApplication::submitOrder(Order order)
    {

        order.setOrderId(idGenerator_.nextOrderId());
        order.setSequenceNumber(nextSequenceNumber_.fetch_add(1, std::memory_order_relaxed));

        std::string reason;
        if (!validator_.validate(order, reason))
        {
            return {createRejectedReport(order, reason)};
        }

        return exchange_.processOrder(std::move(order), *timeProvider_);
    }

    ExecutionReport ExchangeApplication::createRejectedReport(const Order &order,
                                                              const std::string &reason) const
    {
        return ExecutionReport(order.getOrderId(),
                               order.getClientOrderId(),
                               order.getInstrument(),
                               order.getSide(),
                               ExecStatus::Rejected,
                               order.getQuantity(),
                               order.getPrice(),
                               reason,
                               timeProvider_->nowAsString());
    }

} // namespace flower_exchange
