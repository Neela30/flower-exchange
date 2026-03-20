#pragma once

#include <string>

#include "model/Order.h"

namespace flower_exchange {

/**
 * Provides a dedicated validation boundary before orders reach exchange state.
 */
class Validator {
public:
    Validator();
    ~Validator();

    bool isValidInstrument(const std::string& instrument) const;
    bool isValidSide(Side side) const;
    bool isValidQuantity(int quantity) const;
    bool isValidPrice(double price) const;

    /**
     * Validates a full order and explains the first failure once implemented.
     */
    bool validate(const Order& order, std::string& reason) const;
};

}  // namespace flower_exchange
