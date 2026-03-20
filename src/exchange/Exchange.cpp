#include "exchange/Exchange.h"

#include <tuple>
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

    for (const auto instrument : kValidInstruments) {
        orderBooks_.emplace(
            std::piecewise_construct,
            std::forward_as_tuple(std::string(instrument)),
            std::forward_as_tuple(std::string(instrument), matchingEngine_));
    }
}

std::vector<ExecutionReport> Exchange::processOrder(Order order,
                                                    const TimeProvider& timeProvider) {
    auto iterator = orderBooks_.find(order.getInstrument());
    if (iterator == orderBooks_.end()) {
        // TODO: Decide whether missing books should be created dynamically or rejected earlier.
        return {};
    }

    return iterator->second.processOrder(std::move(order), timeProvider);
}

}  // namespace flower_exchange
