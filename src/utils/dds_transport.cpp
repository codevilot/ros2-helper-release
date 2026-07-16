#include "ros2_helper/utils/dds_transport.hpp"

#include <cstdlib>

namespace ros2_helper
{

void configure_default_dds_transport()
{
  setenv("FASTDDS_BUILTIN_TRANSPORTS", "UDPv4", 0);
}

}  // namespace ros2_helper
