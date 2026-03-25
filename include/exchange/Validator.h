#pragma once

#include <string>

#include "model/Order.h"

namespace flower_exchange {

// Validates orders against exchange business rules.
class Validator {
public:
    Validator();
    ~Validator();

    bool isValidInstrument(const std::string& instrument) const;
    bool isValidSide(Side side) const;
    bool isValidQuantity(int quantity) const;
    bool isValidPrice(double price) const;

    // Validates a full order and returns the first rejection reason.
    bool validate(const Order& order, std::string& reason) const;
};

}  // namespace flower_exchange
