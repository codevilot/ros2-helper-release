#include "ros2_helper/graphic/mock/mock_publishers.hpp"

#include <algorithm>
#include <cstdint>
#include <string>
#include <vector>

#include "ros2_helper/graphic/util/png_encoder.hpp"
#include "sensor_msgs/msg/compressed_image.hpp"
#include "sensor_msgs/msg/image.hpp"

namespace ros2_helper
{
namespace
{

std::vector<uint8_t> make_rgb_pattern(int width, int height, int frame)
{
  std::vector<uint8_t> pixels(width * height * 3);
  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      const size_t offset = static_cast<size_t>(y * width + x) * 3;
      const bool stripe = ((x + frame) / 12) % 2 == 0;
      pixels[offset] = static_cast<uint8_t>((x * 255) / std::max(1, width - 1));
      pixels[offset + 1] = static_cast<uint8_t>((y * 255) / std::max(1, height - 1));
      pixels[offset + 2] = stripe ? 255 : 60;
    }
  }
  return pixels;
}

class MockImagePublisher : public rclcpp::Node
{
public:
  MockImagePublisher(const std::string & topic_name, double hz)
  : Node("ros2_helper_mock_image_publisher")
  {
    publisher_ = create_publisher<sensor_msgs::msg::Image>(topic_name, 10);
    timer_ = create_wall_timer(
      std::chrono::duration<double>(1.0 / hz),
      std::bind(&MockImagePublisher::publish_image, this));
    RCLCPP_INFO(get_logger(), "Publishing mock Image on %s", topic_name.c_str());
  }

private:
  void publish_image()
  {
    constexpr int width = 96;
    constexpr int height = 48;
    auto msg = sensor_msgs::msg::Image();
    msg.header.stamp = get_clock()->now();
    msg.header.frame_id = "ros2_helper_mock_camera";
    msg.height = height;
    msg.width = width;
    msg.encoding = "rgb8";
    msg.is_bigendian = 0;
    msg.step = width * 3;
    msg.data = make_rgb_pattern(width, height, frame_++);
    publisher_->publish(msg);
  }

  int frame_{0};
  rclcpp::Publisher<sensor_msgs::msg::Image>::SharedPtr publisher_;
  rclcpp::TimerBase::SharedPtr timer_;
};

class MockCompressedImagePublisher : public rclcpp::Node
{
public:
  MockCompressedImagePublisher(const std::string & topic_name, double hz)
  : Node("ros2_helper_mock_compressed_image_publisher")
  {
    publisher_ = create_publisher<sensor_msgs::msg::CompressedImage>(topic_name, 10);
    timer_ = create_wall_timer(
      std::chrono::duration<double>(1.0 / hz),
      std::bind(&MockCompressedImagePublisher::publish_image, this));
    RCLCPP_INFO(get_logger(), "Publishing mock CompressedImage on %s", topic_name.c_str());
  }

private:
  void publish_image()
  {
    constexpr int width = 96;
    constexpr int height = 48;
    auto msg = sensor_msgs::msg::CompressedImage();
    msg.header.stamp = get_clock()->now();
    msg.header.frame_id = "ros2_helper_mock_camera";
    msg.format = "png";
    msg.data = encode_png_rgb(width, height, make_rgb_pattern(width, height, frame_++));
    publisher_->publish(msg);
  }

  int frame_{0};
  rclcpp::Publisher<sensor_msgs::msg::CompressedImage>::SharedPtr publisher_;
  rclcpp::TimerBase::SharedPtr timer_;
};

}  // namespace

std::shared_ptr<rclcpp::Node> create_mock_image_publisher(const std::string & topic_name, double hz)
{
  return std::make_shared<MockImagePublisher>(topic_name, hz);
}

std::shared_ptr<rclcpp::Node> create_mock_compressed_image_publisher(const std::string & topic_name, double hz)
{
  return std::make_shared<MockCompressedImagePublisher>(topic_name, hz);
}

}  // namespace ros2_helper
