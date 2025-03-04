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

#ifndef STUB_REGULATORY_LOGIC_H
#define STUB_REGULATORY_LOGIC_H

class regulatory_logic {
private:
    vector<vector<string>> broker_info;
    int findMatchingRow(int broker_id, int trader_id);
public:
    regulatory_logic(const vector<vector<string>> info) : broker_info(info) {};
    regulatory_to_broker::BrokerageRuleViolationResponse updateBlockFlag(
            const regulatory_to_broker::BrokerageRuleViolationRequest request
    );
    bool checkBlockFlag(
        const int broker_id,
        const int trader_id
    );
    void incBlockFlags();
};

#endif //STUB_REGULATORY_LOGIC_H
