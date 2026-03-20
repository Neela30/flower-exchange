#include "exchange/OrderBookSide.h"

#include <stdexcept>
#include <utility>

namespace flower_exchange {

OrderBookSide::OrderBookSide(Side side) : side_(side), orders_() {}

OrderBookSide::~OrderBookSide() = default;

void OrderBookSide::insertOrder(const Order& order) {
    // TODO: Insert with price-time priority when matching rules are implemented.
    orders_.push_back(order);
}

void OrderBookSide::insertOrder(Order&& order) {
    // TODO: Insert with price-time priority when matching rules are implemented.
    orders_.push_back(std::move(order));
}

bool OrderBookSide::empty() const {
    return orders_.empty();
}

Order& OrderBookSide::top() {
    if (orders_.empty()) {
        throw std::out_of_range("OrderBookSide::top called on empty side");
    }

    return orders_.front();
}

const Order& OrderBookSide::top() const {
    if (orders_.empty()) {
        throw std::out_of_range("OrderBookSide::top called on empty side");
    }

    return orders_.front();
}

void OrderBookSide::popTop() {
    if (!orders_.empty()) {
        orders_.pop_front();
    }
}

std::list<Order>& OrderBookSide::getOrders() {
    return orders_;
}

const std::list<Order>& OrderBookSide::getOrders() const {
    return orders_;
}

Side OrderBookSide::getSide() const {
    return side_;
}

}  // namespace flower_exchange
