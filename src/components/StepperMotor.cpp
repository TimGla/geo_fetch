#include "StepperMotor.h"
#include <Arduino.h>

StepperMotor::StepperMotor(
    int step_pin, 
    int dir_pin,
    float deg_per_step,
    AccelStepper::MotorInterfaceType type
) : stepper(type, step_pin, dir_pin), degrees_per_step(deg_per_step) {
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

float StepperMotor::setDirection(bool direction) {
    float _speed = direction ? speed : (-1 * speed);
    stepper.setCurrentPosition(0);
    stepper.setSpeed(_speed);
    active = true;
    return stepper.speed();
}

void StepperMotor::press() {
    stepper.runSpeed();
}


void StepperMotor::stop() {
    //stepper.stop();
    stepper.setSpeed(0);
    stepper.setCurrentPosition(0);
    active = false;
}