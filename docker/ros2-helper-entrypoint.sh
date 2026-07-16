#!/usr/bin/env bash
set -e

source /opt/ros/jazzy/setup.bash
source /ws/install/setup.bash

mock_pid=""

cleanup() {
  if [ -n "$mock_pid" ]; then
    kill "$mock_pid" 2>/dev/null || true
    wait "$mock_pid" 2>/dev/null || true
  fi
}
trap cleanup EXIT INT TERM

demo_kind_for_topic() {
  case "$1" in
    /camera/image_raw) echo "image" ;;
    /camera/image_raw/compressed) echo "compressed_image" ;;
    /scan) echo "scan" ;;
    /joint_states) echo "joint_states" ;;
    *) return 1 ;;
  esac
}

maybe_start_demo_publisher() {
  local args=("$@")
  if [ "${args[0]}" = "timeout" ] && [ "${#args[@]}" -ge 3 ]; then
    args=("${args[@]:2}")
  fi

  if [ "${#args[@]}" -lt 4 ]; then
    return 0
  fi
  if [ "${args[0]}" != "ros2" ] || [ "${args[1]}" != "topic" ] || [ "${args[2]}" != "graphic" ]; then
    return 0
  fi

  local topic=""
  local previous=""
  for arg in "${args[@]:3}"; do
    if [ "$previous" = "--fps" ]; then
      previous=""
      continue
    fi
    if [ "$arg" = "--demo" ]; then
      return 0
    fi
    if [ "$arg" = "--fps" ]; then
      previous="$arg"
      continue
    fi
    if [[ "$arg" != -* ]]; then
      topic="$arg"
      break
    fi
  done

  local kind=""
  if ! kind="$(demo_kind_for_topic "$topic")"; then
    return 0
  fi

  ros2 run ros2_helper ros2-helper-graphic-mockup "$kind" --topic "$topic" >/tmp/ros2-helper-mock.log 2>&1 &
  mock_pid=$!
  sleep "${ROS2_HELPER_DOCKER_DEMO_WAIT:-2}"
}

if [ "$#" -eq 0 ]; then
  exec bash
fi

maybe_start_demo_publisher "$@"

"$@"
