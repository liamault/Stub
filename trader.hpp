#ifndef TRADER_HPP
#define TRADER_HPP

#include <iostream>
#include <string>
#include <unordered_map>
#include <cstdint>

#include "common.pb.h"



// Trader class definition
class Trader{
    private:
    // Private functions
    bool checkBlockFlag();
    public:
    uint32_t brokerID;
    uint32_t traderID;
    uint32_t blockFlag = 0;
    std::unordered_map<std::string, uint32_t> stockHoldings; // { "AAPL": 10, "GOOG": 5 }

    Trader() : brokerID(0), traderID(0) {} // for empty map
    // Constructor
    Trader(uint32_t bID, uint32_t tID) : brokerID(bID), traderID(tID) {};

    // Functions for handling stock trading
    bool buyStock(const std::string& ticker, uint32_t quantity, double price, uint32_t transactionID, bool tipFlag, uint32_t hour, uint32_t day, const std::vector<std::string>& priceIncrease);
    bool sellStock(const std::string& ticker, uint32_t quantity, double price, uint32_t transactionID, bool tipFlag, uint32_t hour, uint32_t day);
    void updateBlockFlag(uint32_t block_duration);
    // Function to increment block flags
    void decBlockFlags();

    // Function to display trader's holdings
    void displayHoldings() const;

    // check for price increase
    bool hasPriceIncrease(const std::string& ticker, const std::vector<std::string>& priceIncrease) const;
};

#endif // TRADER_HPP
