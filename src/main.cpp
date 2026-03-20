#include <exception>
#include <iostream>

#include "trader/TraderApplication.h"

int main() {
    try {
        flower_exchange::TraderApplication application;
        application.run("data/orders.csv", "output/execution_rep.csv");
        return 0;
    } catch (const std::exception& exception) {
        std::cerr << "FlowerExchange failed: " << exception.what() << '\n';
    } catch (...) {
        std::cerr << "FlowerExchange failed with an unknown error.\n";
    }

    return 1;
}
