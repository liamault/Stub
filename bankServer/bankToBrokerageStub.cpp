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

# include "bank_to_brokerage.pb.h"
# include "common.pb.h"

using namespace std;
using namespace string_literals;

static unsigned short port = 1866;
static uint32_t maxMesg = 2048;

// inet glal variables
static int sockfd;
static struct sockaddr_in servaddr;
//static atomic<uint32_t> serial = 0;

void startBankServer() {
    struct sockaddr_in servaddr, cliaddr;

    // get a socket to recieve messges
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        perror("socket creation failed");
        return; // this will exit the service thread and stop the server
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
        return; // this will exit the service thread and stop the server
    }

    cout << "running server...\n";

    socklen_t len;
    uint8_t udpMessage[maxMesg];
    int n;

    while(true){
        // wait for a mesage from a client
        len = sizeof(cliaddr);  //len is value/result
        n = recvfrom(sockfd, (uint8_t *)udpMessage, maxMesg, MSG_WAITALL, ( struct sockaddr *) &cliaddr, &len);
        cout << "request received: " << n << "\n";
        bank_to_brokerage::EndOfDaySummaryRequest request;
        if (!request.ParseFromArray(udpMessage, n)) {
            cerr << "Failed to parse request.\n";
            continue;
        }

        // Process the request here
        // For example, print the request details
        cout << "Request details: " << request.DebugString() << "\n";

        // Prepare a response
        bank_to_brokerage::EndOfDayResponse response;
        response.mutable_header()->set_version(1);
        response.mutable_header()->set_magic(BROKERAGE);
        response.mutable_header()->set_serial(1);
        
        response.set_status(bank_to_brokerage::EndOfDayResponse::SUCCESS);

        //does not list success as it is default enum value
        cout << "Sending response: " << response.DebugString() << endl;
        
        // Serialize the response
        string responseStr;
        if (!response.SerializeToString(&responseStr)) {
            cerr << "Failed to serialize response.\n";
            continue;
        }

        // Send the response back to the client
        int servern = sendto(sockfd, responseStr.c_str(), responseStr.size(), MSG_CONFIRM, (const struct sockaddr *) &cliaddr, len);
        if (servern < 0) {
            perror("sendto failed");
        }

//        if(doCommand(udpMessage,n,maxMesg)){
//            int servern = sendto(sockfd, udpMessage, n, MSG_CONFIRM, (const struct sockaddr *) &cliaddr, len);
//        }
    }
}