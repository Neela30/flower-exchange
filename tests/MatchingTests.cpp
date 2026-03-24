#include "exchange/MatchingEngine.h"
#include "exchange/OrderBookSide.h"
#include "io/TimeProvider.h"
#include "model/ExecutionReport.h"
#include "model/Order.h"

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace
{

    using flower_exchange::ExecStatus;
    using flower_exchange::MatchingEngine;
    using flower_exchange::Order;
    using flower_exchange::OrderBookSide;
    using flower_exchange::Side;
    using flower_exchange::TimeProvider;

    Order makeOrder(const std::string &clientOrderId,
                    const std::string &orderId,
                    Side side,
                    int quantity,
                    double price,
                    std::uint64_t sequenceNumber)
    {
        Order order(clientOrderId, "Rose", side, quantity, price);
        order.setOrderId(orderId);
        order.setSequenceNumber(sequenceNumber);
        return order;
    }

    void expect(bool condition, const std::string &message)
    {
        if (!condition)
        {
            throw std::runtime_error(message);
        }
    }

    void testBuyOrderMatchesRestingSell()
    {
        MatchingEngine engine;
        OrderBookSide sellSide(Side::Sell);
        OrderBookSide buySide(Side::Buy);
        TimeProvider timeProvider;

        Order resting = makeOrder("client-rest", "rest-1", Side::Sell, 30, 9.50, 10);
        sellSide.insertOrder(resting);

        Order incoming = makeOrder("client-inc", "inc-1", Side::Buy, 50, 10.00, 11);

        auto reports = engine.match(incoming, sellSide, buySide, timeProvider);

        expect(reports.size() == 2, "Expected two execution reports for a single match");
        expect(reports[0].getOrderId() == "inc-1", "Incoming order report missing");
        expect(reports[0].getStatus() == ExecStatus::Pfill, "Incoming order should be partially filled");
        expect(reports[0].getExecutedQuantity() == 30, "Executed quantity mismatch for incoming order");
        expect(reports[0].getExecutedPrice() == 9.50, "Execution price should match resting order price");
        expect(reports[1].getOrderId() == "rest-1", "Resting order report missing");
        expect(reports[1].getStatus() == ExecStatus::Fill, "Resting order should be fully filled");
        expect(reports[1].getExecutedQuantity() == 30, "Executed quantity mismatch for resting order");
        expect(reports[0].getTransactionTime() == reports[1].getTransactionTime(), "Reports in the same match must share a timestamp");

        expect(sellSide.empty(), "Filled resting order should be removed from the book");
        expect(!buySide.empty(), "Residual buy quantity should rest on the book");
        expect(buySide.top().getRemainingQuantity() == 20, "Residual buy quantity incorrect");
    }

    void testSellOrderMatchesRestingBuy()
    {
        MatchingEngine engine;
        OrderBookSide buySide(Side::Buy);
        OrderBookSide sellSide(Side::Sell);
        TimeProvider timeProvider;

        Order resting = makeOrder("client-rest", "rest-2", Side::Buy, 40, 11.00, 7);
        buySide.insertOrder(resting);

        Order incoming = makeOrder("client-inc", "inc-2", Side::Sell, 40, 10.50, 9);

        auto reports = engine.match(incoming, buySide, sellSide, timeProvider);

        expect(reports.size() == 2, "Expected two execution reports for the sell match");
        expect(reports[0].getOrderId() == "inc-2", "Incoming sell report missing");
        expect(reports[0].getStatus() == ExecStatus::Fill, "Incoming sell should fill completely");
        expect(reports[1].getOrderId() == "rest-2", "Resting buy report missing");
        expect(reports[1].getStatus() == ExecStatus::Fill, "Resting buy should fill completely");
        expect(buySide.empty(), "Filled resting buy should be removed");
        expect(sellSide.empty(), "No residual sell orders expected");
    }

    void testNonCrossingOrderRestsOnSameSide()
    {
        MatchingEngine engine;
        OrderBookSide sellSide(Side::Sell);
        OrderBookSide buySide(Side::Buy);
        TimeProvider timeProvider;

        Order resting = makeOrder("client-rest", "rest-3", Side::Sell, 50, 12.00, 5);
        sellSide.insertOrder(resting);

        Order incoming = makeOrder("client-inc", "inc-3", Side::Buy, 60, 10.00, 6);

        auto reports = engine.match(incoming, sellSide, buySide, timeProvider);

        expect(reports.empty(), "Non-crossing orders must not generate execution reports");
        expect(!sellSide.empty(), "Resting order should remain on the book");
        expect(!buySide.empty(), "Incoming order should rest on its own side");
        expect(buySide.top().getOrderId() == "inc-3", "Residual order not inserted on the same side");
        expect(buySide.top().getRemainingQuantity() == 60, "Residual quantity should equal original size");
    }

} // namespace

int main()
{
    try
    {
        testBuyOrderMatchesRestingSell();
        testSellOrderMatchesRestingBuy();
        testNonCrossingOrderRestsOnSameSide();
    }
    catch (const std::exception &ex)
    {
        std::cerr << "MatchingTests failure: " << ex.what() << '\n';
        return 1;
    }
    return 0;
}
