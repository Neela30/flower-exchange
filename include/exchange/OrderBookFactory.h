#pragma once

#include <map>
#include <string>
#include <string_view>
#include <vector>

#include "exchange/OrderBook.h"

namespace flower_exchange
{

    class MatchingEngine;

    /**
     * Builds the configured collection of order books so Exchange no longer needs
     * to know how instruments are enumerated.
     */
    class OrderBookFactory
    {
    public:
        OrderBookFactory();
        explicit OrderBookFactory(std::vector<std::string> instruments);

        // Populates the provided map with one order book per configured instrument.
        void populateBooks(MatchingEngine &matchingEngine,
                           std::map<std::string, OrderBook> &destination) const;

        // Exposes the configured instruments for callers that need to inspect them.
        const std::vector<std::string> &instruments() const;

    private:
        std::vector<std::string> instruments_;
    };

} // namespace flower_exchange
