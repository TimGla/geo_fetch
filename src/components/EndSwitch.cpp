#include "EndSwitch.h"

EndSwitch::EndSwitch(int pin): pin(pin) {}

void EndSwitch::init() {
    pinMode(pin, INPUT_PULLUP);
}

bool EndSwitch::isActive() {
    return digitalRead(pin) == LOW;
}