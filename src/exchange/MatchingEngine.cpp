#include "exchange/MatchingEngine.h"

#include <algorithm>
#include <memory>
#include <utility>

#include "exchange/OrderBookSide.h"
#include "exchange/PriceTimePriorityStrategy.h"
#include "io/TimeProvider.h"

namespace flower_exchange {

MatchingEngine::MatchingEngine()
    : MatchingEngine(std::make_unique<PriceTimePriorityStrategy>()) {}

MatchingEngine::MatchingEngine(std::unique_ptr<IMatchingStrategy> strategy)
    : strategy_(std::move(strategy)) {
    if (!strategy_) {
        strategy_ = std::make_unique<PriceTimePriorityStrategy>();
    }
}

MatchingEngine::~MatchingEngine() = default;

bool MatchingEngine::canMatch(const Order& incoming, const Order& resting) const {
    return strategy_->canMatch(incoming, resting);
}

double MatchingEngine::determineExecutionPrice(const Order& resting) const {
    return strategy_->determineExecutionPrice(resting);
}

std::vector<ExecutionReport> MatchingEngine::match(Order& incoming,
                                                   OrderBookSide& oppositeSide,
                                                   OrderBookSide& sameSide,
                                                   const TimeProvider& timeProvider) const {
    std::vector<ExecutionReport> reports;
    reports.reserve(8);

    while (!incoming.isFilled()) {
        if (oppositeSide.empty()) {
            break;
        }

        Order& resting = oppositeSide.top();
        if (!canMatch(incoming, resting)) {
            break;
        }

        const int executedQty =
            std::min(incoming.getRemainingQuantity(), resting.getRemainingQuantity());
        if (executedQty <= 0) {
            break;
        }

        const double executionPrice = determineExecutionPrice(resting);

        incoming.reduceRemainingQuantity(executedQty);
        resting.reduceRemainingQuantity(executedQty);

        const std::string timestamp = timeProvider.nowAsString();

        reports.emplace_back(incoming.getOrderId(),
                             incoming.getClientOrderId(),
                             incoming.getInstrument(),
                             incoming.getSide(),
                             incoming.isFilled() ? ExecStatus::Fill : ExecStatus::Pfill,
                             executedQty,
                             executionPrice,
                             std::string{},
                             timestamp);

        reports.emplace_back(resting.getOrderId(),
                             resting.getClientOrderId(),
                             resting.getInstrument(),
                             resting.getSide(),
                             resting.isFilled() ? ExecStatus::Fill : ExecStatus::Pfill,
                             executedQty,
                             executionPrice,
                             std::string{},
                             timestamp);

        if (resting.isFilled()) {
            oppositeSide.popTop();
        }
    }

    if (!incoming.isFilled()) {
        sameSide.insertOrder(incoming);
    }

    return reports;
}

}  // namespace flower_exchange
