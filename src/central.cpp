#include <server/udp.hpp>

#include <iostream>
#include <mutex>

cookie::server::udp server(5000);
std::mutex server_mutex;

std::string callback(std::string cmd, sockaddr_in client) {
    std::lock_guard lock(server_mutex);
    for (const auto & client_address : server.client_addresses) {
        if (client_address.sin_addr.s_addr == client.sin_addr.s_addr && client_address.sin_port == client.sin_port) {
            return cmd;
        }
    }

    server.client_addresses.push_back(client);

    std::cout << cmd << std::endl;
    return cmd;
}

[[noreturn]] int main() {
    server.map_default(callback);

    std::unique_lock lock(server_mutex);
    lock.unlock();

    server.start();

    while (true) {

    }
}