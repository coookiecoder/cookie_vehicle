#include <client/udp.hpp>

#include <fstream>
#include <optional>
#include <iostream>
#include <sstream>

#include "../class/vehicle.hpp"

std::optional<Vehicle> car;

std::string correct_id(const std::string& message) {
    std::stringstream ss(message);
    std::string id, command, value;

    ss >> id;
    ss >> command;
    ss >> value;

    if (command == "set_steering_duty_cycle")
        car->set_steering_duty_cycle(std::stoi(value));
    else if (command == "set_speed_duty_cycle")
        car->set_speed_duty_cycle(std::stoi(value));
    else if (command == "engine_on")
        car->engine_on();
    else if (command == "engine_off")
        car->engine_off();

    return "";
}

int main(int argc, char ** argv) {
    std::ifstream file;

    if (argc == 1)
        file = std::ifstream("/media/mmcblk0p1/vehicle.txt");
    else if (argc == 2)
        file = std::ifstream(argv[1]);
    else
        return 1;

    int id;
    int speed_pwm;
    int steering_pwm;
    int period;
    std::string server;
    int port;

    std::string line;
    while (std::getline(file, line)) {
        std::erase(line, '\n');
        std::erase(line, '\r');
        std::string command = line.substr(0, line.find('='));
        std::string value = line.substr(line.find('=') + 1);

        if (command == "id")
            id = std::stoi(value);
        else if (command == "speed_pwm")
            speed_pwm = std::stoi(value);
        else if (command == "steering_pwm")
            steering_pwm = std::stoi(value);
        else if (command == "period")
            period = std::stoi(value);
        else if (command == "server")
            server = value;
        else if (command == "port")
            port = std::stoi(value);
    }
    file.close();

    std::cout << "id : " << id << std::endl;
    std::cout << "speed_pwm : " << speed_pwm << std::endl;
    std::cout << "steering_pwn : " << steering_pwm << std::endl;
    std::cout << "period : " << period << std::endl;
    std::cout << "server : " << server << std::endl;
    std::cout << "port : " << port << std::endl;

    car.emplace(steering_pwm, speed_pwm, period);

    cookie::client::udp client(server.c_str(), port);
    client.map_response_word(std::to_string(id), correct_id);
    client.start();

    client.send("CAR " + std::to_string(id) + " INIT");
}
