#ifndef DRILL_MANAGER_H
#define DRILL_MANAGER_H

#include <Config.h>
#include <components/Auger.h>
#include <components/EndSwitch.h>
#include <components/Press.h>

enum class DrillState: u_int8_t {
    UNKNOWN = 0,
    READY = 1,
    HOMING = 2,
    DRILLING = 3,
    RETRACTING = 4,
    RETRIEVING = 5,
    CLEANING = 6,
};

class DrillManager {
private:
    Auger *auger;
    Press *press;
    EndSwitch *upperSwitch;
    DrillState state = DrillState::UNKNOWN;

    void readyProcess();
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