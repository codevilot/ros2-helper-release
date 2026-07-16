"""Interactive command loop for `ros2 tui`."""

import sys
import termios
import tty

from ros2_helper.tui.dispatcher import DispatchResult, TuiDispatcher
from ros2_helper.tui.screen import TuiScreen


class TuiApp:
    """Small interactive shell that routes commands to existing ROS 2 CLIs."""

    def __init__(self):
        self._dispatcher = TuiDispatcher()
        self._screen = TuiScreen()

    def run_once(self, command_line):
        result = self._dispatcher.dispatch(command_line)
        self._screen.render(result=result)
        return result.returncode

    def run(self, initial_command=None):
        if not sys.stdin.isatty():
            print('ros2 tui requires an interactive terminal.')
            print('Docker example: docker run --rm -it ros2-helper:jazzy ros2 tui')
            return 2

        result = DispatchResult(stdout=self._screen.help_text())
        if initial_command:
            result = self._dispatcher.dispatch(initial_command)
            if result.should_exit:
                return result.returncode

        self._screen.render(result=result)

        while True:
            try:
                command_line = self._read_command(result)
            except KeyboardInterrupt:
                print()
                return 0

            if command_line is None:
                print()
                return 0

            result = self._dispatcher.dispatch(command_line)
            if result.should_exit:
                return result.returncode
            self._screen.render(result=result)

    def _read_command(self, result):
        prompt = '\033[32mros2 tui>\033[0m '
        sys.stdout.write(prompt)
        sys.stdout.flush()

        old_settings = termios.tcgetattr(sys.stdin.fileno())
        try:
            tty.setraw(sys.stdin.fileno())
            chars = []
            exit_pending = False
            while True:
                char = sys.stdin.read(1)
                if char in {'\r', '\n'}:
                    sys.stdout.write('\n')
                    sys.stdout.flush()
                    return ''.join(chars)
                if char == '\x1b':
                    if exit_pending:
                        return None
                    exit_pending = True
                    chars = []
                    self._screen.render(result=result, exit_pending=True)
                    sys.stdout.write(prompt)
                    sys.stdout.flush()
                    continue
                if char in {'\x03', '\x04'}:
                    raise KeyboardInterrupt
                if exit_pending:
                    exit_pending = False
                    self._screen.render(result=result)
                    sys.stdout.write(prompt)
                    sys.stdout.flush()
                if char in {'\x7f', '\b'}:
                    if chars:
                        chars.pop()
                        sys.stdout.write('\b \b')
                        sys.stdout.flush()
                    continue
                if char == '\x15':
                    while chars:
                        chars.pop()
                        sys.stdout.write('\b \b')
                    sys.stdout.flush()
                    continue
                if char.isprintable():
                    chars.append(char)
                    sys.stdout.write(char)
                    sys.stdout.flush()
        finally:
            termios.tcsetattr(sys.stdin.fileno(), termios.TCSADRAIN, old_settings)
