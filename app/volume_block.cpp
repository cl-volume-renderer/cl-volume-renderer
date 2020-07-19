#include "volume_block.hpp"
#include "nrrd_loader.hpp"

volume_block::volume_block(const unsigned int voxel_count_x,
                           const unsigned int voxel_count_y,
                           const unsigned int voxel_count_z,
                           const float voxel_size_x, const float voxel_size_y,
                           const float voxel_size_z)
    : m_voxel_count_x(voxel_count_x),
      m_voxel_count_y(voxel_count_y),
      m_voxel_count_z(voxel_count_z),
      m_voxel_size_x(voxel_size_x),
      m_voxel_size_y(voxel_size_y),
      m_voxel_size_z(voxel_size_z){};

volume_block::volume_block(std::vector<short>&& input_vector,
                           const unsigned int voxel_count_x,
                           const unsigned int voxel_count_y,
                           const unsigned int voxel_count_z,
                           const float voxel_size_x, const float voxel_size_y,
                           const float voxel_size_z)
    : m_voxels(input_vector),
      m_voxel_count_x(voxel_count_x),
      m_voxel_count_y(voxel_count_y),
      m_voxel_count_z(voxel_count_z),
      m_voxel_size_x(voxel_size_x),
      m_voxel_size_y(voxel_size_y),
      m_voxel_size_z(voxel_size_z){};
