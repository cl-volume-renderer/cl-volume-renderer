#pragma once
#include <CL/opencl.h>
#include <iostream>
#include <string>

//I know it is evil :)
#define clw_fail_hard_on_error(val) h__clw_fail_hard_on_error_message(val, __FILE__,  __PRETTY_FUNCTION__, __LINE__)


/// If non CL_SUCESS is passed, we print the error code and fail hard
///@param status the return value of a OpenCL value that will be checked
static std::string h__get_warning_message(cl_int status) {
  if (status == CL_SUCCESS) return "CL_SUCCESS";
  std::string warning_message;

  switch (status) {
    case -1:
      warning_message = "CL_DEVICE_NOT_FOUND";
      break;
    case -2:
      warning_message = "CL_DEVICE_NOT_AVAILABLE";
      break;
    case -3:
      warning_message = "CL_COMPILER_NOT_AVAILABLE";
      break;
    case -4:
      warning_message = "CL_MEM_OBJECT_ALLOCATION_FAILURE";
      break;
    case -5:
      warning_message = "CL_OUT_OF_RESOURCES";
      break;
    case -6:
      warning_message = "CL_OUT_OF_HOST_MEMORY";
      break;
    case -7:
      warning_message = "CL_PROFILING_INFO_NOT_AVAILABLE";
      break;
    case -8:
      warning_message = "CL_MEM_COPY_OVERLAP";
      break;
    case -9:
      warning_message = "CL_IMAGE_FORMAT_MISMATCH";
      break;
    case -10:
      warning_message = "CL_IMAGE_FORMAT_NOT_SUPPORTED";
      break;
    case -12:
      warning_message = "CL_MAP_FAILURE";
      break;
    case -13:
      warning_message = "CL_MISALIGNED_SUB_BUFFER_OFFSET";
      break;
    case -14:
      warning_message = "CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST";
      break;
    case -15:
      warning_message = "CL_COMPILE_PROGRAM_FAILURE";
      break;
    case -16:
      warning_message = "CL_LINKER_NOT_AVAILABLE";
      break;
    case -17:
      warning_message = "CL_LINK_PROGRAM_FAILURE";
      break;
    case -18:
      warning_message = "CL_DEVICE_PARTITION_FAILED";
      break;
    case -19:
      warning_message = "CL_KERNEL_ARG_INFO_NOT_AVAILABLE";
      break;
    case -30:
      warning_message = "CL_INVALID_VALUE";
      break;
    case -31:
      warning_message = "CL_INVALID_DEVICE_TYPE";
      break;
    case -32:
      warning_message = "CL_INVALID_PLATFORM";
      break;
    case -33:
      warning_message = "CL_INVALID_DEVICE";
      break;
    case -34:
      warning_message = "CL_INVALID_CONTEXT";
      break;
    case -35:
      warning_message = "CL_INVALID_QUEUE_PROPERTIES";
      break;
    case -36:
      warning_message = "CL_INVALID_COMMAND_QUEUE";
      break;
    case -37:
      warning_message = "CL_INVALID_HOST_PTR";
      break;
    case -38:
      warning_message = "CL_INVALID_MEM_OBJECT";
      break;
    case -39:
      warning_message = "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR";
      break;
    case -40:
      warning_message = "CL_INVALID_IMAGE_SIZE";
      break;
    case -41:
      warning_message = "CL_INVALID_SAMPLER";
      break;
    case -42:
      warning_message = "CL_INVALID_BINARY";
      break;
    case -43:
      warning_message = "CL_INVALID_BUILD_OPTIONS";
      break;
    case -44:
      warning_message = "CL_INVALID_PROGRAM";
      break;
    case -45:
      warning_message = "CL_INVALID_PROGRAM_EXECUTABLE";
      break;
    case -46:
      warning_message = "CL_INVALID_KERNEL_NAME";
      break;
    case -47:
      warning_message = "CL_INVALID_KERNEL_DEFINITION";
      break;
    case -48:
      warning_message = "CL_INVALID_KERNEL";
      break;
    case -49:
      warning_message = "CL_INVALID_ARG_INDEX";
      break;
    case -50:
      warning_message = "CL_INVALID_ARG_VALUE";
      break;
    case -51:
      warning_message = "CL_INVALID_ARG_SIZE";
      break;
    case -52:
      warning_message = "CL_INVALID_KERNEL_ARGS";
      break;
    case -53:
      warning_message = "CL_INVALID_WORK_DIMENSION";
      break;
    case -54:
      warning_message = "CL_INVALID_WORK_GROUP_SIZE";
      break;
    case -55:
      warning_message = "CL_INVALID_WORK_ITEM_SIZE";
      break;
    case -56:
      warning_message = "CL_INVALID_GLOBAL_OFFSET";
      break;
    case -57:
      warning_message = "CL_INVALID_EVENT_WAIT_LIST";
      break;
    case -58:
      warning_message = "CL_INVALID_EVENT";
      break;
    case -59:
      warning_message = "CL_INVALID_OPERATION";
      break;
    case -60:
      warning_message = "CL_INVALID_GL_OBJECT";
      break;
    case -61:
      warning_message = "CL_INVALID_BUFFER_SIZE";
      break;
    case -62:
      warning_message = "CL_INVALID_MIP_LEVEL";
      break;
    case -63:
      warning_message = "CL_INVALID_GLOBAL_WORK_SIZE";
      break;
    case -64:
      warning_message = "CL_INVALID_PROPERTY";
      break;
    case -65:
      warning_message = "CL_INVALID_IMAGE_DESCRIPTOR";
      break;
    case -66:
      warning_message = "CL_INVALID_COMPILER_OPTIONS";
      break;
    case -67:
      warning_message = "CL_INVALID_LINKER_OPTIONS";
      break;
    case -68:
      warning_message = "CL_INVALID_DEVICE_PARTITION_COUNT";
      break;
    case -69:
      warning_message = "CL_INVALID_PIPE_SIZE";
      break;
    case -70:
      warning_message = "CL_INVALID_DEVICE_QUEUE";
      break;
    case -71:
      warning_message = "CL_INVALID_SPEC_ID";
      break;
    case -72:
      warning_message = "CL_MAX_SIZE_RESTRICTION_EXCEEDED";
      break;
    case -1002:
      warning_message = "CL_INVALID_D3D10_DEVICE_KHR";
      break;
    case -1003:
      warning_message = "CL_INVALID_D3D10_RESOURCE_KHR";
      break;
    case -1004:
      warning_message = "CL_D3D10_RESOURCE_ALREADY_ACQUIRED_KHR";
      break;
    case -1005:
      warning_message = "CL_D3D10_RESOURCE_NOT_ACQUIRED_KHR";
      break;
    case -1006:
      warning_message = "CL_INVALID_D3D11_DEVICE_KHR";
      break;
    case -1007:
      warning_message = "CL_INVALID_D3D11_RESOURCE_KHR";
      break;
    case -1008:
      warning_message = "CL_D3D11_RESOURCE_ALREADY_ACQUIRED_KHR";
      break;
    case -1009:
      warning_message = "CL_D3D11_RESOURCE_NOT_ACQUIRED_KHR";
      break;
    case -1010:
      warning_message = "CL_INVALID_DX9_MEDIA_ADAPTER_KHR";
      break;
    case -1011:
      warning_message = "CL_INVALID_DX9_MEDIA_SURFACE_KHR";
      break;
    case -1012:
      warning_message = "CL_DX9_MEDIA_SURFACE_ALREADY_ACQUIRED_KHR";
      break;
    case -1013:
      warning_message = "CL_DX9_MEDIA_SURFACE_NOT_ACQUIRED_KHR";
      break;
    case -1093:
      warning_message = "CL_INVALID_EGL_OBJECT_KHR";
      break;
    case -1092:
      warning_message = "CL_EGL_RESOURCE_NOT_ACQUIRED_KHR";
      break;
    case -1001:
      warning_message = "CL_PLATFORM_NOT_FOUND_KHR";
      break;
    case -1057:
      warning_message = "CL_DEVICE_PARTITION_FAILED_EXT";
      break;
    case -1058:
      warning_message = "CL_INVALID_PARTITION_COUNT_EXT";
      break;
    case -1059:
      warning_message = "CL_INVALID_PARTITION_NAME_EXT";
      break;
    case -1094:
      warning_message = "CL_INVALID_ACCELERATOR_INTEL";
      break;
    case -1095:
      warning_message = "CL_INVALID_ACCELERATOR_TYPE_INTEL";
      break;
    case -1096:
      warning_message = "CL_INVALID_ACCELERATOR_DESCRIPTOR_INTEL";
      break;
    case -1097:
      warning_message = "CL_ACCELERATOR_TYPE_NOT_SUPPORTED_INTEL";
      break;
    case -1000:
      warning_message = "CL_INVALID_GL_SHAREGROUP_REFERENCE_KHR";
      break;
    case -1098:
      warning_message = "CL_INVALID_VA_API_MEDIA_ADAPTER_INTEL";
      break;
    case -1099:
      warning_message = "CL_INVALID_VA_API_MEDIA_SURFACE_INTEL";
      break;
    case -1100:
      warning_message = "CL_VA_API_MEDIA_SURFACE_ALREADY_ACQUIRED_INTEL";
      break;
    case -1101:
      warning_message = "CL_VA_API_MEDIA_SURFACE_NOT_ACQUIRED_INTEL";
      break;
    default:
      warning_message = "CL_UNKNOWN_ERROR: ";
			warning_message.append(std::to_string(status));
      break;
  }


  return warning_message;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
static void h__clw_fail_hard_on_error_message(cl_int status,std::string file, std::string function, int line){
  if (status == CL_SUCCESS) return;
	std::cerr << "WARNING OpenCL call failed. See error message:\n";
  std::cerr << "Failed Call Info: \n";
  std::cerr << "---------------------------------------\n";
  std::cerr << "\033[1;31m";
  std::cerr << "File      : " << file << '\n';
  std::cerr << "Function  : " << function << '\n';
  std::cerr << "Line      : " << line << '\n';
  std::cerr << "Error Msg : " << h__get_warning_message(status) << '\n';
  std::cerr << "\033[0m";

  std::cerr << "---------------------------------------\n";
	std::cerr << "Exiting application... \n";

  exit(1);
}
#pragma GCC diagnostic pop
