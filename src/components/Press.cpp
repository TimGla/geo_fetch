#include "Press.h"
#include <Arduino.h>

Press::Press(
    int en_pin, 
    int step_pin, 
    int dir_pin,
    float deg_per_step,
    float maxSpeed,
    AccelStepper::MotorInterfaceType type
) : stepper(type, step_pin, dir_pin), degrees_per_step(deg_per_step), maxSpeed(maxSpeed), en_pin(en_pin) {
}

void Press::init() {
    stepper.setEnablePin(en_pin);
    stepper.setPinsInverted(false, false, true);
    stepper.setMaxSpeed(maxSpeed);
    stepper.setAcceleration(maxSpeed);
    stepper.disableOutputs();
}

void Press::setHome() {
    stepper.setCurrentPosition(0);
}

void Press::setTarget(long target) {
    stepper.moveTo(target);
} 

void Press::setDirection(PressDirection direction) {
    stepper.enableOutputs();
    if (direction == PressDirection::DOWN) {
        status = PressStatus::PRESSING_DOWN;
        return;
    }
    stepper.setSpeed(-1 * maxSpeed);
    status = PressStatus::PRESSING_UP;
}

void Press::press() {
    if (status == PressStatus::PRESSING_DOWN) {
        if (!stepper.run()) {
            stop();
        }
        return;
    }
    stepper.runSpeed();
}

void Press::stop() {
    if (status == PressStatus::IDLE) return;
    stepper.moveTo(stepper.currentPosition());
    stepper.setSpeed(0);
    stepper.disableOutputs();
    status = PressStatus::IDLE;
}


bool Press::isRunning() {
    return status != PressStatus::IDLE;
}

long Press::getPos() {
    return stepper.currentPosition();
}