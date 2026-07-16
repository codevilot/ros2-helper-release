#include "ros2_helper/graphic/viewers/joint_state_viewer.hpp"

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <sstream>

#include "ros2_helper/utils/terminal_utils.hpp"

namespace ros2_helper
{

JointStateViewer::JointStateViewer(const std::string & topic_name, const std::string & topic_type, double fps)
: Node("ros2_helper_joint_state_viewer"), topic_name_(topic_name), limiter_(fps)
{
  subscription_ = create_subscription<sensor_msgs::msg::JointState>(
    topic_name, 10, std::bind(&JointStateViewer::callback, this, std::placeholders::_1));

  std::cout << "[" << topic_name << "] Starting JointState visualization (type: " << topic_type << ")\n";
}

void JointStateViewer::callback(const sensor_msgs::msg::JointState::SharedPtr msg)
{
  const auto now = std::chrono::steady_clock::now();
  message_times_.push_back(now);
  if (message_times_.size() > 60) {
    message_times_.pop_front();
  }

  const size_t joint_count = std::max({
    msg->name.size(),
    msg->position.size(),
    msg->velocity.size(),
    msg->effort.size(),
  });
  update_history(*msg, joint_count);

  if (!limiter_.ready()) {
    return;
  }

  std::ostringstream output;
  output << "\033[H\033[J";
  output << "JointState  " << topic_name_ << "   " << std::setw(5) << format_hz() << " Hz   OK\n\n";
  output << "joint              pos        vel      eff      limit        history\n";
  output << std::string(100, '-') << "\n";

  if (joint_count == 0) {
    output << "(empty JointState message)\n";
  } else {
    const size_t rows_to_render = std::min<size_t>(joint_count, 24);
    for (size_t i = 0; i < rows_to_render; ++i) {
      std::string name = i < msg->name.size() ? msg->name[i] : "joint_" + std::to_string(i);
      const std::string history_key = name;
      const auto position = get_value(msg->position, i);
      const auto velocity = get_value(msg->velocity, i);
      const auto effort = get_value(msg->effort, i);
      if (name.size() > 17) {
        name = name.substr(0, 17);
      }

      output << std::left << std::setw(17) << name << " " << std::right
             << format_number(position, 8, 4) << "    "
             << format_number(velocity, 6, 2) << "   "
             << format_number(effort, 5, 2) << "     "
             << range_bar(position) << "    "
             << sparkline(position_history_[history_key]) << "\n";
    }

    if (joint_count > rows_to_render) {
      output << "... " << (joint_count - rows_to_render) << " more joints not shown\n";
    }
  }

  output << "\nPress Ctrl+C to exit.";
  std::cout << output.str() << std::flush;
}

void JointStateViewer::update_history(const sensor_msgs::msg::JointState & msg, size_t joint_count)
{
  for (size_t i = 0; i < joint_count; ++i) {
    const std::string name = i < msg.name.size() ? msg.name[i] : "joint_" + std::to_string(i);
    const auto position = get_value(msg.position, i);
    if (!position.has_value() || !std::isfinite(*position)) {
      continue;
    }

    auto & history = position_history_[name];
    history.push_back(*position);
    if (history.size() > 24) {
      history.pop_front();
    }
  }
}

std::string JointStateViewer::format_hz() const
{
  if (message_times_.size() < 2) {
    return "--.-";
  }

  const auto elapsed = std::chrono::duration<double>(
    message_times_.back() - message_times_.front()).count();
  if (elapsed <= 0.0) {
    return "--.-";
  }

  std::ostringstream out;
  out << std::fixed << std::setprecision(1)
      << static_cast<double>(message_times_.size() - 1) / elapsed;
  return out.str();
}

}  // namespace ros2_helper
