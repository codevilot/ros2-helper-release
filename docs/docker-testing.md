# Docker Testing

Docker is for local development and demos. User installation should use the ROS
APT package once released: `sudo apt install ros-jazzy-ros2-helper`.

Build the image once before running the feature-specific tests:

```bash
docker build -f docker/Dockerfile.jazzy -t ros2-helper:jazzy .
```

## Feature Tests

- [Graphic Docker testing](docker-testing-graphic.md)
- [TUI Docker testing](docker-testing-tui.md)

## Quick Smoke

```bash
docker run --rm ros2-helper:jazzy timeout 8 ros2 topic graphic /scan --fps 10
```
