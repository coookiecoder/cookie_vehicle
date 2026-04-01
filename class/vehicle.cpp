#include "vehicle.hpp"

Vehicle::Vehicle(const int steering_pwm, const int speed_pwm, const int period) : _period(period), _steering_pwm(steering_pwm), _speed_pwm(speed_pwm), _steering(0), _speed(0) {
    this->_steering.export_channel(steering_pwm);
    this->_speed.export_channel(speed_pwm);

    this->_steering.set_period(steering_pwm, period);
    this->_speed.set_period(speed_pwm, period);
}

void Vehicle::set_steering_duty_cycle(const int duty_cycle) const {
    this->_steering.set_duty_cycle(this->_steering_pwm, duty_cycle);
}

void Vehicle::set_speed_duty_cycle(const int duty_cycle) const {
    this->_speed.set_duty_cycle(this->_speed_pwm, duty_cycle);
}

void Vehicle::engine_on() const {
    this->_speed.enable(this->_speed_pwm);
}

void Vehicle::engine_off() const {
    this->_speed.disable(this->_speed_pwm);
}

Vehicle::~Vehicle() = default;