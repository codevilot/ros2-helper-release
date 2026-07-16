"""Command routing for the ros2-helper TUI."""

from dataclasses import dataclass
import shlex

from ros2_helper.tui.ros_commands import RosCommandRunner


@dataclass
class DispatchResult:
    """Result shown by the TUI screen."""

    returncode: int = 0
    title: str = ''
    stdout: str = ''
    stderr: str = ''
    command_line: str = ''
    active_section: str = 'home'
    should_exit: bool = False


class TuiDispatcher:
    """Translate short TUI commands into regular `ros2 ...` commands."""

    def __init__(self):
        self._runner = RosCommandRunner()

    def dispatch(self, command_line):
        command_line = command_line.strip()
        if not command_line:
            return DispatchResult()

        try:
            tokens = shlex.split(command_line)
        except ValueError as exc:
            return DispatchResult(returncode=2, stderr=str(exc))

        command = tokens[0]
        args = tokens[1:]

        if command in {'exit', 'quit', 'q'}:
            return DispatchResult(should_exit=True)
        if command == 'clear':
            return DispatchResult(title='clear', active_section='home')
        if command in {'help', '?', 'home', 'tui'}:
            return DispatchResult(
                title='home',
                stdout=self._help_text(),
                command_line=command_line,
                active_section='home',
            )
        if command == 'graphic':
            return self._run_ros(['topic', 'graphic', *args], command_line, 'graphic')
        if command in {'topic', 'service', 'node', 'action'}:
            return self._run_ros(self._expand_default_list(command, args), command_line, command)

        return DispatchResult(
            returncode=2,
            command_line=command_line,
            active_section='error',
            stderr=f"unknown command: {command}\n\n{self._help_text()}",
        )

    def _run_ros(self, ros_args, command_line, active_section):
        completed = self._runner.run(ros_args)
        return DispatchResult(
            returncode=completed.returncode,
            title='ros2 ' + ' '.join(ros_args),
            stdout=completed.stdout,
            stderr=completed.stderr,
            command_line=command_line,
            active_section=active_section,
        )

    @staticmethod
    def _expand_default_list(command, args):
        if args:
            return [command, *args]
        return [command, 'list']

    @staticmethod
    def _help_text():
        return '\n'.join(
            [
                'Commands:',
                '  tui | home             -> show this screen',
                '  topic                  -> ros2 topic list',
                '  topic <args>           -> ros2 topic <args>',
                '  service                -> ros2 service list',
                '  service <args>         -> ros2 service <args>',
                '  node                   -> ros2 node list',
                '  action                 -> ros2 action list',
                '  graphic <topic> <args> -> ros2 topic graphic <topic> <args>',
                '  clear',
                '  help',
                '  exit',
            ]
        )
