#pragma once

#include <optional>
#include <string>

#include "rclcpp/rclcpp.hpp"

namespace ros2_helper
{

std::optional<std::string> find_topic_type(
  const rclcpp::Node::SharedPtr & node,
  const std::string & topic_name);

}  // namespace ros2_helper
