#include "common/IdGenerator.h"

namespace flower_exchange {

IdGenerator::IdGenerator() : counter_(1) {}

std::string IdGenerator::nextOrderId() {
    // Return sequential internal order IDs.
    const int orderNumber = counter_.fetch_add(1, std::memory_order_relaxed);
    return "ord" + std::to_string(orderNumber);
}

}  // namespace flower_exchange
