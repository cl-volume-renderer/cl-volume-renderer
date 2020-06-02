#pragma once
#include <cstddef>
#include <iostream>
#include <type_traits>
#include <vector>
#include <array>
#include "clw_context.h"
#include "clw_helper.h"

template <typename TDevice, size_t ChannelSize=1>
class clw_image {
  using TInternal = typename std::remove_const<TDevice>::type;
 public:
  constexpr clw_image(const clw_context& context, std::vector<TInternal>&& data, std::array<size_t, 3> dimensions)
      : m_context(context) {
    cl_int error{0};
    m_host_array = data;

		//Ensure that OpenCL does not cause issues with work_sizes of 0
		constexpr auto remove_zero = [](auto& array){
			for (auto& val : array){
				if(val == 0) val = 1;
			} 
		};
		remove_zero(dimensions);
		m_dimensions = dimensions;
		

    const auto width = dimensions[0];
    const auto height = dimensions[1];
    const auto depth = dimensions[2];

    const auto eval_image_type = [&](){
      if(!(width > 1 && height >= 1 && depth >= 1)){
        std::cerr << "Error, image size is not valid \n";
        exit(1);
      }
      if(width > 1 && height == 1 && depth == 1){
        return CL_MEM_OBJECT_IMAGE1D;
      }else if    (width > 1 && height > 1 && depth == 1){
        return CL_MEM_OBJECT_IMAGE2D; 
      }else if    (width > 1 && height > 1 && depth > 1){
        return CL_MEM_OBJECT_IMAGE3D; 
      }else{
        std::cerr << "Error, image size is not valid \n";
        exit(1);
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
      cl_image_format format{0};
      
      if constexpr(ChannelSize == 1){
        format.image_channel_order = CL_R; 
      }else if(ChannelSize == 2){
        format.image_channel_order = CL_RG; 
      }else if(ChannelSize == 4){
        format.image_channel_order = CL_RGBA; 
      }else{
        //We have no constexpr fail :(
        static_assert(ChannelSize < 5 && ChannelSize != 3 && ChannelSize > 0, "Invalid channel size.");
      }

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
            format.image_channel_data_type = CL_SIGNED_INT32;
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

    const auto eval_image_desc = [&](const auto image_type){
      cl_image_desc desc{0};
      desc.image_type = image_type;
      desc.image_width = width;
      desc.image_height = height;
      desc.image_depth = depth;
      desc.image_array_size = 1;
      desc.image_row_pitch = 0; //TODO correct?
      desc.image_slice_pitch = 0; //TODO correct?
      desc.num_mip_levels = 0;
      desc.num_samples = 0;
      desc.buffer = NULL; //TODO correct?
      return desc;
    };

    const auto image_type =              eval_image_type();
    const auto access_right =            eval_access_right();
    const cl_image_format image_format = eval_image_format();
    const cl_image_desc   image_desc =   eval_image_desc(image_type);
   
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
    std::array<size_t, 3> origin{0,0,0};
    std::array<size_t, 3> region{m_dimensions[0],m_dimensions[1],m_dimensions[2]};
    clw_fail_hard_on_error(clEnqueueWriteImage(
        m_context.get_cl_command_queue(), m_device_array, CL_TRUE /*blocking*/,
        origin.data(),region.data(),0,0,m_host_array.data(),0,NULL,NULL));
  }
  /// Pulls device data
  void pull() {
    std::array<size_t, 3> origin{0,0,0};
    std::array<size_t, 3> region{m_dimensions[0],m_dimensions[1],m_dimensions[2]};
    clw_fail_hard_on_error(clEnqueueReadImage(
        m_context.get_cl_command_queue(), m_device_array, CL_TRUE /*blocking*/,
        origin.data(),region.data(),0,0,m_host_array.data(),0,NULL,NULL));
  }

  /// Returns a reference to the internal opencl object
  const cl_mem& get_device_reference() const{
    return m_device_array;
  }

  /// Returns the size of the vector
  size_t size() const{
    return m_host_array.size();
  }

  /// Returns the dimensions of the image
  const std::array<size_t,3>& get_dimensions(){
    return m_dimensions;
  }

 private:
  cl_mem m_device_array;
  std::vector<TInternal> m_host_array;
  const clw_context& m_context;
  std::array<size_t, 3> m_dimensions;
};
