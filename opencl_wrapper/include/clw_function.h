#pragma once
#include <CL/cl.h>
#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <type_traits>
#include <vector>
#include <tuple>
#include <filesystem>
#include <algorithm>
#include "clw_context.h"
#include "clw_helper.h"
#include "clw_vector.h"

#ifndef KERNEL_DIR
#define KERNEL_DIR ""
#endif

class clw_function{
  public:
  [[deprecated]] clw_function(const clw_context& context, const std::vector<std::string>& paths, const std::string& function_name): m_function_name(function_name), m_context(context){
    std::string full_code = "";
    for (auto path : paths) {
      std::ifstream file_stream(KERNEL_DIR + path);
      if(file_stream.fail()){
        std::cerr << "Failed to open file: " << path << ".\n";
        exit(1);
      }
      std::string opencl_code((std::istreambuf_iterator<char>(file_stream)), std::istreambuf_iterator<char>());
      full_code += "\n";
      full_code += opencl_code;
    }

    cl_int error;
    const char* indirection[1]{full_code.data()};
    m_program = clCreateProgramWithSource(m_context.get_cl_context(), 1, indirection, NULL, &error);
    clw_fail_hard_on_error(error);

    error = clBuildProgram(m_program, 0, NULL, "-cl-mad-enable -cl-std=CL1.2", NULL, NULL);
    if(error != CL_SUCCESS){
      //Failed, print the error log
      std::cout << "Kernel failed to compile:\n";
      std::array<char, 4096> buffer;
      error = clGetProgramBuildInfo(m_program, m_context.get_cl_device_id(), CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer.data(), NULL);
      std::cout << "---------------------------------------\n";
      std::cout << "\033[1;33m" << buffer.data() << "\033[0m" << '\n';
      std::cout << "---------------------------------------\n";
      clw_fail_hard_on_error(error);
    }
    clw_fail_hard_on_error(error);

    cl_build_status build_status;
    error = clGetProgramBuildInfo(m_program, m_context.get_cl_device_id(), CL_PROGRAM_BUILD_STATUS, sizeof(cl_build_status), &build_status, NULL);

    m_kernel = clCreateKernel(m_program, function_name.data(), NULL);
    clw_fail_hard_on_error(error);
  }

  const std::string do_include(const std::string& path, std::vector<std::string>& already_included){
    std::filesystem::path path_to_file(path);
    std::filesystem::path directory_of_file = path_to_file.parent_path();
    std::ifstream file_stream(path_to_file);

    std::string code_string = "";
  
    //Ensure each file is only included once!
    if(std::find(already_included.begin(), already_included.end(), path_to_file.lexically_normal()) != already_included.end()){
      file_stream.close();
      return "";
    }

    if(file_stream.fail()){
      std::cerr << "Failed to open file: " << path_to_file << "\n";
      exit(1);
    }

    already_included.push_back(path_to_file.lexically_normal());
    
    
    std::string line;
    while(std::getline(file_stream, line)){
      size_t include_position = line.find("#clw_include");
      if(include_position != std::string::npos){
        size_t include_start = line.find_first_of('"', include_position);
        size_t include_end = line.find_first_of('"', include_start + 1);
        if(include_start == std::string::npos || include_end == std::string::npos){
          std::cout << "Error when parsing OpenCL program: \n"
                    << " #cl_include not of the expected format: cl_include \"...\"\n";
          exit(1);
        }
        include_start++;
        size_t include_size = include_end - include_start;
        size_t include_size_all = include_end - include_position + 1;
        
        std::string include_file_name = line.substr(include_start, include_size);

        std::filesystem::path tmp_dir = directory_of_file;
        std::string include_content = do_include(directory_of_file/include_file_name, already_included);
        line.replace(include_position, include_size_all, include_content);
        code_string += line + '\n';
      }else{
        code_string += line + '\n'; 
      }
    }
    file_stream.close();

    return code_string;
  }

  clw_function(const clw_context& context, const std::string& path, const std::string& function_name): m_function_name(function_name), m_context(context){
    std::vector<std::string> already_included;
    //std::cout << do_include(/*KERNEL_DIR +*/ path, already_included) << '\n';
		std::string opencl_code = do_include(KERNEL_DIR + path, already_included);
		
    cl_int error;
    const char* indirection[1]{opencl_code.data()};
    m_program = clCreateProgramWithSource(m_context.get_cl_context(), 1, indirection, NULL, &error);
    clw_fail_hard_on_error(error);

    error = clBuildProgram(m_program, 0, NULL, "-cl-mad-enable -cl-std=CL2.0", NULL, NULL);
    if(error != CL_SUCCESS){
      //Failed, print the error log
      std::cout << "Kernel failed to compile:\n";
      std::array<char, 4096> buffer;
      error = clGetProgramBuildInfo(m_program, m_context.get_cl_device_id(), CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer.data(), NULL);
      std::cout << "---------------------------------------\n";
      std::cout << "\033[1;33m" << buffer.data() << "\033[0m" << '\n';
      std::cout << "---------------------------------------\n";
      clw_fail_hard_on_error(error);
    }
    clw_fail_hard_on_error(error);

    cl_build_status build_status;
    error = clGetProgramBuildInfo(m_program, m_context.get_cl_device_id(), CL_PROGRAM_BUILD_STATUS, sizeof(cl_build_status), &build_status, NULL);

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
  [[deprecated("Use 'execute(...) instead'")]] void run(const Args&... arg) const{
    static_assert(GX >= LX, "Error, global size x < local_size x.");
    static_assert(GY >= LY, "Error, global size y < local size y.");
    static_assert(GZ >= LZ, "Error, global size z < local size z.");
    static_assert((GX % LX) == 0, "Error, global size x is not a multiple of local size x.");
    static_assert((GY % LY) == 0, "Error, global size y is not a multiple of local size y.");
    static_assert((GZ % LZ) == 0, "Error, global size z is not a multiple of local size z.");
    recurse_helper<0>(arg...);
    cl_int error;

    constexpr const std::array<size_t, 3> global_size{GX, GY, GZ};
    constexpr const std::array<size_t, 3> local_size{LX, LY, LZ};
    error = clEnqueueNDRangeKernel(m_context.get_cl_command_queue(), m_kernel, global_size.size(), NULL, global_size.data(), local_size.data(), 0, NULL, NULL);

    //clFinish(m_context.get_cl_command_queue());
    clw_fail_hard_on_error(error);
  }

  /// @tparam GX the global X size
  /// @tparam GY the global Y size
  /// @tparam LX the local Y size
  /// @tparam LY the local Y size
  template <size_t GX, size_t GY, size_t LX, size_t LY, typename ...Args>
  [[deprecated("Use 'execute(...) instead'")]] void run(const Args&... arg) const{
    run<GX,GY,1,LX,LY,1>(arg...);
  }


  /// @tparam GX the global X size
  /// @tparam LX the local Y size
  template <size_t GX, size_t LX, typename ...Args>
  [[deprecated("Use 'execute(...) instead'")]] void run(const Args&... arg) const{
    run<GX,1,1,LX,1,1>(arg...);
  }


  /// @param global_size a vector representing the global size, it is suggested
  //         to use initilizer_lists, example: 1D: {256}, 2D: {256,128} or 3D: {256,128,512}
  /// @param local_size a vector representing the local size
  //         to use initilizer_lists, example: 1D: {64}, 2D: {8,8} or 3D: {4,4,4}
  template<typename ...Args>
  constexpr void execute(std::array<size_t, 3> global_size, std::array<size_t, 3> local_size, const Args&... arg) const{
    //Ensure that OpenCL does not cause issues with work_sizes of 0
    constexpr auto remove_zero = [](auto& array){
      for (auto& val : array){
        if(val == 0) val = 1;
      }
    };
    remove_zero(global_size);
    remove_zero(local_size);

    if(local_size[0]*local_size[1]*local_size[2] > 256){
      std::cerr << "Warning, creating local size incompatible with AMD GPUs.\n";
      std::cerr << "  used local size: " << local_size[0]*local_size[1]*local_size[2] << " > 256\n";
    }

    assert(global_size[0] >= local_size[0]); //Error, global size x < local_size x.
    assert(global_size[1] >= local_size[1]); //Error, global size y < local_size y.
    assert(global_size[2] >= local_size[2]); //Error, global size z < local_size z.
    assert((global_size[0] % local_size[0]) == 0); //Error, global size x is not a multiple of local size x
    assert((global_size[1] % local_size[1]) == 0); //Error, global size y is not a multiple of local size y
    assert((global_size[2] % local_size[2]) == 0); //Error, global size z is not a multiple of local size z

    recurse_helper<0>(arg...);
    cl_int error{0};
    error = clEnqueueNDRangeKernel(m_context.get_cl_command_queue(), m_kernel, global_size.size(), NULL, global_size.data(), local_size.data(), 0, NULL, NULL);
    //clFinish(m_context.get_cl_command_queue());
    clw_fail_hard_on_error(error);
  }

private:
  cl_program m_program;
  cl_kernel m_kernel;
  const std::string m_function_name;
  const clw_context& m_context;
};
