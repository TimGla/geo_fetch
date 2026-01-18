#include "ContainerSpinner.h"

ContainerSpinner::ContainerSpinner(int sig_pin) {
    servo.attach(sig_pin);
}

void ContainerSpinner::setVelocity(int velocity) {
    servo.write(velocity);
}