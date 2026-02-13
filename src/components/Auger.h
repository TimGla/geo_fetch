#ifndef AUGER_H
#define AUGER_H

#include <BTS7960.h>
#include "Config.h"

enum class AugerStatus { 
    SPIN_LEFT, 
    SPIN_RIGHT,
    IDLE,
};

class Auger {
private:
    BTS7960 motorController;
    AugerStatus status = AugerStatus::IDLE;
    uint8_t speed = 0;
    
public:
    Auger(int l_en_pin, int r_en_pin, int l_pwn_pin, int r_pwm_pin);
    void init();
    void turnLeft(uint8_t speed);
    void turnRight(uint8_t speed);
    void softStop();
    void stop();
    bool isRunning();
};

#endif