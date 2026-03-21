#include <client/udp.hpp>

#include <fstream>
#include <optional>

#include "../class/vehicle.hpp"

std::optional<Vehicle> truck;

std::string correct_id(const std::string& message) {
    std::string command = message.substr(message.find(' ') + 1);

    if (command == "speed_up")
        truck->speed_up();
    else if (command == "slow_down")
        truck->slow_down();
    else if (command == "start")
        truck->engine_on();
    else if (command == "stop")
        truck->engine_off();

    return "";
}

int main() {
    std::fstream file("/media/mmcblk0p1/vehicle.txt");

    int id;
    int pwn;
    int period;
    std::string server;
    int port;

    std::string line;
    while (std::getline(file, line)) {
        std::string command = line.substr(0, line.find('='));
        std::string value = line.substr(line.find('=') + 1);

        if (command == "id")
            id = std::stoi(value);
        else if (command == "pwn")
            pwn = std::stoi(value);
        else if (command == "period")
            period = std::stoi(value);
        else if (command == "server")
            server = value;
        else if (command == "port")
            port = std::stoi(value);
    }
    file.close();

    truck.emplace(pwn, period);

    cookie::client::udp client(server.c_str(), port);
    client.map_response_word(std::to_string(id), correct_id);
    client.start();

    client.send("TRUCK " + std::to_string(id) + " INIT");
}
