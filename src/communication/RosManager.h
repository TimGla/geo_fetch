#ifndef ROS_MANAGER_H
#define ROS_MANAGER_H

#include <Arduino.h>
#include <micro_ros_platformio.h>
#include <rcl/rcl.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include <std_srvs/srv/trigger.h>
#include "ServiceWrapper.h" 
#include <vector>

class RosManager {
private:
    rcl_allocator_t allocator;
    rclc_support_t support;
    rcl_node_t node;
    rclc_executor_t executor;
    
    unsigned int max_handles;
    std::vector<ServiceWrapper*> services;

public:
    RosManager(unsigned int max_handles) : max_handles(max_handles) {}

    bool init() {

        set_microros_serial_transports(Serial);
        
        allocator = rcl_get_default_allocator();
        delay(2000);

        if (rclc_support_init(&support, 0, NULL, &allocator) != RCL_RET_OK) return false;

        if (rclc_node_init_default(&node, "esp32", "", &support) != RCL_RET_OK) return false;

        if (rclc_executor_init(&executor, &support.context, max_handles, &allocator) != RCL_RET_OK) return false;

        return true;
    }

    void registerService(const char* name, std::function<void(std_srvs__srv__Trigger_Response*)> cb) {
        auto wrapper = new ServiceWrapper(&node, name, cb);
        wrapper->addToExecutor(&executor);
        services.push_back(wrapper);
    }

    void spin() {
        rclc_executor_spin_some(&executor, RCL_MS_TO_NS(10));
    }

    rcl_node_t* getNode() { return &node; }
    rclc_executor_t* getExecutor() { return &executor; }
};

#endif