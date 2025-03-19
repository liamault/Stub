#include "trader.hpp"
#include "stockClient/brokerageToStock.hpp"

// Constructor
//Trader::Trader(uint32_t bID, uint32_t tID) : brokerID(bID), traderID(tID) {}
// Buy stocks function (adds stock to holdings)

//checks if stock requires price increase
bool Trader::hasPriceIncrease(const std::string& ticker, const std::vector<std::string>& priceIncrease) const {
    return std::find(priceIncrease.begin(), priceIncrease.end(), ticker) != priceIncrease.end();
}
//need to send: int brokerageId, int traderId, string stockTicker, int quantity, int dollars, int cents, int transactionId, bool tipFlag, int hour, int day
bool Trader::buyStock(const std::string& ticker, uint32_t quantity, double price, uint32_t transactionID, bool tipFlag, uint32_t hour, uint32_t day, const std::vector<std::string>& priceIncrease) {
    //check for block flag
    if (blockFlag) {
        std::cerr << "Trader " << traderID << " is blocked from buying stocks!" << std::endl;
        return false;
    }
    if (hasPriceIncrease(ticker, priceIncrease)) {
        std::cerr << "Due to recent news, a price increase is applied to " << ticker << std::endl;
        price = static_cast<uint32_t>(price * 1.05);  // Increase by 5%
    }
    
    uint32_t dollars = static_cast<uint32_t>(price);  // Whole dollar amount
    uint32_t cents = static_cast<uint32_t>((price - dollars) * 100 + 0.5);  // Round cents correctly
    bool buy = true;

    bool response = sendBuySellRequest(buy, brokerID, traderID, ticker, quantity, dollars, cents , transactionID, tipFlag, hour, day);

    if (!response) {
        std::cerr << "Buy request was unsuccessful!" << std::endl;
        return false;
    }
    // If stock is not in holdings, add it
    if (stockHoldings.find(ticker) == stockHoldings.end()) {
        stockHoldings[ticker] = 0;
    }
    stockHoldings[ticker] += quantity; // Increase stock quantity
    std::cout << "Trader: " << traderID << " is buying " << quantity << " shares of " << ticker << " at $" << price << std::endl;
    return true;
    
}

// Sell stocks function (removes stock from holdings)
bool Trader::sellStock(const std::string& ticker, uint32_t quantity, double price, uint32_t transactionID, bool tipFlag, uint32_t hour, uint32_t day) {
    if (blockFlag) {
        std::cerr << "Trader " << traderID << " is blocked from buying stocks!" << std::endl;
        return false;
    }
    uint32_t dollars = static_cast<uint32_t>(price);  // Whole dollar amount
    uint32_t cents = static_cast<uint32_t>((price - dollars) * 100 + 0.5);  // Round cents correctly

    //check if trader owns stock
    if (stockHoldings.find(ticker) == stockHoldings.end()) {
        std::cerr << "Trader " << traderID << " does not have any " << ticker << " shares to sell!" << std::endl;
        return false;
    }

    if (stockHoldings[ticker] >= quantity) {//checks if trader has enough shares to sell
        bool buy = false;
        bool response = sendBuySellRequest(buy, brokerID, traderID, ticker, quantity, dollars, cents, transactionID, tipFlag, hour, day);
        //check if response is successful
        if (!response) {
            std::cerr << "Sell request was unsuccessful!" << std::endl;
            return false;
        }
        stockHoldings[ticker] -= quantity; // Reduce stock quantity
        std::cout << "Trader " << traderID << " sold " << quantity << " shares of " 
                  << ticker << ". Remaining shares: " << stockHoldings[ticker] << std::endl;
        return true;
    } else {
        std::cerr << "Trader " << traderID << " does not have enough " 
                  << ticker << " shares to sell!" << std::endl;
        return false;
    }
}

// Display trader's stock holdings
void Trader::displayHoldings() const {
    std::cout << "Trader " << traderID << " Holdings:" << std::endl;
    for (const auto& pair : stockHoldings) {
        std::cout << "  " << pair.first << ": " << pair.second << " shares" << std::endl;
    }
}


//------------------------------------------BLOCK FLAG FUNCTIONS----------------------------------------



bool Trader::checkBlockFlag() {
    return (blockFlag);
}

void Trader::updateBlockFlag(uint32_t block_duration) {
    uint32_t currentBlockDur = blockFlag;
    if (currentBlockDur < block_duration) {
        blockFlag = block_duration;
        std::cout << "New Lock set for Trader " << traderID << " for " << block_duration << " days." << std::endl;
    }
    return;
}

void Trader::decBlockFlags() {
    if (blockFlag > 0) {
        blockFlag--;
    }
    return;
}
