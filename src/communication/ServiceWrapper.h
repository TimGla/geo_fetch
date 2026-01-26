#ifndef SERVICE_WRAPPER_H
#define SERVICE_WRAPPER_H

#include <rcl/rcl.h>
#include <rclc/rclc.h>
#include <std_srvs/srv/trigger.h>
#include <rclc/executor.h>
#include <functional>

class ServiceWrapper {
private:
    rcl_service_t service;
    std_srvs__srv__Trigger_Request request;
    std_srvs__srv__Trigger_Response response;
    
    std::function<void(std_srvs__srv__Trigger_Response*)> callback;

public:
    ServiceWrapper(
        rcl_node_t* node, 
        const char* service_name, 
        std::function<void(std_srvs__srv__Trigger_Response*)> cb
    ) : callback(cb) {
        
        rclc_service_init_default(
            &service, node,
            ROSIDL_GET_SRV_TYPE_SUPPORT(std_srvs, srv, Trigger),
            service_name
        );
    }

    static void static_callback(const void* req, void* res, void* context) {
        ServiceWrapper* self = (ServiceWrapper*)context;
        std_srvs__srv__Trigger_Response* res_msg = (std_srvs__srv__Trigger_Response*)res;
    
        if (self->callback) {
            self->callback(res_msg);
        }
    }

    void addToExecutor(rclc_executor_t* executor) {
        rclc_executor_add_service_with_context(
            executor, &service, &request, &response,
            &ServiceWrapper::static_callback, this
        );
    }
};

#endif