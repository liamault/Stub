#ifndef BROKERAGE_TO_STOCK_HPP
#define BROKERAGE_TO_STOCK_HPP

#include <iostream>
#include <arpa/inet.h>

// Function Declarations
bool getStockAddress(const char* name, in_addr& addr);
bool sendBuySellRequest(bool buy, int brokerageId, int traderId, std::string stockTicker, int quantity, 
                        int dollars, int cents, int transactionId, bool tipFlag, int hour, int day);
bool sendBuySpecRequest(int brokerageId, int traderId, int transactionId);
bool sendCancelRequest(int brokerageId, int traderId, int transactionId);

bool sendSODQueryRequest(int brokerID, int traderID, std::string ticker);

#endif // BROKERAGE_TO_STOCK_HPP