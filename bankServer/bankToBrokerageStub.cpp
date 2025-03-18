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

#include "bank_to_brokerage.pb.h"
#include "common.pb.h"

#include "../shutdownFlag.hpp"
#include "bankToBrokerageStub.hpp"
#include "../naming/svcDirClient.hpp"



using namespace std;
using namespace string_literals;

static uint16_t bankServerPort = 1866;
static uint32_t bankServerMaxMesg = 2048;

// inet glal variables
static int sockfd;
static struct sockaddr_in servaddr;
//static atomic<uint32_t> serial = 0;
static atomic<uint32_t> serialNumber_bankS = numeric_limits<uint32_t>::max();// serial number initialized as max, matches client serial number for the first message

string serverAddress_bankS = "ServiceServer.elec477grp2";
static svcDir::serverEntity entity{"brokerage", uint16_t(1866)};
//std::atomic<bool> shutdownFlag(false);


void startBankServer() {
    //set server address
    if (!svcDir::setSeverAddress(serverAddress_bankS)) {
        cerr << "Failed to set server address!" << endl;
        exit(1);
    }
    else { cout << "Server Address set for BankLedger." << endl; }


    //register service: BankLedger 
    if (!svcDir::registerService("BankLedger", entity)) {
        cerr << "Failed to register service!" << endl;
        exit(1);
    }
    else { cout << "Service BankLedger registered." << endl; }


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
    servaddr.sin_port = htons(bankServerPort);

    // Bind the socket with the server address
    if (::bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0 )
    {
        perror("bind failed");
        return; // this will exit the service thread and stop the server
    }

    cout << "running Bank server" << "\n" << endl;

    socklen_t len;
    uint8_t udpMessage[bankServerMaxMesg];
    int n;

    struct timeval timeout;//timeouts for checking the shutdown flag
    timeout.tv_sec = 1;  // 1 second timeout
    timeout.tv_usec = 0;
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
    
    while(!shutdownFlag.load()){
        // wait for a mesage from a client
        len = sizeof(cliaddr);  //len is value/result
        n = recvfrom(sockfd, (uint8_t *)udpMessage, bankServerMaxMesg, MSG_WAITALL, ( struct sockaddr *) &cliaddr, &len);

        if (n < 0) {  
            if (errno == EWOULDBLOCK || errno == EAGAIN) {
                continue;  // Just loop back to check shutdownFlag
            } else {
                perror("recvfrom failed");
                break;
            }
        }
        
        cout << "request received from bank: " << n << "\n";
        brokerage::EndOfDaySummaryRequest request;
        if (!request.ParseFromArray(udpMessage, n)) {
            cerr << "Failed to parse request.\n";
            continue;
        }

        if (serialNumber_bankS.load() == numeric_limits<uint32_t>::max() || static_cast<uint32_t>(request.header().serial()) > serialNumber_bankS.load()){
            serialNumber_bankS.store(static_cast<uint32_t>(request.header().serial()));///This is better

            // Process the request here
            // For example, print the request details
            cout << "Request details: " << request.DebugString() << "\n";
        }

        // Prepare a response
        brokerage::EndOfDayResponse response;
        response.mutable_header()->set_version(request.header().version());
        response.mutable_header()->set_magic(BROKERAGE);
        response.mutable_header()->set_serial(request.header().serial());
        
        response.set_status(brokerage::EndOfDayResponse::SUCCESS);

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
    }

    if (!removeService("BankLedger", entity)) {
        cerr << "Failed to remove BankLedger service!" << endl;
        exit(1);
    }
    else { cout << "BankLedger service removed." << endl; }


    close(sockfd);
}


//int main(){
//    startBankServer();
//}
