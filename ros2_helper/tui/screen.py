"""Screen output helpers for the ros2-helper TUI."""

import shutil


RESET = '\033[0m'
BOLD = '\033[1m'
DIM = '\033[2m'
CYAN = '\033[36m'
GREEN = '\033[32m'
YELLOW = '\033[33m'
RED = '\033[31m'
REVERSE = '\033[7m'


class TuiScreen:
    """Render a compact single-page command prompt and command output."""

    def clear(self):
        print('\033[2J\033[H', end='')

    def render(self, result=None, exit_pending=False):
        size = shutil.get_terminal_size((100, 30))
        width = max(72, size.columns)
        height = max(20, size.lines)

        self.clear()
        self._print_header(width, result)
        self._print_tabs(width, result)
        self._print_result_panel(width, height, result)
        self._print_footer(width, exit_pending=exit_pending)

    def print_result(self, result):
        self.render(result)

    def _print_header(self, width, result):
        status = 'ready'
        status_color = GREEN
        if result and result.returncode:
            status = f'exit {result.returncode}'
            status_color = RED

        title = ' ros2-helper tui '
        subtitle = ' terminal-first ROS 2 helper shell '
        right = f' {status_color}{status}{RESET} '
        line = self._fit(f'{BOLD}{CYAN}{title}{RESET}{DIM}{subtitle}{RESET}', width - len(status) - 2)
        print(line + right)
        print(DIM + '-' * width + RESET)

    def _print_tabs(self, width, result):
        active = result.active_section if result else 'home'
        tabs = ['home', 'topic', 'service', 'node', 'action', 'graphic']
        rendered = []
        for tab in tabs:
            label = f' {tab} '
            if tab == active:
                rendered.append(f'{REVERSE}{label}{RESET}')
            else:
                rendered.append(f'{DIM}{label}{RESET}')
        print(self._fit(' '.join(rendered), width))
        print()

    def _print_result_panel(self, width, height, result):
        panel_height = max(8, height - 8)
        content_width = width - 4

        title = self._panel_title(result)
        print('+' + '-' * (width - 2) + '+')
        print('| ' + self._fit(title, content_width) + ' |')
        print('|' + '-' * (width - 2) + '|')

        lines = self._result_lines(result)
        for line in lines[:panel_height]:
            print('| ' + self._fit(line, content_width) + ' |')
        for _ in range(max(0, panel_height - len(lines))):
            print('| ' + ' ' * content_width + ' |')
        print('+' + '-' * (width - 2) + '+')

    def _print_footer(self, width, exit_pending=False):
        print()
        if exit_pending:
            hints = 'Press Esc again to exit. Type or press any other key to continue.'
            print(YELLOW + self._fit(hints, width) + RESET)
            return

        hints = 'Enter: topic | service | node | action | graphic /scan --fps 10 | tui | exit'
        print(DIM + self._fit(hints, width) + RESET)

    def _panel_title(self, result):
        if not result or not result.title:
            return 'Home'
        if result.command_line:
            return f'{result.title}    ({result.command_line})'
        return result.title

    def _result_lines(self, result):
        if not result:
            return self.help_text().splitlines()
        if result.title == 'clear':
            return self.help_text().splitlines()

        lines = []
        if result.stdout:
            lines.extend(result.stdout.rstrip().splitlines())
        if result.stderr:
            if lines:
                lines.append('')
            lines.extend((RED + line + RESET) for line in result.stderr.rstrip().splitlines())
        if result.returncode:
            lines.append('')
            lines.append(f'{RED}[exit {result.returncode}]{RESET}')
        if not lines:
            lines.append(DIM + 'No results.' + RESET)
        return lines

    @staticmethod
    def help_text():
        return '\n'.join(
            [
                'Start with a short command. The TUI keeps you on one screen.',
                '',
                'Common pages:',
                '  topic                  list topics',
                '  service                list services',
                '  node                   list nodes',
                '  action                 list actions',
                '',
                'Pass through to ROS 2 CLI:',
                '  topic info /scan',
                '  service type /clear',
                '  node info /my_node',
                '',
                'Helpers:',
                '  graphic /scan --fps 10',
                '  tui                    return home',
                '  clear                  return home',
                '  exit                   close',
            ]
        )

    @staticmethod
    def _fit(text, width):
        if width <= 0:
            return ''
        plain_len = _visible_len(text)
        if plain_len <= width:
            return text + ' ' * (width - plain_len)
        return _truncate_ansi(text, width)


def _visible_len(text):
    length = 0
    in_escape = False
    for char in text:
        if char == '\033':
            in_escape = True
            continue
        if in_escape:
            if char.isalpha():
                in_escape = False
            continue
        length += 1
    return length


def _truncate_ansi(text, width):
    result = []
    visible = 0
    in_escape = False
    for char in text:
        if char == '\033':
            in_escape = True
            result.append(char)
            continue
        if in_escape:
            result.append(char)
            if char.isalpha():
                in_escape = False
            continue
        if visible >= max(0, width - 1):
            break
        result.append(char)
        visible += 1
    if width > 0:
        result.append(' ')
    result.append(RESET)
    return ''.join(result)
