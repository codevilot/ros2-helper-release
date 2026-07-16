#include "ros2_helper/utils/rate_limiter.hpp"

namespace ros2_helper
{

RateLimiter::RateLimiter(double fps)
: min_interval_(fps > 0.0 ? std::chrono::duration<double>(1.0 / fps) : std::chrono::duration<double>(0.0))
{
}

bool RateLimiter::ready()
{
  if (min_interval_.count() <= 0.0) {
    return true;
  }

  const auto now = std::chrono::steady_clock::now();
  if (rendered_once_ && now - last_render_ < min_interval_) {
    return false;
  }

  rendered_once_ = true;
  last_render_ = now;
  return true;
}

}  // namespace ros2_helper
