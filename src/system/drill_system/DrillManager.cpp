#include "DrillManager.h"

DrillManager::DrillManager(
    Auger *auger,
    Press *press, 
    EndSwitch *upperSwitch, 
    EndSwitch *lowerSwitch)
    : auger(auger), press(press), upperSwitch(upperSwitch), lowerSwitch(lowerSwitch) {
}

void DrillManager::drill() {
    state = DrillState::DRILLING;
    auger->turnRight();
    press->setDirection(PressDirection::DOWN);
}

void DrillManager::retract() {
    state = DrillState::RETRACTING;
    auger->softStop();
    press->setDirection(PressDirection::UP);
}

void DrillManager::clean() {
    state = DrillState::CLEANING;
    cleanStartTime = millis();
    press->stop();
    auger->softStop();
    auger->turnLeft();
}


void DrillManager::home() {
    state = DrillState::HOMING;
    press->stop();
    auger->softStop();
    press->setDirection(PressDirection::UP);
}

void DrillManager::update() {
    switch (state) {
        case DrillState::IDLE:
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
        case DrillState::CLEANING:
            cleaningProcess();
            break;
        case DrillState::ERROR:
            break;
    }
}

void DrillManager::homingProcess() {
    if (upperSwitch->isActive()) {
        press->stop();
        state = DrillState::IDLE;
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
        state = DrillState::IDLE;
        return;
    }
    press->press();
}

void DrillManager::cleaningProcess() {
    if (millis() - cleanStartTime >= Settings::DRILL_CLEAN_DURATION_MS) {
        auger->softStop();
        state = DrillState::IDLE;
        return;
    }
}