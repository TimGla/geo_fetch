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
    RETRIEVING,
    CLEANING,
};

class DrillManager {
private:
    Auger *auger;
    Press *press;
    EndSwitch *upperSwitch;
    DrillState state = DrillState::UNKNOWN;

    void homingProcess();
    void drillingProcess();
    void retractingProcess();

public:
    DrillManager(Auger *auger, Press *press, EndSwitch *upperSwitch);
    void initState();
    void home();
    void drill();
    void retract();
    void retrieve();
    void stopRetrieving();
    void clean();
    void stopCleaning();
    void update();
    DrillState getState();
};

#endif