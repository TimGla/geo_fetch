#include "Press.h"
#include <Arduino.h>

Press::Press(
    int en_pin, 
    int step_pin, 
    int dir_pin,
    float deg_per_step,
    AccelStepper::MotorInterfaceType type
) : stepper(type, step_pin, dir_pin), degrees_per_step(deg_per_step) {
        steps_per_revolution = round(360.0 / deg_per_step);
        this->en_pin = en_pin;
}

void Press::init() {
    stepper.setEnablePin(en_pin);
    stepper.setPinsInverted(false, false, true);
    stepper.setMaxSpeed(speed);
    stepper.setSpeed(0);
    stepper.setCurrentPosition(0);
    stepper.disableOutputs();
}

bool Press::isActive() {
    return active;
}

void Press::setDirection(bool direction) {
    active = true;
    stepper.enableOutputs();
    float _speed = direction ? speed : (-1 * speed);
    stepper.setCurrentPosition(0);
    stepper.setSpeed(_speed);
}

void Press::press() {
    stepper.runSpeed();
}


void Press::stop() {
    stepper.setSpeed(0);
    stepper.setCurrentPosition(0);
    stepper.disableOutputs();
    active = false;
}
