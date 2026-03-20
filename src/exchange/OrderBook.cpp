#include "exchange/OrderBook.h"

#include <utility>

#include "exchange/MatchingEngine.h"
#include "io/TimeProvider.h"

namespace flower_exchange {

OrderBook::OrderBook(std::string instrument, MatchingEngine& matchingEngine)
    : instrument_(std::move(instrument)),
      buySide_(Side::Buy),
      sellSide_(Side::Sell),
      matchingEngine_(matchingEngine) {}

OrderBook::~OrderBook() = default;

std::vector<ExecutionReport> OrderBook::processOrder(Order order,
                                                     const TimeProvider& timeProvider) {
    OrderBookSide& sameSide = order.isBuy() ? buySide_ : sellSide_;
    OrderBookSide& oppositeSide = order.isBuy() ? sellSide_ : buySide_;

    auto reports = matchingEngine_.match(order, oppositeSide, sameSide, timeProvider);

    // TODO: Store residual quantity and emit NEW/PFILL/FILL reports when order-book behavior is implemented.
    if (!order.isFilled()) {
        (void)sameSide;
    }

    return reports;
}

const std::string& OrderBook::getInstrument() const {
    return instrument_;
}

OrderBookSide& OrderBook::getBuySide() {
    return buySide_;
}

OrderBookSide& OrderBook::getSellSide() {
    return sellSide_;
}

const OrderBookSide& OrderBook::getBuySide() const {
    return buySide_;
}

const OrderBookSide& OrderBook::getSellSide() const {
    return sellSide_;
}

}  // namespace flower_exchange
