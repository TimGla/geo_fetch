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
        spinner->stop();
        spinner->setHome();
        state = ContainerState::CLOSED;
    }
}

void ContainerManager::close() {
    if (state != ContainerState::UNKNOWN 
        && state != ContainerState::CONTAINER_FULL
        && state != ContainerState::READY) return;

    if (state == ContainerState::CONTAINER_FULL) {
        currentSample = 0;
    }
    state = ContainerState::CLOSING;
    spinner->stop();
    spinner->setMode(SpinMode::RETURN);
}

void ContainerManager::nextSample() {
    if (state != ContainerState::CLOSED) return;
    state = ContainerState::REVOLVING;
    spinner->setTarget(
        spinner->getPosition() + (++currentSample * ContainerSettings::TARGET_PER_COMPARTMENT)
    );
    spinner->setMode(SpinMode::SPIN);
}

void ContainerManager::reset() {
    if (state != ContainerState::CLOSED 
    && state != ContainerState::CONTAINER_FULL
    && state != ContainerState::READY) return;

    if (state == ContainerState::CLOSED) {
        currentSample = 0;
        return;
    }
    currentSample = 0;
    close();
}

void ContainerManager::tareLoadCell() {
    loadCell->tare();
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
        case ContainerState::CLOSING:
            closingProcess();
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
        state = currentSample < maxSamples ? ContainerState::READY : ContainerState::CONTAINER_FULL;
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