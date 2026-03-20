#include "exchange/Validator.h"

namespace flower_exchange {

Validator::Validator() = default;

Validator::~Validator() = default;

bool Validator::isValidInstrument(const std::string& instrument) const {
    (void)instrument;
    // TODO: Implement instrument lookup against the configured instrument list.
    return true;
}

bool Validator::isValidSide(Side side) const {
    (void)side;
    // TODO: Implement side validation once parsing rules are finalized.
    return true;
}

bool Validator::isValidQuantity(int quantity) const {
    (void)quantity;
    // TODO: Implement quantity range and step validation.
    return true;
}

bool Validator::isValidPrice(double price) const {
    (void)price;
    // TODO: Implement minimum price and precision validation.
    return true;
}

bool Validator::validate(const Order& order, std::string& reason) const {
    (void)order;
    // TODO: Compose the field-level validation results and return a descriptive reason.
    reason.clear();
    return true;
}

}  // namespace flower_exchange
