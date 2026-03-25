#pragma once

#include <string>

namespace flower_exchange {

/**
 * Small utility that returns the current local timestamp string.
 */
class TimeProvider {
public:
    TimeProvider();
    ~TimeProvider();

    std::string nowAsString() const;
};

}  // namespace flower_exchange
