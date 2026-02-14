#include "GeoFetchSystem.h"

GeoFetchSystem::GeoFetchSystem(
    ContainerManager *containerSystem, 
    DrillManager *drillSystem
): containerSystem(containerSystem), drillSystem(drillSystem) {
    
}

void GeoFetchSystem::prepareSystem() {
    if (state != SystemState::UNKNOWN && state != SystemState::PREPARING) return;
    
    state = SystemState::PREPARING;

    if (containerSystem->getState() != ContainerState::CLOSED) {
        containerSystem->close();
    }

    if (drillSystem->getState() == DrillState::UNKNOWN) {
        drillSystem->home();
    } else if (drillSystem->getState() == DrillState::DRILLING) {
        drillSystem->retract();
    } else if (drillSystem->getState() == DrillState::CLEANING) {
        drillSystem->stopCleaning();
    } else if (drillSystem->getState() == DrillState::RETRIEVING) {
        drillSystem->stopRetrieving();
    }
}

void GeoFetchSystem::drill() {
    if (state != SystemState::READY) return;
    state = SystemState::DRILLING;
    drillStartTime = millis();
    drillSystem->drill();
}


void GeoFetchSystem::collectMaterial() {
    if (state != SystemState::READY) return;

    state = SystemState::COLLECTING_MATERIAL;

    containerSystem->nextSample();
}

void GeoFetchSystem::drillProcess() {
    if (drillSystem->getState() == DrillState::READY) {
        state = SystemState::READY;
        return;
    }

    if (drillSystem->getState() == DrillState::DRILLING 
        && millis() - drillStartTime >= SystemSettings::DRILLING_DURATION
    ) {
        drillSystem->retract();
    }
}


void GeoFetchSystem::collectMaterialProcess() {
    DrillState drillState = drillSystem->getState();
    ContainerState containerState = containerSystem->getState();

    if (containerState == ContainerState::READY && drillState != DrillState::RETRIEVING) {
        containerSystem->tareLoadCell();
        drillSystem->retrieve();
        retrievingStartTime = millis();
        return;
    }
    if (containerState == ContainerState::READY && drillState == DrillState::RETRIEVING
        && (millis() - retrievingStartTime >= SystemSettings::RETRIEVING_TIMEOUT
        || containerSystem->getWeightOfCurrentSample() >= SystemSettings::TARGET_WEIGHT)) {
            drillSystem->stopRetrieving();
            drillSystem->clean();
            cleaningStartTime = millis();
            state = SystemState::CLEANING;
            return;
    }
}

void GeoFetchSystem::cleaningProcess() {
    DrillState drillState = drillSystem->getState();
    ContainerState containerState = containerSystem->getState();

    if (containerState == ContainerState::READY
        && drillState == DrillState::CLEANING
        && millis() - cleaningStartTime >= SystemSettings::CLEAN_DURATION
    ) {
        drillSystem->stopCleaning();
        containerSystem->close();
        return;
    }

    if (containerState == ContainerState::CLOSED) {
        state = SystemState::READY;
    }
}

void GeoFetchSystem::preparingProcess() {
    if (containerSystem->getState() == ContainerState::CLOSED && drillSystem->getState() == DrillState::READY) {
        state = SystemState::READY;
        return;
    }
}

void GeoFetchSystem::checkSystemStatus() {
    if (drillSystem->getState() == DrillState::READY && containerSystem->getState() == ContainerState::CLOSED) {
        state = SystemState::READY;
    }
}


void GeoFetchSystem::update() {
    switch (state) {
        case SystemState::UNKNOWN:
            checkSystemStatus();
            break;
        case SystemState::PREPARING:
            preparingProcess();
            break;
        case SystemState::DRILLING:
            drillProcess();
            break;
        case SystemState::COLLECTING_MATERIAL:
            collectMaterialProcess();
            break;
        case SystemState::CLEANING:
            cleaningProcess();
            break;
        default:
            break;
    }
}

SystemState GeoFetchSystem::getSystemState() {
    return state;
}
