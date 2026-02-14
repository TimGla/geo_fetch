#include "LoadCell.h"

LoadCell::LoadCell(
    int dout_pin, 
    int sck_pin, 
    float calibration_factor
): dout_pin(dout_pin), sck_pin(sck_pin), calibration_factor(calibration_factor) {

}

void LoadCell::init() {
    scale.begin(dout_pin, sck_pin);
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