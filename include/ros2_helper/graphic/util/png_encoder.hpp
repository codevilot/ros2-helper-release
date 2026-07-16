#pragma once

#include <cstdint>
#include <vector>

namespace ros2_helper
{

std::vector<uint8_t> encode_png_rgb(int width, int height, const std::vector<uint8_t> & rgb);

}  // namespace ros2_helper
