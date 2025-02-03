#include <iostream>
#include <cstring>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "common.pb.h"
#include "stock_exchange.pb.h"


using namespace std;
static string serverName = "stockexchange";
static unsigned short port = 1864;
static uint32_t maxMesg = 2048;

bool getAddress(const char* name, in_addr& addr) {
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

void sendBuyRequest(int brokerageId, int traderId, string stockTicker, int quantity, int dollars, int cents){
    int sockfd;
    struct sockaddr_in servaddr;
    char buffer[maxMesg];
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if (sockfd < 0) {
        cerr << "Socket creation failed" << endl;
        return;
    }
    if (serverName.empty()) {
        cerr << "Server Address not specified!!" << endl;
        return;
    }
    // Lookup Server Information
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);

    if (!getAddress(serverName.c_str(), servaddr.sin_addr)) {
        cerr << "Failed to resolve server address!\n";
        return;
    }

    // Prepare Buy protobuf request
    stock_exchange::Packet request;
    request.mutable_header()->set_version(1);
    request.mutable_header()->set_magic(BROKERAGE);
    request.mutable_header()->set_serial(1);

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
    buy->set_transaction_id(12345);
    buy->set_tip_flag(false); // No tip flag by default
    buy->set_hour(14);
    buy->set_day(3);

    //serizlize request into array
    char serializedRequest[maxMesg];
    size_t requestSize = request.ByteSizeLong();

    if (!request.SerializeToArray(serializedRequest, requestSize)) {
        cerr << "Failed to serialize request" << endl;
        return;
    }

    // Send request to the stock exchange server
    sendto(sockfd, serializedRequest, requestSize, 0,
           (struct sockaddr*)&servaddr, sizeof(servaddr));

    cout << "Sent buy request to stock exchange\n";


    // Receive response
    socklen_t len = sizeof(servaddr);
    int n = recvfrom(sockfd, buffer, sizeof(buffer), 0, 
                (struct sockaddr*)&servaddr, &len);

    if (n > 0) {
        stock_exchange::Response response;
        response.ParseFromArray(buffer, n);

        cout << "Stock Exchange Response: "
             << (response.response() == stock_exchange::Response::SUCCESS ? "SUCCESS" : "ERROR")
             << endl;
    } else {
        cerr << "No response received from stock exchange\n";
    }

    close(sockfd);

}

int main() {
    int brokerageId = 1;
    int traderId = 10;   // Example trader ID
    string stockTicker = "AAPL"; // Buying Apple stock
    int quantity = 10;   // Buy 10 shares
    int dollars = 50;    // Price: $50.99
    int cents = 99;

    sendBuyRequest(brokerageId, traderId, stockTicker, quantity, dollars, cents);
    return 0;
}