#include "vehicle.hpp"

#include <fstream>
#include <iostream>

Vehicle::Vehicle(int pwm, int period) : _pwm(pwm), _period(period) {
    std::fstream pwm_file("/sys/class/pwm/pwmchip0/export");
    pwm_file << pwm;
    pwm_file.close();

    std::fstream period_file("/sys/class/pwm/pwmchip0/pwm" + std::to_string(pwm) + "/period");
    period_file << period;
    period_file.close();
}

void Vehicle::set_duty_cycle(const int duty_cycle) {
    std::fstream duty_cycle_file("/sys/class/pwm/pwmchip0/pwm" + std::to_string(_pwm) + "/duty_cycle");
    duty_cycle_file << duty_cycle;
    duty_cycle_file.close();

    this->_duty_cycle = duty_cycle;
}

void Vehicle::speed_up() {
    this->set_duty_cycle(this->_duty_cycle + 1000);
}

void Vehicle::slow_down() {
    this->set_duty_cycle(this->_duty_cycle - 1000);
}

void Vehicle::engine_on() const {
    std::fstream enable_file("/sys/class/pwm/pwmchip0/pwm" + std::to_string(_pwm) + "/enable");
    enable_file << 1;
    enable_file.close();
}

void Vehicle::engine_off() const {
    std::fstream enable_file("/sys/class/pwm/pwmchip0/pwm" + std::to_string(_pwm) + "/enable");
    enable_file << 0;
    enable_file.close();
}

Vehicle::~Vehicle() = default;