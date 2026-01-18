#ifndef Press_H
#define Press_H

#include <AccelStepper.h>

class Press {
private:
    AccelStepper stepper;
    float degrees_per_step;
    float speed = 1000;
    long steps_per_revolution;
    bool active = false;
    int en_pin;

public:
    Press(
        int en_pin,
        int step_pin, 
        int dir_pin,
        float deg_per_step, 
        AccelStepper::MotorInterfaceType motor_type = AccelStepper::MotorInterfaceType::DRIVER
    );
    void init();
    bool isActive();
    void setDirection(bool direction);
    void press();
    void stop();
};


#endif



