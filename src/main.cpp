#include <Arduino.h>
#include <Config.h>
#include "components/Auger.h"
#include "components/Press.h"
#include "components/EndSwitch.h"
#include "components/ContainerSpinner.h"
#include "components/LoadCell.h"
#include "system/container_system/ContainerManager.h"
#include "system/drill_system/DrillManager.h"
#include "system/GeoFetchSystem.h"
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

// Automation System
GeoFetchSystem *geoFetchsystem; 

// Communiaction
RosManager *ros;


void initializeMicroRos() {
  ros = new RosManager(20);
  if (ros->init()) {
    // Drill Services

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

    // Container Services

    ros->registerService("/container/close", [](std_srvs__srv__Trigger_Response* res) {  
      containerSystem->close();
      static char res_msg[] = "Closing...";
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

    ros->registerService("/container/reset", [](std_srvs__srv__Trigger_Response* res) {  
      containerSystem->reset();
      static char res_msg[] = "Reseting...";
      res->message.data = res_msg;
      res->message.size = strlen(res_msg);
      res->message.capacity = strlen(res_msg) + 1;
      res->success = true;
    });

    ros->registerService("/container/tare", [](std_srvs__srv__Trigger_Response* res) {  
      containerSystem->tareLoadCell();
      static char res_msg[] = "Tared loadcell";
      res->message.data = res_msg;
      res->message.size = strlen(res_msg);
      res->message.capacity = strlen(res_msg) + 1;
      res->success = true;
    });

    // System Services

    ros->registerService("/system/prepare", [](std_srvs__srv__Trigger_Response* res) {  
      geoFetchsystem->prepareSystem();
      static char res_msg[] = "Preparing...";
      res->message.data = res_msg;
      res->message.size = strlen(res_msg);
      res->message.capacity = strlen(res_msg) + 1;
      res->success = true;
    });

    ros->registerService("/system/drill", [](std_srvs__srv__Trigger_Response* res) {  
      geoFetchsystem->drill();
      static char res_msg[] = "Drilling...";
      res->message.data = res_msg;
      res->message.size = strlen(res_msg);
      res->message.capacity = strlen(res_msg) + 1;
      res->success = true;
    });

    ros->registerService("/system/collect", [](std_srvs__srv__Trigger_Response* res) {  
      geoFetchsystem->collectMaterial();
      static char res_msg[] = "Collecting...";
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
    DrillSettings::PRESS_MAX_DRILLING_SPEED,
    DrillSettings::PRESS_MAX_RETRACTING_SPEED
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
  loadCell->init();
  homeSwitch->init();

  containerSystem->initiState();
}

void initializeGeoFetchSystem() {
  geoFetchsystem = new GeoFetchSystem(
    containerSystem,
    drillSystem
  );
}


void setup() {
  Serial.begin(115200);
  initializeDrillSystem();
  initializeContainerSystem();
  initializeGeoFetchSystem();
  initializeMicroRos();
}

void loop() {
  if (ros != NULL) ros->spin();

  if (drillSystem != NULL && containerSystem != NULL && system != NULL) {
    geoFetchsystem->update();
    drillSystem->update();
    containerSystem->update();
    ros->setDrillState(drillSystem->getState());
    ros->setSystemState(geoFetchsystem->getSystemState());
    ros->setContainerState(containerSystem->getState());
    // TODO: Reading weight is blocking. Try using additonal thread
    if (containerSystem->getState() == ContainerState::READY && drillSystem->getState() == DrillState::RETRIEVING) {
      ros->setWeight(containerSystem->getWeightOfCurrentSample());
    }
  }
}