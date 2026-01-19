#include "Auger.h"

Auger::Auger(
    int l_en_pin, 
    int r_en_pin,
    int l_pwn_pin,
    int r_pwm_pin)
    : motorController(l_en_pin, r_en_pin, l_pwn_pin, r_pwm_pin) {};

void Auger::init() {
    stop();
}

void Auger::turnLeft() {
    status = AugerStatus::SPIN_LEFT;
    motorController.Enable();
    speed = maxSpeed;
    motorController.TurnLeft(speed);
}

void Auger::turnRight() {
    status = AugerStatus::SPIN_RIGHT;
    motorController.Enable();
    speed = maxSpeed;
    motorController.TurnRight(speed);
}

void Auger::softStop() {
    if (status == AugerStatus::IDLE) return;
    while (speed != 0) { 
        speed >>= 1;
        if (speed == 0) break;
        if (status == SPIN_LEFT) {
            motorController.TurnLeft(speed);
            delay(100);
            continue;
        }
        motorController.TurnRight(speed);
        delay(100);
    }
    stop();
}

void Auger::stop() {
    speed = 0;
    motorController.Stop();
    motorController.Disable();
    status = AugerStatus::IDLE;
}

bool Auger::isRunning() {
    return status != AugerStatus::IDLE;
}

