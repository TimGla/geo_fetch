#ifndef CONTAINER_SPINNER_H
#define CONTAINER_SPINNER_H

#include <ESP32Servo.h>

class ContainerSpinner {
private:
    Servo servo;

public:
    ContainerSpinner(int sig_pin);
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