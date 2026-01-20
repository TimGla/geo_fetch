#include "DrillManager.h"

DrillManager::DrillManager(
    Auger *auger,
    Press *press, 
    EndSwitch *upperSwitch, 
    EndSwitch *lowerSwitch)
    : auger(auger), press(press), upperSwitch(upperSwitch), lowerSwitch(lowerSwitch) {
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
    press->setDirection(PressDirection::DOWN);
}

void DrillManager::retract() {
    if (state != DrillState::DRILLING) return;
    state = DrillState::RETRACTING;
    auger->softStop();
    press->setDirection(PressDirection::UP);
}

void DrillManager::clean() {
    if (state != DrillState::READY) return;
    state = DrillState::CLEANING;
    cleanStartTime = millis();
    press->stop();
    auger->softStop();
    auger->turnLeft();
}

DrillState DrillManager::getState() {
    return state; 
}

void DrillManager::update() {
    switch (state) {
        case DrillState::HOMING:
            homingProcess();
            break;
        case DrillState::DRILLING:
            drillingProcess();
            break;
        case DrillState::RETRACTING:
            retractingProcess();
            break;
        case DrillState::CLEANING:
            cleaningProcess();
            break;
        default:
            break;
    }
}

void DrillManager::homingProcess() {
    if (upperSwitch->isActive()) {
        press->stop();
        state = DrillState::READY;
        return;
    }
    press->press();
}

void DrillManager::drillingProcess() {
    if (lowerSwitch->isActive()) {
        press->stop();
        return;
    }
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

void DrillManager::cleaningProcess() {
    if (millis() - cleanStartTime >= DrillSettings::CLEAN_DURATION_MS) {
        auger->softStop();
        state = DrillState::READY;
        return;
    }
}