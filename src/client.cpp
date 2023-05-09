#include <iostream>
#include <thallium.hpp>
#include "types.hpp"

namespace tl = thallium;

int main(int argc, char** argv) {
    if(argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <server-address>" << std::endl;
        exit(0);
    }

    auto server_address = std::string{argv[1]};
    auto protocol = server_address.substr(0, server_address.find(':'));

    tl::engine myEngine(protocol, THALLIUM_CLIENT_MODE);
    tl::remote_procedure sum = myEngine.define("sum");
    /* (5) Register the insert and lookup remote procedures here,
     * obtaining the insert and lookup remote_procedure objects. */

    tl::endpoint server = myEngine.lookup(server_address);

    vector3d a{1,2,3}, b{4,5,6};
    vector3d result = sum.on(server)(a, b);

    std::cout << "Result is {" << result.x << ", " << result.y << ", " << result.z << "}" << std::endl;

    return 0;
}
