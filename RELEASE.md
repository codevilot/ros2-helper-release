# Release Notes

## Project Rename

This repository is `ros2-helper` and keeps the existing git history.

- GitHub repository: `ros2-helper`
- ROS package: `ros2_helper`
- Official ROS apt package: `ros-jazzy-ros2-helper`
- Description: Terminal-first helper tools for ROS 2

The `graphic` functionality is the first helper in the renamed `ros2_helper`
package. Existing `v0.1.0` and `v0.1.1` tags belong to the former
`ros2_echo_graphic` / `echo_graphic` package, so the first `ros2_helper`
release used `0.1.2` and tag `v0.1.2`. The current release is `0.1.3` with
package homepage metadata. Create a new bloom release repository
later as `ros2-helper-release`.

## GitHub Release

1. Update `package.xml` and `CMakeLists.txt` versions.
2. Commit the version change.
3. Create and push a matching tag:

```bash
git tag -a v0.1.3 -m "ros2_helper 0.1.3"
git push origin main v0.1.3
```

Users should prefer official ROS packages after the ROS build farm release:

```bash
sudo apt update
sudo apt install ros-jazzy-ros2-helper
```

## ROS Build Farm Release

Official apt distribution through the ROS build farm requires a release
repository and a rosdistro pull request.

Prerequisites on Ubuntu/ROS 2 Jazzy:

```bash
sudo apt install python3-bloom python3-rosdep fakeroot debhelper dh-python
sudo rosdep init
rosdep update
```

Then run bloom against this upstream repository:

```bash
bloom-release \
  --rosdistro jazzy \
  --track jazzy \
  --new-track \
  ros2_helper
```

Use this upstream repository when bloom prompts for it:

```text
https://github.com/codevilot/ros2-helper.git
```

Use a new release repository:

```text
https://github.com/codevilot/ros2-helper-release.git
```

Do not create a custom apt repository. The official apt package name is derived
from the ROS package name and rosdistro: `ros-jazzy-ros2-helper`.

## ROS Build Farm Checklist

1. Confirm the package is `ros2_helper` and the version is `0.1.3`.
2. Confirm `<exec_depend>chafa</exec_depend>` resolves through rosdep.
3. Commit the release-prep changes:

```bash
git commit -am "Prepare ros2_helper 0.1.3 release"
```

4. Create an annotated tag:

```bash
git tag -a v0.1.3 -m "ros2_helper 0.1.3"
```

5. Run `bloom-release --rosdistro jazzy ...`.
6. Wait for the rosdistro release PR to merge.
7. Confirm the build farm completed and the apt repository synced.
8. Test the official apt package:

```bash
sudo apt update
sudo apt install ros-jazzy-ros2-helper
source /opt/ros/jazzy/setup.bash
ros2 topic graphic --help
```
