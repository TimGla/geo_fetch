#ifndef Press_H
#define Press_H

#include <AccelStepper.h>

enum class PressDirection {
    UP,
    DOWN
};

enum class PressStatus {
    IDLE,
    PRESSING_DOWN,
    PRESSING_UP
};

class Press {
private:
    AccelStepper stepper;
    PressStatus status = PressStatus::IDLE;
    
    float max_down_speed;
    float max_up_speed;
    float degrees_per_step;
    int en_pin;

public:
    Press(
        int en_pin,
        int step_pin, 
        int dir_pin,
        float deg_per_step,
        float max_drilling_speed,
        float max_retracting_speed,
        AccelStepper::MotorInterfaceType motor_type = AccelStepper::MotorInterfaceType::DRIVER
    );

    void init();
    void setHome();
    void setTarget(long target);
    void setDirection(PressDirection direction);
    void press();
    void stop();
    bool isRunning();
    long getPos();
};


#endif



