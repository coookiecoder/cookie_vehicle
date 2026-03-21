#pragma once

class Vehicle {
    public:
        explicit Vehicle(int pwm, int period);
        ~Vehicle();

        void set_duty_cycle(int duty_cycle);

        void speed_up();
        void slow_down();

        void engine_on() const;
        void engine_off() const;

    private:
        const int _pwm;
        const int _period;

        int _duty_cycle = 0;
};