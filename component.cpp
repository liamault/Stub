#include <iostream>
#include <unordered_map>
#include <string>
#include <cstdint>
#include <variant>
#include <optional>
#include <vector>
#include <atomic> 
#include <chrono>
#include <thread>
#include <mutex>
#include <tuple>
#include <cmath>


#include "elec477grp2-main/inputLoaders/InitializationLoader.hpp"
#include "elec477grp2-main/inputLoaders/InputFeeder.hpp"
#include "trader.hpp"
#include "regServer/regServerMain.h"
#include "stockClient/brokerageToStock.hpp"
#include "bankServer/bankToBrokerageStub.hpp"
#include "bankClient/brokerageToBank.hpp"

#include "shutdownFlag.hpp"  // Flag to signal server shutdown
std::atomic<bool> shutdownFlag(false);

// Global map to store all traders (traderID -> Trader object)
std::unordered_map<uint32_t, Trader> traders;
//global map to store successful transaction histories, first key = day, second key = transaction ID and value stores the trader ID, broker ID, Buy/Sell
std::unordered_map<uint32_t, std::unordered_map<uint32_t, std::tuple<uint32_t, uint32_t, bool>>> transactionHistory;
//global vector to store price increases due to news
std::vector<std::string> priceIncrease; 
std::mutex traderMutex; // Mutex to protect traders map

// Function to get a trader by ID (returns a pointer)
Trader* getTrader(uint32_t traderID) {
    std::lock_guard<std::mutex> lock(traderMutex);
    auto it = traders.find(traderID);
    if (it != traders.end()) {
        return &(it->second);  // Return pointer to trader
    }
    return nullptr;  // Trader not found
}

// Function to update a trader object
void updateTrader(uint32_t traderID, const Trader& updatedTrader) {
    std::lock_guard<std::mutex> lock(traderMutex);
    if (traders.find(traderID) != traders.end()) {
        traders[traderID] = updatedTrader;  // Update trader object
    }
}

//functions to safely extract values from optional fields
double getDoubleValue(const std::optional<uint32_t>& optVal, double defaultValue = 0.0) {
    return optVal.has_value() ? static_cast<double>(optVal.value()) : defaultValue;
}
uint32_t getUIntValue(const std::optional<std::variant<int, unsigned int>>& optVal, uint32_t defaultValue = 0) {
    if (optVal.has_value()) {
        return std::visit([](auto&& arg) -> uint32_t { return static_cast<uint32_t>(arg); }, optVal.value());
    }
    return defaultValue;
}
std::string getStringValue(const std::optional<std::string>& optVal) {
    return optVal.has_value() ? optVal.value() : "";  // Return empty string instead of null
}


//initialize component
void initComponent(std::string initCSV){
    InitializationLoader initLoader(initCSV);
    auto fileData = initLoader.CSVtoVector();

    std::cout << "Initialization Data: " << std::endl;
    for (const auto &row : fileData){
        if (row.size() < 4) {  
            std::cerr << "Error: Malformed CSV row. Skipping..." << std::endl;
            continue;  // Prevents accessing missing fields
        }
        uint32_t brokerID = static_cast<uint32_t>(std::stoul(row[0]));
        uint32_t traderID = static_cast<uint32_t>(std::stoul(row[1]));
        std::string ticker = row[2].empty() ? "UNKNOWN" : row[2];  // Avoid null strings
        uint32_t quantity = static_cast<uint32_t>(std::stoul(row[3]));


        auto [it, inserted] = traders.emplace(traderID, Trader(brokerID, traderID));
        it->second.stockHoldings[ticker] = quantity;

    }

    for (const auto& [id, trader] : traders) {//display holdings
        trader.displayHoldings();
    }
}
// Event handler for buy and sell events
void buySellHandler(Event e){
    uint32_t traderID = getUIntValue(e.Op1);
    std::string ticker = getStringValue(e.Op2);
    uint32_t quantity = getUIntValue(e.Op3);
    double price = getDoubleValue(e.Op4);
    uint32_t TransactionID = getUIntValue(e.Op5);
    bool tipFlag = e.Op6.value_or(false);
    uint32_t hour = e.hourTimestamp;
    uint32_t day = e.dayTimestamp;

    if (ticker.empty()) {
        std::cerr << "Error: Missing ticker value for event " << e.eventID << std::endl;
        return;  // Skip processing if ticker is empty
    }
    if (traders.find(traderID) == traders.end()) {
        std::cerr << "Error: Trader " << traderID << " not found." << std::endl;
        return;  // Skip processing if trader is not found
    }

    bool response;

    if (e.eventID == "BUY") {
        bool response = traders[traderID].buyStock(ticker, quantity, price, TransactionID, tipFlag, hour, day, priceIncrease);
    } else if (e.eventID == "SELL") {
        bool response = traders[traderID].sellStock(ticker, quantity, price, TransactionID, tipFlag, hour, day);
    }
    if (response && e.eventID == "BUY" ) {
        cout << "Transaction " << TransactionID << " was successful." << std::endl;
        transactionHistory[day][TransactionID] = std::make_tuple(traderID, traders[traderID].brokerID, true);
    }
    else if (response && e.eventID == "SELL") {
        cout << "Transaction " << TransactionID << " was successful." << std::endl;
        transactionHistory[day][TransactionID] = std::make_tuple(traderID, traders[traderID].brokerID, false);
    }
}

void buySpecHandler(Event e){
    uint32_t day = e.dayTimestamp;
    uint32_t TransactionID = getUIntValue(e.Op1);
    if (transactionHistory.find(day) == transactionHistory.end() || transactionHistory[day].find(TransactionID) == transactionHistory[day].end()) {
        std::cerr << "Error: Transaction " << TransactionID << " not found." << std::endl;
        //print all transaction tuples under current day
        
        return;  // Skip processing if transaction is not found
    }
    //check if transaction was a sell
    if (std::get<2>(transactionHistory[day][TransactionID]) == true) {
        std::cerr << "Error: Transaction " << TransactionID << " was a buy transaction." << std::endl;
        return;  // Skip processing if transaction was a buy
    }
    //get traderID and brokerID from the transaction history
    uint32_t traderID = std::get<0>(transactionHistory[day][TransactionID]);
    uint32_t brokerID = std::get<1>(transactionHistory[day][TransactionID]);

    bool response = sendBuySpecRequest(brokerID, traderID, TransactionID);
    if (!response) {
        std::cerr << "BuySpec request was unsuccessful!" << std::endl;
        return;
    }

}

void cancelHandler(Event e){
    uint32_t day = e.dayTimestamp;
    uint32_t TransactionID = getUIntValue(e.Op1);
    if (transactionHistory.find(day) == transactionHistory.end() || transactionHistory[day].find(TransactionID) == transactionHistory[day].end()) {
        std::cerr << "Error: Transaction " << TransactionID << " not found." << std::endl;
        return;  // Skip processing if transaction is not found
    }
    //get traderID and brokerID from the transaction history
    uint32_t traderID = std::get<0>(transactionHistory[day][TransactionID]);
    uint32_t brokerID = std::get<1>(transactionHistory[day][TransactionID]);

    bool response = sendCancelRequest(brokerID, traderID, TransactionID);
    if (!response) {
        std::cerr << "Cancel request was unsuccessful!" << std::endl;
        return;
    }

    //remove transaction from transaction history
    transactionHistory[day].erase(TransactionID);
}

void newsHandler(Event e){
    std::string ticker = getStringValue(e.Op2);
    uint32_t value = getUIntValue(e.Op1);
    //check if value = 1
    if (value == 1) {
        priceIncrease.push_back(ticker);
    }
        
}

void sodQueryHandler(Event e){
    uint32_t brokerID = 1;
    uint32_t traderID = 1;
    std::string ticker = "ABC";

    bool response = sendSODQueryRequest(brokerID, traderID, ticker);
    if (!response) {
        std::cerr << "SOD Query request was unsuccessful!" << std::endl;
        return;
    }
}

void eodHandler(Event e){
    for (auto& [traderID, trader] : traders) {  
        trader.decBlockFlags();  // Call the function for each trader
    }

}

void depositWithdrawHandler(Event e){
    uint32_t brokerageID = 1;
    double amount = getDoubleValue(getUIntValue(e.Op1));

    uint32_t dollars = static_cast<uint32_t>(amount); 
    uint32_t cents = static_cast<uint32_t>((amount - dollars) * 100 + 0.5);

    cout << "Sending deposit/withdraw request to bank" << endl;
    sendTransactionRequest(brokerageID, dollars, cents, e.eventID == "DEPOSIT");
}
    
void eventHandler(Event e) {
    if (e.eventID == "BUY"){buySellHandler(e);}
    else if (e.eventID == "SELL"){buySellHandler(e);}
    else if (e.eventID == "BUYSPEC"){buySpecHandler(e);}
    else if (e.eventID == "CANCEL"){cancelHandler(e);}
    else if (e.eventID == "EOD"){eodHandler(e);}//decrement block 
    else if (e.eventID == "SOD"){sodQueryHandler(e);}//send query on start of day for all transctions under brokerage 1 ticker = ABC
    else if (e.eventID == "NEWS"){newsHandler(e);}
    else if (e.eventID == "DEPOSIT"){depositWithdrawHandler(e);}
    else if (e.eventID == "WITHDRAW"){depositWithdrawHandler(e);}
}


int main(int argc, char* argv[]){
    if (argc < 2) {
        std::cerr << "Error: Missing start time argument." << std::endl;
        return 1;
    }
    time_t eventStartTime = static_cast<time_t>(std::stol(argv[1]));
    initComponent("/home/ubuntu/broker1_init.csv");

    regServer regulatoryServer;
    std::thread regServerThread(&regServer::startServer, &regulatoryServer);
    std::thread bankServerThread(startBankServer);

    InputFeeder inputFeeder("/home/ubuntu/broker1.csv", eventStartTime, eventHandler);

    inputFeeder.waitUntilDone();

    // **Now, wait 5 seconds before shutting down**
    std::cout << "All events processed. Waiting 5 seconds before stopping servers...\n";
    std::this_thread::sleep_for(std::chrono::seconds(5));


    // **Trigger shutdown of both servers**
    std::cout << "Stopping servers now...\n";
    shutdownFlag.store(true);

    // Wait for the regulatory server to finish (if needed)
    regServerThread.join();
    bankServerThread.join();

    std::cout << "Servers have shut down. Exiting program." << std::endl;
    return 0;
}
