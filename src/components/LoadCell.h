#ifndef LOADCELL_H
#define LOADCELL_H

#include <HX711.h>

class LoadCell {
private:
    HX711 scale;
    float calibration_factor;
    
public:
    LoadCell(int dout_pin, int sck_pin);
    void init();
    float getWeight();
    void tare();
    bool isReady();
};

#endif