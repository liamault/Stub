
#include <iostream>
#include <string>

#include "svcDirClient.hpp"

using namespace svcDir;

int main(int argc, char * argv[]){
    if (argc != 5){
        std::cerr << "Usage: " << argv[0] << " ipaddress serviceName serverName port" << std::endl;
        return 1;
    }


    std::cout << "***setting server address to " << argv[1] << std::endl;
    setSeverAddress(argv[1]);

    std::string serviceName = argv[2];
    serverEntity se = serverEntity{argv[3],static_cast<uint16_t>(atoi(argv[4]))};


    std::cout << "***registering server " << se << " as " << serviceName  << std::endl;
    bool abc = registerService(serviceName,se);

    std::cout << "registerService returned " << abc << std::endl;
}
