#include <iostream>
#include <thallium.hpp>
#include "types.hpp"

namespace tl = thallium;

int main(int argc, char** argv) {
    if(argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <protocol> <server-address>" << std::endl;
        exit(0);
    }
    tl::engine myEngine(argv[1], THALLIUM_CLIENT_MODE);
    tl::remote_procedure sum = myEngine.define("sum");
    tl::endpoint server = myEngine.lookup(argv[2]);

    vector3d a{1,2,3}, b{4,5,6};
    vector3d result = sum.on(server)(a, b);

    return 0;
}
