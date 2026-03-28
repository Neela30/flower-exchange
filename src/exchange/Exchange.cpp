#include "exchange/Exchange.h"

#include <string>
#include <string_view>
#include <utility>

#include "io/TimeProvider.h"
#include "model/Constants.h"

namespace flower_exchange
{

    Exchange::Exchange(OrderBookFactory orderBookFactory)
        : orderBooks_(),
          orderBookMutexes_(),
          matchingEngine_(),
          orderBookFactory_(std::move(orderBookFactory))
    {
        initializeBooks();
    }

    Exchange::~Exchange() = default;

    void Exchange::initializeBooks()
    {
        std::unique_lock<std::shared_mutex> indexLock(orderBooksIndexMutex_);
        orderBooks_.clear();
        orderBookMutexes_.clear();

        orderBookFactory_.populateBooks(matchingEngine_, orderBooks_);
        for (const auto &entry : orderBooks_)
        {
            orderBookMutexes_.try_emplace(entry.first);
        }
    }

    std::vector<ExecutionReport> Exchange::processOrder(Order order,
                                                        const TimeProvider &timeProvider)
    {
        // Shared lock protects map/index lifetime while allowing concurrent reads.
        std::shared_lock<std::shared_mutex> indexLock(orderBooksIndexMutex_);
        const std::string &instrument = order.getInstrument();
        auto bookIterator = orderBooks_.find(instrument);
        auto mutexIterator = orderBookMutexes_.find(instrument);
        if (bookIterator == orderBooks_.end() || mutexIterator == orderBookMutexes_.end())
        {
            // Unknown instrument book: no processing is performed.
            return {};
        }

        // Lock only one instrument book so different instruments can process concurrently.
        std::lock_guard<std::mutex> lock(mutexIterator->second);
        OrderBook &orderBook = bookIterator->second;
        return orderBook.processOrder(std::move(order), timeProvider);
    }

} // namespace flower_exchange
