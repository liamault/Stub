#include <iostream>
#include <cstring>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "common.pb.h"
#include "brokerage_to_bank.pb.h"
#include "brokerageToBank.hpp"

using namespace std;

static string serverName = "bank";
static unsigned short bankClientPort = 1864;
static uint32_t bankClientMaxMesg = 2048;

bool getBankAddress(const char* name, in_addr& addr) {
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

void sendTransactionRequest(int brokerageId, int dollars, int cents, bool deposit) {
    int sockfd;
    struct sockaddr_in servaddr;
    char buffer[bankClientMaxMesg];

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
    servaddr.sin_port = htons(bankClientPort);

    if (!getBankAddress(serverName.c_str(), servaddr.sin_addr)) {
        cerr << "Failed to resolve server address!\n";
        return;
    }

    // Prepare Deposit protobuf request
    brokerage_to_bank::UpdateAccount request;
    request.mutable_header()->set_version(1);
    request.mutable_header()->set_magic(BROKERAGE);
    request.mutable_header()->set_serial(1);

    // Set the payload as a deposit
    if (deposit) {
        auto* transaction = request.mutable_deposit();  // Fix for oneof fields
        auto* brokerageIdMessage = transaction->mutable_brokerage_id();
        brokerageIdMessage->set_brokerage(brokerageId);
        transaction->mutable_price()->set_dollars(dollars);
        transaction->mutable_price()->set_cents(cents);
    } else {
        auto* transaction = request.mutable_withdraw();  // Fix for oneof fields
        auto* brokerageIdMessage = transaction->mutable_brokerage_id();
        brokerageIdMessage->set_brokerage(brokerageId);
        transaction->mutable_price()->set_dollars(dollars);
        transaction->mutable_price()->set_cents(cents);
    }

    // Serialize request
    char serializedRequest[bankClientMaxMesg];
    size_t requestSize = request.ByteSizeLong();
    if (!request.SerializeToArray(serializedRequest, requestSize)) {
        cerr << "Failed to serialize request" << endl;
        return;
    }

    
    // Send request to the bank
    sendto(sockfd, serializedRequest, requestSize, 0,
           (struct sockaddr*)&servaddr, sizeof(servaddr));

    cout << "Sent deposit request to bank\n";    
    
    // Receive response
    socklen_t len = sizeof(servaddr);
    int n = recvfrom(sockfd, buffer, sizeof(buffer), 0, 
                    (struct sockaddr*)&servaddr, &len);

    if (n > 0) {
        brokerage_to_bank::Ack response;
        response.ParseFromArray(buffer, n);

        cout << "Bank response: "
             << (response.response() == brokerage_to_bank::Ack::SUCCESS ? "SUCCESS" : "INVALID TRANSACTION")
             << endl;
    } else {
        cerr << "No response received from bank\n";
    }

    close(sockfd);

}

// int main() {
//     int brokerageId = 1;

//     // Test deposit
//     sendTransactionRequest(brokerageId, 500, 25, 1);
//     sendTransactionRequest(brokerageId, 500, 25, 2);
//     sendTransactionRequest(brokerageId, 500, 25, 4);
//     sendTransactionRequest(brokerageId, 500, 25, 6);
//     return 0;
// }