# ros2-helper

Terminal-first helper tools for ROS 2.

```bash
ros2 topic graphic /scan
ros2 topic graphic /camera/image_raw --fps 10
ros2 topic graphic /joint_states
```

The `ros2_helper` package ships `graphic`, a terminal viewer for common sensor
topics. The ROS 2 CLI plugin is intentionally thin and delegates rendering to
the C++ executable `ros2-helper-graphic`.

## Supported Graphic Topics

- `sensor_msgs/msg/Image`
- `sensor_msgs/msg/CompressedImage`
- `sensor_msgs/msg/LaserScan`
- `sensor_msgs/msg/JointState`

## Install

### Official ROS Packages

After this package is released through the ROS build farm for Jazzy:

```bash
sudo apt update
sudo apt install ros-jazzy-ros2-helper
source /opt/ros/jazzy/setup.bash
```

The apt package name follows the official ROS convention:
`ros-$ROS_DISTRO-ros2-helper`.

### From Source

Until the ROS build farm release is available, build from a ROS 2 workspace:

```bash
source /opt/ros/$ROS_DISTRO/setup.bash
colcon build --packages-select ros2_helper
source install/setup.bash
```

If this package is not in a workspace yet:

```bash
mkdir -p ~/ros2_ws/src
cd ~/ros2_ws/src
git clone https://github.com/codevilot/ros2-helper.git
cd ~/ros2_ws
source /opt/ros/$ROS_DISTRO/setup.bash
colcon build --packages-select ros2_helper
source install/setup.bash
```

## Reproducible Docker Environment

Docker is for reproducible development, testing, and demos. It does not replace
the official ROS APT installation path; production and user installation should
use `sudo apt install ros-jazzy-ros2-helper` once the package is released.

Build the Jazzy source-build image:

```bash
docker build -f docker/Dockerfile.jazzy -t ros2-helper:jazzy .
```

Run an interactive container:

```bash
docker run --rm -it ros2-helper:jazzy
```

Run a focused graphic demo:

```bash
docker run --rm -it ros2-helper:jazzy ros2 topic graphic /scan --fps 10
```

Run the interactive helper shell:

```bash
docker run --rm -it ros2-helper:jazzy ros2 tui
```

Open the helper shell directly on the node page:

```bash
docker run --rm -it ros2-helper:jazzy ros2 tui node
```

For an automatic smoke test:

```bash
docker run --rm ros2-helper:jazzy timeout 8 ros2 topic graphic /scan --fps 10
```

The interactive shell sources ROS and the workspace setup files automatically.
You can also source them explicitly:

```bash
source /opt/ros/jazzy/setup.bash
source /ws/install/setup.bash
ros2 topic graphic --help
```

Advanced: test with a mock publisher in one shell:

```bash
ros2 run ros2_helper ros2-helper-graphic-mockup scan
```

Then run the graphic viewer in another container shell:

```bash
ros2 topic graphic /scan --fps 10
```

See [Docker Testing](docs/docker-testing.md) for type-specific commands.

## Usage

Open the interactive helper shell:

```bash
ros2 tui
```

Open it directly on a first page:

```bash
ros2 tui topic
ros2 tui service
ros2 tui node
```

Inside `ros2 tui`, short commands route to the normal ROS 2 CLI:

```text
topic
service
node
graphic /scan --fps 10
exit
```

View a laser scan:

```bash
ros2 topic graphic /scan --fps 10
```

View a raw image topic:

```bash
ros2 topic graphic /camera/image_raw --fps 10
```

View a compressed image topic:

```bash
ros2 topic graphic /camera/image_raw/compressed --fps 10
```

View joint states:

```bash
ros2 topic graphic /joint_states --fps 10
```

If the topic is not active yet, the viewer waits instead of exiting. A waiting
message is printed every 10 seconds.

## Advanced Usage

The C++ executable remains available directly:

```bash
ros2 run ros2_helper ros2-helper-graphic /scan --fps 10
```

## Mock Publishers

Use the built-in mock publishers for local testing.

Raw image:

```bash
ros2 run ros2_helper ros2-helper-graphic-mockup image
ros2 topic graphic /camera/image_raw
```

Compressed image:

```bash
ros2 run ros2_helper ros2-helper-graphic-mockup compressed_image
ros2 topic graphic /camera/image_raw/compressed
```

Laser scan:

```bash
ros2 run ros2_helper ros2-helper-graphic-mockup scan
ros2 topic graphic /scan --fps 10
```

Joint state:

```bash
ros2 run ros2_helper ros2-helper-graphic-mockup joint_states
ros2 topic graphic /joint_states
```

Mock publishers accept `--topic` and `--hz`:

```bash
ros2 run ros2_helper ros2-helper-graphic-mockup scan --topic /debug_scan --hz 5
```

## Verification

After building from source:

```bash
colcon build --packages-select ros2_helper
source install/setup.bash
ros2 topic --help
ros2 topic graphic --help
```

Then test with a mock publisher:

```bash
ros2 run ros2_helper ros2-helper-graphic-mockup scan
ros2 topic graphic /scan --fps 10
```

## Runtime Dependencies

`chafa` is required for terminal image rendering. The package declares:

```xml
<exec_depend>chafa</exec_depend>
```

## Roadmap

- `ros2 topic graphic`: complete
- `ros2 tui`: initial interactive helper shell
- `plot`: planned
- `explain`: planned
- `tf doctor`: planned

## Notes

Fast DDS shared-memory transport can emit noisy `RTPS_TRANSPORT_SHM` messages on
some machines. The executables default `FASTDDS_BUILTIN_TRANSPORTS` to `UDPv4`
unless the variable is already set by the user.

## Release Checklist

1. Confirm the package is `ros2_helper` and the version is `0.1.2`.
2. Confirm `<exec_depend>chafa</exec_depend>` resolves through rosdep.
3. Commit the release-prep changes with `git commit -am "Prepare ros2_helper 0.1.2 release"`.
4. Create an annotated tag with `git tag -a v0.1.2 -m "ros2_helper 0.1.2"`.
5. Run `bloom-release --rosdistro jazzy ...`.
6. Wait for the rosdistro release PR to merge.
7. Confirm the build farm completed and the apt repository synced.
8. Test `sudo apt install ros-jazzy-ros2-helper`.
