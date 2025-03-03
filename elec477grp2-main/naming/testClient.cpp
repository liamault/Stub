
#include <iostream>

#include "svcDirClient.hpp"

using namespace svcDir;

int main(int argc, char * argv[]){
    if (argc != 2){
        std::cerr << "Usage: " << argv[0] << " ipaddress" << std::endl;
        return 1;
    }

    std::cout << "***seetting server address to " << argv[1] << std::endl;
    setSeverAddress(argv[1]);

    std::cout << "***registering server axcd as mabxox "  << std::endl;
    serverEntity se = { "axcd", 3 };
    bool abc = registerService("mabxox",se);
    
    std::cout << "***registering server take2 as mabxox "  << std::endl;
    se = serverEntity{"take2",25};
    abc = registerService("mabxox",se);
   
    std::cout << "***registering server take3 as mabxox "  << std::endl;
    se = serverEntity{"take3",25};
    abc = registerService("mabxox",se);
    
    std::cout << "***searching mabxox "  << std::endl;
    se = searchService("mabxox");
    std::cout << ">>search returned " << se << std::endl;
    
    std::cout << "***removing server take2 as mabxox "  << std::endl;
    se = serverEntity{"take2",25};
    abc =removeService("mabxox",se);

    std::cout << "***searching mabxox "  << std::endl;
    se = searchService("mabxox");
    std::cout << ">>search returned " << se << std::endl;

    std::cout << "***searching mabxox "  << std::endl;
    se = searchService("mabxox");
    std::cout << ">>search returned " << se << std::endl;

    std::cout << "***searching mabxox "  << std::endl;
    se = searchService("mabxox");
    std::cout << ">>search returned " << se << std::endl;
    
    resetServiceServer();
    
    std::cout << "***searching mabxox "  << std::endl;
    se = searchService("mabxox");
    std::cout << ">>search returned " << se << std::endl;

}
