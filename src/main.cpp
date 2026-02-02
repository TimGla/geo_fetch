#include <Arduino.h>
#include <Config.h>
#include "components/Auger.h"
#include "components/Press.h"
#include "components/EndSwitch.h"
#include "components/ContainerSpinner.h"
#include "components/LoadCell.h"
#include "system/container_system/ContainerManager.h"
#include "system/drill_system/DrillManager.h"
#include "communication/RosManager.h"


// Drilling system hardware wrappers
Auger *auger;
Press *press;
EndSwitch *upperSwitch;

// Container system hardware wrappers
ContainerSpinner *spinner;
LoadCell *loadCell;
EndSwitch *homeSwitch;

// Subystem managers
DrillManager *drillSystem;
ContainerManager *containerSystem;

// Communiaction
RosManager *ros;


void initializeMicroRos() {
  ros = new RosManager(20);
  if (ros->init()) {
    ros->registerService("/drill/home", [](std_srvs__srv__Trigger_Response* res) {
      drillSystem->home();
      static char res_msg[] = "Homing...";
      res->message.data = res_msg;
      res->message.size = strlen(res_msg);
      res->message.capacity = strlen(res_msg) + 1;
      res->success = true;
    });
    
    ros->registerService("/drill/drill", [](std_srvs__srv__Trigger_Response* res) {  
      drillSystem->drill();
      static char res_msg[] = "Drilling...";
      res->message.data = res_msg;
      res->message.size = strlen(res_msg);
      res->message.capacity = strlen(res_msg) + 1;
      res->success = true;
    });
    
    ros->registerService("/drill/retract", [](std_srvs__srv__Trigger_Response* res) {  
      drillSystem->retract();
      static char res_msg[] = "Retracting...";
      res->message.data = res_msg;
      res->message.size = strlen(res_msg);
      res->message.capacity = strlen(res_msg) + 1;
      res->success = true;
    });

    
    ros->registerService("/drill/retrieve", [](std_srvs__srv__Trigger_Response* res) {  
      drillSystem->retrieve();
      static char res_msg[] = "Retrieving...";
      res->message.data = res_msg;
      res->message.size = strlen(res_msg);
      res->message.capacity = strlen(res_msg) + 1;
      res->success = true;
    });

    ros->registerService("/drill/stopRetrieving", [](std_srvs__srv__Trigger_Response* res) {  
      drillSystem->stopRetrieving();
      static char res_msg[] = "Stopped retrieve";
      res->message.data = res_msg;
      res->message.size = strlen(res_msg);
      res->message.capacity = strlen(res_msg) + 1;
      res->success = true;
    });
    
    ros->registerService("/drill/clean", [](std_srvs__srv__Trigger_Response* res) {  
      drillSystem->clean();
      static char res_msg[] = "Clearing...";
      res->message.data = res_msg;
      res->message.size = strlen(res_msg);
      res->message.capacity = strlen(res_msg) + 1;
      res->success = true;
    });

    ros->registerService("/drill/stopCleaning", [](std_srvs__srv__Trigger_Response* res) {  
      drillSystem->stopCleaning();
      static char res_msg[] = "Stopped cleaning";
      res->message.data = res_msg;
      res->message.size = strlen(res_msg);
      res->message.capacity = strlen(res_msg) + 1;
      res->success = true;
    });

    ros->registerService("/container/close", [](std_srvs__srv__Trigger_Response* res) {  
      containerSystem->close();
      static char res_msg[] = "Closing...";
      res->message.data = res_msg;
      res->message.size = strlen(res_msg);
      res->message.capacity = strlen(res_msg) + 1;
      res->success = true;
    });

    ros->registerService("/container/open", [](std_srvs__srv__Trigger_Response* res) {  
      containerSystem->open();
      static char res_msg[] = "Opening...";
      res->message.data = res_msg;
      res->message.size = strlen(res_msg);
      res->message.capacity = strlen(res_msg) + 1;
      res->success = true;
    });

    ros->registerService("/container/nextSample", [](std_srvs__srv__Trigger_Response* res) {  
      containerSystem->nextSample();
      static char res_msg[] = "Rotating...";
      res->message.data = res_msg;
      res->message.size = strlen(res_msg);
      res->message.capacity = strlen(res_msg) + 1;
      res->success = true;
    });

    ros->addPublishers();
  }
}

void initializeDrillSystem() {
  auger = new Auger(
    DrillPins::AUGER_L_EN_PIN,
    DrillPins::AUGER_R_EN_PIN,
    DrillPins::AUGER_L_PWN_PIN,
    DrillPins::AUGER_R_PWN_PIN
  );
  
  press = new Press(
    DrillPins::PRESS_EN_PIN,
    DrillPins::PRESS_STEP_PIN,
    DrillPins::PRESS_DIR_PIN,
    DrillSettings::PRESS_DEG_PER_STEP,
    DrillSettings::PRESS_MAX_SPEED
  );
  
  upperSwitch = new EndSwitch(
    DrillPins::UPPER_SWITCH_SIG_PIN
  );

  drillSystem = new DrillManager(
    auger,
    press,
    upperSwitch
  );

  auger->init();
  press->init();
  upperSwitch->init();

  drillSystem->initState();
}

void initializeContainerSystem() {
  spinner = new ContainerSpinner(
    ContainerPins::SPINNER_EN_PIN,
    ContainerPins::SPINNER_STEP_PIN,
    ContainerPins::SPINNER_DIR_PIN,
    ContainerSettings::SPINNER_DEG_PER_STEP,
    ContainerSettings::SPINNER_MAX_SPEED
  );

  loadCell = new LoadCell(
    ContainerPins::LOADCELL_DOUT_PIN,
    ContainerPins::LOADCELL_SCK_PIN,
    ContainerSettings::LOADCELL_CALIBRATION_FACTOR
  );

  homeSwitch = new EndSwitch(
    ContainerPins::HOME_SWITCH_SIG_PIN
  );

  containerSystem = new ContainerManager(
    spinner,
    homeSwitch,
    loadCell,
    ContainerSettings::NUM_COMPARTMENTS
  );

  spinner->init();
  //loadCell->init();
  homeSwitch->init();

  containerSystem->initiState();
}


void setup() {
  Serial.begin(115200);
  initializeDrillSystem();
  initializeContainerSystem();
  initializeMicroRos();
}


void drillTestLoop() {
  drillSystem->update();
  DrillState state = drillSystem->getState();
  if (state == DrillState::UNKNOWN) {
    drillSystem->home();
  } else if (state == DrillState::READY) {
    drillSystem->drill();
  }
}

void containerTestLoop() {
  containerSystem->update();
  ContainerState state = containerSystem->getState();
  if (state == ContainerState::UNKNOWN) {
    containerSystem->close();
  } else if (state == ContainerState::CLOSED) {
    delay(5000);
    containerSystem->open();
  } else if (state == ContainerState::READY) {
    delay(2000);
    containerSystem->nextSample();
  } else if (state == ContainerState::CONTAINER_FULL) {
    delay(10000);
    containerSystem->close();
  }
}

void loop() {
  if (ros != NULL) ros->spin();
  if (drillSystem != NULL) {
    drillSystem->update();
    ros->setDrillState(drillSystem->getState());
  }
  if (containerSystem != NULL) {
    containerSystem->update();
    ros->setContainerState(containerSystem->getState());
    ros->setWeight(0); // containerSystem->getWeightOfCurrentSample()
  }
}