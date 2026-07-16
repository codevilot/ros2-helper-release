#pragma once

#include <chrono>

namespace ros2_helper
{

class RateLimiter
{
public:
  explicit RateLimiter(double fps);

  bool ready();

private:
  std::chrono::duration<double> min_interval_;
  std::chrono::steady_clock::time_point last_render_{};
  bool rendered_once_{false};
};

}  // namespace ros2_helper
