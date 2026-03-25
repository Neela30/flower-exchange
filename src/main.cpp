#include <exception>
#include <iostream>
#include <string>

#include "trader/TraderApplication.h"

int main(int argc, char* argv[]) {
    try {
        const bool useParallelMode = argc > 1 && std::string(argv[1]) == "--parallel";

        flower_exchange::TraderApplication application;
        if (useParallelMode) {
            application.runParallel("data/orders.csv", "output/execution_rep.csv");
        } else {
            application.run("data/orders.csv", "output/execution_rep.csv");
        }
        return 0;
    } catch (const std::exception& exception) {
        std::cerr << "FlowerExchange failed: " << exception.what() << '\n';
    } catch (...) {
        std::cerr << "FlowerExchange failed with an unknown error.\n";
    }

    return 1;
}
