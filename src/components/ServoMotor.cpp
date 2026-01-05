#include "ServoMotor.h"

ServoMotor::ServoMotor(int sig_pin) {
    servo.attach(sig_pin);
}

void ServoMotor::init() {

}

void ServoMotor::setVelocity(int velocity) {
    servo.write(velocity);
}