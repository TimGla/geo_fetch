#ifndef LOADCELL_H
#define LOADCELL_H

#include <HX711.h>

class LoadCell {
private:
    HX711 scale;
    int dout_pin;
    int sck_pin;
    float calibration_factor;
    
public:
    LoadCell(int dout_pin, int sck_pin, float claibration_factor);
    void init();
    float getWeight();
    void tare();
    bool isReady();
};

#endif