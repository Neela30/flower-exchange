#pragma once

#include <string>

namespace flower_exchange {

/**
 * Generates stable exchange order identifiers.
 *
 * This utility is header-only so it fits the provided project layout,
 * which does not include a dedicated src/common implementation file.
 */
class IdGenerator {
public:
    IdGenerator();
    ~IdGenerator();

    std::string nextOrderId();

private:
    int counter_;
};

inline IdGenerator::IdGenerator() : counter_(1) {}

inline IdGenerator::~IdGenerator() = default;

inline std::string IdGenerator::nextOrderId() {
    return "ord" + std::to_string(counter_++);
}

}  // namespace flower_exchange
