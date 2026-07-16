#include "ros2_helper/utils/topic_discovery.hpp"

#include <chrono>
#include <iostream>
#include <thread>

using namespace std::chrono_literals;

namespace ros2_helper
{

std::optional<std::string> find_topic_type(
  const rclcpp::Node::SharedPtr & node,
  const std::string & topic_name)
{
  auto next_status = std::chrono::steady_clock::now();

  while (rclcpp::ok()) {
    const auto topics = node->get_topic_names_and_types();
    const auto found = topics.find(topic_name);
    if (found != topics.end() && !found->second.empty()) {
      return found->second.front();
    }

    const auto now = std::chrono::steady_clock::now();
    if (now >= next_status) {
      std::cout << "[Waiting] Topic '" << topic_name << "' is not active yet. Press Ctrl+C to exit.\n";
      next_status = now + 10s;
    }

    rclcpp::spin_some(node);
    std::this_thread::sleep_for(200ms);
  }

  return std::nullopt;
}

}  // namespace ros2_helper
