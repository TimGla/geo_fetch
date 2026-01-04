#include <Arduino.h>
#include <micro_ros_platformio.h>
#include <HX711.h>
#include <rcl/rcl.h>
#include <std_srvs/srv/set_bool.h>  // Service for triggering motors
#include <std_srvs/srv/trigger.h>   // Service for calibrating the scale
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include <std_msgs/msg/float32.h>
#include <rmw_microros/rmw_microros.h>

#define RCCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){error_loop();}}
#define RCSOFTCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){}}

// Error handle loop
void error_loop() {
  while(1) {
    delay(100);
  }
}

// Pins
const int LOADCELL_DOUT_PIN = 13;
const int LOADCELL_SCK_PIN  = 12;

// Node
rcl_node_t node;

// Calibration Service
rcl_service_t calibration_service;
std_srvs__srv__Trigger_Request callibration_req_msg;
std_srvs__srv__Trigger_Response callibration_res_msg;

// Stepper Motor Service
rcl_service_t stepper_service;
std_srvs__srv__SetBool_Request stepper_req_msg;
std_srvs__srv__SetBool_Response stepper_res_msg;

// Load Cell
HX711 scale;
float calibration_factor = 415.0;
rcl_publisher_t publisher;
std_msgs__msg__Float32 msg;

// Executor
rclc_executor_t executor;
rclc_support_t support;
rcl_allocator_t allocator;
rcl_timer_t timer;

void timer_callback(rcl_timer_t * timer, int64_t last_call_time) {
  if (timer != NULL) {
    if (scale.is_ready()) {
      float current_weight = scale.get_units(5);
      msg.data = current_weight;
      RCSOFTCHECK(rcl_publish(&publisher, &msg, NULL));
    } else {
      msg.data = -1.0;
      RCSOFTCHECK(rcl_publish(&publisher, &msg, NULL));
    }
  }
}

void stepper_callback(const void  *request, void *response) {
  std_srvs__srv__SetBool_Response *res = (std_srvs__srv__SetBool_Response *) response;
  std_srvs__srv__SetBool_Request *req = (std_srvs__srv__SetBool_Request *) request;
  if (req->data) {
    // TODO: Start stepper
    res->success = true;
    static char start_msg[] = "Stepper is running...";
    res->message.data = start_msg;
    res->message.size = strlen(start_msg);
    res->message.capacity = strlen(start_msg) + 1;
    return;
  }
  res->success = false;
  static char stop_msg[] = "Stopped stepper";
  res->message.data = stop_msg;
  res->message.size = strlen(stop_msg);
  res->message.capacity = strlen(stop_msg) + 1;
}

// Service callback
void calibration_callback(const void *request, void *response) {
  std_srvs__srv__Trigger_Response *res = (std_srvs__srv__Trigger_Response *) response;
  
  if (!scale.is_ready()) {
    res->success = false;
    static char error_msg[] = "Error while calibrating scale";
    res->message.data = error_msg;
    res->message.size = strlen(error_msg);
    res->message.capacity = strlen(error_msg) + 1;
    return;
  }
  
  scale.tare();
  res->success = true;
  static char success_msg[] = "Successfully calibrated scale";
  res->message.data = success_msg;
  res->message.size = strlen(success_msg);
  res->message.capacity = strlen(success_msg) + 1;
}

void setup() {
  // Configure serial transport
  Serial.begin(115200);
  set_microros_serial_transports(Serial);
  delay(2000);

  // Set up scale
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(calibration_factor);
  scale.tare();

  // Create init_options
  allocator = rcl_get_default_allocator();
  RCCHECK(rclc_support_init(&support, 0, NULL, &allocator));

  // Create node
  RCCHECK(rclc_node_init_default(
    &node, 
    "esp32", 
    "", 
    &support)
  );

  // Create load cell publisher
  RCCHECK(rclc_publisher_init_default(
    &publisher,
    &node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Float32),
    "weight_publisher")
  );

  // Create stepper service
  RCCHECK(rclc_service_init_default(
    &stepper_service,
    &node,
    ROSIDL_GET_SRV_TYPE_SUPPORT(std_srvs, srv, SetBool),
    "update_stepper")
  );

  // Create calibration service
  RCCHECK(rclc_service_init_default(
    &calibration_service,
    &node,
    ROSIDL_GET_SRV_TYPE_SUPPORT(std_srvs, srv, Trigger),
    "calibrate_scale")
  );

  // Create timer (1000ms/ 1Hz)
  const unsigned int timer_timeout = 1000;
  RCCHECK(rclc_timer_init_default(
    &timer,
    &support,
    RCL_MS_TO_NS(timer_timeout),
    timer_callback)
  );

  // Create executor with 2 handles
  RCCHECK(rclc_executor_init(
    &executor, 
    &support.context, 
    3, 
    &allocator)
  );
  
  // Add timer to executor
  RCCHECK(rclc_executor_add_timer(&executor, &timer));

  // Add stepper service to executor
  RCCHECK(rclc_executor_add_service(
    &executor,
    &stepper_service,
    &stepper_req_msg,
    &stepper_res_msg,
    stepper_callback)
  );
  
  // Add callibration service to executor
  RCCHECK(rclc_executor_add_service(
    &executor,
    &calibration_service,
    &callibration_req_msg,
    &callibration_res_msg,
    calibration_callback)
  );
}

void loop() {
  delay(100);
  RCSOFTCHECK(rclc_executor_spin_some(&executor, RCL_MS_TO_NS(100)));
}