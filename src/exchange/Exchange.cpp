#include "exchange/Exchange.h"

#include <string>
#include <string_view>
#include <utility>

#include "io/TimeProvider.h"
#include "model/Constants.h"

namespace flower_exchange {

Exchange::Exchange() : orderBooks_(), matchingEngine_() {
    initializeBooks();
}

Exchange::~Exchange() = default;

void Exchange::initializeBooks() {
    orderBooks_.clear();

    for (const std::string_view instrument : kValidInstruments) {
        const std::string instrumentName(instrument);
        orderBooks_.try_emplace(instrumentName, instrumentName, matchingEngine_);
    }
}

std::vector<ExecutionReport> Exchange::processOrder(Order order,
                                                    const TimeProvider& timeProvider) {
    const std::string& instrument = order.getInstrument();
    auto iterator = orderBooks_.find(instrument);
    if (iterator == orderBooks_.end()) {
        // Unknown instrument book: no processing is performed.
        return {};
    }

    OrderBook& orderBook = iterator->second;
    return orderBook.processOrder(std::move(order), timeProvider);
}

}  // namespace flower_exchange
