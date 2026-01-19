#ifndef DRILL_MANAGER_H
#define DRILL_MANAGER_H

#include <Config.h>
#include <components/Auger.h>
#include <components/EndSwitch.h>
#include <components/Press.h>

enum DrillState {
    IDLE,
    HOMING,
    DRILLING,
    RETRACTING,
    CLEANING,
    ERROR,
};

class DrillManager {
private:
    Auger *auger;
    Press *press;
    EndSwitch *upperSwitch;
    EndSwitch *lowerSwitch;
    DrillState state = DrillState::IDLE;
    unsigned long cleanStartTime = 0;

    void homingProcess();
    void drillingProcess();
    void retractingProcess();
    void cleaningProcess();

public:
    DrillManager(Auger *auger, Press *press, EndSwitch *upperSwitch, EndSwitch *lowerSwitch);
    void update();
    void home();
    void drill();
    void retract();
    void clean();
};

#endif