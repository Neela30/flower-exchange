#include <exception>
#include <iostream>
#include <string>

#include "trader/TraderApplication.h"

int main(int argc, char* argv[]) {
    const std::string defaultInputPath = "data/orders.csv";
    const std::string defaultOutputPath = "output/execution_rep.csv";

    std::string inputPath = defaultInputPath;
    std::string outputPath = defaultOutputPath;

    if (argc == 3) {
        inputPath = argv[1];
        outputPath = argv[2];
    } else if (argc != 1) {
        const std::string executableName = argc > 0 ? argv[0] : "flower_exchange";
        std::cerr << "Usage: " << executableName << " [<input_csv> <output_csv>]\n"
                  << "Examples:\n"
                  << "  " << executableName << '\n'
                  << "  " << executableName << " data/sampleOrder1.csv output/execReports1.csv\n";
        return 1;
    }

    try {
        flower_exchange::TraderApplication application;
        application.runParallel(inputPath, outputPath);
        return 0;
    }
    catch (const std::exception &exception)
    {
        std::cerr << "FlowerExchange failed: " << exception.what() << '\n';
    }
    catch (...)
    {
        std::cerr << "FlowerExchange failed with an unknown error.\n";
    }

    return 1;
}
