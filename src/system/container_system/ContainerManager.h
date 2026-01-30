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
    OPENING = 3,
    READY = 4,
    REVOLVING = 5,
    CONTAINER_FULL = 6,
};

class ContainerManager {
private:
    ContainerSpinner *spinner;
    EndSwitch *homeSwitch;
    LoadCell *loadCell;
    ContainerState state = ContainerState::UNKNOWN;

    unsigned int maxSamples;
    unsigned int currentSample = 1;

    void readyCheck();
    void closingProcess();
    void openingProcess();
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
    void open();
    void nextSample();

    float getWeightOfCurrentSample();
    void update();
    ContainerState getState();
};

#endif