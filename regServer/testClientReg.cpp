#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "regulatory_to_broker.pb.h"
#include "common.pb.h"
#include <google/protobuf/descriptor.h>

using namespace std;

#define SERVER_PORT 1867
#define SERVER_IP "127.0.0.1"
#define MAX_MSG_SIZE 2048

int main() {
    // Initialize protobuf
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    // Create UDP socket
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        return 1;
    }

    // Configure server address
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERVER_PORT);
    servaddr.sin_addr.s_addr = inet_addr(SERVER_IP);

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
    char buffer[MAX_MSG_SIZE];
    n = recvfrom(sockfd, buffer, MAX_MSG_SIZE, MSG_WAITALL,
                 (struct sockaddr*)&servaddr, &len);
    if (n < 0) {
        perror("recvfrom failed");
        close(sockfd);
        return 1;
    }

    // Deserialize response
    regulatory_to_broker::BrokerageRuleViolationResponse response;
    if (!response.ParseFromArray(buffer, n)) {
        cerr << "Failed to parse response.\n";
        close(sockfd);
        return 1;
    }

    cout << "Server Response: " << response.DebugString() << endl;

    // Cleanup
    close(sockfd);
    google::protobuf::ShutdownProtobufLibrary();

    return 0;
}