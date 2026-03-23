#include "exchange/Validator.h"

#include "model/Constants.h"

namespace flower_exchange {

Validator::Validator() = default;
Validator::~Validator() = default;

bool Validator::isValidInstrument(const std::string& instrument) const {
    for (const std::string_view validInstrument : kValidInstruments) {
        if (instrument == validInstrument) {
            return true;
        }
    }
    return false;
}

bool Validator::isValidSide(Side side) const {
    return side == Side::Buy || side == Side::Sell;
}

bool Validator::isValidQuantity(int quantity) const {
    if (quantity < kMinQuantity || quantity > kMaxQuantity) {
        return false;
    }
    return quantity % kQuantityStep == 0;
}

bool Validator::isValidPrice(double price) const {
    return price > kMinPrice;
}

bool Validator::validate(const Order& order, std::string& reason) const {
    if (!isValidInstrument(order.getInstrument())) {
        reason = "Invalid instrument";
        return false;
    }

    if (!isValidSide(order.getSide())) {
        reason = "Invalid side";
        return false;
    }

    if (!isValidQuantity(order.getQuantity())) {
        reason = "Invalid quantity";
        return false;
    }

    if (!isValidPrice(order.getPrice())) {
        reason = "Invalid price";
        return false;
    }

    reason.clear();
    return true;
}

}  // namespace flower_exchange
