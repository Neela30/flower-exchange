#include "Enums.h"
#include <string>

using namespace std;
class ExecutionReport
{

public:
    ExecutionReport() {}

private:
    int client_order_id_;
    int order_id_;
    Instrument instrument_;
    Side side_;
    double price_;
    int quantity_;
    Status status_;
    std::string reason_;
};