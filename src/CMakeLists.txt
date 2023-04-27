add_executable (server server.cpp)
target_link_libraries (server thallium)

add_executable (client client.cpp)
target_link_libraries (client thallium)
