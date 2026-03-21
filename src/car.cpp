#include <client/udp.hpp>

#include <fstream>
#include <optional>
#include <iostream>

#include "../class/vehicle.hpp"

std::optional<Vehicle> car;

std::string correct_id(const std::string& message) {
    std::string command = message.substr(message.find(' ') + 1);

    if (command == "speed_up")
        car->speed_up();
    else if (command == "slow_down")
        car->slow_down();
    else if (command == "start")
        car->engine_on();
    else if (command == "stop")
        car->engine_off();

    return "";
}

int main() {
    std::ifstream file("/media/mmcblk0p1/vehicle.txt");

    int id;
    int pwm;
    int period;
    std::string server;
    int port;

    std::string line;
    while (std::getline(file, line)) {
        std::string command = line.substr(0, line.find('='));
        std::string value = line.substr(line.find('=') + 1);

        if (command == "id")
            id = std::stoi(value);
        else if (command == "pwm")
            pwm = std::stoi(value);
        else if (command == "period")
            period = std::stoi(value);
        else if (command == "server")
            server = value;
        else if (command == "port")
            port = std::stoi(value);
    }
    file.close();

    std::cout << "id : " << id << std::endl;
    std::cout << "pwm : " << pwm << std::endl;
    std::cout << "period : " << period << std::endl;
    std::cout << "server : " << server << std::endl;
    std::cout << "port : " << port << std::endl;

    car.emplace(pwm, period);

    cookie::client::udp client(server.c_str(), port);
    client.map_response_word(std::to_string(id), correct_id);
    client.start();

    client.send("CAR " + std::to_string(id) + " INIT");
}
