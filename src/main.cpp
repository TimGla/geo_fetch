#include <Arduino.h>
#include <micro_ros_platformio.h>
#include <HX711.h>
#include <rcl/rcl.h>
#include <std_srvs/srv/set_bool.h>  // Service for triggering motors
#include <std_srvs/srv/trigger.h>   // Service for calibrating the scale
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include <std_msgs/msg/float32.h>
#include <std_msgs/msg/bool.h>
#include <rmw_microros/rmw_microros.h>
#include <ESP32Servo.h>
#include <components/LoadCell.h>
#include <components/ServoMotor.h>

#define RCCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){error_loop();}}
#define RCSOFTCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){}}

// Globals
const unsigned int timer_timeout = 1000;

// Pins (Broken: 14)
const int LOADCELL_DOUT_PIN = 13;
const int LOADCELL_SCK_PIN  = 12;
const int UPPER_END_STOP_SWITCH_PIN = 22;
const int CONTAINER_SERVO_PIN = 27;

// Components
LoadCell loadCell(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
ServoMotor container_servo(CONTAINER_SERVO_PIN);



// Node
rcl_node_t node;



// Publishers:

// Weight Publisher
rcl_publisher_t weight_publisher;
std_msgs__msg__Float32 weight_msg;

// Upper End Stop Switch
rcl_publisher_t upper_switch_publisher;
std_msgs__msg__Bool upper_switch_msg;



// Services:

// Taring Service for scale
rcl_service_t taring_service;
std_srvs__srv__Trigger_Request taring_req_msg;
std_srvs__srv__Trigger_Response taring_res_msg;

// Press Service (0 for going down and 1 for going up)
rcl_service_t start_press_service;
std_srvs__srv__SetBool_Request start_press_req_msg;
std_srvs__srv__SetBool_Response start_press_res_msg;



// Executor
rclc_executor_t executor;
rclc_support_t support;
rcl_allocator_t allocator;
rcl_timer_t timer;


void error_loop() {
  while(1) {
    delay(100);
  }
}

void timer_callback(rcl_timer_t * timer, int64_t last_call_time) {
  if (timer != NULL) {
    if (loadCell.isReady()) {
      float current_weight = loadCell.getWeight();
      weight_msg.data = current_weight;
      RCSOFTCHECK(rcl_publish(&weight_publisher, &weight_msg, NULL));
    } else {
      weight_msg.data = -1.0;
      RCSOFTCHECK(rcl_publish(&weight_publisher, &weight_msg, NULL));
    }

    upper_switch_msg.data = (digitalRead(UPPER_END_STOP_SWITCH_PIN) == LOW);
    rcl_publish(&upper_switch_publisher, &upper_switch_msg, NULL);
  }
}

void start_press_callback(const void  *request, void *response) {
  std_srvs__srv__SetBool_Response *res = (std_srvs__srv__SetBool_Response *) response;
  std_srvs__srv__SetBool_Request *req = (std_srvs__srv__SetBool_Request *) request;
  if (req->data) {
    res->success = true;
    static char start_msg[] = "Press is going up...";
    res->message.data = start_msg;
    res->message.size = strlen(start_msg);
    res->message.capacity = strlen(start_msg) + 1;
    return;
  }
  res->success = true;
  static char stop_msg[] = "Press is going down...";
  res->message.data = stop_msg;
  res->message.size = strlen(stop_msg);
  res->message.capacity = strlen(stop_msg) + 1;
}

void tare_load_cell_callback(const void *request, void *response) {
  std_srvs__srv__Trigger_Response *res = (std_srvs__srv__Trigger_Response *) response;
  
  if (!loadCell.isReady()) {
    res->success = false;
    static char error_msg[] = "Error while calibrating scale";
    res->message.data = error_msg;
    res->message.size = strlen(error_msg);
    res->message.capacity = strlen(error_msg) + 1;
    return;
  }
  
  loadCell.tare();
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

  // Set up components
  loadCell.init();
  container_servo.init();

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
    &weight_publisher,
    &node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Float32),
    "weight_publisher")
  );

  // Create upper stop switch publisher
  RCCHECK(rclc_publisher_init_default(
    &upper_switch_publisher, &node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Bool),
    "upper_switch_state")
  );



  // Create press service
  RCCHECK(rclc_service_init_default(
    &start_press_service,
    &node,
    ROSIDL_GET_SRV_TYPE_SUPPORT(std_srvs, srv, SetBool),
    "set_press_direction")
  );

  // Create taring service
  RCCHECK(rclc_service_init_default(
    &taring_service,
    &node,
    ROSIDL_GET_SRV_TYPE_SUPPORT(std_srvs, srv, Trigger),
    "tare_scale")
  );

  // Create timer (1000ms/ 1Hz)
  RCCHECK(rclc_timer_init_default(
    &timer,
    &support,
    RCL_MS_TO_NS(timer_timeout),
    timer_callback)
  );

  // Create executor
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
    &start_press_service,
    &start_press_req_msg,
    &start_press_res_msg,
    start_press_callback)
  );
  
  // Add callibration service to executor
  RCCHECK(rclc_executor_add_service(
    &executor,
    &taring_service,
    &taring_req_msg,
    &taring_res_msg,
    tare_load_cell_callback)
  );
}

void loop() {
  delay(100);
  RCSOFTCHECK(rclc_executor_spin_some(&executor, RCL_MS_TO_NS(100)));
}