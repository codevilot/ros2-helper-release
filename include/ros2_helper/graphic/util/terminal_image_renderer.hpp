#pragma once

#include <chrono>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace ros2_helper
{

struct TerminalImageStatus
{
  std::string topic_name;
  std::string topic_type;
  std::optional<std::string> image_size;
  double render_hz{0.0};
};

double update_render_hz(std::optional<std::chrono::steady_clock::time_point> & last_render_time);
bool render_terminal_image(const std::vector<uint8_t> & image_data, const TerminalImageStatus & status);

}  // namespace ros2_helper
