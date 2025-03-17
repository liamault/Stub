#include <iostream>
#include <cstring>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "common.pb.h"
#include "stock_exchange.pb.h"
#include "brokerageToStock.hpp"
#include "../naming/svcDirClient.hpp"



using namespace std;
// static string serverName = "localhost";
// static unsigned short stockClientPort = 1337;
static uint32_t stockClientMaxMesg = 2048;
string serviceName_stock = "StockExchange";
string serverAddress_stock = "ServiceServer.elec477grp2";
static atomic<uint32_t> serialNumber_stock = 1;


bool getStockAddress(const char* name, in_addr& addr) {
    struct addrinfo *addr_result;
    struct addrinfo hints;

    // IPv4, UDP
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;  // IPv4 only
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = 0;
    hints.ai_flags = 0;

    int errcode = getaddrinfo(name, nullptr, &hints, &addr_result);
    if (errcode != 0) {
        cerr << "Error finding address of " << name << ": " << gai_strerror(errcode) << endl;
        return false;
    }

    // Iterate through available addresses
    struct addrinfo *rover = addr_result;
    while (rover != nullptr) {
        struct sockaddr_in* ipv4 = (struct sockaddr_in*)rover->ai_addr;
        if (ipv4->sin_addr.s_addr != 0) {
            cout << "Address of server " << name << ": " << inet_ntoa(ipv4->sin_addr) << endl;
            addr = ipv4->sin_addr;
            freeaddrinfo(addr_result);
            return true;
        }
        rover = rover->ai_next;
    }

    cerr << "No valid address found for " << name << endl;
    freeaddrinfo(addr_result);
    return false;
}

svcDir::serverEntity getEntity(){
    if (!svcDir::setSeverAddress(serverAddress_stock)) {
        cerr << "Failed to set server address!" << endl;
        exit(1);
    }
    else { cout << "Server Address set." << endl;}

    svcDir::serverEntity entity = svcDir::searchService(serviceName_stock);
    if (entity.name == "init" || entity.port == 0) {
        std::cerr << "Error: No entity found for service " << serviceName_stock << std::endl;
        exit(1); // Exit program if service was not found
    }
    else{cout << "Service StockExchange found" << endl;}
    return entity;
}


bool sendBuySellRequest(bool buy, int brokerageId, int traderId, string stockTicker, int quantity, int dollars, int cents, int transactionId, bool tipFlag, int hour, int day){
    svcDir::serverEntity entity = getEntity();
    std::string serverName = entity.name;
    unsigned short stockClientPort = entity.port;
    
    
    int sockfd;
    struct sockaddr_in servaddr;
    char buffer[stockClientMaxMesg];
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if (sockfd < 0) {
        cerr << "Socket creation failed" << endl;
        return false;
    }
    if (serverName.empty()) {
        cerr << "Server Address not specified!!" << endl;
        return false;
    }
    // Lookup Server Information
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(stockClientPort);

    if (!getStockAddress(serverName.c_str(), servaddr.sin_addr)) {
        cerr << "Failed to resolve server address!\n";
        return false;
    }

    // Prepare Buy protobuf request
    stock_exchange::Packet request;
    request.mutable_header()->set_version(1);
    request.mutable_header()->set_magic(BROKERAGE);
    request.mutable_header()->set_serial(serialNumber_stock);
    serialNumber_stock += 1;


    if (buy){
        // Set the payload as a buy request
        auto* buy = request.mutable_buy();  // Access the oneof field
        buy->set_stock_ticker(stockTicker);
        buy->set_quantity(quantity);
        buy->mutable_price()->set_dollars(dollars);
        buy->mutable_price()->set_cents(cents);

        // Set Brokerage ID
        auto* brokerageIdMessage = buy->mutable_brokerage_id();
        brokerageIdMessage->set_brokerage(brokerageId);
        brokerageIdMessage->set_trader(traderId);  // Include trader ID

        //other required fields
        buy->set_transaction_id(transactionId);
        buy->set_tip_flag(tipFlag); // No tip flag by default
        buy->set_hour(hour);
        buy->set_day(day);
    }else{
        // Set the payload as a sell request
        auto* sell = request.mutable_sell();  // Access the oneof field
        sell->set_stock_ticker(stockTicker);
        sell->set_quantity(quantity);
        sell->mutable_price()->set_dollars(dollars);
        sell->mutable_price()->set_cents(cents);

        // Set Brokerage ID
        auto* brokerageIdMessage = sell->mutable_brokerage_id();
        brokerageIdMessage->set_brokerage(brokerageId);
        brokerageIdMessage->set_trader(traderId);  // Include trader ID

        //other required fields
        sell->set_transaction_id(transactionId);
        sell->set_tip_flag(tipFlag); // No tip flag by default
        sell->set_hour(hour);
        sell->set_day(day);
    }

    //serizlize request into array
    char serializedRequest[stockClientMaxMesg];
    size_t requestSize = request.ByteSizeLong();

    if (!request.SerializeToArray(serializedRequest, requestSize)) {
        cerr << "Failed to serialize request" << endl;
        return false;
    }

    // Send request to the stock exchange server
    sendto(sockfd, serializedRequest, requestSize, 0,
           (struct sockaddr*)&servaddr, sizeof(servaddr));

    if (buy){
        cout << "Sent buy request to stock exchange\n";
    }else{
        cout << "Sent sell request to stock exchange\n";
    }


    // Receive response
    socklen_t len = sizeof(servaddr);
    int n = recvfrom(sockfd, buffer, sizeof(buffer), 0, 
                (struct sockaddr*)&servaddr, &len);

    // if success received, return true, else return false
    if (n > 0) {
        stock_exchange::Response response;
        if (!response.ParseFromArray(buffer, n)) {
            std::cerr << "Failed to parse response" << std::endl;
            return false;
        }
        bool success = (response.response() == stock_exchange::Response::SUCCESS);

        std::cout << "Stock Exchange Response: " 
        << (success ? "SUCCESS" : "ERROR") << std::endl;
        return success;
    } else {
        std::cerr << "Failed to receive response" << std::endl;
        return false;
    }

    close(sockfd);
    return false;

}

bool sendBuySpecRequest(int brokerageId, int traderId, int transactionId){
    svcDir::serverEntity entity = getEntity();
    std::string serverName = entity.name;
    unsigned short stockClientPort = entity.port;

    int sockfd;
    struct sockaddr_in servaddr;
    char buffer[stockClientMaxMesg];

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        cerr << "Socket creation failed" << endl;
        return false;
    }
    if (serverName.empty()) {
        cerr << "Server Address not specified!!" << endl;
        return false;
    }

    // Lookup Server Information
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(stockClientPort);

    if (!getStockAddress(serverName.c_str(), servaddr.sin_addr)) {
        cerr << "Failed to resolve server address!\n";
        return false;
    }
    
    // Prepare BUYSPEC protobuf request
    stock_exchange::Packet request;
    request.mutable_header()->set_version(1);
    request.mutable_header()->set_magic(BROKERAGE);
    request.mutable_header()->set_serial(serialNumber_stock);
    serialNumber_stock += 1;    

    // Set BUYSPEC payload (Only brokerageId & transactionId)
    auto* buyspec = request.mutable_buy_spec();
    buyspec->mutable_brokerage_id()->set_brokerage(brokerageId);
    buyspec->mutable_brokerage_id()->set_trader(traderId);
    buyspec->set_transaction_id(transactionId);

    // Serialize request into array
    char serializedRequest[stockClientMaxMesg];
    size_t requestSize = request.ByteSizeLong();

    if (!request.SerializeToArray(serializedRequest, requestSize)) {
        cerr << "Failed to serialize request" << endl;
        return false;
    }

    // Send request to stock exchange
    sendto(sockfd, serializedRequest, requestSize, 0, 
        (struct sockaddr*)&servaddr, sizeof(servaddr));

    cout << "Sent buyspec request to stock exchange\n";

    // Receive response
    socklen_t len = sizeof(servaddr);
    int n = recvfrom(sockfd, buffer, sizeof(buffer), 0, 
                (struct sockaddr*)&servaddr, &len);

                    // if success received, return true, else return false
    if (n > 0) {
        stock_exchange::Response response;
        if (!response.ParseFromArray(buffer, n)) {
            std::cerr << "Failed to parse response" << std::endl;
            return false;
        }
        bool success = (response.response() == stock_exchange::Response::SUCCESS);

        std::cout << "Stock Exchange Response: " 
        << (success ? "SUCCESS" : "ERROR") << std::endl;
        return success;
    } else {
        std::cerr << "Failed to receive response" << std::endl;
        return false;
    }

    close(sockfd);
    return false;         
}

bool sendCancelRequest(int brokerageId, int traderId, int transactionId){
    svcDir::serverEntity entity = getEntity();
    std::string serverName = entity.name;
    unsigned short stockClientPort = entity.port;
    
    int sockfd;
    struct sockaddr_in servaddr;
    char buffer[stockClientMaxMesg];

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        cerr << "Socket creation failed" << endl;
        return false;
    }
    if (serverName.empty()) {
        cerr << "Server Address not specified!!" << endl;
        return false;
    }

    // Lookup Server Information
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(stockClientPort);

    if (!getStockAddress(serverName.c_str(), servaddr.sin_addr)) {
        cerr << "Failed to resolve server address!\n";
        return false;
    }
    
    // Prepare cancel protobuf request
    stock_exchange::Packet request;
    request.mutable_header()->set_version(1);
    request.mutable_header()->set_magic(BROKERAGE);
    request.mutable_header()->set_serial(serialNumber_stock);    
    serialNumber_stock += 1;

    // Set cancel payload (Only brokerageId & transactionId)
    auto* cancel = request.mutable_cancel();
    cancel->mutable_brokerage_id()->set_brokerage(brokerageId);
    cancel->mutable_brokerage_id()->set_trader(traderId);
    cancel->set_transaction_id(transactionId);

    // Serialize request into array
    char serializedRequest[stockClientMaxMesg];
    size_t requestSize = request.ByteSizeLong();

    if (!request.SerializeToArray(serializedRequest, requestSize)) {
        cerr << "Failed to serialize request" << endl;
        return false;
    }

    // Send request to stock exchange
    sendto(sockfd, serializedRequest, requestSize, 0, 
        (struct sockaddr*)&servaddr, sizeof(servaddr));

    cout << "Sent cancel request to stock exchange\n";

    // Receive response
    socklen_t len = sizeof(servaddr);
    int n = recvfrom(sockfd, buffer, sizeof(buffer), 0, 
                (struct sockaddr*)&servaddr, &len);

                    // if success received, return true, else return false
    if (n > 0) {
        stock_exchange::Response response;
        if (!response.ParseFromArray(buffer, n)) {
            std::cerr << "Failed to parse response" << std::endl;
            return false;
        }
        bool success = (response.response() == stock_exchange::Response::SUCCESS);

        std::cout << "Stock Exchange Response: " 
        << (success ? "SUCCESS" : "ERROR") << std::endl;
        return success;
    } else {
        std::cerr << "Failed to receive response" << std::endl;
        return false;
    }

    
    close(sockfd);
    return false;            
}

bool sendSODQueryRequest(int brokerageId, int traderId, string ticker){
    svcDir::serverEntity entity = getEntity();
    std::string serverName = entity.name;
    unsigned short stockClientPort = entity.port;
    
    int sockfd;
    struct sockaddr_in servaddr;
    uint8_t buffer[stockClientMaxMesg];

    // Create UDP socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        cerr << "Socket creation failed" << endl;
        return false;
    }

    // Lookup Server Information
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(stockClientPort);

    if (!getStockAddress(serverName.c_str(), servaddr.sin_addr)) {
        cerr << "Failed to resolve server address!" << endl;
        close(sockfd);
        return false;
    }
    // Prepare Query request
    stock_exchange::Packet request;
    request.mutable_header()->set_version(1);
    request.mutable_header()->set_magic(BROKERAGE);
    request.mutable_header()->set_serial(serialNumber_stock);
    serialNumber_stock += 1;

    auto* query = request.mutable_query();
    query->mutable_brokerage_id()->set_brokerage(brokerageId);
    query->mutable_brokerage_id()->set_trader(traderId);

    // **Filters to only show transactions related to the same brokerage**
    query->mutable_brokerage_id_look()->set_brokerage(brokerageId);

    query->set_stock_ticker(ticker);
    query->set_filters(2); // Using 2 filters: brokerage_id_look and stock_ticker

    // Serialize request into array
    size_t requestSize = request.ByteSizeLong();
    if (!request.SerializeToArray(buffer, requestSize)) {
        cerr << "Failed to serialize query request" << endl;
        close(sockfd);
        return false;
    }

    // Send request to the stock exchange server
    sendto(sockfd, buffer, requestSize, 0, (struct sockaddr*)&servaddr, sizeof(servaddr));
    cout << "Sent SOD Query request for stock: " << ticker << " under brokerage " << brokerageId << endl;

    // Receive response
    socklen_t len = sizeof(servaddr);
    int n = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&servaddr, &len);

    if (n > 0) {
        stock_exchange::Response response;
        if (!response.ParseFromArray(buffer, n)) {
            cerr << "Failed to parse response" << endl;
            close(sockfd);
            return false;
        }

        // Print response details
        cout << "Stock Exchange Response: "
             << (response.response() == stock_exchange::Response::SUCCESS ? "SUCCESS" : "ERROR") << endl;
        
        // Print returned buy/sell transactions if available
        for (const auto& buy : response.buys()) {
            cout << "[BUY] Trader: " << buy.brokerage_id().trader()
                 << ", Stock: " << buy.stock_ticker()
                 << ", Quantity: " << buy.quantity()
                 << ", Price: $" << buy.price().dollars() << "." << buy.price().cents()
                 << endl;
        }

        for (const auto& sell : response.sells()) {
            cout << "[SELL] Trader: " << sell.brokerage_id().trader()
                 << ", Stock: " << sell.stock_ticker()
                 << ", Quantity: " << sell.quantity()
                 << ", Price: $" << sell.price().dollars() << "." << sell.price().cents()
                 << endl;
        }
        close(sockfd);
        return response.response() == stock_exchange::Response::SUCCESS;
    } else {
        cerr << "No response received from stock exchange" << endl;
        close(sockfd);
        return false;
    }
}

// int main() {
//     int brokerageId = 1;
//     int traderId = 10;   // Example trader ID
//     string stockTicker = "AAPL"; // Buying Apple stock
//     int quantity = 10;   // Buy 10 shares
//     int dollars = 50;    // Price: $50.99
//     int cents = 99;

//     //sendBuyRequest(brokerageId, traderId, stockTicker, quantity, dollars, cents);
//     //sendSellRequest(brokerageId, traderId, stockTicker, quantity, dollars, cents);
//     return 0;
// }
