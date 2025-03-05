#ifndef BROKERAGE_TO_BANK_HPP
#define BROKERAGE_TO_BANK_HPP

#include <iostream>
#include <cstring>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>


bool getBankAddress(const char* name, in_addr& addr);
void sendTransactionRequest(int brokerageId, int dollars, int cents, bool deposit);

#endif // BROKERAGE_TO_BANK_HPP
