#ifndef CONTAINER_SPINNER_H
#define CONTAINER_SPINNER_H

#include <AccelStepper.h>

enum class SpinMode {
    SPIN, 
    RETURN
};

enum class ContainerSpinnerStatus {
    IDLE,
    SPINNING,
    RETURNING,
};

class ContainerSpinner {
private:
    AccelStepper stepper;
    ContainerSpinnerStatus status = ContainerSpinnerStatus::IDLE;

    float maxSpeed;
    float degrees_per_step;
    int en_pin;

public:
    ContainerSpinner(
        int en_pin,
        int step_pin, 
        int dir_pin,
        float deg_per_step,
        float maxSpeed,
        AccelStepper::MotorInterfaceType motor_type = AccelStepper::MotorInterfaceType::DRIVER
    );

    void init();
    void setHome();
    void setTarget(long target);
    void setMode(SpinMode spinMode);
    void spin();
    void stop();
    bool isRunning();
};

#endif