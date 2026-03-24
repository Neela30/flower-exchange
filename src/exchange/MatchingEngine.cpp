#include "exchange/MatchingEngine.h"

#include <algorithm>

#include "exchange/OrderBookSide.h"
#include "io/TimeProvider.h"

namespace flower_exchange
{

    MatchingEngine::MatchingEngine() = default;

    MatchingEngine::~MatchingEngine() = default;

    bool MatchingEngine::canMatch(const Order &incoming, const Order &resting) const
    {
        if (incoming.getSide() == resting.getSide())
        {
            return false;
        }

        if (incoming.isBuy())
        {
            // Buy order crosses when it’s willing to pay at least the resting sell price
            return incoming.getPrice() >= resting.getPrice();
        }

        // Sell order crosses when it’s willing to accept at most the resting buy price
        return incoming.getPrice() <= resting.getPrice();
    }

    double MatchingEngine::determineExecutionPrice(const Order &resting) const
    {
        return resting.getPrice();
    }

    std::vector<ExecutionReport> MatchingEngine::match(Order &incoming,
                                                       OrderBookSide &oppositeSide,
                                                       OrderBookSide &sameSide,
                                                       const TimeProvider &timeProvider) const
    {
        std::vector<ExecutionReport> reports;
        while (!incoming.isFilled())
        {
            if (oppositeSide.empty())
            {
                break;
            }

            Order &resting = oppositeSide.top();
            if (!canMatch(incoming, resting))
            {
                break;
            }

            const int executedQty = std::min(incoming.getRemainingQuantity(), resting.getRemainingQuantity());
            if (executedQty <= 0)
            {
                break;
            }

            const double executionPrice = determineExecutionPrice(resting);

            incoming.reduceRemainingQuantity(executedQty);
            resting.reduceRemainingQuantity(executedQty);

            const std::string timestamp = timeProvider.nowAsString();

            ExecutionReport report1(
                incoming.getOrderId(),
                incoming.getClientOrderId(),
                incoming.getInstrument(),
                incoming.getSide(),
                incoming.isFilled() ? ExecStatus::Fill : ExecStatus::Pfill,
                executedQty,
                executionPrice,
                std::string{},
                timestamp);

            reports.push_back(report1);

            ExecutionReport report2(
                resting.getOrderId(),
                resting.getClientOrderId(),
                resting.getInstrument(),
                resting.getSide(),
                resting.isFilled() ? ExecStatus::Fill : ExecStatus::Pfill,
                executedQty,
                executionPrice,
                std::string{},
                timestamp);

            reports.push_back(report2);

            if (resting.isFilled())
            {
                oppositeSide.popTop();
            }
        }

        if (!incoming.isFilled())
        {
            sameSide.insertOrder(incoming);
        }

        return reports;
    }

} // namespace flower_exchange
