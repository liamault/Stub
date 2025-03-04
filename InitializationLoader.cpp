#include "InitializationLoader.h"

using namespace std;

// helper function to trim whitespace from both ends of a string
string trim(const string& str) {
    auto start = str.begin();
    while (start != str.end() && isspace(*start)) {
        start++;
    }

    auto end = str.end();
    do {
        end--;
    } while (distance(start, end) > 0 && isspace(*end));

    return string(start, end + 1);
}


// returns a vectorized version of a CSV row
vector<string> readRow(string &row, char delimiter) {

    vector<string> tokens;
    stringstream ss(row);
    string token;

    while (getline(ss, token, delimiter)) {
        tokens.push_back(trim(token));
    }

    return tokens;

}

// returns a vector of all row vectors within a CSV file (skipping first line as it is the header line)
vector<vector<string>> InitializationLoader::CSVtoVector(){
    ifstream ifs(dataFile, ifstream::in);
    vector<vector<string>> data;
    string line;

    if (!ifs.is_open()) {
        cerr << "Failed to open the file: " << dataFile << endl;
        return data;
    }

    // skip first line (header line)
    getline(ifs, line);

    while (getline(ifs, line)) {

        // skip empty lines (in case of extra newlines)
        if (line.empty()) continue;

        // parse the row and store it in the InputLoaders vector
        vector<string> row = readRow(line, ',');
        data.push_back(row);
    }
    ifs.close();
    return data;
}
