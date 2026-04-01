#pragma once

#include <gpio/pwm.hpp>

class Vehicle {
    public:
        explicit Vehicle(int steering_pwm, int speed_pwm, int period);
        ~Vehicle();

        void set_steering_duty_cycle(int duty_cycle) const;
        void set_speed_duty_cycle(int duty_cycle) const;

        void engine_on() const;
        void engine_off() const;

    private:
        const int _period;

        const int _steering_pwm;
        const int _speed_pwm;

        const cookie::gpio::Pwm _steering;
        const cookie::gpio::Pwm _speed;
};