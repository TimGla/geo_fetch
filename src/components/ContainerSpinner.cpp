#include "ContainerSpinner.h"

ContainerSpinner::ContainerSpinner(
    int en_pin,
    int step_pin, 
    int dir_pin,
    float deg_per_step,
    float maxSpeed,
    AccelStepper::MotorInterfaceType type
) : stepper(type, step_pin, dir_pin), degrees_per_step(deg_per_step), maxSpeed(maxSpeed), en_pin(en_pin) {

}

void ContainerSpinner::init() {
    stepper.setEnablePin(en_pin);
    stepper.setPinsInverted(false, false, true);
    stepper.setMaxSpeed(maxSpeed);
    stepper.disableOutputs();
}

void ContainerSpinner::setHome() {
    stepper.setCurrentPosition(0);
}

void ContainerSpinner::setTarget(long target) {
    stepper.moveTo(target);
}

void ContainerSpinner::setMode(SpinMode mode) {
    stepper.enableOutputs();
    if (mode == SpinMode::SPIN) {
        status = ContainerSpinnerStatus::SPINNING;
        return;
    }
    stepper.setSpeed(-1 * maxSpeed);
    status = ContainerSpinnerStatus::RETURNING;
}

void ContainerSpinner::spin() {
    if (status == ContainerSpinnerStatus::SPINNING) {
        if (!stepper.run()) {
            stop();
        }
        return;
    }
    stepper.runSpeed();
}

void ContainerSpinner::stop() {
    if (status == ContainerSpinnerStatus::IDLE) return;
    stepper.moveTo(stepper.currentPosition());
    stepper.setSpeed(0);
    stepper.disableOutputs();
    status = ContainerSpinnerStatus::IDLE;
}

long ContainerSpinner::getPosition() {
    return stepper.currentPosition();
}

bool ContainerSpinner::isRunning() {
    return status != ContainerSpinnerStatus::IDLE;
}