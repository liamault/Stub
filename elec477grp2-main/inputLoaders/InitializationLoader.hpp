#ifndef CSVLOADER_HPP
#define CSVLOADER_HPP

#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>

using namespace std;

// helper functions
vector<string> readRow(string &row, char delimiter);
string trim(const string& str);

class InitializationLoader {
private:
    string dataFile;
public:
    InitializationLoader(const string &file) : dataFile(file) {};
    vector<vector<string>> CSVtoVector();
};


#endif //CSVLOADER_HPP
