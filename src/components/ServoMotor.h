#ifndef SERVOMOTOR_H
#define SERVOMOTOR_H

#include <ESP32Servo.h>

class ServoMotor {
private:
    Servo servo;

public:
    ServoMotor(int sig_pin);
    void init();
    /**
     * We assume a 360 degree servo.
     * The parameter doesn't correspond to the angle but to the velocity:
     * 90:      Stop
     * 0-89:    Clockwise
     * 91-180:  Counter Clockwise
     */
    void setVelocity(int velocity);
};

#endif