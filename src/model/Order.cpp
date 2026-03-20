#include "model/Order.h"

#include <algorithm>
#include <utility>

namespace flower_exchange {

Order::Order()
    : orderId_(),
      clientOrderId_(),
      instrument_(),
      side_(Side::Buy),
      quantity_(0),
      price_(0.0),
      remainingQuantity_(0),
      sequenceNumber_(0) {}

Order::Order(std::string clientOrderId,
             std::string instrument,
             Side side,
             int quantity,
             double price)
    : orderId_(),
      clientOrderId_(std::move(clientOrderId)),
      instrument_(std::move(instrument)),
      side_(side),
      quantity_(quantity),
      price_(price),
      remainingQuantity_(quantity),
      sequenceNumber_(0) {}

Order::~Order() = default;

const std::string& Order::getOrderId() const {
    return orderId_;
}

const std::string& Order::getClientOrderId() const {
    return clientOrderId_;
}

const std::string& Order::getInstrument() const {
    return instrument_;
}

Side Order::getSide() const {
    return side_;
}

int Order::getQuantity() const {
    return quantity_;
}

double Order::getPrice() const {
    return price_;
}

int Order::getRemainingQuantity() const {
    return remainingQuantity_;
}

std::uint64_t Order::getSequenceNumber() const {
    return sequenceNumber_;
}

void Order::setOrderId(std::string orderId) {
    orderId_ = std::move(orderId);
}

void Order::setSequenceNumber(std::uint64_t sequenceNumber) {
    sequenceNumber_ = sequenceNumber;
}

bool Order::isBuy() const {
    return side_ == Side::Buy;
}

bool Order::isSell() const {
    return side_ == Side::Sell;
}

bool Order::isFilled() const {
    return remainingQuantity_ <= 0;
}

bool Order::isPartiallyFilled() const {
    return remainingQuantity_ > 0 && remainingQuantity_ < quantity_;
}

void Order::reduceRemainingQuantity(int matchedQty) {
    // TODO: Revisit this behavior once the matching flow is implemented in detail.
    remainingQuantity_ = std::max(0, remainingQuantity_ - matchedQty);
}

}  // namespace flower_exchange
