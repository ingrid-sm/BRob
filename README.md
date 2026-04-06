# BRob

BRob is an inspection tracked robot project built with ROS 2, Python, ESP32, sensors, and computer vision.

## Current features

- tracked movement: forward, backward, left, right, stop
- neck movement: left, center, right
- encoder feedback in ROS 2
- ultrasonic distance feedback in ROS 2
- local obstacle-stop safety on the ESP32
- OLED status screen
- vision target request and result through ROS 2

## Tech stack

- ROS 2
- Python
- C++ / Arduino
- ESP32
- ESP32-CAM
- Linux / Ubuntu VM
- TCP communication

## Project structure

```text
esp32/
  brob_control/

vision/
  camera_first_test/
  target_finder/

ros2/
  brob_hardware/
  brob_bringup/

Main ROS 2 nodes
    •    command_bridge_node.py
    •    vision_bridge_node.py

Useful ROS 2 commands

Move the robot:

ros2 topic pub /brob_command std_msgs/msg/String "{data: forward}" --once
ros2 topic pub /brob_command std_msgs/msg/String "{data: backward}" --once
ros2 topic pub /brob_command std_msgs/msg/String "{data: left}" --once
ros2 topic pub /brob_command std_msgs/msg/String "{data: right}" --once
ros2 topic pub /brob_command std_msgs/msg/String "{data: stop}" --once

Move the neck:

ros2 topic pub /brob_command std_msgs/msg/String "{data: neck_left}" --once
ros2 topic pub /brob_command std_msgs/msg/String "{data: neck_center}" --once
ros2 topic pub /brob_command std_msgs/msg/String "{data: neck_right}" --once

Vision target:

ros2 topic pub /vision/target std_msgs/msg/String "{data: bottle}" --once

Inspect feedback:

ros2 topic echo /left_encoder
ros2 topic echo /right_encoder
ros2 topic echo /distance
ros2 topic echo /vision/result

Local-only files

These files should not be committed:
    •    esp32/brob_control/wifi_secrets.h
    •    vision/camera_first_test/wifi_secrets.h
    •    vision/target_finder/local_config.py

Status

Active project. Current working areas include:
    •    embedded robot control
    •    ROS 2 communication
    •    sensor feedback
    •    local safety logic
    •    vision requests
