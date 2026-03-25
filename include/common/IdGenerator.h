#pragma once

#include <string>

namespace flower_exchange {

// Generates internal order IDs such as ord1, ord2, ord3.
class IdGenerator {
public:
    IdGenerator();
    ~IdGenerator() = default;

    std::string nextOrderId();

private:
    int counter;
};

}  // namespace flower_exchange
