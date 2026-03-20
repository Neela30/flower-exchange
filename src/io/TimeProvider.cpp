#include "io/TimeProvider.h"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

namespace flower_exchange {

TimeProvider::TimeProvider() = default;

TimeProvider::~TimeProvider() = default;

std::string TimeProvider::nowAsString() const {
    const auto now = std::chrono::system_clock::now();
    const std::time_t currentTime = std::chrono::system_clock::to_time_t(now);

    std::tm timeInfo{};
#if defined(_WIN32)
    localtime_s(&timeInfo, &currentTime);
#else
    localtime_r(&currentTime, &timeInfo);
#endif

    std::ostringstream stream;
    stream << std::put_time(&timeInfo, "%Y-%m-%d %H:%M:%S");
    return stream.str();
}

}  // namespace flower_exchange
