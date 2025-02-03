#include <cstring>
#include <string>
#include <iostream>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include "bank_to_brokerage.pb.h"
#include "common.pb.h"

using namespace std;

static const unsigned short port = 1866;
static const uint32_t maxMesg = 2048;

int main(int argc, char *argv[]) {
    int sockfd;
    struct sockaddr_in servaddr;

    // Create socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        return 1;
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    servaddr.sin_addr.s_addr = INADDR_ANY; // Change this if the server runs on another machine

    // Construct a request message
    bank_to_brokerage::EndOfDaySummaryRequest request;
    
    // Adding transactions to the request
    for (int i = 0; i < 3; ++i) { // Example of adding 3 transactions
        auto *transaction = request.add_transactions();
        transaction->set_transaction_id(i + 100);
        transaction->set_amount(100.50 * (i + 1));
        transaction->set_type(bank_to_brokerage::TransactionType::DEPOSIT);
    }

    string requestStr;
    if (!request.SerializeToString(&requestStr)) {
        cerr << "Failed to serialize request." << endl;
        return 1;
    }

    // Send the request
    if (sendto(sockfd, requestStr.c_str(), requestStr.size(), MSG_CONFIRM, 
               (const struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
        perror("sendto failed");
        return 1;
    }

    cout << "Request sent." << endl;

    // Receive response
    uint8_t udpMessage[maxMesg];
    socklen_t len = sizeof(servaddr);
    int n = recvfrom(sockfd, udpMessage, maxMesg, MSG_WAITALL, 
                     (struct sockaddr *) &servaddr, &len);
    if (n < 0) {
        perror("recvfrom failed");
        return 1;
    }

    cout << "Response received: " << n << " bytes" << endl;

    // Parse response
    bank_to_brokerage::EndOfDayResponse response;
    if (!response.ParseFromArray(udpMessage, n)) {
        cerr << "Failed to parse response." << endl;
        return 1;
    }

    cout << "Response status: " << response.status() << endl;

    close(sockfd);
    return 0;
}
