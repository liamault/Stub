#include "InputFeeder.hpp"

InputFeeder::InputFeeder(const string &fname, time_t start, function<void(Event)> dispatcher, uint32_t timescale) {

    fileName = fname;
    startTime = start;
    eventDispatcher = dispatcher;
    timeScale = timescale;

    // open the input event file
    file.open(fileName);
    if (!file.is_open()) {
        cerr << "Error: Unable to open file " << fileName << endl;
    }

    // delay starting event reader thread based on specified delay
    feederThread = thread(&InputFeeder::initializeEventThread, this);
}

InputFeeder::~InputFeeder() {
    // Ensure the thread is joined before destruction
    if (feederThread.joinable()) {
        feederThread.join();
    }
}

void InputFeeder::initializeEventThread() {

    // wait until specified start time
    this_thread::sleep_until(chrono::system_clock::from_time_t(startTime));

    // convert startTime (time_t) to tm struct for local time
    tm localTm;
    localtime_r(&startTime, &localTm);  // thread-safe version of localtime()
    cout << "Thread started at: " << put_time(&localTm, "%H:%M:%S") << "\n";

    feedProcess();
}

void InputFeeder::feedProcess() {

    string line;
    vector<string> vectorizedLine;
    uint32_t dayTimestamp, hourTimestamp, traderID, quantity, price, transactionID;
    string eventID, ticker;
    int newsType;
    bool fraudFlag;

    int previousHour = -1;
    int previousDay = -1;

    // skip first line (header line)
    getline(file, line);

    while(getline(file, line)){

        // skip empty lines (in case of extra newlines)
        if (line.empty()){  continue;}
        vectorizedLine = readRow(line, ',');

        // skip lines that are incorrectly formatted
        if (vectorizedLine.size() != 3 &&  vectorizedLine.size() != 4 && vectorizedLine.size() != 5 && vectorizedLine.size() != 9) {
            cerr << "Error: Malformed line: " << line << "\n";
            continue;
        }

        dayTimestamp = static_cast<uint32_t>(stoul(vectorizedLine[0]));
        hourTimestamp= static_cast<uint32_t>(stoul(vectorizedLine[1]));
        eventID = vectorizedLine[2];
        Event e = {dayTimestamp, hourTimestamp, eventID};

        if (vectorizedLine.size() == 4){ //BUYSPEC/CANCEL/WITHDRAW/DEPOSIT
            transactionID = static_cast<uint32_t>(stoul(vectorizedLine[3]));
            e.Op1 = transactionID;
        }

        if (vectorizedLine.size() == 5){ // NEWS
            newsType = stoi(vectorizedLine[3]);
            ticker = vectorizedLine[4];
            e.Op1 = newsType;
            e.Op2 = ticker;
        }

        if (vectorizedLine.size() == 9){ //BUY/SELL
            traderID = static_cast<uint32_t>(stoul(vectorizedLine[3]));
            ticker = vectorizedLine[4];
            quantity = static_cast<uint32_t>(stoul(vectorizedLine[5]));
            price = static_cast<uint32_t>(stoul(vectorizedLine[6]));
            transactionID = static_cast<uint32_t>(stoul(vectorizedLine[7]));
            fraudFlag = (vectorizedLine[8] == "1");
            e.Op1 = traderID;
            e.Op2 = ticker;
            e.Op3 = quantity;
            e.Op4 = price;
            e.Op5 = transactionID;
            e.Op6 = fraudFlag;
        }

        // If not the first line, wait if needed
        if (previousHour >= 0) {
            int waitTime = ((dayTimestamp - previousDay)* 24 + (hourTimestamp - previousHour)) * timeScale;
            if (waitTime > 0) {
                this_thread::sleep_for(chrono::seconds(waitTime));
            }
        }

        //cout << e << endl;
        eventDispatcher(e);
        previousHour = hourTimestamp;
        previousDay = dayTimestamp;

    }
    file.close(); // close filestream
}