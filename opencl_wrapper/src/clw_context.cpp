#include "clw_context.h"
#include <vector>
#include "clw_helper.h"

///Helper function to return all platforms
const std::vector<cl_platform_id> get_platforms(){
  cl_uint platform_id_count = 0;
  clGetPlatformIDs(0, NULL, &platform_id_count);
  clw_fail_hard_on_error(clGetPlatformIDs(0, NULL, &platform_id_count));

  if(platform_id_count == 0){
    std::cerr << "Error, no OpenCL platform detected.\n";
    exit(1);
  }
  
  std::vector<cl_platform_id> platform_ids(platform_id_count);
  clw_fail_hard_on_error(clGetPlatformIDs(platform_id_count, platform_ids.data(), NULL));
  return platform_ids;
}

///Helper function to return all devices for a specific platform
const std::vector<cl_device_id> get_devices(const cl_platform_id platform){
  cl_uint device_id_count = 0;
  clGetPlatformIDs(0, NULL, &device_id_count);
  clw_fail_hard_on_error(clGetPlatformIDs(0, NULL, &device_id_count));

  if(device_id_count == 0){
    std::cerr << "Error, no OpenCL device detected.\n";
    exit(1);
  }
  
  std::vector<cl_device_id> device_ids(device_id_count);
  clw_fail_hard_on_error(clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, device_id_count, device_ids.data(), NULL));
  return device_ids;
}

clw_context::clw_context(){
  cl_int error; 
  auto platforms = get_platforms();
  auto devices = get_devices(platforms[0]);
  const auto device_to_use = 0;

  m_context = clCreateContext(0,1,&devices[device_to_use], NULL, NULL, &error);
  clw_fail_hard_on_error(error);
  m_command_queue = clCreateCommandQueue(m_context, devices[device_to_use], CL_QUEUE_PROFILING_ENABLE, &error);
}

clw_context::~clw_context(){
  clw_fail_hard_on_error(clReleaseCommandQueue(m_command_queue));
  clw_fail_hard_on_error(clReleaseContext(m_context));
}
const cl_context clw_context::get_cl_context() const{
  return m_context;
}

const cl_command_queue clw_context::get_cl_command_queue() const{
  return m_command_queue;
}
