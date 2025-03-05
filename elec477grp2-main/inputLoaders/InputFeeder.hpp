#ifndef INPUTFEEDER_HPP
#define INPUTFEEDER_HPP

#include <string>
#include <thread>
#include <iostream>
#include <fstream>
#include <ctime>
#include <iomanip>
#include <functional>
#include <variant>
#include <optional>
#include <atomic>

#include "InitializationLoader.hpp"

#define DEFAULT_TIMESCALE 1 // default timescale for thread event passing (sec/event hour)
using namespace std;

struct Event {
    uint32_t dayTimestamp;
    uint32_t hourTimestamp;
    string eventID;
    optional<variant<int,uint32_t>> Op1;
    optional<string> Op2;
    optional<uint32_t> Op3;
    optional<uint32_t> Op4;
    optional<uint32_t> Op5;
    optional<bool> Op6;

    friend ostream& operator<<(ostream& os, const Event& event) {
        os << "Day Timestamp: " << event.dayTimestamp
           << ", Hour Timestamp: " << event.hourTimestamp
           << ", eventID: " + event.eventID
           << ", Op1: " ;

        if (event.Op1.has_value()){
            if (event.Op1.has_value() && std::holds_alternative<int>(event.Op1.value())){
                os << get<int>(event.Op1.value());
            }
            else if (event.Op1.has_value() && std::holds_alternative<uint32_t>(event.Op1.value())){
                os << get<uint32_t>(event.Op1.value());
            }
        }
        else{
            os << "null";
        }

        os << ", Op2: " << (event.Op2.has_value() ? *event.Op2 : "null")
           << ", Op3: " << (event.Op3.has_value() ? to_string(event.Op3.value()) : "null")
           << ", Op4: " << (event.Op4.has_value() ? to_string(event.Op3.value()) : "null")
           << ", Op6: " << (event.Op3.has_value() ? to_string(event.Op5.value()) : "null")
           << ", Op6: " << (event.Op6.has_value() ? (*event.Op6 ? "TRUE" : "FALSE") : "null");

        return os;
    }
};

class InputFeeder {
private:
    string fileName;
    ifstream file;
    time_t startTime;
    uint32_t timeScale;
    thread feederThread;
    atomic<bool> isFinished{false}; // Flag to track completion
    void initializeEventThread();
    void feedProcess();
    function<void(Event)> eventDispatcher;
public:
    InputFeeder(const string &file, time_t start, function<void(Event)> dispatcher,uint32_t timescale = DEFAULT_TIMESCALE);
    ~InputFeeder();

    void waitUntilDone(); // wait for the thread to finish
};

#endif //INPUTFEEDER_HPP
