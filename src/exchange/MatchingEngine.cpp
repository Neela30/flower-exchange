#include "exchange/MatchingEngine.h"

#include "exchange/OrderBookSide.h"
#include "io/TimeProvider.h"

namespace flower_exchange {

MatchingEngine::MatchingEngine() = default;

MatchingEngine::~MatchingEngine() = default;

bool MatchingEngine::canMatch(const Order& incoming, const Order& resting) const {
    (void)incoming;
    (void)resting;
    // TODO: Implement price/side compatibility checks.
    return false;
}

double MatchingEngine::determineExecutionPrice(const Order& resting) const {
    // TODO: Revisit pricing rules once the matching policy is defined.
    return resting.getPrice();
}

std::vector<ExecutionReport> MatchingEngine::match(Order& incoming,
                                                   OrderBookSide& oppositeSide,
                                                   OrderBookSide& sameSide,
                                                   const TimeProvider& timeProvider) const {
    (void)incoming;
    (void)oppositeSide;
    (void)sameSide;
    (void)timeProvider;
    // TODO: Implement the matching workflow and execution report generation.
    return {};
}

}  // namespace flower_exchange
