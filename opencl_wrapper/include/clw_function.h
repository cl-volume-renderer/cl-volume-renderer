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

#ifndef KERNEL_DIR
#define KERNEL_DIR ""
#endif

class clw_function{
  public:
  clw_function(const clw_context& context, const std::string& path, const std::string& function_name): m_function_name(function_name), m_context(context){
    std::ifstream file_stream(KERNEL_DIR + path);
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

  /// @tparam GX the global X size
  /// @tparam GY the global Y size
  /// @tparam GZ the global Z size
  /// @tparam LX the local Y size
  /// @tparam LY the local Y size
  /// @tparam LZ the local Z size
  template <size_t GX, size_t GY, size_t GZ, size_t LX, size_t LY, size_t LZ, typename ...Args>
  void run(const Args&... arg) const{
    static_assert(GX >= LX, "Error, global size x < local_size x.");
    static_assert(GY >= LY, "Error, global size y < local size y.");
    static_assert(GZ >= LZ, "Error, global size z < local size z.");
    static_assert((GX % LX) == 0, "Error, global size x is not a multiple of local size x.");
    static_assert((GY % LY) == 0, "Error, global size y is not a multiple of local size y.");
    static_assert((GZ % LZ) == 0, "Error, global size z is not a multiple of local size z.");
    recurse_helper<0>(arg...);
    cl_int error;
    
    constexpr const std::array<size_t, 3> global_size{GX, GY, GZ};
    constexpr const std::array<size_t, 3> local_size{GX, GY, GZ};
    error = clEnqueueNDRangeKernel(m_context.get_cl_command_queue(), m_kernel, global_size.size(), NULL, global_size.data(), local_size.data(), 0, NULL, NULL);

    //clFinish(m_context.get_cl_command_queue());
    clw_fail_hard_on_error(error);
  }

  /// @tparam GX the global X size
  /// @tparam GY the global Y size
  /// @tparam LX the local Y size
  /// @tparam LY the local Y size
  template <size_t GX, size_t GY, size_t LX, size_t LY, typename ...Args>
  void run(const Args&... arg) const{
    run<GX,GY,1,LX,LY,1>(arg...);
  }


  /// @tparam GX the global X size
  /// @tparam LX the local Y size
  template <size_t GX, size_t LX, typename ...Args>
  void run(const Args&... arg) const{
    run<GX,1,1,LX,1,1>(arg...);
  }

private:
  cl_program m_program;
  cl_kernel m_kernel;
  const std::string m_function_name;
  const clw_context& m_context;
};
