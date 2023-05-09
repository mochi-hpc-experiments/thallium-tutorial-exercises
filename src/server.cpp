#include <iostream>
#include <thallium.hpp>
#include "types.hpp"

namespace tl = thallium;

int main(int argc, char** argv) {

    if(argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <protocol>" << std::endl;
        exit(0);
    }

    /* (1) Instanciate a phonebook */

    tl::engine myEngine(argv[1], THALLIUM_SERVER_MODE);

    std::function<void(const tl::request&, const vector3d&, const vector3d&)> sum =
        [](const tl::request& req, const vector3d& a, const vector3d& b) {
            std::cout << "Received vectors a = {"
                << a.x << ", " << a.y << ", " << a.z << "} and b = {"
                << b.x << ", " << b.y << ", " << b.z << "}" << std::endl;
            req.respond(a+b);
        };

    /* (3) Add the definitions for the insert and lookup lambdas.
     * Don't forget to pass a reference to the phonebook in their capture.
     */

    myEngine.define("sum", sum);
    /* (4) Register the insert and lookup RPC handlers with the engine */

    std::cout << "Server is running at address " << myEngine.self() << std::endl;
    std::cout << "Copy this address to pass it to clients" << std::endl;

    myEngine.wait_for_finalize();

    return 0;
}
