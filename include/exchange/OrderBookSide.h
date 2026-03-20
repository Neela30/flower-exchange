#pragma once

#include <list>

#include "model/Order.h"

namespace flower_exchange {

/**
 * Represents one side of an instrument order book and preserves order priority.
 */
class OrderBookSide {
public:
    explicit OrderBookSide(Side side);
    ~OrderBookSide();

    void insertOrder(const Order& order);
    void insertOrder(Order&& order);
    bool empty() const;
    Order& top();
    const Order& top() const;
    void popTop();
    std::list<Order>& getOrders();
    const std::list<Order>& getOrders() const;
    Side getSide() const;

private:
    Side side_;
    std::list<Order> orders_;
};

}  // namespace flower_exchange
