#ifndef SERVOMOTOR_H
#define SERVOMOTOR_H

#include <ESP32Servo.h>

class ServoMotor {
private:
    Servo servo;

public:
    ServoMotor(int sig_pin);
    void init();
    void rotateTo(int angle);  
};

#endif