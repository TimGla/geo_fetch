#ifndef CONTAINER_MANAGER_H
#define CONTAINER_MANAGER_H

#include <components/EndSwitch.h>
#include <components/ContainerSpinner.h>
#include <components/LoadCell.h>
#include <Config.h>

enum class ContainerState: u_int8_t {
    UNKNOWN = 0,
    CLOSING = 1,
    CLOSED = 2,
    READY = 3,
    REVOLVING = 4,
    CONTAINER_FULL = 5,
};

class ContainerManager {
private:
    ContainerSpinner *spinner;
    EndSwitch *homeSwitch;
    LoadCell *loadCell;
    ContainerState state = ContainerState::UNKNOWN;

    unsigned long maxSamples;
    unsigned long currentSample = 0;

    void readyCheck();
    void closingProcess();
    void revolvingProcess();

public:
    ContainerManager(
        ContainerSpinner *spinner, 
        EndSwitch *homeSwitch, 
        LoadCell *loadCell,
        unsigned int maxSamples
    );
    void initiState();
    void close();
    void nextSample();
    void tareLoadCell();
    float getWeightOfCurrentSample();
    void update();
    ContainerState getState();
};

#endif