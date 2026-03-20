#pragma once

#include <string>

namespace flower_exchange {

/**
 * Abstracts time formatting away from application services and engines.
 */
class TimeProvider {
public:
    TimeProvider();
    ~TimeProvider();

    std::string nowAsString() const;
};

}  // namespace flower_exchange
