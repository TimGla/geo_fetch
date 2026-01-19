#ifndef STOP_SWITCH_H
#define STOP_SWITCH_H

#include <Arduino.h>
class EndSwitch {
private:    
    int pin;

public:
    EndSwitch(int pin);
    bool isActive();
};

#endif