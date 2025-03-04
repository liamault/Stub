
#include <iostream>

#include "svcDirClient.hpp"

using namespace svcDir;

int main(int argc, char * argv[]){
    if (argc != 2){
        std::cerr << "Usage: " << argv[0] << " ipaddress" << std::endl;
        return 1;
    }

    std::cout << "***setting server address to " << argv[1] << std::endl;
    setSeverAddress(argv[1]);

    resetServiceServer();
    
}
