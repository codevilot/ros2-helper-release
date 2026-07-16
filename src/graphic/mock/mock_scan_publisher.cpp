#include "ros2_helper/graphic/mock/mock_publishers.hpp"

#include <cmath>
#include <string>

#include "sensor_msgs/msg/laser_scan.hpp"

namespace ros2_helper
{
namespace
{

class MockScanPublisher : public rclcpp::Node
{
public:
  MockScanPublisher(const std::string & topic_name, double hz)
  : Node("ros2_helper_mock_scan_publisher")
  {
    publisher_ = create_publisher<sensor_msgs::msg::LaserScan>(topic_name, 10);
    timer_ = create_wall_timer(
      std::chrono::duration<double>(1.0 / hz),
      std::bind(&MockScanPublisher::publish_scan, this));
    RCLCPP_INFO(get_logger(), "Publishing mock LaserScan on %s", topic_name.c_str());
  }

private:
  void publish_scan()
  {
    constexpr int sample_count = 181;
    auto msg = sensor_msgs::msg::LaserScan();
    msg.header.stamp = get_clock()->now();
    msg.header.frame_id = "ros2_helper_mock_laser";
    msg.angle_min = -M_PI / 2.0;
    msg.angle_max = M_PI / 2.0;
    msg.angle_increment = (msg.angle_max - msg.angle_min) / static_cast<float>(sample_count - 1);
    msg.scan_time = 0.1;
    msg.range_min = 0.1;
    msg.range_max = 8.0;

    const double obstacle_angle = std::sin(phase_) * 0.8;
    msg.ranges.reserve(sample_count);
    msg.intensities.reserve(sample_count);
    for (int i = 0; i < sample_count; ++i) {
      const double angle = msg.angle_min + i * msg.angle_increment;
      const double wall = 3.5 + 0.4 * std::sin(angle * 4.0 + phase_);
      const double obstacle = std::abs(angle - obstacle_angle) < 0.10 ? 1.2 : wall;
      msg.ranges.push_back(static_cast<float>(obstacle));
      msg.intensities.push_back(1.0F);
    }

    publisher_->publish(msg);
    phase_ += 0.08;
  }

  double phase_{0.0};
  rclcpp::Publisher<sensor_msgs::msg::LaserScan>::SharedPtr publisher_;
  rclcpp::TimerBase::SharedPtr timer_;
};

}  // namespace

std::shared_ptr<rclcpp::Node> create_mock_scan_publisher(const std::string & topic_name, double hz)
{
  return std::make_shared<MockScanPublisher>(topic_name, hz);
}

}  // namespace ros2_helper
