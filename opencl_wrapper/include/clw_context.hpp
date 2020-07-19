#pragma once
#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#include <CL/opencl.h>
///Setup the GL context on creation, and unint. the context on EOL
class clw_context{
  public:
  ///Setup the OpenCL context or fail hard
  clw_context();
  ///Setup the OpenCL context or fail hard
  ///Share group with OpenGL context
  ///@param gl_context a pointer to the GL context
  clw_context(void * gl_context);
  ///Free CL context and queue
  ~clw_context();
  clw_context(const clw_context&) = delete;
  clw_context(clw_context&&) = delete;
  clw_context& operator=(const clw_context&) = delete;
  clw_context& operator=(clw_context&&) = delete;

  const cl_context get_cl_context() const;
  const cl_command_queue get_cl_command_queue() const;
  const cl_device_id get_cl_device_id() const;

  private:
  cl_context m_context;
  cl_command_queue m_command_queue;
  cl_device_id m_device_id;
};
