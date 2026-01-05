#include "LoadCell.h"

LoadCell::LoadCell(int dout_pin, int sck_pin) {
    scale.begin(dout_pin, sck_pin);
    calibration_factor = 415.0;
}

void LoadCell::init() {
    scale.set_scale(calibration_factor);
    scale.tare();
}

float LoadCell::getWeight() {
    return scale.get_units(10);
}

void LoadCell::tare() {
    scale.tare();
}

bool LoadCell::isReady() {
    return scale.is_ready();
}