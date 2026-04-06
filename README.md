# BRob

This is my robot project.

I am building it while learning software engineering, ROS 2, ESP32 programming and computer vision.

It is not finished. Right now it is more like different parts that already work and that I am trying to connect better.

## What is in this repo

`esp32/brob_control`

This is the main code for the robot body.
It runs on an ESP32 and it does things like:

- motor control
- encoder reading
- ultrasonic distance check
- small OLED screen status
- neck motor movement
- TCP connection so ROS can send commands to the robot

`vision/camera_first_test`

This is my first ESP32-CAM test.
It connects to Wi-Fi and gives me a simple camera page in the browser so I can request photos.

`vision/target_finder`

This is the Python vision part.
It gets one image from the camera and uses OWLv2 to look for a target like `bottle` or `person`.

It can be used in two ways:

- as a normal test script
- as an API that ROS can call

## ROS 2 part

I am also using ROS 2, but that part is in a separate Ubuntu VM workspace right now, not inside this repo.

The ROS workspace currently has these packages:

- `brob_hardware`
- `brob_bringup`

The main nodes I am using there are:

- `command_bridge_node.py`
- `vision_bridge_node.py`

What they do in a simple way:

- `command_bridge_node.py` sends robot movement commands from ROS to the ESP32 over TCP
- it also reads encoder and distance data from the ESP32 and publishes them back to ROS topics
- `vision_bridge_node.py` sends a target request to the vision API and publishes the result back into ROS

So the robot system is split between:

- ESP32 code
- ESP32-CAM code
- Python vision code on my Mac
- ROS 2 nodes in my Ubuntu VM

## How I usually run it

### 1. Robot ESP32

Open `esp32/brob_control/brob_control.ino` in Arduino IDE.

Before uploading, create this file from the example:

- `esp32/brob_control/wifi_secrets.h`
- example: `esp32/brob_control/wifi_secrets_example.h`

Then upload it to the ESP32 that controls the robot.

### 2. Camera ESP32-CAM

Open `vision/camera_first_test/camera_first_test.ino` in Arduino IDE.

Before uploading, create this file from the example:

- `vision/camera_first_test/wifi_secrets.h`
- example: `vision/camera_first_test/wifi_secrets_example.h`

Then upload it to the ESP32-CAM.

After that, open the serial monitor and check the camera IP.

### 3. Vision code on the Mac

Go here:

```bash
cd /Users/ingrid.sm/Desktop/Projects/BRob/vision/target_finder
```

Copy the example config and edit it with your own camera IP:

```bash
cp local_config_example.py local_config.py
```

Install the Python packages:

```bash
python3 -m pip install -r requirements.txt
```

If I only want to test the model by hand, I run:

```bash
python3 run_target_finder.py
```

If I want ROS to use the vision model, I run:

```bash
./run_vision_api.sh
```

That starts the vision API on port `8001`.

### 4. ROS 2 side

In my Ubuntu VM, I run the ROS 2 workspace and start the bridge nodes from there.

That part is not fully documented in this repo yet because the ROS workspace is separate at the moment.

## Current idea of the system

Very roughly, the flow is this:

1. ROS sends a movement command.
2. `command_bridge_node.py` sends it to the ESP32 robot controller.
3. The ESP32 sends back encoder and distance data.
4. ROS can also send a vision target.
5. `vision_bridge_node.py` sends that request to the Python vision API.
6. The vision API gets one photo from the ESP32-CAM and returns the detection result.

## Private files

These files are not meant to be uploaded:

- `esp32/brob_control/wifi_secrets.h`
- `vision/camera_first_test/wifi_secrets.h`
- `vision/target_finder/local_config.py`

I kept the example files in the repo, but not the real private ones.

## Notes

This project is still a learning project.
Some parts are cleaner than others.
I am still building it step by step.
