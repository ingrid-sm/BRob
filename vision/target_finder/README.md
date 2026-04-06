# Target Finder

This is the first AI step.

It is deliberately simple:

1. Ask the ESP32-CAM for one photo.
2. Ask a vision model to look for your target text.
3. Save a debug image with boxes.
4. Print the best match score and box.

## Why this is a good next step

It keeps the system easy to explain:

- ESP32-CAM = takes photos
- MacBook = runs the AI
- Python script = connects both parts

That is much simpler than jumping into ROS too early.

## Files

- `run_target_finder.py`: command line entry point
- `simple_target_finder.py`: the actual beginner-friendly AI code
- `local_config.py`: your local camera settings

## First setup

Copy `local_config_example.py` to `local_config.py` and check:

- `CAMERA_BASE_URL`
- `PHOTO_MODE`
- `USE_FLASH`

If your camera page is something like `http://YOUR_CAMERA_IP`, then `CAMERA_BASE_URL` should be:

- `http://YOUR_CAMERA_IP`

## Python environment

Your existing robot environment already seems to have the needed packages in:

- `/Users/ingrid.sm/Desktop/Projects/robot/.venv`

So the easiest beginner-friendly path is to use that environment.

## How to run

From Terminal:

```bash
cd /Users/ingrid.sm/Desktop/Projects/BRob/vision/target_finder
source /Users/ingrid.sm/Desktop/Projects/robot/.venv/bin/activate
python run_target_finder.py
```

Then type a target such as:

```text
bottle
```

You can also give synonyms:

```text
bottle, water bottle
```

## What you get

The script will:

- fetch one image from the camera
- run `google/owlv2-base-patch16`
- print the best detection
- save an image like `output_bottle.jpg`
- keep the model loaded while you test multiple targets

## About speed

This first version is designed for clarity, not max speed.

That means:

- model loading is simple to understand
- one photo is easier than a live video stream
- it is fine for a first portfolio version

Later, if you want, the next improvement is:

1. load the model once and keep it in memory
2. ask the camera for repeated photos
3. turn it into a ROS node

## Should you put this into ROS right now?

No. Not yet.

First, run it as a normal Python script until it works well.

That is the cleanest beginner workflow.

Later, the professional next step is to wrap this same logic in a ROS package.

In other words:

- first make the vision logic work
- then make ROS call it

That is better than debugging AI and ROS at the same time.
