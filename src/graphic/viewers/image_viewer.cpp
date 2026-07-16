#include "ros2_helper/graphic/viewers/image_viewer.hpp"

#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "ros2_helper/graphic/util/png_encoder.hpp"
#include "ros2_helper/graphic/util/terminal_image_renderer.hpp"
#include "ros2_helper/utils/terminal_utils.hpp"

namespace ros2_helper
{

ImageViewer::ImageViewer(const std::string & topic_name, const std::string & topic_type, double fps)
: Node("ros2_helper_image_viewer"), limiter_(fps), topic_name_(topic_name), topic_type_(topic_type)
{
  if (!command_exists("chafa")) {
    throw std::runtime_error("'chafa' is not installed. Install it with 'sudo apt install chafa'.");
  }

  subscription_ = create_subscription<sensor_msgs::msg::Image>(
    topic_name, 10, std::bind(&ImageViewer::callback, this, std::placeholders::_1));

  std::cout << "[" << topic_name << "] Starting image stream (type: " << topic_type << ")\n";
}

void ImageViewer::callback(const sensor_msgs::msg::Image::SharedPtr msg)
{
  if (!limiter_.ready() || msg->data.empty()) {
    return;
  }

  const auto rgb = to_rgb(*msg);
  if (rgb.empty()) {
    RCLCPP_ERROR(get_logger(), "Unsupported image encoding: %s", msg->encoding.c_str());
    return;
  }

  const auto png = encode_png_rgb(static_cast<int>(msg->width), static_cast<int>(msg->height), rgb);
  TerminalImageStatus status;
  status.topic_name = topic_name_;
  status.topic_type = topic_type_;
  status.image_size = std::to_string(msg->width) + "x" + std::to_string(msg->height);
  status.render_hz = ros2_helper::update_render_hz(last_render_time_);

  if (!render_terminal_image(png, status)) {
    RCLCPP_ERROR(get_logger(), "Failed to start chafa.");
  }
}

std::vector<uint8_t> ImageViewer::to_rgb(const sensor_msgs::msg::Image & msg) const
{
  size_t channels = 0;
  if (msg.encoding == "rgb8" || msg.encoding == "bgr8") {
    channels = 3;
  } else if (msg.encoding == "rgba8" || msg.encoding == "bgra8") {
    channels = 4;
  } else if (msg.encoding == "mono8" || msg.encoding == "8UC1") {
    channels = 1;
  } else {
    return {};
  }

  std::vector<uint8_t> rgb(static_cast<size_t>(msg.width) * msg.height * 3);
  for (size_t y = 0; y < msg.height; ++y) {
    for (size_t x = 0; x < msg.width; ++x) {
      const size_t src = y * msg.step + x * channels;
      const size_t dst = (y * msg.width + x) * 3;
      if (src + channels > msg.data.size()) {
        return {};
      }

      if (msg.encoding == "rgb8" || msg.encoding == "rgba8") {
        rgb[dst] = msg.data[src];
        rgb[dst + 1] = msg.data[src + 1];
        rgb[dst + 2] = msg.data[src + 2];
      } else if (msg.encoding == "bgr8" || msg.encoding == "bgra8") {
        rgb[dst] = msg.data[src + 2];
        rgb[dst + 1] = msg.data[src + 1];
        rgb[dst + 2] = msg.data[src];
      } else {
        rgb[dst] = msg.data[src];
        rgb[dst + 1] = msg.data[src];
        rgb[dst + 2] = msg.data[src];
      }
    }
  }
  return rgb;
}

}  // namespace ros2_helper
