#include "common/IdGenerator.h"

namespace flower_exchange {

IdGenerator::IdGenerator() : counter(1) {}

std::string IdGenerator::nextOrderId() {
    // Return sequential internal order IDs.
    return "ord" + std::to_string(counter++);
}

}  // namespace flower_exchange
