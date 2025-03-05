#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>
#include <vector>
#include "common.pb.h"  // BuySell is here
#include "stock_exchange.pb.h"

using namespace std;

static unsigned short port = 1337; // Server listens on this port
static uint32_t maxMesg = 2048;

// Store Buy/Sell Transactions
std::vector<BuySell> buyTransactions;
std::vector<BuySell> sellTransactions;

void startServer() {
    int sockfd;
    struct sockaddr_in servaddr, clientaddr;
    uint8_t buffer[maxMesg];

    // Create UDP socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        cerr << "Socket creation failed" << endl;
        return;
    }

    // Bind to port
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(port);

    if (bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        cerr << "Bind failed on port " << port << endl;
        close(sockfd);
        return;
    }

    cout << "Stock Exchange Server listening on port " << port << "...\n";

    while (true) {
        socklen_t clientLen = sizeof(clientaddr);
        int n = recvfrom(sockfd, buffer, sizeof(buffer), 0, 
            (struct sockaddr*)&clientaddr, &clientLen);

        if (n > 0) {
            // Parse the received message
            stock_exchange::Packet request;
            if (!request.ParseFromArray(buffer, n)) {
                cerr << "Failed to parse incoming request\n";
                continue;
            }

            stock_exchange::Response response;
            response.mutable_header()->set_version(1);
            response.mutable_header()->set_magic(BROKERAGE);

            if (request.has_buy()) {  
                cout << "Received Buy Request: "
                     << "Stock = " << request.buy().stock_ticker()
                     << ", Quantity = " << request.buy().quantity()
                     << ", Price = $" << request.buy().price().dollars()
                     << "." << request.buy().price().cents()
                     << endl;
                
                // Store buy transaction
                BuySell buyOrder;
                buyOrder.CopyFrom(request.buy());
                buyTransactions.push_back(buyOrder);

                response.set_response(stock_exchange::Response::SUCCESS);
            }
            else if (request.has_sell()) {  
                cout << "Received Sell Request: "
                     << "Stock = " << request.sell().stock_ticker()
                     << ", Quantity = " << request.sell().quantity()
                     << ", Price = $" << request.sell().price().dollars()
                     << "." << request.sell().price().cents()
                     << endl;
                
                // Store sell transaction
                BuySell sellOrder;
                sellOrder.CopyFrom(request.sell());
                sellTransactions.push_back(sellOrder);

                response.set_response(stock_exchange::Response::SUCCESS);
            }
            else if (request.has_buy_spec()) {  
                cout << "Received BuySpec Request: "
                     << "Brokerage ID = " << request.buy_spec().brokerage_id().brokerage()
                     << ", Trader ID = " << request.buy_spec().brokerage_id().trader()
                     << ", Transaction ID = " << request.buy_spec().transaction_id()
                     << endl;
                
                response.set_response(stock_exchange::Response::SUCCESS);
            }
            else if (request.has_cancel()) {  
                cout << "Received Cancel Request: "
                     << "Brokerage ID = " << request.cancel().brokerage_id().brokerage()
                     << ", Trader ID = " << request.cancel().brokerage_id().trader()
                     << ", Transaction ID = " << request.cancel().transaction_id()
                     << endl;
                
                response.set_response(stock_exchange::Response::SUCCESS);
            }
            else if (request.has_query()) {  
                cout << "Received SOD Query Request for stock: " 
                     << request.query().stock_ticker() << endl;

                uint32_t brokerageId = request.query().brokerage_id_look().brokerage();
                string stockTicker = request.query().stock_ticker();

                // Find matching buy transactions
                for (const auto& buy : buyTransactions) {
                    if (buy.stock_ticker() == stockTicker && buy.brokerage_id().brokerage() == brokerageId) {
                        BuySell* newBuy = response.add_buys();
                        newBuy->CopyFrom(buy);
                    }
                }

                // Find matching sell transactions
                for (const auto& sell : sellTransactions) {
                    if (sell.stock_ticker() == stockTicker && sell.brokerage_id().brokerage() == brokerageId) {
                        BuySell* newSell = response.add_sells();
                        newSell->CopyFrom(sell);
                    }
                }

                response.set_response(stock_exchange::Response::SUCCESS);
                cout << "Sent SOD Query response for " << stockTicker << endl;
            }
            else {
                cout << "Error: Received request with no valid order!" << endl;
                response.set_response(stock_exchange::Response::INVALID);
            }

            // Serialize Response
            size_t respSize = response.ByteSizeLong();
            response.SerializeToArray(buffer, respSize);

            // Send response back to client
            sendto(sockfd, buffer, respSize, 0,
                   (struct sockaddr*)&clientaddr, clientLen);

            cout << "Sent response to client\n";
        }
    }

    close(sockfd);
}

int main() {
    startServer();
    return 0;
}
