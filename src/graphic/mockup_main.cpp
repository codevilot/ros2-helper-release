#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "rclcpp/rclcpp.hpp"

#include "ros2_helper/utils/dds_transport.hpp"
#include "ros2_helper/graphic/mock/mock_publishers.hpp"

namespace
{

struct CliOptions
{
  std::string kind;
  std::string topic_name;
  double hz{10.0};
};

void print_usage()
{
  std::cerr << "Usage: ros2 run ros2_helper ros2-helper-graphic-mockup "
            << "<image|compressed_image|scan|joint_states> [--topic <topic>] [--hz <hz>]\n";
}

CliOptions parse_args(int argc, char ** argv)
{
  CliOptions options;
  std::vector<std::string> args;
  for (int i = 1; i < argc; ++i) {
    args.emplace_back(argv[i]);
  }

  for (size_t i = 0; i < args.size(); ++i) {
    if (args[i] == "--topic") {
      if (i + 1 >= args.size()) {
        throw std::invalid_argument("--topic requires a value");
      }
      options.topic_name = args[++i];
    } else if (args[i] == "--hz") {
      if (i + 1 >= args.size()) {
        throw std::invalid_argument("--hz requires a value");
      }
      options.hz = std::stod(args[++i]);
      if (options.hz <= 0.0) {
        throw std::invalid_argument("--hz must be greater than 0");
      }
    } else if (options.kind.empty()) {
      options.kind = args[i];
    } else {
      throw std::invalid_argument("unexpected argument: " + args[i]);
    }
  }

  if (options.kind.empty()) {
    throw std::invalid_argument("mock kind is required");
  }
  if (options.topic_name.empty()) {
    options.topic_name = ros2_helper::default_mock_topic(options.kind);
  }
  return options;
}

}  // namespace

int main(int argc, char ** argv)
{
  ros2_helper::configure_default_dds_transport();
  rclcpp::init(argc, argv);

  try {
    const CliOptions options = parse_args(argc, argv);
    rclcpp::spin(ros2_helper::create_mock_publisher(
      options.kind, options.topic_name, options.hz));
  } catch (const std::exception & error) {
    std::cerr << "[Error] " << error.what() << "\n";
    print_usage();
    rclcpp::shutdown();
    return 2;
  }

  rclcpp::shutdown();
  return 0;
}
