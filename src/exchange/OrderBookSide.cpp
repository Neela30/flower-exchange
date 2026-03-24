#include "exchange/OrderBookSide.h"

#include <stdexcept>
#include <utility>

namespace flower_exchange {

OrderBookSide::OrderBookSide(Side side) : side_(side), orders_() {}

OrderBookSide::~OrderBookSide() = default;

void OrderBookSide::insertOrder(const Order& order) {
    auto it = orders_.begin();
    for (; it != orders_.end(); it++){
        const Order& current = *it;
        if (betterPrice(current, order) || samePriceHigherPriority(current, order)){
            break;
        }
   
    }
    orders_.insert(it, order);
}

bool OrderBookSide::betterPrice(const Order& current, const Order& newOrder){
    if (side_ == Side::Buy){
        if (newOrder.getPrice()>current.getPrice()){ return true; }
    } else {
        if (newOrder.getPrice()<current.getPrice()){ return true; }
    }
    return false;
}

bool OrderBookSide::samePriceHigherPriority(const Order& current, const Order& newOrder){
    return newOrder.getPrice() == current.getPrice() && newOrder.getSequenceNumber() < current.getSequenceNumber();
}

void OrderBookSide::insertOrder(Order&& order) {
    auto it = orders_.begin();
    for (; it != orders_.end(); ++it) {
        if (betterPrice(*it, order) || samePriceHigherPriority(*it, order)) {
            break;
        }
    }
    orders_.insert(it, std::move(order));
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
