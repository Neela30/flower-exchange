#include "exchange/OrderBookFactory.h"

#include <utility>

#include "model/Constants.h"

namespace flower_exchange
{

    OrderBookFactory::OrderBookFactory()
        : instruments_(kValidInstruments.begin(), kValidInstruments.end()) {}

    OrderBookFactory::OrderBookFactory(std::vector<std::string> instruments)
        : instruments_(std::move(instruments)) {}

    void OrderBookFactory::populateBooks(MatchingEngine &matchingEngine,
                                         std::map<std::string, OrderBook> &destination) const
    {
        for (const std::string &instrument : instruments_)
        {
            destination.try_emplace(instrument, instrument, matchingEngine);
        }
    }

    const std::vector<std::string> &OrderBookFactory::instruments() const
    {
        return instruments_;
    }

} // namespace flower_exchange
