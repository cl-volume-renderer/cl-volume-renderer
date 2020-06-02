#pragma once
#include <vector>
#include <string>

//A block of voxels with a fixed width, height and length
struct volume_block {
  std::vector<short> m_voxels;
  const unsigned int m_voxel_count_x;
  const unsigned int m_voxel_count_y;
  const unsigned int m_voxel_count_z;
  const float m_voxel_size_x;
  const float m_voxel_size_y;
  const float m_voxel_size_z;

  ///Create an "empty" volume block.
  ///@param voxel_count_x the number of voxels on the x axis
  ///@param voxel_count_y the number of voxels on the y axis
  ///@param voxel_count_z the number of voxels on the z axis
  ///@param voxel_size_x the width of a voxel along the x axis
  ///@param voxel_size_y the width of a voxel along the y axis
  ///@param voxel_size_z the width of a voxel along the z axis
  volume_block(const unsigned int voxel_count_x, const unsigned int voxel_count_y, const unsigned int voxel_count_z,
               const float voxel_size_x,         const float voxel_size_y,         const float voxel_size_z);

  ///Create a volume block using r-value reference to data vector
  ///@param voxel_count_x the number of voxels on the x axis
  ///@param voxel_count_y the number of voxels on the y axis
  ///@param voxel_count_z the number of voxels on the z axis
  ///@param voxel_size_x the width of a voxel along the x axis
  ///@param voxel_size_y the width of a voxel along the y axis
  ///@param voxel_size_z the width of a voxel along the z axis
  volume_block(std::vector<short>&& input_vector, const unsigned int voxel_count_x, const unsigned int voxel_count_y, const unsigned int voxel_count_z,
               const float voxel_size_x,         const float voxel_size_y,         const float voxel_size_z);
};
