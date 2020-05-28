#pragma once
#include <cstddef>
#include <iostream>
#include <fstream>
#include <type_traits>
#include <vector>
#include <tuple>
#include "clw_context.h"
#include "clw_helper.h"

class clw_function{
  clw_function(const clw_context& context, const std::string& path, const std::string& function_name): m_function_name(function_name), m_context(context){
    std::ifstream file_stream(path);
    if(file_stream.fail()){
      std::cerr << "Failed to open file: " << path << ".\n";
      exit(1);
    }
    std::string opencl_code((std::istreambuf_iterator<char>(file_stream)), std::istreambuf_iterator<char>());

    cl_int error;
    const char* indirection[1]{opencl_code.data()};
    m_program = clCreateProgramWithSource(m_context.get_cl_context(), 1, indirection, NULL, &error);
    clw_fail_hard_on_error(error);

    error = clBuildProgram(m_program, 0, NULL, "-cl-mad-enable", NULL, &error);
    clw_fail_hard_on_error(error);

    m_kernel = clCreateKernel(m_program, function_name.data(), NULL);
    clw_fail_hard_on_error(error);
  }

  ~clw_function(){
    if (m_kernel != NULL && m_program != NULL) {                                  
        clw_fail_hard_on_error(clReleaseKernel(m_kernel));  
        clw_fail_hard_on_error(clReleaseProgram(m_program));  
    } else {                                                       
         //This should never happen.                                 
      std::cerr << "Warning, double free of device mem. object.\n";
    }                                                              
  }

  clw_function(const clw_function&) = delete;            
  clw_function(clw_function&&) = delete;                 
  clw_function& operator=(const clw_function&) = delete; 
  clw_function& operator=(clw_function&&) = delete;      



  template <size_t GlobalSize, size_t LocalSize, typename ...Args>
  void run(Args... arg) const{
    std::tuple<Args...> arguments = std::tuple<Args...>(arg...);
    constexpr int args_length = sizeof...(arg);
    cl_int error;
    for(size_t i = 0; i < args_length; ++i){
      error = clSetKernelArg(m_kernel, i, sizeof(std::get<i>(arguments)), &std::get<i>(arguments));
      clw_fail_hard_on_error(error);
    }
    
    constexpr const size_t global_size = GlobalSize;
    constexpr const size_t local_size = LocalSize;
    error = clEnqueueNDRangeKernel(m_context.get_cl_command_queue(), m_kernel, 1, NULL, &global_size, &local_size, 0, NULL, NULL);
    clw_fail_hard_on_error(error);
  }

private:
  cl_program m_program;
  cl_kernel m_kernel;
  const std::string m_function_name;
  const clw_context& m_context;
};
