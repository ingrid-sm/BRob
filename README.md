# BRob

This is a small robot project I am building while learning software engineering, embedded programming and computer vision.

Right now this repo has three main parts.

1. `esp32/brob_control`
   Main ESP32 code for the robot body. It has motor control, encoder reading, ultrasonic safety check, neck movement and a small screen.

2. `vision/camera_first_test`
   First test for the ESP32-CAM. It connects to Wi-Fi and gives a simple web page where I can ask for a photo.

3. `vision/target_finder`
   Python code that asks the camera for one image and tries to find an object in it with an OWLv2 model.

## What I was trying to do

The idea of this project is:

- move the robot from an ESP32 board
- get photos from an ESP32-CAM
- run the vision part on my laptop
- later connect everything in a cleaner way

So this is still a learning project and not a finished robot yet.

## How to run the parts

### Robot control

Open `esp32/brob_control/brob_control.ino` in Arduino IDE.

Before uploading, create this file:

- `esp32/brob_control/wifi_secrets.h`

Use the example file as reference:

- `esp32/brob_control/wifi_secrets_example.h`

Then upload the sketch to the ESP32 board.

### Camera test

Open `vision/camera_first_test/camera_first_test.ino` in Arduino IDE.

Before uploading, create this file:

- `vision/camera_first_test/wifi_secrets.h`

Use this example:

- `vision/camera_first_test/wifi_secrets_example.h`

After upload, open the serial monitor and check the camera IP.

### Python target finder

Go into `vision/target_finder`.

Copy `local_config_example.py` to `local_config.py` and put your own camera IP there.

Install the packages from `requirements.txt`.

Then run:

```bash
python run_target_finder.py
```

You can type something like:

```text
bottle
```

or

```text
bottle, water bottle
```

## Notes

I did not upload private Wi-Fi files or my local camera config.

This repo is still messy in some places because I am using it to learn and test things step by step.
