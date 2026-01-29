#include "ContainerManager.h"

ContainerManager::ContainerManager(
    ContainerSpinner *spinner,
    EndSwitch *homeSwitch,
    LoadCell *loadCell,
    unsigned int maxSamples
): spinner(spinner), homeSwitch(homeSwitch),  loadCell(loadCell), maxSamples(maxSamples) {

};

void ContainerManager::initiState() {
    if (homeSwitch->isActive()) {
        spinner->setHome();
        state = ContainerState::CLOSED;
    }
}

void ContainerManager::close() {
    if (state != ContainerState::UNKNOWN 
        && state != ContainerState::CONTAINER_FULL
        && state != ContainerState::READY) return;
    state = ContainerState::CLOSING;
    spinner->setMode(SpinMode::RETURN);
}


void ContainerManager::open() {
    if (state != ContainerState::CLOSED) return;
    state = ContainerState::OPENING;
    spinner->setMode(SpinMode::SPIN);
    spinner->setTarget(openingTarget);
}

void ContainerManager::nextSample() {
    if (state != ContainerState::READY) return;
    state = ContainerState::REVOLVING;
    spinner->setMode(SpinMode::SPIN);
    spinner->setTarget(spinner->getPosition() + nextTargetPerCompartment);
}

float ContainerManager::getWeightOfCurrentSample() {
    return loadCell->getWeight();
}

ContainerState ContainerManager::getState() {
    return state;
}

void ContainerManager::update() {
    switch (state) {
        case ContainerState::UNKNOWN:
            readyCheck();
            break;
        case ContainerState::READY:
            readyCheck();
            break;
        case ContainerState::CLOSING:
            closingProcess();
            break;
        case ContainerState::OPENING:
            openingProcess();
            break;
        case ContainerState::REVOLVING:
            revolvingProcess();
            break;
        default:
            break;    
    }
}

void ContainerManager::readyCheck() {
    if (!homeSwitch->isActive()) {
        state = ContainerState::UNKNOWN;
        return;
    }
    state = ContainerState::READY;
}

void ContainerManager::revolvingProcess() {
    if (!spinner->isRunning()) {
        currentSample++;
        loadCell->tare();
        state = currentSample < maxSamples ? ContainerState::READY : ContainerState::CONTAINER_FULL;
        return;
    }
    spinner->spin();
}


void ContainerManager::openingProcess() {
    if (!spinner->isRunning()) {
        currentSample = 1;
        loadCell->tare();
        state = ContainerState::READY;
        return;
    }
    spinner->spin();
}


void ContainerManager::closingProcess() {
    if (homeSwitch->isActive()) {
        spinner->stop();
        spinner->setHome();
        currentSample = 0;
        state = ContainerState::CLOSED;
        return;
    }
    spinner->spin();
}