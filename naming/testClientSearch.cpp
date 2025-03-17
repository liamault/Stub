
#include <iostream>
#include <string>

#include "svcDirClient.hpp"

using namespace svcDir;

int main(int argc, char * argv[]){
    if (argc != 3){
        std::cerr << "Usage: " << argv[0] << " ipaddress serviceName" << std::endl;
        return 1;
    }

    std::string serviceName = argv[2];

    std::cout << "***seetting server address to " << argv[1] << std::endl;
    setSeverAddress(argv[1]);

    std::cout << "***searching " << serviceName  << std::endl;
    serverEntity se = searchService(serviceName);
    std::cout << ">>search returned " << se << std::endl;
}
