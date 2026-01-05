#include "ServoMotor.h"

ServoMotor::ServoMotor(int sig_pin) {
    servo.attach(sig_pin);
}

void ServoMotor::init() {

}

void ServoMotor::rotateTo(int angle) {
    servo.write(angle);
}