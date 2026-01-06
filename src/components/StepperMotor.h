#ifndef STEPPER_MOTOR_H
#define STEPPER_MOTOR_H

#include <AccelStepper.h>

class StepperMotor {
private:
    AccelStepper stepper;
    float degrees_per_step;
    long steps_per_revolution;
    bool active = false;

public:
    StepperMotor(
        int pin_1, 
        int pin_2, 
        int pin_3, 
        int pin_4, 
        float deg_per_step, 
        AccelStepper::MotorInterfaceType motor_type = AccelStepper::MotorInterfaceType::FULL4WIRE
    );
    void init();
    bool isActive();
    void setDirection(bool direction, float speed = 50);
    void press();
    void stop();
};


#endif