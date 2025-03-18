#include <iostream>
#include <cstring>
#include <netdb.h>  
#include <sys/types.h> 
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "regulatory_to_broker.pb.h"
#include "common.pb.h"
#include <google/protobuf/descriptor.h>
#include "../naming/svcDirClient.hpp"

using namespace std;

#define SERVER_PORT 1867
#define SERVER_IP "127.0.0.1"
#define MAX_MSG_SIZE 2048

string serviceName_reg = "brokerageCompliance";
string serverAddress_reg = "ServiceServer.elec477grp2";
static atomic<uint32_t> serialNumber_reg = 1;

int sendRequest() {
    // Initialize protobuf
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    if (!svcDir::setSeverAddress(serverAddress_reg)) {
        cerr << "Failed to set server address!" << endl;
        exit(1);
    }
    else { cout << "Server Address set." << endl;}

    svcDir::serverEntity entity = svcDir::searchService(serviceName_reg);
    if (entity.name == "init" || entity.port == 0) {
        std::cerr << "Error: No entity found for service " << serviceName_reg << std::endl;
        exit(1); // Exit program if service was not found
    }
    std::string serverName = entity.name;
    unsigned short regClientPort = entity.port;
    std::cout << "Found service: " << serverName << " on port " << regClientPort << std::endl;


    // Resolve the IP address from the service name
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;   // IPv4
    hints.ai_socktype = SOCK_DGRAM; // UDP
    hints.ai_protocol = IPPROTO_UDP;

    int status = getaddrinfo(serverName.c_str(), nullptr, &hints, &res);
    if (status != 0) {
        std::cerr << "Failed to resolve server address: " << gai_strerror(status) << std::endl;
        return 1;
    }

    // Extract IP address
    struct sockaddr_in *addr = (struct sockaddr_in *)res->ai_addr;
    char ipStr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &addr->sin_addr, ipStr, sizeof(ipStr));

    std::cout << "Resolved IP Address: " << ipStr << std::endl;


    // Configure server address
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(regClientPort);
    servaddr.sin_addr = addr->sin_addr;

    freeaddrinfo(res); // Free the addrinfo struct


    // Create UDP socket
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        return 1;
    }


    // Construct EndOfDaySummaryRequest message
    regulatory_to_broker::BrokerageRuleViolationRequest request;
    request.mutable_header()->set_version(1);
    request.mutable_header()->set_magic(REGULATORY);
    request.mutable_header()->set_serial(1);

    request.mutable_brokerage()->set_brokerage(1);
    request.mutable_brokerage()->set_trader(2);

    request.set_block_duration(2);

    request.set_reason(regulatory_to_broker::Trade_Block_Violation);

    // Serialize request
    string requestStr;
    if (!request.SerializeToString(&requestStr)) {
        cerr << "Failed to serialize request.\n";
        close(sockfd);
        return 1;
    }

    // Send request to server
    socklen_t len = sizeof(servaddr);
    int n = sendto(sockfd, requestStr.c_str(), requestStr.size(), MSG_CONFIRM,
                   (const struct sockaddr*)&servaddr, len);
    if (n < 0) {
        perror("sendto failed");
        close(sockfd);
        return 1;
    }

    cout << "Request sent successfully.\n";

    // Receive response
    struct timeval timeout;
    timeout.tv_sec = 2;
    timeout.tv_usec = 0;

    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
        close(sockfd);
        cerr << "Response timeout failed" << endl;
        return false;
    }

    char buffer[MAX_MSG_SIZE];
    n = recvfrom(sockfd, buffer, MAX_MSG_SIZE, MSG_WAITALL,
                 (struct sockaddr*)&servaddr, &len);
    if (n < 0) {
        perror("recvfrom failed");
        close(sockfd);
        sendRequest();
    }
    else{
        regulatory_to_broker::BrokerageRuleViolationResponse response;
        if (!response.ParseFromArray(buffer, n)) {
            cerr << "Failed to parse response.\n";
            close(sockfd);
            return 1;
        }
        cout << "Server Response: " << response.DebugString() << endl;
        serialNumber_reg.fetch_add(1);
        close(sockfd);
    }
    google::protobuf::ShutdownProtobufLibrary();
    
    return 0;
}

int main() {
    sendRequest();
}