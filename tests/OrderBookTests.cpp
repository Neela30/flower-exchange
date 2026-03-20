#include "exchange/MatchingEngine.h"
#include "exchange/OrderBook.h"

int main() {
    // TODO: Add OrderBook unit tests.
    flower_exchange::MatchingEngine matchingEngine;
    flower_exchange::OrderBook book("Rose", matchingEngine);
    (void)book;
    return 0;
}
