#include "StepperMotor.h"
#include <Arduino.h>

StepperMotor::StepperMotor(
    int pin_1, 
    int pin_2, 
    int pin_3, 
    int pin_4, 
    float deg_per_step,
    AccelStepper::MotorInterfaceType type
) : stepper(type, pin_1, pin_2, pin_3, pin_4), degrees_per_step(deg_per_step) {
        steps_per_revolution = round(360.0 / deg_per_step);
}

void StepperMotor::init() {
    stepper.setMaxSpeed(1000);
    //stepper.setAcceleration(200.0);
    stepper.setSpeed(0);                // Motor cant move
    stepper.setCurrentPosition(0);
}

bool StepperMotor::isActive() {
    return active;
}

void StepperMotor::setDirection(bool direction, float speed) {
    float _speed = direction ? speed : (-1 * speed);
    stepper.setSpeed(_speed);
    active = true;
}

void StepperMotor::press() {
    stepper.runSpeed();
}


void StepperMotor::stop() {
    stepper.stop();
    stepper.setSpeed(0);
    active = false;
}