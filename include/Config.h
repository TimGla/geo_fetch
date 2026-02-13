#ifndef CONFIG_H
#define CONFIG_H

namespace DrillSettings {
    const unsigned long CLEAN_DURATION_MS = 10000;

    const int AUGER_DRILL_SPEED = 71;
    const int AUGER_RETRIEVE_SPEED = 255;

    const long PRESS_DRILLING_TARGET = 100000; 
    const float PRESS_MAX_RETRACTING_SPEED = 6400;
    const float PRESS_MAX_DRILLING_SPEED = 1600;
    const float PRESS_DEG_PER_STEP = 1.8;
}

namespace ContainerSettings {
    const unsigned int NUM_COMPARTMENTS = 5;

    const float SPINNER_MAX_SPEED = 640;
    const float SPINNER_DEG_PER_STEP = 1.8;
    const float LOADCELL_CALIBRATION_FACTOR = 415.0;

    const long OPENING_TARGET = 1950;
    const long TARGET_PER_COMPARTMENT = 2150;
}


namespace ContainerPins {
    const int SPINNER_EN_PIN = 12;
    const int SPINNER_STEP_PIN = 14;
    const int SPINNER_DIR_PIN = 27;

    const int LOADCELL_DOUT_PIN = 35;
    const int LOADCELL_SCK_PIN = 32;

    const int HOME_SWITCH_SIG_PIN = 26;
}



namespace DrillPins {
    const int AUGER_L_EN_PIN = 15;
    const int AUGER_R_EN_PIN = 2;
    const int AUGER_L_PWN_PIN = 16;
    const int AUGER_R_PWN_PIN = 4;

    const int PRESS_EN_PIN = 34;
    const int PRESS_STEP_PIN = 33;
    const int PRESS_DIR_PIN = 25;

    const int UPPER_SWITCH_SIG_PIN = 39;
}

#endif