#include "trader/OrderSender.h"

#include "exchange/ExchangeApplication.h"

namespace flower_exchange {

OrderSender::OrderSender(ExchangeApplication& exchangeApplication)
    : exchangeApplication_(exchangeApplication) {}

OrderSender::~OrderSender() = default;

std::vector<ExecutionReport> OrderSender::sendOrder(Order order) {
    // TODO: Add trader-side transport and retry behavior if needed.
    return exchangeApplication_.submitOrder(std::move(order));
}

}  // namespace flower_exchange
