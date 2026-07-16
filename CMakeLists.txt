cmake_minimum_required(VERSION 3.8)
project(ros2_helper VERSION 0.1.2)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(ament_cmake_python REQUIRED)
find_package(rclcpp REQUIRED)
find_package(sensor_msgs REQUIRED)

set(GRAPHIC_VIEWER_SOURCES
  src/graphic/main.cpp
  src/utils/dds_transport.cpp
  src/utils/rate_limiter.cpp
  src/utils/terminal_utils.cpp
  src/utils/topic_discovery.cpp
  src/graphic/util/png_encoder.cpp
  src/graphic/util/terminal_image_renderer.cpp
  src/graphic/viewers/compressed_image_viewer.cpp
  src/graphic/viewers/image_viewer.cpp
  src/graphic/viewers/joint_state_viewer.cpp
  src/graphic/viewers/lidar_viewer.cpp
)

add_executable(ros2-helper-graphic ${GRAPHIC_VIEWER_SOURCES})
target_compile_features(ros2-helper-graphic PUBLIC cxx_std_17)
target_include_directories(ros2-helper-graphic PRIVATE include)
ament_target_dependencies(ros2-helper-graphic rclcpp sensor_msgs)

add_executable(ros2-helper-graphic-mockup
  src/graphic/mockup_main.cpp
  src/graphic/mock/mock_image_publisher.cpp
  src/graphic/mock/mock_joint_state_publisher.cpp
  src/graphic/mock/mock_publishers.cpp
  src/graphic/mock/mock_scan_publisher.cpp
  src/utils/dds_transport.cpp
  src/graphic/util/png_encoder.cpp
)
target_compile_features(ros2-helper-graphic-mockup PUBLIC cxx_std_17)
target_include_directories(ros2-helper-graphic-mockup PRIVATE include)
ament_target_dependencies(ros2-helper-graphic-mockup rclcpp sensor_msgs)

install(TARGETS ros2-helper-graphic ros2-helper-graphic-mockup
  DESTINATION lib/${PROJECT_NAME}
)

ament_python_install_package(${PROJECT_NAME})
set(_ros2topic_entry_points "${CMAKE_CURRENT_BINARY_DIR}/ros2topic_entry_points.txt")
file(WRITE "${_ros2topic_entry_points}"
  "[ros2cli.command]\n"
  "tui = ros2_helper.command.tui:TuiCommand\n"
  "\n"
  "[ros2topic.verb]\n"
  "graphic = ros2_helper.verb.graphic:GraphicVerb\n"
)
install(
  FILES "${_ros2topic_entry_points}"
  RENAME entry_points.txt
  DESTINATION
    "${PYTHON_INSTALL_DIR}/${PROJECT_NAME}-${PROJECT_VERSION}-py${Python3_VERSION_MAJOR}.${Python3_VERSION_MINOR}.egg-info"
)

ament_package()
