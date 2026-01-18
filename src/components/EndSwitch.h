#ifndef STOP_SWITCH_H
#define STOP_SWITCH_H

#include <Arduino.h>
class EndSwitch {
    int pin;

    EndSwitch(int pin);
    bool isActive();
};

#endif