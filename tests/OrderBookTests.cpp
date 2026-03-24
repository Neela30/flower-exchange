#include "exchange/MatchingEngine.h"
#include "exchange/OrderBook.h"
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
    using flower_exchange::OrderBook;
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

    void testProcessOrderMatchesRestingSell()
    {
        MatchingEngine engine;
        OrderBook book("Rose", engine);
        TimeProvider timeProvider;

        Order resting = makeOrder("client-rest", "rest-1", Side::Sell, 30, 9.75, 5);
        book.getSellSide().insertOrder(resting);

        Order incoming = makeOrder("client-inc", "inc-1", Side::Buy, 50, 10.00, 6);

        auto reports = book.processOrder(incoming, timeProvider);

        expect(reports.size() == 2, "OrderBook should surface matching engine reports");
        expect(reports[0].getOrderId() == "inc-1", "Incoming order report missing");
        expect(reports[0].getStatus() == ExecStatus::Pfill, "Incoming order should be partially filled");
        expect(reports[0].getExecutedQuantity() == 30, "Executed quantity mismatch for incoming order");
        expect(reports[1].getOrderId() == "rest-1", "Resting order report missing");
        expect(reports[1].getStatus() == ExecStatus::Fill, "Resting order should be filled");

        expect(book.getSellSide().empty(), "Filled resting sell should be removed from the book");
        expect(!book.getBuySide().empty(), "Residual buy quantity should rest on the book");
        expect(book.getBuySide().top().getRemainingQuantity() == 20, "Residual quantity incorrect");
    }

    void testProcessOrderWithNoOppositeLiquidityRestsOrder()
    {
        MatchingEngine engine;
        OrderBook book("Rose", engine);
        TimeProvider timeProvider;

        Order incoming = makeOrder("client-inc", "inc-2", Side::Buy, 40, 8.50, 2);

        auto reports = book.processOrder(incoming, timeProvider);

        expect(reports.empty(), "No matches should yield no execution reports");
        expect(!book.getBuySide().empty(), "Incoming order should rest on the buy side");
        expect(book.getBuySide().top().getOrderId() == "inc-2", "Residual order ID mismatch");
        expect(book.getBuySide().top().getRemainingQuantity() == 40, "Quantity should remain intact when no match occurs");
    }

} // namespace

int main()
{
    try
    {
        testProcessOrderMatchesRestingSell();
        testProcessOrderWithNoOppositeLiquidityRestsOrder();
    }
    catch (const std::exception &ex)
    {
        std::cerr << "OrderBookTests failure: " << ex.what() << '\n';
        return 1;
    }
    return 0;
}
