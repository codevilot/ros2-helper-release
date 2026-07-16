"""Subprocess wrapper for ROS 2 CLI commands."""

import subprocess


class RosCommandRunner:
    """Run existing ROS 2 CLI commands without reimplementing their logic."""

    def run(self, args):
        return subprocess.run(
            ['ros2', *args],
            check=False,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True,
        )
