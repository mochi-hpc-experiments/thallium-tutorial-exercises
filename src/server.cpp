#include <iostream>
#include <thallium.hpp>
#include "types.hpp"

namespace tl = thallium;

int main(int argc, char** argv) {

    if(argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <protocol>" << std::endl;
        exit(0);
    }

    tl::engine myEngine(argv[1], THALLIUM_SERVER_MODE);

    std::function<void(const tl::request&, const vector3d&, const vector3d&)> sum =
        [](const tl::request& req, const vector3d& a, const vector3d& b) {
            req.respond(a+b);
        };

    myEngine.define("sum", sum);

    myEngine.wait_for_finalize();

    return 0;
}
