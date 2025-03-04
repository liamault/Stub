#include <iostream>
#include <thread>
#include "regServer/regServerMain.h"
#include "InitializationLoader.h"

using namespace std;
extern void startBankServer();


void initComponent(string initCSV) {
    InitializationLoader initLoader(initCSV);
    // FIXME: implement traders class assignment logic
//    auto broker_info = initLoader.CSVtoVector();
//
//    // add block flag and duration column
//    for (size_t i = 0; i < broker_info.size(); ++i) {
//        broker_info[i].push_back("0"); // block flag: 0 = no block, 1 = block
//        broker_info[i].push_back("0"); // block duration (days)
//    }

    regServer regulatory_server;
    std::thread regThread([&regulatory_server, broker_info]() { regulatory_server.startServer(broker_info); });
    std::thread bankThread(startBankServer);
//    std::thread eventHandling(/*TODO: event handling method here*/);
    regThread.join();
    bankThread.join();
//    eventHandling.join();
};

int main(int argc, char * argv[]) {
    initComponent("broker1_init.csv");
};
