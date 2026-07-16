# Docker Testing: Graphic

The Docker entrypoint auto-starts a matching mock publisher for these demo
topics, so each test stays focused on `ros2 topic graphic`.

Build the image from the repo root if needed:

```bash
docker build -f docker/Dockerfile.jazzy -t ros2-helper:jazzy .
```

## Image

```bash
docker run --rm -it ros2-helper:jazzy ros2 topic graphic /camera/image_raw --fps 10
```

## Compressed Image

```bash
docker run --rm -it ros2-helper:jazzy ros2 topic graphic /camera/image_raw/compressed --fps 10
```

## LaserScan

```bash
docker run --rm -it ros2-helper:jazzy ros2 topic graphic /scan --fps 10
```

## JointState

```bash
docker run --rm -it ros2-helper:jazzy ros2 topic graphic /joint_states --fps 10
```

Press `Ctrl+C` to stop a foreground demo.

## Automatic Smoke

```bash
docker run --rm ros2-helper:jazzy timeout 8 ros2 topic graphic /scan --fps 10
```
