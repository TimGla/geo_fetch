#include "Auger.h"

Auger::Auger(
    int l_en_pin, 
    int r_en_pin,
    int l_pwn_pin,
    int r_pwm_pin)
    : motorController(l_en_pin, r_en_pin, l_pwn_pin, r_pwm_pin) {
    };

void Auger::init() {

    stop();
}

void Auger::turnLeft(uint8_t speed) {
    status = AugerStatus::SPIN_LEFT;
    motorController.Enable();
    this->speed = speed;
    motorController.TurnLeft(speed);
}

void Auger::turnRight(uint8_t speed) {
    status = AugerStatus::SPIN_RIGHT;
    motorController.Enable();
    this->speed = speed;
    motorController.TurnRight(speed);
}

void Auger::softStop() {
    if (status == AugerStatus::IDLE) return;
    while (speed != 0) { 
        speed = speed <= 20 ? 0 : (speed - 20);
        if (speed == 0) break;
        if (status == AugerStatus::SPIN_LEFT) {
            motorController.TurnLeft(speed);
            delay(50);
            continue;
        }
        motorController.TurnRight(speed);
        delay(50);
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

