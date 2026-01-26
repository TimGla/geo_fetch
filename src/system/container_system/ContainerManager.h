#ifndef CONTAINER_MANAGER_H
#define CONTAINER_MANAGER_H

#include <components/EndSwitch.h>
#include <components/ContainerSpinner.h>
#include <components/LoadCell.h>
#include <Config.h>

enum class ContainerState {
    UNKNOWN,
    CLOSING,
    CLOSED,
    OPENING,
    READY,
    REVOLVING,
    CONTAINER_FULL,
};

class ContainerManager {
private:
    ContainerSpinner *spinner;
    EndSwitch *homeSwitch;
    LoadCell *loadCell;
    ContainerState state = ContainerState::UNKNOWN;

    unsigned int maxSamples;
    unsigned int currentSample = 1;

    long openingTarget = ContainerSettings::OPENING_TARGET;
    long nextTargetPerCompartment = ContainerSettings::TARGET_PER_COMPARTMENT;

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