#include <cstring>
#include <string>
#include <iostream>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <atomic>

#include "common.pb.h"
#include "regulatory_to_broker.pb.h"

using namespace std;

static unsigned short port = 1867;
static uint32_t maxMesg = 2048;

// inet glal variables
static int sockfd;
static struct sockaddr_in servaddr;
static atomic<uint32_t> serial = 0;

int main(int argc, char * argv[]) {
    cout << "test" << endl;


    struct sockaddr_in servaddr, cliaddr;

    // get a socket to recieve messges
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        perror("socket creation failed");
        return 1; // this will exit the service thread and stop the server
    }

    // clear variables before initializing
    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));

    // Port and Interfact information for binding
    // the socket
    servaddr.sin_family = AF_INET;        // IPv4
    servaddr.sin_addr.s_addr = INADDR_ANY;   // whatever interface is available
    servaddr.sin_port = htons(port);

    // Bind the socket with the server address
    if (::bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0 )
    {
        perror("bind failed");
        return 1; // this will exit the service thread and stop the server
    }

    socklen_t len;
    uint8_t udpMessage[maxMesg];
    int n;

    while(true){
        // wait for a message from a client
        len = sizeof(cliaddr);  //len is value/result
        n = recvfrom(sockfd, (uint8_t *)udpMessage, maxMesg, MSG_WAITALL, ( struct sockaddr *) &cliaddr, &len);
        cout << "request received: " << n << endl;

        regulatory_to_broker::BrokerageRuleViolationRequest request;
        regulatory_to_broker::BrokerageRuleViolationResponse reply;

        if (!request.ParseFromArray(udpMessage, n)) {
            cerr << "Failed to parse Message" << endl;
            return 0;
        }

        cout << "message parsed:\n";
        cout << "trader " << request.brokerage().trader() << " at brokerage " << request.brokerage().brokerage();
        cout << " will be banned for " << request.block_duration() << " due to " << request.reason();

        reply.mutable_header()->CopyFrom(request.header());
        reply.mutable_header()->set_magic(::Magic::BROKERAGE);
        reply.set_response(regulatory_to_broker::ResponseType::OK);

        uint32_t replyVal = reply.ByteSizeLong();
        int servern = sendto(sockfd, udpMessage, replyVal, MSG_CONFIRM, (const struct sockaddr *) &cliaddr, len);

    }
}