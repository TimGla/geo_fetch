#ifndef GEO_FETCH_SYSTEM_H
#define GEO_FETCH_SYSTEM_H

#include "container_system/ContainerManager.h"
#include "drill_system/DrillManager.h"

enum class SystemState: u_int8_t {
    UNKNOWN = 0,
    PREPARING = 1,
    READY = 2,
    DRILLING = 3,
    COLLECTING_MATERIAL = 4,
    CLEANING = 5,
    ERROR = 6,
};

class GeoFetchSystem {
private:
    ContainerManager *containerSystem;
    DrillManager *drillSystem;
    SystemState state = SystemState::UNKNOWN;

    unsigned long drillStartTime = 0;
    unsigned long retrievingStartTime = 0;
    unsigned long cleaningStartTime = 0;

    void checkSystemStatus();
    void preparingProcess();
    void drillProcess();
    void collectMaterialProcess();
    void cleaningProcess();

public:
    GeoFetchSystem(ContainerManager *containerSystem, DrillManager *drillSystem);

    void prepareSystem();
    void drill();
    void collectMaterial();

    void update();
    SystemState getSystemState();
};

#endif