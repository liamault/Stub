#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include "bank_to_brokerage.pb.h"

using namespace std;

// Server details
static const char* serverIP = "127.0.0.1";
static unsigned short port = 50051;
static uint32_t maxMesg = 2048;
static uint32_t magic = 'E477';
static const uint32_t version1x = 0x0100;

int main() {
    int sockfd;
    struct sockaddr_in servaddr;
    uint8_t buffer[maxMesg];

    // Create socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        return -1;
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    servaddr.sin_addr.s_addr = inet_addr(serverIP);

    // Create request message
    bank_to_brokerage::EndOfDaySummaryRequest request;
    request.mutable_header()->set_magic(magic);
    request.mutable_header()->set_version(version1x);
    request.mutable_header()->set_serial(1);
    
    auto* transaction = request.add_transactions();
    transaction->set_operation(bank_to_brokerage::EndOfDaySummaryRequest::DEPOSIT);
    transaction->mutable_amount()->set_value(500.0);

    int blen = request.ByteSizeLong();
    request.SerializeToArray(buffer, blen);

    sendto(sockfd, buffer, blen, MSG_CONFIRM, (const struct sockaddr*)&servaddr, sizeof(servaddr));

    // Receive response
    socklen_t len = sizeof(servaddr);
    int n = recvfrom(sockfd, buffer, maxMesg, MSG_WAITALL, (struct sockaddr*)&servaddr, &len);
    cout << "Received response from server" << endl;

    close(sockfd);
    return 0;
}
