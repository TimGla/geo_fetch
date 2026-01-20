#include "ContainerManager.h"

ContainerManager::ContainerManager(
    ContainerSpinner *spinner,
    EndSwitch *homeSwitch,
    LoadCell *loadCell,
    float targetWeight,
    long openingTarget,
    long collectingTarget
): spinner(spinner), homeSwitch(homeSwitch),  loadCell(loadCell), targetWeight(targetWeight), openingTarget(openingTarget), collectingTarget(collectingTarget) {

};

void ContainerManager::initiState() {
    if (homeSwitch->isActive()) {
        spinner->setHome();
        state = ContainerState::CLOSED;
    }
}

void ContainerManager::close() {
    if (state != ContainerState::UNKNOWN && state != ContainerState::FINISHED) return;
    state = ContainerState::CLOSING;
    spinner->setMode(SpinMode::RETURN);
}


void ContainerManager::open() {
    if (state != ContainerState::CLOSED) return;
    state = ContainerState::OPENING;
    spinner->setMode(SpinMode::SPIN);
    spinner->setTarget(openingTarget);
}

void ContainerManager::collect() {
    if (state != ContainerState::READY) return;
    state = ContainerState::COLLECTING;
    loadCell->tare();
    spinner->setMode(SpinMode::SPIN);
    spinner->setTarget(collectingTarget);
}

void ContainerManager::smartCollect() {
    if (state != ContainerState::READY) return;
    state = ContainerState::SMART_COLLECTING;
    loadCell->tare();
    spinner->setMode(SpinMode::SPIN);
    // TODO
}

void ContainerManager::update() {
    switch (state) {
        case ContainerState::CLOSING:
            closingProcess();
            break;
        case ContainerState::COLLECTING:
            collectingProcess();
            break;
        case ContainerState::OPENING:
            openingProcess();
            break;
        case ContainerState::SMART_COLLECTING:
            smartCollectingProcess();
            break;
        default:
            break;    
    }
}

void ContainerManager::smartCollectingProcess() {
    // TODO
}

void ContainerManager::collectingProcess() {
    if (!spinner->isRunning() || loadCell->getWeight() >= targetWeight) {
        spinner->stop();
        state = ContainerState::FINISHED;
        return;
    }
    spinner->spin();
}


void ContainerManager::openingProcess() {
    if (!spinner->isRunning()) {
        state = ContainerState::READY;
        return;
    }
    spinner->spin();
}


void ContainerManager::closingProcess() {
    if (homeSwitch->isActive()) {
        spinner->stop();
        spinner->setHome();
        state = ContainerState::CLOSED;
        return;
    }
    spinner->spin();
}