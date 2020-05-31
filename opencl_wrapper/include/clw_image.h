#pragma once
#include <cstddef>
#include <iostream>
#include <type_traits>
#include <vector>
#include "clw_context.h"
#include "clw_helper.h"

template <typename TDevice, size_t Width, size_t Height = 0, size_t Depth = 0>
class clw_image {
  using TInternal = typename std::remove_const<TDevice>::type;
 public:
  clw_image(const clw_context& context, std::vector<TInternal>&& data)
      : m_context(context) {
    cl_int error;
    m_host_array = data;

    constexpr const auto eval_image_type = [&](){
      static_assert(Width > 0 && Height >= 0 && Depth >= 0, "Invalid image size.");
      if constexpr(Width > 0 && Height == 0 && Depth == 0){
        return CL_MEM_OBJECT_IMAGE1D;
      }else if    (Width > 0 && Height > 0 && Depth == 0){
        return CL_MEM_OBJECT_IMAGE2D; 
      }else if    (Width > 0 && Height > 0 && Depth > 0){
        return CL_MEM_OBJECT_IMAGE3D; 
      }else{
        // If we have a 3D image, the middle value can not be 0
        static_assert(Width > 0 && Height != 0 && Depth > 0, "Invalid image size.");
      }
    };
    
    constexpr const auto eval_access_right = [&](){
      if constexpr(std::is_const<TDevice>::value) {
        return CL_MEM_READ_ONLY;
      }else{
        return CL_MEM_READ_WRITE;
      }
    };

    constexpr const auto eval_image_format = [&](){
      cl_image_format format;
      format.image_channel_order = CL_R; 
      constexpr bool is_signed = std::is_signed<TDevice>::value;
      constexpr size_t type_size = sizeof(TDevice);
      constexpr bool is_integral = std::is_integral<TDevice>::value;
      static_assert(type_size <= 4, "Error, only 32 bit or smaller values are supported.");
      if constexpr(is_integral){
        if constexpr(is_signed){
          if constexpr(type_size == 1){
            format.image_channel_data_type = CL_SIGNED_INT8;
          }else if(type_size == 2){
            format.image_channel_data_type = CL_SIGNED_INT16; 
          }else{
            return CL_SIGNED_INT32;
          }
        }else{
          if constexpr(type_size == 1){
            format.image_channel_data_type = CL_UNSIGNED_INT8;
          }else if(type_size == 2){
            format.image_channel_data_type = CL_UNSIGNED_INT16; 
          }else{
            format.image_channel_data_type = CL_UNSIGNED_INT32;
          }
        }
      }else{
        static_assert(type_size == 4, "Error, only 32 bit floats are supported.");
        format.image_channel_data_type = CL_FLOAT;
      }
      return format;
    };

    constexpr const auto eval_image_desc = [&](){
      cl_image_desc desc;
      desc.image_type = eval_image_type();
      desc.image_width = Width;
      desc.image_height = Height;
      desc.image_depth = Depth;
      desc.image_row_pitch = 0; //TODO correct?
      desc.image_slice_pitch = 0; //TODO correct?
      desc.num_mip_levels = 0;
      desc.num_samples = 0;
      desc.buffer = NULL; //TODO correct?
    };

    constexpr const auto image_type =              eval_image_type();
    constexpr const auto access_right =            eval_access_right();
    constexpr const cl_image_format image_format = eval_image_format();
    constexpr const cl_image_desc   image_desc =   eval_image_desc();
   
    m_device_array = clCreateImage(context.get_cl_context(), access_right, &image_format, &image_desc, NULL, &error);
    clw_fail_hard_on_error(error);
  }

  ~clw_image() {
    if (m_device_array != NULL) {
      clw_fail_hard_on_error(clReleaseMemObject(m_device_array));
    } else {
      // This should never happen.
      std::cerr << "Warning, double free of device mem. object.\n";
    }
  }
  // Delete special member functions for now,
  // can be implemented if needed
  clw_image(const clw_image&) = delete;
  clw_image(clw_image&&) = delete;
  clw_image& operator=(const clw_image&) = delete;
  clw_image& operator=(clw_image&&) = delete;

  TInternal& operator[](std::size_t index) { 
    return m_host_array[index];
  }
  const TInternal& operator[](std::size_t index) const {
    return m_host_array[index];
  }

  /// Pushes host data to device
  void push() const{
    clw_fail_hard_on_error(clEnqueueWriteBuffer(
        m_context.get_cl_command_queue(), m_device_array, CL_TRUE /*blocking*/,
        0, m_host_array.size() * sizeof(TDevice), m_host_array.data(), 0,
        NULL, NULL));
  }
  /// Pulls device data
  void pull() {
        clw_fail_hard_on_error(clEnqueueReadBuffer(
            m_context.get_cl_command_queue(), m_device_array,
            CL_TRUE /*blocking*/, 0, m_host_array.size() * sizeof(TDevice),
            m_host_array.data(), 0, NULL, NULL));
  }

  /// Returns a reference to the internal opencl object
  const cl_mem& get_device_reference() const{
    return m_device_array;
  }

  /// Returns the size of the vector
  size_t size() const{
    return m_host_array.size();
  }

 private:
  cl_mem m_device_array;
  std::vector<TInternal> m_host_array;
  const clw_context& m_context;
};
