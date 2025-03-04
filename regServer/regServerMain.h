#ifndef REG_SERVER_H
#define REG_SERVER_H

#include <cstring>
#include <string>
#include <iostream>
#include <vector>
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

class regServer {
public:
//    vector<vector<string>> broker_info;

//    regServer() : broker_info(info) {};
    void startServer(vector<vector<string>> info);
};

#endif // REG_SERVER_H
