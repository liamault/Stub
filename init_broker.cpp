#include <iostream>
#include "regServer/regServerMain.h"
#include "InitializationLoader.h"

using namespace std;

//void iinitComponent(string initCSV) {
//
//
//}

int main(int argc, char * argv[]) {
    InitializationLoader initLoader("broker1_init.csv");
    auto broker_info = initLoader.CSVtoVector();

    // add block flag and duration column
    for (size_t i = 0; i < broker_info.size(); ++i) {
        broker_info[i].push_back("0"); // block flag: 0 = no block, 1 = block
        broker_info[i].push_back("0"); // block duration (days)
    }

    regServer regulatory_server;
//    cout << "ticker value is: " << regulatory_server.broker_info[0][2] << endl;
    regulatory_server.startServer(broker_info);
};
