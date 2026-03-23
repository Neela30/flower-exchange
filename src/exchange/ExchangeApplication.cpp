#include "exchange/ExchangeApplication.h"

#include <utility>

namespace flower_exchange {

ExchangeApplication::ExchangeApplication()
    : validator_(),
      exchange_(),
      idGenerator_(),
      timeProvider(),
      nextSequenceNumber(1) {}

ExchangeApplication::~ExchangeApplication() = default;

std::vector<ExecutionReport> ExchangeApplication::submitOrder(Order order) {
    // Assign exchange-generated metadata before validation/matching.
    order.setOrderId(idGenerator_.nextOrderId());
    order.setSequenceNumber(nextSequenceNumber++);

    std::string reason;
    if (!validator_.validate(order, reason)) {
        return {createRejectedReport(order, reason)};
    }

    return exchange_.processOrder(std::move(order), timeProvider);
}

ExecutionReport ExchangeApplication::createRejectedReport(const Order& order,
                                                          const std::string& reason) const {
    return ExecutionReport(order.getOrderId(),
                           order.getClientOrderId(),
                           order.getInstrument(),
                           order.getSide(),
                           ExecStatus::Rejected,
                           order.getQuantity(),
                           order.getPrice(),
                           reason,
                           timeProvider.nowAsString());
}

}  // namespace flower_exchange
