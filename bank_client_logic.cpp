//load brokerage init data - initComponent
//merge event data - ./merge_events.sh broker1_events.csv news.csv broker1.csv
//event handling
//implement network communication
#include <cstdint> 
#include "elec477grp2-main/inputLoaders/InitializationLoader.hpp"
#include "elec477grp2-main/inputLoaders/InputFeeder.hpp"

void initComponent(string initCSV){

    InitializationLoader initLoader(initCSV);
    //col1: brokerID, col2: traderID, col3: ticker, col4: quantity
    uint32_t brokerID, traderID, quantity; 
    std::string ticker;

    auto fileData = initLoader.CSVtoVector();

    std::cout<<"Initialization Data: "<<std::endl;
    for (const auto &row : fileData){
        brokerID  = static_cast<uint32_t>(std::stoul(row[0]));
        traderID = static_cast<uint32_t>(std::stoul(row[1]));
        ticker = row[2];
        quantity = static_cast<uint32_t>(std::stoul(row[3]));

        std:: cout << "BrokerID: " << brokerID << " TraderID: " << traderID << " Ticker: " << ticker << " Quantity: " << quantity << std::endl;
    }
}

void eventHandler(Event e) {
    // if (e.eventID == "BUY") {
    //     std:: cout<<"Trader " << e.Op1 << " is buying " << e.Op3 
    //               << " shares of " << e.Op2 << " at $" << e.Op4 << std::endl;
    // } else if (e.eventID == "SELL") {
    //     std::cout << "Trader " << e.Op1 << " is selling " << e.Op3 
    //               << " shares of " << e.Op2 << " at $" << e.Op4 << std::endl;
    // } else if (e.eventID == "NEWS") {
    //     std::cout << "News Event: " << e.Op2 << " sentiment: " << e.Op3 << std::endl;
    // }

    if (e.eventID == "BUY"){}
    else if (e.eventID == "SELL"){}
    else if (e.eventID == "BUYSPEC"){}
    else if (e.eventID == "CANCEL"){}
    else if (e.eventID == "DEPOSIT"){}
    else if (e.eventID == "WITHDRAW"){}
    else if (e.eventID == "NEWS"){}
    else if (e.eventID == "EOD"){}
}

void buySellHandler(Event e){
    uint32_t traderID = static_cast<uint32_t>(std::stoul(e.Op1));
    std::string ticker = e.Op2;
    uint32_t quantity = static_cast<uint32_t>(std::stoul(e.Op3));
    

    if (e.eventID == "BUY") { 
        std::cout << "Trader: " << traderID << " is buying " << quantity << " shares of " << ticker << " at $" << ticker << std::endl;
    } else if (e.eventID == "SELL") {
        std::cout << "Trader: " << traderID << " is selling " << quantity << " shares of " << ticker << " at $" << e.Op4 << std::endl;
    }
}

int main(int argc, char* argv[]){
    time_t eventStartTime = std::stol(argv[1]);

    initComponent("elec477grp2-main/input_files/broker1_init.csv");
    InputFeeder inputFeeder("broker1.csv", eventStartTime, eventHandler);
    
    return 0;
}