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
#include <thread>
#include <chrono>

#include "common.pb.h"
#include "regulatory_to_broker.pb.h"

#include "regServerMain.h"
#include "../component.hpp"
#include "../naming/svcDirClient.hpp"

#include "../shutdownFlag.hpp"


using namespace std;

static unsigned short regServerPort = 1867;
static uint32_t regServerMaxMesg = 2048;
string serverAddress = "ServiceServer.elec477grp2";//

static svcDir::serverEntity entity{"brokerage", uint16_t(1867)};

// inet glal variables
static int regServerSockfd;
static atomic<uint32_t> serialNumber_reg = 1;
void regServer::startServer() {
    //set server address
    
    if (!svcDir::setSeverAddress(serverAddress)) {
        cerr << "Failed to set server address!" << endl;
        exit(1);
    }
    else { cout << "Server Address set for brokerageCompliance." << endl; }

    //register service: brokerageCompliance 
    if (!svcDir::registerService("brokerageCompliance", entity)) {
        cerr << "Failed to register service!" << endl;
        exit(1);
    }
    else { cout << "Service brokerageCompliance registered." << endl; }


    cout << "Running Regulatory service..." << endl;

    struct sockaddr_in servaddr, cliaddr;

    // get a socket to recieve messges
    if ( (regServerSockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
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
    servaddr.sin_port = htons(regServerPort);

    // Bind the socket with the server address
    if (::bind(regServerSockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0 )
    {
        perror("bind failed");
        return; // this will exit the service thread and stop the server
    }

    socklen_t len;
    uint8_t udpMessage[regServerMaxMesg];
    int n;

    //timeouts for checking the shutdown flag
    struct timeval timeout;
    timeout.tv_sec = 1;  // 1 second timeout
    timeout.tv_usec = 0;
    setsockopt(regServerSockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));


    while(!shutdownFlag.load()){
        // wait for a message from a client
        len = sizeof(cliaddr);  //len is value/result
        n = recvfrom(regServerSockfd, (uint8_t *)udpMessage, regServerMaxMesg, MSG_WAITALL, ( struct sockaddr *) &cliaddr, &len);

        if (n < 0) {  
            if (errno == EWOULDBLOCK || errno == EAGAIN) {
                continue;  // Just loop back to check shutdownFlag
            } else {
                perror("recvfrom failed");
                break;
            }
        }
        cout << "request received: " << n << endl;

        regulatory_to_broker::BrokerageRuleViolationRequest request;
        regulatory_to_broker::BrokerageRuleViolationResponse reply;

        if (!request.ParseFromArray(udpMessage, n)) {
            cerr << "Failed to parse Message" << endl;
            return;
        }

        cout << "message parsed:" << endl;
        cout << "trader " << request.brokerage().trader() << " at brokerage " << request.brokerage().brokerage() << " will be banned for " << request.block_duration() << " days due to reason #" << request.reason() << endl;

        // pass request here
        uint32_t traderID = request.brokerage().trader();
        uint32_t blockDuration = request.block_duration();
        Trader* trader = getTrader(traderID);

        if (trader) {
            trader->updateBlockFlag(blockDuration);
            updateTrader(traderID, *trader);

            reply.mutable_header()->set_serial(serialNumber_reg);
            reply.mutable_header()->set_version(request.header().version());
            reply.mutable_header()->set_magic(BROKERAGE);

            //set valid respinse
            reply.set_response(regulatory_to_broker::ResponseType::OK);
        } else {
            std::cerr << "Trader " << traderID << " not found!" << std::endl;

            reply.mutable_header()->set_serial(serialNumber_reg);
            reply.mutable_header()->set_version(request.header().version());
            reply.mutable_header()->set_magic(BROKERAGE);
    
            //set invalid respinse
            reply.set_response(regulatory_to_broker::ResponseType::INVALID);

        }
        serialNumber_reg += 1;
        
        cout << "Sending response: " << reply.DebugString() << endl;

        string responseStr;
        if (!reply.SerializeToString(&responseStr)) {
            cerr << "Failed to serialize response" << endl;
            continue;
        }

        int servern = sendto(regServerSockfd, responseStr.c_str(), responseStr.size(), MSG_CONFIRM, (const struct sockaddr *) &cliaddr, len);
        if (servern < 0) {
            perror("sendto failed");
        }
    }

    if (!removeService("brokerageCompliance", entity)) {
        cerr << "Failed to remove brokerageCompliance service!" << endl;
        exit(1);
    }
    else { cout << "brokerageCompliance service removed." << endl; }

    close(regServerSockfd);
}
