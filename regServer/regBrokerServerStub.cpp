#include <iostream>
#include <cstring>
#include <atomic>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>

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