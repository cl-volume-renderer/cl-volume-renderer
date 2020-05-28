#pragma once
#include <cstddef>
#include <iostream>
#include <type_traits>
#include <vector>
#include "clw_context.h"
#include "clw_helper.h"

template <typename THost, typename TDevice>
class clw_vector {
 public:
  clw_vector(const clw_context& context, std::vector<THost>&& data)
      : m_context(context) {
    cl_int error;
    m_host_array = data;
    static_assert(
        std::is_same<typename std::remove_const<THost>::type,
                     typename std::remove_const<TDevice>::type>::value,
        "Error, except for const-qualifier the types must be the same.");

    if constexpr(std::is_const<TDevice>::value) {
        m_device_array =
            clCreateBuffer(context.get_cl_context(), CL_MEM_READ_ONLY,
                           m_host_array.size() * sizeof(TDevice), NULL, &error);
      }
    {
      m_device_array =
          clCreateBuffer(context.get_cl_context(), CL_MEM_READ_WRITE,
                         m_host_array.size() * sizeof(TDevice), NULL, &error);
    }

    clw_fail_hard_on_error(error);
  }

  ~clw_vector() {
    if (m_device_array != NULL) {
      clw_fail_hard_on_error(clReleaseMemObject(m_device_array));
    } else {
      // This should never happen.
      std::cerr << "Warning, double free of device mem. object.\n";
    }
  }
  // Delete special member functions for now,
  // can be implemented if needed
  clw_vector(const clw_vector&) = delete;
  clw_vector(clw_vector&&) = delete;
  clw_vector& operator=(const clw_vector&) = delete;
  clw_vector& operator=(clw_vector&&) = delete;

  THost& operator[](std::size_t index) { return m_host_array[index]; }
  const THost& operator[](std::size_t index) const {
    return m_host_array[index];
  }

  // Pushes host data to device
  void push() {
    clw_fail_hard_on_error(clEnqueueWriteBuffer(
        m_context.get_cl_command_queue(), m_device_array, CL_TRUE /*blocking*/,
        0, m_host_array.size() * sizeof(TDevice), m_host_array.data(), 0,
        NULL, NULL));
  }
  // Pulls device data
  void pull() {
    if constexpr(!std::is_const<THost>::value) {
        clw_fail_hard_on_error(clEnqueueReadBuffer(
            m_context.get_cl_command_queue(), m_device_array,
            CL_TRUE /*blocking*/, 0, m_host_array.size() * sizeof(TDevice),
            m_host_array.data(), 0, NULL, NULL));
      }
    else {
      std::cerr
          << "Warning, you are trying to pull into const memory: ignored.";
    }
  }

 private:
  cl_mem m_device_array;
  std::vector<THost> m_host_array;
  const clw_context& m_context;
};
