#include <server/udp.hpp>

#include <mutex>
#include <chrono>
#include <fstream>

cookie::server::udp server(5000);
std::mutex client_mutex;

std::map<uint16_t, std::string> client_types;
std::map<uint16_t, std::chrono::steady_clock::time_point> client_last_seen;

std::string car_register(std::string message, sockaddr_in client) {
    std::lock_guard<std::mutex> client_lock(client_mutex);
    for (const auto & client_address : server.client_addresses) {
        if (client_address.sin_addr.s_addr == client.sin_addr.s_addr && client_address.sin_port == client.sin_port) {
            break;
        }
    }

    server.client_addresses.push_back(client);
    client_types[client.sin_port] = "CAR";
    client_last_seen[client.sin_port] = std::chrono::steady_clock::now();

    return message;
}

std::string truck_register(std::string message, sockaddr_in client) {
    std::lock_guard<std::mutex> client_lock(client_mutex);
    for (const auto & client_address : server.client_addresses) {
        if (client_address.sin_addr.s_addr == client.sin_addr.s_addr && client_address.sin_port == client.sin_port) {
            break;
        }
    }

    server.client_addresses.push_back(client);
    client_types[client.sin_port] = "TRUCK";
    client_last_seen[client.sin_port] = std::chrono::steady_clock::now();

    return message;
}

[[noreturn]] int main() {
    FILE* tty1 = fopen("/dev/tty1", "w");
    if (!tty1) {
        perror("Failed to open /dev/tty1");
        return 1;
    }

    fprintf(tty1, "\033[2J\033[H"); 
    fflush(tty1);

    server.map_response_word("CAR", car_register);
    server.map_response_word("TRUCK", truck_register);

    server.start();

    int speed = 0;
    std::unique_lock<std::mutex> client_lock(client_mutex);
    client_lock.unlock();

    while (true) {
        client_lock.lock();
        if (server.client_addresses.empty() == false)
            server.broadcast("SPEED " + std::to_string(speed));
        client_lock.unlock();

	fprintf(tty1, "\033[H");

        client_lock.lock();
	int line = 0;
        for (auto client_address : server.client_addresses) {
            const std::string type = client_types[client_address.sin_port];
            const long seconds_ago = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - client_last_seen[client_address.sin_port]).count();
            fprintf(tty1, "%s ON PORT : %d, last info : %ld ms\n", type.c_str(), client_address.sin_port, seconds_ago);

            if (line > 20) break;
        }
        client_lock.unlock();
	fflush(tty1);

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}
