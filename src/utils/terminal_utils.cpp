#include "ros2_helper/utils/terminal_utils.hpp"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iomanip>
#include <sstream>
#include <sys/ioctl.h>
#include <unistd.h>

namespace ros2_helper
{

bool command_exists(const char * command)
{
  std::string test = "command -v ";
  test += command;
  test += " >/dev/null 2>&1";
  return std::system(test.c_str()) == 0;
}

TerminalSize get_terminal_size()
{
  winsize size{};
  if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &size) == 0 && size.ws_col > 0 && size.ws_row > 0) {
    return TerminalSize{static_cast<int>(size.ws_col), static_cast<int>(size.ws_row)};
  }
  return {};
}

std::optional<double> get_value(const std::vector<double> & values, size_t index)
{
  if (index >= values.size()) {
    return std::nullopt;
  }
  return values[index];
}

std::string format_number(std::optional<double> value, int width, int precision)
{
  std::ostringstream out;
  out << std::setw(width);
  if (!value.has_value()) {
    out << "n/a";
  } else if (!std::isfinite(*value)) {
    out << *value;
  } else {
    out << std::fixed << std::setprecision(precision) << *value;
  }
  return out.str();
}

std::string range_bar(std::optional<double> position, size_t width)
{
  if (!position.has_value() || !std::isfinite(*position)) {
    return "[" + std::string(width * 3, ' ') + "]";
  }

  const double normalized = std::max(-1.0, std::min(1.0, *position / M_PI));
  size_t filled = static_cast<size_t>(std::round(((normalized + 1.0) / 2.0) * width));
  filled = std::min(width, filled);

  std::string bar = "[";
  for (size_t i = 0; i < filled; ++i) {
    bar += "█";
  }
  for (size_t i = filled; i < width; ++i) {
    bar += "░";
  }
  bar += "]";
  return bar;
}

std::string sparkline(const std::deque<double> & values, size_t width)
{
  if (values.empty()) {
    return std::string(width, ' ');
  }

  std::vector<double> samples;
  const size_t start = values.size() > width ? values.size() - width : 0;
  for (size_t i = start; i < values.size(); ++i) {
    samples.push_back(values[i]);
  }
  if (samples.size() == 1) {
    samples.assign(width, samples.front());
  } else if (samples.size() < width) {
    samples.insert(samples.begin(), width - samples.size(), samples.front());
  }

  const auto [low_it, high_it] = std::minmax_element(samples.begin(), samples.end());
  const double low = *low_it;
  const double high = *high_it;
  const std::vector<std::string> ticks = {"▁", "▂", "▃", "▄", "▅", "▆", "▇", "█"};
  if (std::abs(high - low) < 1e-9) {
    std::string flat;
    for (size_t i = 0; i < width; ++i) {
      flat += ticks.front();
    }
    return flat;
  }

  std::string result;
  const double scale = static_cast<double>(ticks.size() - 1) / (high - low);
  for (const double sample : samples) {
    size_t index = static_cast<size_t>(std::round((sample - low) * scale));
    index = std::min(index, ticks.size() - 1);
    result += ticks[index];
  }
  return result;
}

}  // namespace ros2_helper
