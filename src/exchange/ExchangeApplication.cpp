#include "exchange/ExchangeApplication.h"

#include <utility>

namespace flower_exchange {

ExchangeApplication::ExchangeApplication()
    : validator_(),
      exchange_(),
      idGenerator_(),
      timeProvider_(),
      nextSequenceNumber_(1) {}

ExchangeApplication::~ExchangeApplication() = default;

std::vector<ExecutionReport> ExchangeApplication::submitOrder(Order order) {
    order.setOrderId(idGenerator_.nextOrderId());
    order.setSequenceNumber(nextSequenceNumber_++);

    std::string reason;
    if (!validator_.validate(order, reason)) {
        return {createRejectedReport(order, reason)};
    }

    return exchange_.processOrder(std::move(order), timeProvider_);
}

ExecutionReport ExchangeApplication::createRejectedReport(const Order& order,
                                                          const std::string& reason) const {
    return ExecutionReport(order.getOrderId(),
                           order.getClientOrderId(),
                           order.getInstrument(),
                           order.getSide(),
                           ExecStatus::Rejected,
                           0,
                           0.0,
                           reason,
                           timeProvider_.nowAsString());
}

}  // namespace flower_exchange
