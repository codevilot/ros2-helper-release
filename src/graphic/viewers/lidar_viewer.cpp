#include "ros2_helper/graphic/viewers/lidar_viewer.hpp"

#include <cmath>
#include <iostream>
#include <sstream>
#include <vector>

namespace ros2_helper
{

LidarViewer::LidarViewer(const std::string & topic_name, const std::string & topic_type, double fps)
: Node("ros2_helper_lidar_viewer"), limiter_(fps)
{
  subscription_ = create_subscription<sensor_msgs::msg::LaserScan>(
    topic_name, 10, std::bind(&LidarViewer::callback, this, std::placeholders::_1));

  std::cout << "[" << topic_name << "] Starting LiDAR visualization (type: " << topic_type << ")\n";
}

void LidarViewer::callback(const sensor_msgs::msg::LaserScan::SharedPtr msg)
{
  if (!limiter_.ready()) {
    return;
  }

  constexpr int rows = 40;
  constexpr int cols = 80;
  constexpr double max_distance = 5.0;
  std::vector<std::string> grid(rows, std::string(cols, ' '));
  const int center_r = rows / 2;
  const int center_c = cols / 2;
  grid[center_r][center_c] = 'O';

  for (size_t i = 0; i < msg->ranges.size(); ++i) {
    const float range = msg->ranges[i];
    if (!std::isfinite(range) || range < msg->range_min || range > msg->range_max) {
      continue;
    }

    const double angle = msg->angle_min + static_cast<double>(i) * msg->angle_increment;
    const double x = range * std::cos(angle);
    const double y = range * std::sin(angle);
    const int row = center_r - static_cast<int>((x / max_distance) * center_r);
    const int col = center_c - static_cast<int>((y / max_distance) * center_c);
    if (row >= 0 && row < rows && col >= 0 && col < cols && grid[row][col] == ' ') {
      grid[row][col] = '*';
    }
  }

  std::ostringstream output;
  output << "\033[H\033[J";
  output << "=== Terminal LiDAR Viewer (range limit: " << max_distance << "m) ===\n";
  for (const auto & row : grid) {
    output << row << "\n";
  }
  output << "Press Ctrl+C to exit.";
  std::cout << output.str() << std::flush;
}

}  // namespace ros2_helper
