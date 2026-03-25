#include "exchange/OrderBookSide.h"
#include "model/Order.h"

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace
{

    using flower_exchange::Order;
    using flower_exchange::OrderBookSide;
    using flower_exchange::Side;

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

    void verifyOrderSequence(const OrderBookSide &side, const std::vector<std::string> &expectedOrderIds)
    {
        const auto &orders = side.getOrders();
        auto it = orders.begin();
        for (const auto &id : expectedOrderIds)
        {
            expect(it != orders.end(), "OrderBookSide contained fewer orders than expected");
            expect(it->getOrderId() == id, "OrderBookSide priority order mismatch");
            ++it;
        }
        expect(it == orders.end(), "OrderBookSide contained more orders than expected");
    }

    void testBuySidePriceTimePriority()
    {
        OrderBookSide buySide(Side::Buy);
        buySide.insertOrder(makeOrder("c1", "o1", Side::Buy, 100, 100.0, 10));
        buySide.insertOrder(makeOrder("c2", "o2", Side::Buy, 100, 101.0, 11));
        buySide.insertOrder(makeOrder("c3", "o3", Side::Buy, 100, 101.0, 5));

        verifyOrderSequence(buySide, {"o3", "o2", "o1"});
    }

    void testSellSidePriceTimePriority()
    {
        OrderBookSide sellSide(Side::Sell);
        sellSide.insertOrder(makeOrder("c1", "o1", Side::Sell, 100, 10.0, 3));
        sellSide.insertOrder(makeOrder("c2", "o2", Side::Sell, 100, 9.5, 7));
        sellSide.insertOrder(makeOrder("c3", "o3", Side::Sell, 100, 9.5, 5));

        verifyOrderSequence(sellSide, {"o3", "o2", "o1"});
    }

    void testPopTopRemovesHighestPriority()
    {
        OrderBookSide buySide(Side::Buy);
        buySide.insertOrder(makeOrder("c1", "o1", Side::Buy, 100, 100.0, 1));
        buySide.insertOrder(makeOrder("c2", "o2", Side::Buy, 100, 101.0, 2));

        expect(buySide.top().getOrderId() == "o2", "Top order should be highest priority");
        buySide.popTop();
        expect(buySide.top().getOrderId() == "o1", "Pop should remove the previous top order");
        buySide.popTop();
        expect(buySide.empty(), "All orders should be removed after consecutive pop operations");
    }

} // namespace

int main()
{
    try
    {
        testBuySidePriceTimePriority();
        testSellSidePriceTimePriority();
        testPopTopRemovesHighestPriority();
    }
    catch (const std::exception &ex)
    {
        std::cerr << "OrderBookSideTests failure: " << ex.what() << '\n';
        return 1;
    }
    return 0;
}
