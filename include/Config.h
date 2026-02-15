#ifndef CONFIG_H
#define CONFIG_H


namespace SystemSettings {
    const unsigned long DRILLING_DURATION = 100000; //ms
    const unsigned long CLEAN_DURATION = 10000;
    const unsigned long RETRIEVING_TIMEOUT = 60000;

    const float TARGET_WEIGHT = 15.0;
}

namespace DrillSettings {

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
    const float LOADCELL_CALIBRATION_FACTOR = 903.0;

    // Totoal rotation 10550
    const long TARGET_PER_COMPARTMENT = 10550 / NUM_COMPARTMENTS;
}



namespace ContainerPins {
    const int SPINNER_EN_PIN = 25;
    const int SPINNER_DIR_PIN = 26;
    const int SPINNER_STEP_PIN = 27;

    const int LOADCELL_DOUT_PIN = 35; // green
    const int LOADCELL_SCK_PIN = 32; // orange

    const int HOME_SWITCH_SIG_PIN = 33;
}



namespace DrillPins {
    const int AUGER_L_EN_PIN = 18;
    const int AUGER_R_EN_PIN = 5;
    const int AUGER_L_PWN_PIN = 17;
    const int AUGER_R_PWN_PIN = 16;

    const int PRESS_EN_PIN = 14;
    const int PRESS_DIR_PIN = 12;
    const int PRESS_STEP_PIN = 13;

    const int UPPER_SWITCH_SIG_PIN = 23;
}

#endif