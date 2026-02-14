#ifndef ROS_MANAGER_H
#define ROS_MANAGER_H

#include <Arduino.h>
#include <micro_ros_platformio.h>
#include <rcl/rcl.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include <std_srvs/srv/trigger.h>
#include <std_msgs/msg/float32.h>
#include <std_msgs/msg/int8.h>
#include "ServiceWrapper.h"
#include <vector>


template <typename T>
struct Publisher {
    rcl_publisher_t publisher;
    T message;
};

class RosManager {
private:
    static RosManager *instance;
    rcl_allocator_t allocator;
    rclc_support_t support;
    rcl_node_t node;
    rclc_executor_t executor;
    rcl_timer_t publishing_timer;
    
    unsigned int max_handles;
    std::vector<ServiceWrapper*> services;
    Publisher<std_msgs__msg__Int8> drillStatePublisher;
    Publisher<std_msgs__msg__Int8> containerStatePublisher;
    Publisher<std_msgs__msg__Int8> systemStatePublisher;
    Publisher<std_msgs__msg__Float32> weightPublisher;



public:
    RosManager(unsigned int max_handles) : max_handles(max_handles) {}

    bool init() {
        instance = this;

        set_microros_serial_transports(Serial);
        
        allocator = rcl_get_default_allocator();
        delay(2000);

        if (rclc_support_init(&support, 0, NULL, &allocator) != RCL_RET_OK) return false;

        if (rclc_node_init_default(&node, "esp32", "", &support) != RCL_RET_OK) return false;

        if (rclc_executor_init(&executor, &support.context, max_handles, &allocator) != RCL_RET_OK) return false;

        return true;
    }
    
    void registerService(const char *name, std::function<void(std_srvs__srv__Trigger_Response*)> cb) {
        auto wrapper = new ServiceWrapper(&node, name, cb);
        wrapper->addToExecutor(&executor);
        services.push_back(wrapper);
    }

    void addPublishers() {
        drillStatePublisher.publisher = rcl_get_zero_initialized_publisher();
        rclc_publisher_init_default(
            &drillStatePublisher.publisher, &node,
            ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int8), "drill/state");

        containerStatePublisher.publisher = rcl_get_zero_initialized_publisher();
        rclc_publisher_init_default(
            &containerStatePublisher.publisher, &node,
            ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int8), "container/state");

        systemStatePublisher.publisher = rcl_get_zero_initialized_publisher();
        rclc_publisher_init_default(
            &systemStatePublisher.publisher, &node,
            ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int8), "system/state");    

        weightPublisher.publisher = rcl_get_zero_initialized_publisher();
        rclc_publisher_init_default(
            &weightPublisher.publisher, &node,
            ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Float32), "container/weight");

        publishing_timer = rcl_get_zero_initialized_timer();
        rclc_timer_init_default(
            &publishing_timer, &support,
            RCL_MS_TO_NS(2000), timer_callback);

        rclc_executor_add_timer(&executor, &publishing_timer);
    }

    void spin() {
        rclc_executor_spin_some(&executor, RCL_MS_TO_NS(0));
    }

    static void timer_callback(rcl_timer_t *timer, int64_t last_call) {
        if (timer == NULL || instance == NULL) return;
        rcl_publish(&instance->drillStatePublisher.publisher, 
                    &instance->drillStatePublisher.message, NULL);
        
        rcl_publish(&instance->containerStatePublisher.publisher, 
                    &instance->containerStatePublisher.message, NULL);

        rcl_publish(&instance->systemStatePublisher.publisher, 
                    &instance->systemStatePublisher.message, NULL);
        
        rcl_publish(&instance->weightPublisher.publisher, 
                    &instance->weightPublisher.message, NULL);
    }

    void setDrillState(DrillState state) { 
        drillStatePublisher.message.data = static_cast<uint8_t>(state); 
    }

    void setContainerState(ContainerState state) { 
        containerStatePublisher.message.data = static_cast<uint8_t>(state);
    }

    void setSystemState(SystemState state) { 
        systemStatePublisher.message.data = static_cast<uint8_t>(state);
    }
    
    void setWeight(float weight) {
         weightPublisher.message.data = weight; 
    }

    rcl_node_t* getNode() { return &node; }
    rclc_executor_t* getExecutor() { return &executor; }
};

RosManager* RosManager::instance = nullptr;
#endif