#include "ros2_helper/graphic/mock/mock_publishers.hpp"

#include <stdexcept>

namespace ros2_helper
{

std::string default_mock_topic(const std::string & kind)
{
  if (kind == "image") {
    return "/camera/image_raw";
  }
  if (kind == "compressed_image" || kind == "compressedimage") {
    return "/camera/image_raw/compressed";
  }
  if (kind == "scan") {
    return "/scan";
  }
  if (kind == "joint_states" || kind == "jointstate") {
    return "/joint_states";
  }
  throw std::invalid_argument("unknown mock kind: " + kind);
}

std::shared_ptr<rclcpp::Node> create_mock_publisher(
  const std::string & kind,
  const std::string & topic_name,
  double hz)
{
  if (kind == "image") {
    return create_mock_image_publisher(topic_name, hz);
  }
  if (kind == "compressed_image" || kind == "compressedimage") {
    return create_mock_compressed_image_publisher(topic_name, hz);
  }
  if (kind == "scan") {
    return create_mock_scan_publisher(topic_name, hz);
  }
  if (kind == "joint_states" || kind == "jointstate") {
    return create_mock_joint_state_publisher(topic_name, hz);
  }
  throw std::invalid_argument("kind must be one of: image, compressed_image, scan, joint_states");
}

}  // namespace ros2_helper
