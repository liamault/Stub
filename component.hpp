#ifndef COMPONENT_HPP
#define COMPONENT_HPP

#include <iostream>
#include <unordered_map>
#include <string>
#include <cstdint>
#include <variant>
#include <optional>
#include <vector>
#include <thread>
#include <mutex>
#include "elec477grp2-main/inputLoaders/InitializationLoader.hpp"
#include "elec477grp2-main/inputLoaders/InputFeeder.hpp"
#include "trader.hpp"
#include "regServer/regServerMain.h"
#include "stockClient/brokerageToStock.hpp"
#include "bankServer/bankToBrokerageStub.hpp"
#include "bankClient/brokerageToBank.hpp"

// Global Variables
extern std::unordered_map<uint32_t, Trader> traders;
extern std::unordered_map<uint32_t, std::unordered_map<uint32_t, std::tuple<uint32_t, uint32_t, bool>>> transactionHistory;
extern std::vector<std::string> priceIncrease;
extern std::mutex traderMutex;

// Function Declarations

// Trader Management
Trader* getTrader(uint32_t traderID);
void updateTrader(uint32_t traderID, const Trader& updatedTrader);

// Utility Functions for Extracting Values
double getDoubleValue(const std::optional<uint32_t>& optVal, double defaultValue = 0.0);
uint32_t getUIntValue(const std::optional<std::variant<int, unsigned int>>& optVal, uint32_t defaultValue = 0);
std::string getStringValue(const std::optional<std::string>& optVal);

// Component Initialization
void initComponent(const std::string& initCSV);

// Event Handlers
void buySellHandler(Event e);
void buySpecHandler(Event e);
void cancelHandler(Event e);
void newsHandler(Event e);
void sodQueryHandler(Event e);
void eodHandler(Event e);
void depositWithdrawHandler(Event e);
void eventHandler(Event e);

// Main Execution
int main(int argc, char* argv[]);

#endif // COMPONENT_HPP
