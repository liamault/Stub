#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <netdb.h>  // For getaddrinfo()
#include <unistd.h> // For close()
#include "common.pb.h"
#include "brokerage_to_bank.pb.h"

using namespace std;

static unsigned short port = 5003; // The port this server listens on
static uint32_t maxMesg = 2048;    // Max message size

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

    // Bind to port 5000
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(port);

    if (bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        cerr << "Bind failed on port " << port << endl;
        close(sockfd);
        return;
    }

    cout << "Bank Server listening on port " << port << "...\n";

    while (true) {
        socklen_t clientLen = sizeof(clientaddr);
        int n = recvfrom(sockfd, buffer, sizeof(buffer), 0, 
                         (struct sockaddr*)&clientaddr, &clientLen);

        if (n > 0) {
            // Parse the received message as an UpdateAccount request
            brokerage_to_bank::UpdateAccount request;
            if (!request.ParseFromArray(buffer, n)) {
                cerr << "Failed to parse incoming request\n";
                continue;
            }

            if (request.has_deposit()) {  // Use has_deposit() for oneof fields
                cout << "Received Deposit Request: Brokerage ID = "
                    << request.deposit().brokerage_id().brokerage()
                    << ", Trader ID = " << request.deposit().brokerage_id().trader()
                    << ", Amount = $" 
                    << request.deposit().price().dollars()
                    << "." << request.deposit().price().cents()
                    << endl;
            } else if (request.has_withdraw()) {  // Handle Withdraw request
                cout << "Received Withdraw Request: Brokerage ID = "
                    << request.deposit().brokerage_id().brokerage()
                    << ", Trader ID = " << request.deposit().brokerage_id().trader()
                    << ", Amount = $"
                    << request.withdraw().price().dollars()
                    << "." << request.withdraw().price().cents()
                    << endl;
            } else {
                cout << "Error: Received UpdateAccount request with no valid payload!" << endl;
            }

            // Prepare Ack response
            brokerage_to_bank::Ack response;
            response.set_response(brokerage_to_bank::Ack::SUCCESS);  // Assume always success

            // Serialize Ack response
            size_t respSize = response.ByteSizeLong();
            response.SerializeToArray(buffer, respSize);

            // Send Ack response back to the client
            sendto(sockfd, buffer, respSize, 0,
                   (struct sockaddr*)&clientaddr, clientLen);

            cout << "Sent Ack response to client\n";
        }
    }

    close(sockfd);
}

int main() {
    startServer();
    return 0;
}