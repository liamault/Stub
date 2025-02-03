#include <iostream>
#include <cstring>
#include <atomic>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>

#include "bank_to_brokerage.pb.h"

using namespace std;

// Server settings
static unsigned short port = 50051;  // UDP port
static uint32_t maxMesg = 2048;
static uint32_t magic = 'E477';
static const uint32_t version1x = 0x0100;

// UDP Socket globals
static bool ready = false;
static int sockfd;
static struct sockaddr_in servaddr, clientaddr;
static atomic<uint32_t> serial = 0;

// Function Prototypes
static bool init();
static void shutdown();
void processMessage(uint8_t *buffer, int len);

//+
// Function: init
//
// Initializes the UDP socket and binds it to the server's address.
//-
static bool init() {
    // Register shutdown routine to close socket on exit
    if (atexit(shutdown)) {
        cerr << "Could not register shutdown routine" << endl;
        return false;
    }

    // Create UDP socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        return false;
    }

    // Server address setup
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(port);

    // Bind the socket to the server address
    if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("Bind failed");
        return false;
    }

    cout << "Bank-to-Brokerage server running on port " << port << endl;
    ready = true;
    return true;
}

//+
// Function: shutdown
//
// Closes the UDP socket when the program exits.
//-
static void shutdown() {
    if (!ready) return;
    close(sockfd);
    ready = false;
}

//+
// Function: processMessage
//
// Processes incoming UDP messages, parses protobuf, and replies with a response.
//-
void processMessage(uint8_t *buffer, int len) {
    bank_to_brokerage::EndOfDaySummaryRequest request;
    bank_to_brokerage::EndOfDayResponse response;

    if (!request.ParseFromArray(buffer, len)) {
        cerr << "Could not parse incoming message" << endl;
        return;
    }

    // Validate message magic, version, and serial
    if (request.header().magic() != magic) {
        cerr << "Magic mismatch! Ignoring message." << endl;
        return;
    }
    if (request.header().version() != version1x) {
        cerr << "Version mismatch! Ignoring message." << endl;
        return;
    }

    // Log transactions
    cout << "Received End of Day Summary with " << request.transactions_size() << " transactions." << endl;
    for (const auto& transaction : request.transactions()) {
        string operation = (transaction.operation() == bank_to_brokerage::EndOfDaySummaryRequest::DEPOSIT) ? "DEPOSIT" : "WITHDRAW";
        cout << "Transaction: " << operation << " Amount: " << transaction.amount().value() << endl;
    }

    // Prepare response
    response.mutable_header()->set_magic(magic);
    response.mutable_header()->set_version(version1x);
    response.mutable_header()->set_serial(request.header().serial());
    response.set_status(bank_to_brokerage::EndOfDayResponse::SUCCESS);

    // Serialize response
    uint8_t sendBuffer[maxMesg];
    int blen = response.ByteSizeLong();
    if (blen > maxMesg) {
        cerr << "Response too large!" << endl;
        return;
    }
    response.SerializeToArray(sendBuffer, blen);

    // Send response to client
    socklen_t clientlen = sizeof(clientaddr);
    sendto(sockfd, sendBuffer, blen, MSG_CONFIRM, (const struct sockaddr *)&clientaddr, clientlen);
    cout << "Sent EndOfDayResponse: SUCCESS" << endl;
}

//+
// Function: main
//
// Main server loop that listens for messages and processes them.
//-
int main() {
    if (!init()) {
        cerr << "Failed to initialize server!" << endl;
        return -1;
    }

    uint8_t buffer[maxMesg];
    socklen_t clientlen = sizeof(clientaddr);

    while (true) {
        int n = recvfrom(sockfd, buffer, maxMesg, MSG_WAITALL, (struct sockaddr *)&clientaddr, &clientlen);
        if (n < 0) {
            cerr << "Error receiving data" << endl;
            continue;
        }
        processMessage(buffer, n);
    }

    return 0;
}
