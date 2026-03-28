#pragma once

#include <string>

namespace flower_exchange
{

    class ITimeProvider
    {
    public:
        virtual ~ITimeProvider() = default;
        virtual std::string nowAsString() const = 0;
    };

    /**
     * Default implementation that returns the current local timestamp string.
     */
    class TimeProvider : public ITimeProvider
    {
    public:
        TimeProvider();
        ~TimeProvider() override;

        std::string nowAsString() const override;
    };

} // namespace flower_exchange
