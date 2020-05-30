#pragma once
#include <CL/cl.h>
#include <cstddef>
#include <iostream>
#include <fstream>
#include <type_traits>
#include <vector>
#include <tuple>
#include "clw_context.h"
#include "clw_helper.h"
#include "clw_vector.h"

class clw_function{
  public:
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

    error = clBuildProgram(m_program, 0, NULL, "-cl-mad-enable", NULL, NULL);
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



  template <size_t Pos, typename Arg, typename ...Rest>
  void recurse_helper(Arg& a, Rest&... rest) const{
    cl_int error;
    

    if constexpr (std::is_arithmetic<typename std::remove_reference<Arg>::type>::value){
      error = clSetKernelArg(m_kernel, Pos, sizeof(a), &a);
    }else{
      error = clSetKernelArg(m_kernel, Pos, sizeof(a.get_device_reference()), &(a.get_device_reference()));
    }

    if constexpr (sizeof...(rest) > 0){
      recurse_helper<Pos + 1>(rest...); 
    }
    clw_fail_hard_on_error(error);

  }

  template <size_t GlobalSize, size_t LocalSize, typename ...Args>
  void run(const Args&... arg) const{
    recurse_helper<0>(arg...);
    cl_int error;
    
    constexpr const size_t global_size = GlobalSize;
    constexpr const size_t local_size = LocalSize;
    error = clEnqueueNDRangeKernel(m_context.get_cl_command_queue(), m_kernel, 1, NULL, &global_size, &local_size, 0, NULL, NULL);

    //clFinish(m_context.get_cl_command_queue());
    clw_fail_hard_on_error(error);
  }

private:
  cl_program m_program;
  cl_kernel m_kernel;
  const std::string m_function_name;
  const clw_context& m_context;
};
