#pragma once

#include <memory>
#include <string>

#include "rclcpp/rclcpp.hpp"

namespace ros2_helper
{

std::string default_mock_topic(const std::string & kind);
std::shared_ptr<rclcpp::Node> create_mock_publisher(
  const std::string & kind,
  const std::string & topic_name,
  double hz);

std::shared_ptr<rclcpp::Node> create_mock_image_publisher(const std::string & topic_name, double hz);
std::shared_ptr<rclcpp::Node> create_mock_compressed_image_publisher(const std::string & topic_name, double hz);
std::shared_ptr<rclcpp::Node> create_mock_scan_publisher(const std::string & topic_name, double hz);
std::shared_ptr<rclcpp::Node> create_mock_joint_state_publisher(const std::string & topic_name, double hz);

}  // namespace ros2_helper
