#ifndef CONTAINER_MANAGER_H
#define CONTAINER_MANAGER_H

#include <components/EndSwitch.h>
#include <components/ContainerSpinner.h>
#include <components/LoadCell.h>

enum ContainerState {
    UNKNOWN,
    CLOSING,
    CLOSED,
    OPENING,
    READY,
    SMART_COLLECTING,
    COLLECTING,
    FINISHED,
};

class ContainerManager {
private:
    ContainerSpinner *spinner;
    EndSwitch *homeSwitch;
    LoadCell *loadCell;
    ContainerState state = ContainerState::UNKNOWN;
    
    float targetWeight;

    long openingTarget;
    long collectingTarget;

    void closingProcess();
    void openingProcess();
    void smartCollectingProcess();
    void collectingProcess();

public:
    ContainerManager(ContainerSpinner *spinner, EndSwitch *homeSwitch, LoadCell *loadCell, float targetWeight, long openingTarget, long collectingTarget);
    void initiState();
    void close();
    void open();
    void collect();
    void smartCollect();
    void update();
    ContainerState getState();

};

#endif