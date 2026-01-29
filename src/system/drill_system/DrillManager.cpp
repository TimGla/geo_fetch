#include "DrillManager.h"

DrillManager::DrillManager(
    Auger *auger,
    Press *press, 
    EndSwitch *upperSwitch
) : auger(auger), press(press), upperSwitch(upperSwitch) {
}

void DrillManager::initState() {
    if (upperSwitch->isActive()) {
        auger->softStop();
        press->stop();
        state = DrillState::READY;
    }
}

void DrillManager::home() {
    if (state != DrillState::UNKNOWN) return;
    state = DrillState::HOMING;
    press->stop();
    auger->softStop();
    press->setDirection(PressDirection::UP);
}

void DrillManager::drill() {
    if (state != DrillState::READY) return;
    state = DrillState::DRILLING;
    auger->turnRight();
    press->setTarget(DrillSettings::PRESS_DRILLING_TARGET);
    press->setDirection(PressDirection::DOWN);
}

void DrillManager::retract() {
    if (state != DrillState::DRILLING) return;
    state = DrillState::RETRACTING;
    auger->softStop();
    press->setDirection(PressDirection::UP);
}

void DrillManager::retrieve() {
    if (state != DrillState::READY) return;
    state = DrillState::RETRIEVING;
    press->stop();
    auger->softStop();
    auger->turnRight();
}

void DrillManager::stopRetrieving() {
    if (state != DrillState::RETRIEVING) return;
    auger->softStop();
    state = DrillState::READY;
}

void DrillManager::clean() {
    if (state != DrillState::READY) return;
    state = DrillState::CLEANING;
    press->stop();
    auger->softStop();
    auger->turnLeft();
}

void DrillManager::stopCleaning() {
    if (state != DrillState::CLEANING) return;
    auger->softStop();
    state = DrillState::READY;
}

DrillState DrillManager::getState() {
    return state; 
}

void DrillManager::update() {
    switch (state) {
        case DrillState::UNKNOWN:
            readyCheck();
            break;
        case DrillState::READY:
            readyCheck();
            break;
        case DrillState::HOMING:
            homingProcess();
            break;
        case DrillState::DRILLING:
            drillingProcess();
            break;
        case DrillState::RETRACTING:
            retractingProcess();
            break;
        default:
            break;
    }
}

void DrillManager::readyCheck() {
    if (!upperSwitch->isActive()) {
        state = DrillState::UNKNOWN;
        return;
    }
    state = DrillState::READY;
}

void DrillManager::homingProcess() {
    if (upperSwitch->isActive()) {
        press->stop();
        press->setHome();
        state = DrillState::READY;
        return;
    }
    press->press();
}

void DrillManager::drillingProcess() {
    press->press();
}

void DrillManager::retractingProcess() {
    if (upperSwitch->isActive()) {
        press->stop();
        state = DrillState::READY;
        return;
    }
    press->press();
}