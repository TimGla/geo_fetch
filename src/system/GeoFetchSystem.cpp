#include "GeoFetchSystem.h"

GeoFetchSystem::GeoFetchSystem(
    ContainerManager *containerSystem, 
    DrillManager *drillSystem
): containerSystem(containerSystem), drillSystem(drillSystem) {
    
}

void GeoFetchSystem::prepareSystem() {
    if (state != SystemState::UNKNOWN && state != SystemState::FINISHED) return;
    state = SystemState::PREPARING;

    // Determine state of subsystems
    containerSystem->initiState();
    drillSystem->initState();
    
    // Start first step of preperation of container system
    if (containerSystem->getState() == ContainerState::UNKNOWN) {
        containerSystem->close();
    } else if (containerSystem->getState() == ContainerState::CLOSED) {
        containerSystem->open();
    } else {
        state = SystemState::ERROR;
        return;
    }

    // Start first step of preperation of drill system
    if (drillSystem->getState() == DrillState::UNKNOWN) {
        drillSystem->home();
    } else if (drillSystem->getState() != DrillState::READY) {
        state = SystemState::ERROR;
        return;
    }
}

void GeoFetchSystem::takeSample() {
    if (state != SystemState::READY) return;
    state = SystemState::TAKING_SAMPLE;
    drillSystem->drill();
}

void GeoFetchSystem::takingSampleProcess() {
    // TODO (At what time start collecting?)
}

void GeoFetchSystem::preparingProcess() {
    if (containerSystem->getState() == ContainerState::READY && drillSystem->getState() == DrillState::READY) {
        state = SystemState::READY;
        return;
    }
    if (containerSystem->getState() == ContainerState::CLOSED) {
        containerSystem->open();
    }
}



void GeoFetchSystem::update() {
    containerSystem->update();
    drillSystem->update();
    switch (state) {
        case SystemState::PREPARING:
            preparingProcess();
            break;
        case SystemState::TAKING_SAMPLE:
            takingSampleProcess();
            break;
        default:
            break;
    }
}
