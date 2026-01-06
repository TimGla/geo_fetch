#include <Arduino.h>
#include <micro_ros_platformio.h>
#include <rcl/rcl.h>
#include <std_srvs/srv/set_bool.h>
#include <std_srvs/srv/trigger.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include <std_msgs/msg/float32.h>
#include <std_msgs/msg/bool.h>
#include <std_msgs/msg/int32.h>
#include <rmw_microros/rmw_microros.h>
#include <components/LoadCell.h>
#include <components/ServoMotor.h>
#include <components/StepperMotor.h>

#define RCCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){error_loop();}}
#define RCSOFTCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){}}

// Globals
const unsigned int num_handles = 4;
const unsigned int timer_timeout = 1000;

// Pins (Broken: 14)
const int LOADCELL_DOUT_PIN = 13;
const int LOADCELL_SCK_PIN  = 12;

const int UPPER_END_STOP_SWITCH_PIN = 22;

const int CONTAINER_SERVO_PIN = 27;

const int PRESS_PIN_1 = 19;
const int PRESS_PIN_2 = 18;
const int PRESS_PIN_3 = 17;
const int PRESS_PIN_4 = 16;

// Components
LoadCell loadCell(
  LOADCELL_DOUT_PIN, 
  LOADCELL_SCK_PIN
);
ServoMotor container_servo(
  CONTAINER_SERVO_PIN
);
StepperMotor press(
  PRESS_PIN_1,
  PRESS_PIN_2,
  PRESS_PIN_3,
  PRESS_PIN_4,
  1.8
);


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



// Subscriptions:

// Setting velocity of container servo
rcl_subscription_t container_servo_velocity_sub;
std_msgs__msg__Int32 container_servo_velocity_msg;



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

    bool upper_switch_status = (digitalRead(UPPER_END_STOP_SWITCH_PIN) == LOW);
    upper_switch_msg.data = upper_switch_status;

    if (upper_switch_status) {
      press.stop();
    }

    RCSOFTCHECK(rcl_publish(&upper_switch_publisher, &upper_switch_msg, NULL));
  }
}

void start_press_callback(const void  *request, void *response) {
  std_srvs__srv__SetBool_Response *res = (std_srvs__srv__SetBool_Response *) response;
  std_srvs__srv__SetBool_Request *req = (std_srvs__srv__SetBool_Request *) request;
  press.setDirection(req->data);
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

void set_container_servo_velocity_callback(const void *msg_in) {
  const std_msgs__msg__Int32 *direction_and_speed = (const std_msgs__msg__Int32 *)msg_in;
  container_servo.setVelocity(direction_and_speed->data);
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
    "current_weight")
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
    "start_press")
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

  // Create Subscription for container servo
  RCCHECK(rclc_subscription_init_default(
    &container_servo_velocity_sub,
    &node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
    "container_servo_velocity")
  );

  // Create executor
  RCCHECK(rclc_executor_init(
    &executor, 
    &support.context, 
    num_handles, 
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

  RCCHECK(rclc_executor_add_subscription(
    &executor,
    &container_servo_velocity_sub,
    &container_servo_velocity_msg,
    &set_container_servo_velocity_callback,
    ON_NEW_DATA)
  );
}

void loop() {
  RCSOFTCHECK(rclc_executor_spin_some(&executor, RCL_MS_TO_NS(100)));
  if (press.isActive()) {
    press.press();
  }
}