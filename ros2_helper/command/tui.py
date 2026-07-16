"""`ros2 tui` command."""

from ros2cli.command import CommandExtension

from ros2_helper.tui.app import TuiApp


class TuiCommand(CommandExtension):
    """Open the ros2-helper interactive terminal UI."""

    def add_arguments(self, parser, cli_name, *, argv=None):
        parser.add_argument(
            'initial_command',
            nargs='*',
            help='Optional command to show first, for example: node, topic, service.',
        )
        parser.add_argument(
            '--once',
            metavar='COMMAND',
            help='Run one command and exit. Intended for automated smoke tests.',
        )

    def main(self, *, parser, args):
        app = TuiApp()
        if args.once:
            return app.run_once(args.once)
        initial_command = ' '.join(args.initial_command) if args.initial_command else None
        return app.run(initial_command=initial_command)
