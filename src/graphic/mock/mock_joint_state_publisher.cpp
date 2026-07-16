#include "ros2_helper/graphic/mock/mock_publishers.hpp"

#include <cmath>
#include <string>

#include "sensor_msgs/msg/joint_state.hpp"

namespace ros2_helper
{
namespace
{

class MockJointStatePublisher : public rclcpp::Node
{
public:
  MockJointStatePublisher(const std::string & topic_name, double hz)
  : Node("ros2_helper_mock_joint_state_publisher")
  {
    publisher_ = create_publisher<sensor_msgs::msg::JointState>(topic_name, 10);
    timer_ = create_wall_timer(
      std::chrono::duration<double>(1.0 / hz),
      std::bind(&MockJointStatePublisher::publish_joint_state, this));
    RCLCPP_INFO(get_logger(), "Publishing mock JointState on %s", topic_name.c_str());
  }

private:
  void publish_joint_state()
  {
    auto msg = sensor_msgs::msg::JointState();
    msg.header.stamp = get_clock()->now();
    msg.header.frame_id = "ros2_helper_mock_robot";
    msg.name = {"base_yaw", "shoulder_pitch", "elbow_pitch", "wrist_roll", "gripper_left", "gripper_right"};

    msg.position = {
      std::sin(phase_) * M_PI,
      std::sin(phase_ * 0.8) * 1.2,
      std::cos(phase_ * 0.9),
      std::sin(phase_ * 1.4) * M_PI,
      0.03 + 0.02 * (1.0 + std::sin(phase_ * 1.7)),
      0.03 + 0.02 * (1.0 + std::sin(phase_ * 1.7)),
    };
    msg.velocity = {
      std::cos(phase_) * 0.6,
      std::cos(phase_ * 0.8) * 0.4,
      -std::sin(phase_ * 0.9) * 0.5,
      std::cos(phase_ * 1.4) * 0.8,
      std::cos(phase_ * 1.7) * 0.03,
      std::cos(phase_ * 1.7) * 0.03,
    };
    msg.effort = {
      0.6 + 0.2 * std::sin(phase_ * 0.7),
      1.4 + 0.4 * std::cos(phase_ * 0.5),
      1.0 + 0.3 * std::sin(phase_ * 0.9),
      0.3 + 0.1 * std::cos(phase_ * 1.1),
      0.05,
      0.05,
    };

    publisher_->publish(msg);
    phase_ += 0.08;
  }

  double phase_{0.0};
  rclcpp::Publisher<sensor_msgs::msg::JointState>::SharedPtr publisher_;
  rclcpp::TimerBase::SharedPtr timer_;
};

}  // namespace

std::shared_ptr<rclcpp::Node> create_mock_joint_state_publisher(const std::string & topic_name, double hz)
{
  return std::make_shared<MockJointStatePublisher>(topic_name, hz);
}

}  // namespace ros2_helper
