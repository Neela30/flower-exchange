#include <exception>
#include <iostream>
#include <string>

#include "trader/TraderApplication.h"

namespace
{

    void printUsage(const char *executable)
    {
        std::cerr << "Usage: " << executable << " <input_csv> <output_csv>" << std::endl;
    }

} // namespace

int main(int argc, char *argv[])
{
    try
    {
        std::string inputPath = "data/sampleOrder3.csv";
        std::string outputPath = "output/execution_rep.csv";

        if (argc == 1)
        {
            std::cout
                << "No CLI arguments detected. Using default dataset data/sampleOrder3.csv and writing to output/execution_rep.csv."
                << std::endl;
        }
        else if (argc == 3)
        {
            inputPath = argv[1];
            outputPath = argv[2];
        }
        else
        {
            printUsage(argv[0]);
            return 1;
        }

        flower_exchange::TraderApplication application;
        application.run(inputPath, outputPath);
        std::cout << "Execution report written to " << outputPath << std::endl;
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
