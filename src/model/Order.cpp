#include "Enums.h"
#include "io/TimeProvider.h"

using namespace std;

class OrderBook {

public:
    OrderBook(){
    }


private:
    int client_order_id_;
    Instrument instrument_;
    Side side_;
    double price_;
    int quantity_;

};

