#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>
#include "common.pb.h"
#include "stock_exchange.pb.h"

using namespace std;

static unsigned short port = 5003; // Server listens on this port
static uint32_t maxMesg = 2048;


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

    // Bind to port 5003
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

            if (request.has_buy()) {  // Check for Buy request
                cout << "Received Buy Request: "
                     << "Stock = " << request.buy().stock_ticker()
                     << ", Quantity = " << request.buy().quantity()
                     << ", Price = $" << request.buy().price().dollars()
                     << "." << request.buy().price().cents()
                     << endl;

                // Prepare Response
                stock_exchange::Response response;
                response.set_response(stock_exchange::Response::SUCCESS); // Assume successful trade

                // Serialize Response
                size_t respSize = response.ByteSizeLong();
                response.SerializeToArray(buffer, respSize);

                // Send Ack Response back to client
                sendto(sockfd, buffer, respSize, 0,
                       (struct sockaddr*)&clientaddr, clientLen);

                cout << "Sent Ack response to client\n";
            } else {
                cout << "Error: Received request with no valid order!" << endl;
            }
        }

    }
    close(sockfd);
}

int main() {
    startServer();
    return 0;
}