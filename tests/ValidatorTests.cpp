#include <cassert>
#include <string>

#include "exchange/Validator.h"

namespace {

using flower_exchange::Order;
using flower_exchange::Side;
using flower_exchange::Validator;

void testValidOrderPasses() {
    Validator validator;
    const Order order("client-1", "Rose", Side::Buy, 10, 10.5);

    std::string reason;
    const bool valid = validator.validate(order, reason);

    assert(valid);
    assert(reason.empty());
}

void testInvalidInstrumentFails() {
    Validator validator;
    const Order order("client-2", "InvalidFlower", Side::Buy, 10, 10.5);

    std::string reason;
    const bool valid = validator.validate(order, reason);

    assert(!valid);
    assert(!reason.empty());
}

void testInvalidQuantityFails() {
    Validator validator;
    const Order order("client-3", "Rose", Side::Buy, 15, 10.5);

    std::string reason;
    const bool valid = validator.validate(order, reason);

    assert(!valid);
    assert(!reason.empty());
}

void testInvalidPriceFails() {
    Validator validator;
    const Order order("client-4", "Rose", Side::Buy, 10, 0.0);

    std::string reason;
    const bool valid = validator.validate(order, reason);

    assert(!valid);
    assert(!reason.empty());
}

}  // namespace

int main() {
    // Keep tests intentionally small and direct.
    testValidOrderPasses();
    testInvalidInstrumentFails();
    testInvalidQuantityFails();
    testInvalidPriceFails();
    return 0;
}
