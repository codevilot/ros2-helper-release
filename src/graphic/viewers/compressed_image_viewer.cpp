#include "ros2_helper/graphic/viewers/compressed_image_viewer.hpp"

#include <iostream>
#include <stdexcept>

#include "ros2_helper/graphic/util/terminal_image_renderer.hpp"
#include "ros2_helper/utils/terminal_utils.hpp"

namespace ros2_helper
{

CompressedImageViewer::CompressedImageViewer(
  const std::string & topic_name,
  const std::string & topic_type,
  double fps)
: Node("ros2_helper_compressed_image_viewer"), limiter_(fps)
, topic_name_(topic_name), topic_type_(topic_type)
{
  if (!command_exists("chafa")) {
    throw std::runtime_error("'chafa' is not installed. Install it with 'sudo apt install chafa'.");
  }

  subscription_ = create_subscription<sensor_msgs::msg::CompressedImage>(
    topic_name, 10, std::bind(&CompressedImageViewer::callback, this, std::placeholders::_1));

  std::cout << "[" << topic_name << "] Starting compressed image stream (type: " << topic_type << ")\n";
}

void CompressedImageViewer::callback(const sensor_msgs::msg::CompressedImage::SharedPtr msg)
{
  if (!limiter_.ready() || msg->data.empty()) {
    return;
  }

  TerminalImageStatus status;
  status.topic_name = topic_name_;
  status.topic_type = topic_type_;
  status.render_hz = ros2_helper::update_render_hz(last_render_time_);

  if (!render_terminal_image(msg->data, status)) {
    RCLCPP_ERROR(get_logger(), "Failed to start chafa.");
  }
}

}  // namespace ros2_helper
