#ifndef BANK_TO_BROKERAGE_STUB_HPP
#define BANK_TO_BROKERAGE_STUB_HPP

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

// Function to start the bank server
void startBankServer();

#endif // BANK_TO_BROKERAGE_STUB_HPP
