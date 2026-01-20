#ifndef Press_H
#define Press_H

#include <AccelStepper.h>

enum class PressStatus {
    IDLE,
    PRESSING_DOWN,
    PRESSING_UP
};

enum class PressDirection {
    UP,
    DOWN
};

class Press {
private:
    PressStatus status = PressStatus::IDLE;
    AccelStepper stepper;
    
    float degrees_per_step;
    float speed = 1000;
    long steps_per_revolution;
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
    bool isRunning();
    void setDirection(PressDirection direction);
    void press();
    void stop();
};


#endif



