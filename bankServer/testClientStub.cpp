#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "bank_to_brokerage.pb.h"
#include "common.pb.h"
#include <google/protobuf/descriptor.h>

using namespace std;

#define SERVER_PORT 1866
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
    bank_to_brokerage::EndOfDaySummaryRequest request;
    request.mutable_header()->set_version(1);
    request.mutable_header()->set_magic(BANK);
    request.mutable_header()->set_serial(1);

    auto* transaction1 = request.add_transactions();
    transaction1->set_operation(bank_to_brokerage::EndOfDaySummaryRequest::DEPOSIT);
    transaction1->mutable_amount()->set_dollars(1);
    transaction1->mutable_amount()->set_cents(2); 
    transaction1->set_component(BROKERAGE);

    auto* transaction2 = request.add_transactions();
    transaction2->set_operation(bank_to_brokerage::EndOfDaySummaryRequest::WITHDRAW);
    transaction2->mutable_amount()->set_dollars(3);
    transaction2->mutable_amount()->set_cents(4); 
    transaction2->set_component(BROKERAGE);

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
    bank_to_brokerage::EndOfDayResponse response;
    if (!response.ParseFromArray(buffer, n)) {
        cerr << "Failed to parse response.\n";
        close(sockfd);
        return 1;
    }

    const std::string statusStr = bank_to_brokerage::EndOfDayResponse::Status_Name(response.status());
    cout << "Server Response: " << statusStr << endl;

    // Cleanup
    close(sockfd);
    google::protobuf::ShutdownProtobufLibrary();

    return 0;
}
