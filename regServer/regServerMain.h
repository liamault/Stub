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

using namespace std;

class regServer {
public:
    void startServer();
};

#endif // REG_SERVER_H
