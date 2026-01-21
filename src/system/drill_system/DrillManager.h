#ifndef DRILL_MANAGER_H
#define DRILL_MANAGER_H

#include <Config.h>
#include <components/Auger.h>
#include <components/EndSwitch.h>
#include <components/Press.h>

enum class DrillState {
    UNKNOWN,
    READY,
    HOMING,
    DRILLING,
    RETRACTING,
    CLEANING,
};

class DrillManager {
private:
    Auger *auger;
    Press *press;
    EndSwitch *upperSwitch;
    unsigned long cleanStartTime = 0;
    DrillState state = DrillState::UNKNOWN;

    void homingProcess();
    void drillingProcess();
    void retractingProcess();
    void cleaningProcess();

public:
    DrillManager(Auger *auger, Press *press, EndSwitch *upperSwitch);
    void initState();
    void home();
    void drill();
    void retract();
    void clean();
    void update();
    DrillState getState();
};

#endif