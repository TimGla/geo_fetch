#ifndef AUGER_H
#define AUGER_H

#include <BTS7960.h>

enum AugerStatus { 
    SPIN_LEFT, 
    SPIN_RIGHT,
    STOP,
};

class Auger {
private:
    BTS7960 motorController;
    AugerStatus status = STOP;
    uint8_t speed = 0;
    uint8_t maxSpeed = 255;
    
public:
    Auger(int l_en_pin, int r_en_pin, int l_pwn_pin, int r_pwm_pin);
    void init();
    void turnLeft();
    void turnRight();
    void softStop();
    void stop();
};

#endif