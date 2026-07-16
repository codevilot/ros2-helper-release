#include "ros2_helper/graphic/util/terminal_image_renderer.hpp"

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <sstream>

#include "ros2_helper/utils/terminal_utils.hpp"

namespace ros2_helper
{

namespace
{

std::string chafa_command(int columns, int rows)
{
  std::ostringstream command;
  command << "chafa --polite on --animate off --size " << std::max(1, columns) << "x"
          << std::max(1, rows) << " -";
  return command.str();
}

void restore_terminal_cursor()
{
  std::cout << "\033[?25h" << std::flush;
}

void prepare_terminal_frame()
{
  static bool restore_registered = false;
  if (!restore_registered) {
    std::atexit(restore_terminal_cursor);
    restore_registered = true;
  }

  std::cout << "\033[?25l\033[H" << std::flush;
}

void print_status_line(const TerminalSize & terminal_size, int image_rows, const TerminalImageStatus & status)
{
  std::ostringstream line;
  line << "\033[" << terminal_size.rows << ";1H\033[K"
       << status.topic_name << "  " << status.topic_type << "  ";
  if (status.image_size.has_value()) {
    line << *status.image_size << "  ";
  }
  line << "fit " << terminal_size.columns << "x" << image_rows << "  "
       << std::fixed << std::setprecision(1) << status.render_hz << " Hz";
  std::cout << line.str() << std::flush;
}

}  // namespace

double update_render_hz(std::optional<std::chrono::steady_clock::time_point> & last_render_time)
{
  const auto now = std::chrono::steady_clock::now();
  double hz = 0.0;
  if (last_render_time.has_value()) {
    const auto elapsed = std::chrono::duration<double>(now - *last_render_time).count();
    if (elapsed > 1e-9) {
      hz = 1.0 / elapsed;
    }
  }
  last_render_time = now;
  return hz;
}

bool render_terminal_image(const std::vector<uint8_t> & image_data, const TerminalImageStatus & status)
{
  const auto terminal_size = get_terminal_size();
  const int image_rows = std::max(1, terminal_size.rows - 1);
  const std::string command = chafa_command(terminal_size.columns, image_rows);

  prepare_terminal_frame();
  FILE * pipe = popen(command.c_str(), "w");
  if (!pipe) {
    return false;
  }

  fwrite(image_data.data(), 1, image_data.size(), pipe);
  pclose(pipe);
  print_status_line(terminal_size, image_rows, status);
  return true;
}

}  // namespace ros2_helper
