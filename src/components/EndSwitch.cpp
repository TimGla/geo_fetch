#include "EndSwitch.h"

EndSwitch::EndSwitch(int pin): pin(pin) {}

bool EndSwitch::isActive() {
    return digitalRead(pin) == LOW;
}