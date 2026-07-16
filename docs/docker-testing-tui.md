# Docker Testing: TUI

`ros2 tui` is an interactive helper shell for common ROS 2 CLI discovery
commands. It routes input to existing `ros2` commands instead of reimplementing
ROS graph logic.

## Open TUI

Use `-it`; the TUI needs an interactive terminal.

```bash
docker run --rm -it ros2-helper:jazzy ros2 tui
```

Open directly on the node page:

```bash
docker run --rm -it ros2-helper:jazzy ros2 tui node
```

Open directly on the topic page:

```bash
docker run --rm -it ros2-helper:jazzy ros2 tui topic
```

Inside the prompt:

```text
topic
service
node
graphic /scan --fps 10
tui
exit
```

`tui` or `home` returns to the home screen.

Press `Esc` once to show an exit confirmation, then press `Esc` again to close
the TUI. `Ctrl+C` and `exit` also close it.

## Help

```bash
docker run --rm ros2-helper:jazzy ros2 tui --help
```
