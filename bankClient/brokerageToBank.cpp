#include <iostream>
#include <cstring>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "common.pb.h"
#include "brokerage_to_bank.pb.h"
#include "brokerageToBank.hpp"
#include "../naming/svcDirClient.hpp"


using namespace std;

string serviceName_bank = "brokerage_server";
string serverAddress_bankc = "ServiceServer.final-integration4";
static uint32_t bankClientMaxMesg = 2048;
static atomic<uint32_t> serialNumber_bankC = 1;

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

    if (!svcDir::setSeverAddress(serverAddress_bankc)) {
        cerr << "Failed to set server address!" << endl;
        exit(1);
    }
    else { cout << "Server Address set." << endl; }

    svcDir::serverEntity entity = svcDir::searchService(serviceName_bank);
    if (entity.name == "init" || entity.port == 0) {
        std::cerr << "Error: No entity found for service " << serviceName_bank << std::endl;
        exit(1); // Exit program if service was not found
    }
    std::string serverName = entity.name;
    unsigned short bankClientPort = entity.port;
    std::cout << "Found service: " << serverName << " on port " << bankClientPort << std::endl;

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

    // Prepare protobuf request
    brokerage::Update_BrokerageAccount_Req request;
    request.mutable_header()->set_version(1);
    request.mutable_header()->set_magic(BROKERAGE);
    request.mutable_header()->set_serial(serialNumber_bankC.load());

    // Set the payload as a deposit
    if (deposit) {
        auto* transaction = request.mutable_deposit();  // Fix for oneof fields
        auto* brokerageIdMessage = transaction->mutable_brokerage_id();
        brokerageIdMessage->set_brokerage(brokerageId);
        transaction->mutable_price()->set_dollars(dollars);
        transaction->mutable_price()->set_cents(cents);
        cout << "Sent deposit request to bank\n"; 
    } else {
        auto* transaction = request.mutable_withdraw();  // Fix for oneof fields
        auto* brokerageIdMessage = transaction->mutable_brokerage_id();
        brokerageIdMessage->set_brokerage(brokerageId);
        transaction->mutable_price()->set_dollars(dollars);
        transaction->mutable_price()->set_cents(cents);
        cout << "Sent withdraw request to bank\n"; 
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

       
    
    // Receive response
    struct timeval timeout;
    timeout.tv_sec = 2;
    timeout.tv_usec = 0;

    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
        close(sockfd);
        cerr << "Response timeout failed" << endl;
        return;
    }

    socklen_t len = sizeof(servaddr);
    int n = recvfrom(sockfd, buffer, sizeof(buffer), 0, 
                    (struct sockaddr*)&servaddr, &len);

    if (n > 0) {
        brokerage::Update_BrokerageAccount_Rep response;
        response.ParseFromArray(buffer, n);

        cout << "Bank response: ";
        switch (response.response()) {
            case brokerage::Update_BrokerageAccount_Rep::SUCCESS:
                cout << "SUCCESS" << endl;
                break;
            case brokerage::Update_BrokerageAccount_Rep::INVALID:
                cout << "INVALID TRANSACTION" << endl;
                break;
            case brokerage::Update_BrokerageAccount_Rep::MALFORMED_MESSAGE:
                cout << "MALFORMED MESSAGE" << endl;
                break;
            case brokerage::Update_BrokerageAccount_Rep::INVALID_TRANSACTION_ID:
                cout << "INVALID TRANSACTION ID" << endl;
                break;
            case brokerage::Update_BrokerageAccount_Rep::INVALID_BROKERAGE_ID:
                cout << "INVALID BROKERAGE ID" << endl;
                break;
            default:
                cout << "UNKNOWN RESPONSE" << endl;
                break;

        serialNumber_bankC.fetch_add(1);
        close(sockfd);
    } else {
        cerr << "No response received from bank\n";
        close(sockfd);
        sendTransactionRequest(brokerageId, dollars, cents, deposit);
    }
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
