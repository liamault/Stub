#include <iostream>

#include "common.pb.h"
#include "regulatory_to_broker.pb.h"

#include "regulatory_logic.h"

int regulatory_logic::findMatchingRow(int broker_id, int trader_id) {
    // Iterate over rows
    for (size_t i = 0; i < broker_info.size(); ++i) {
        if (broker_info[i].size() >= 2 && std::stoi(broker_info[i][0]) == broker_id && std::stoi(broker_info[i][1]) == trader_id) {
            return i; // Return the matching index
        }
    }

    return -1;
}

regulatory_to_broker::BrokerageRuleViolationResponse regulatory_logic::updateBlockFlag(
        const regulatory_to_broker::BrokerageRuleViolationRequest request_info
    ) {
        int broker_id = request_info.brokerage().brokerage();
        int trader_id = request_info.brokerage().trader();
        regulatory_to_broker::BrokerageRuleViolationResponse reply;
        reply.mutable_header()->set_serial(request_info.header().serial());
        reply.mutable_header()->set_version(request_info.header().version());
        reply.mutable_header()->set_magic(::Magic::BROKERAGE);

        int row_index = findMatchingRow(broker_id, trader_id);

        if (row_index < 0) {
            // couldn't find matching trader, so return invalid request
            reply.set_response(regulatory_to_broker::ResponseType::INVALID);
            return reply;
        };

        int currentBlockDur = std::stoi(broker_info[row_index][5]);
        if (currentBlockDur < request_info.block_duration()) {
            // only update to increase the block duration
            broker_info[row_index][4] = "1";
            broker_info[row_index][5] = std::to_string(request_info.block_duration());
        }

        reply.set_response(regulatory_to_broker::ResponseType::OK);
        return reply;
}

bool regulatory_logic::checkBlockFlag(const int broker_id, const int trader_id) {
    int trader_index = findMatchingRow(broker_id, trader_id);

    if (trader_index < 0) {
        return true;
    }
    return (std::stoi(broker_info[trader_index][4]));
}

void regulatory_logic::incBlockFlags() {
    for (size_t i = 0; i < broker_info.size(); ++i) {
        int blockDuration = std::stoi(broker_info[i][5]);
        if (blockDuration > 0) {
            broker_info[i][5] = std::to_string(blockDuration - 1);
        }
    }
    return;
}
