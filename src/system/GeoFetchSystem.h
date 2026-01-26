#ifndef GEO_FETCH_SYSTEM_H
#define GEO_FETCH_SYSTEM_H

#include "container_system/ContainerManager.h"
#include "drill_system/DrillManager.h"

enum class SystemState {
    UNKNOWN,
    PREPARING,
    READY,
    TAKING_SAMPLE,
    FINISHED,
    ERROR,
};

class GeoFetchSystem {
private:
    ContainerManager *containerSystem;
    DrillManager *drillSystem;
    SystemState state = SystemState::UNKNOWN;

    void preparingProcess();
    void takingSampleProcess();

public:
    GeoFetchSystem(ContainerManager *containerSystem, DrillManager *drillSystem);

    void prepareSystem();
    void takeSample();

    void update();
    SystemState getSystemState();
};

#endif