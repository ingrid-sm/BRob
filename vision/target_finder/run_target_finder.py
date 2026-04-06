from __future__ import annotations

import sys

from simple_target_finder import (
    SimpleTargetFinder,
    build_output_path,
    describe_best_detection,
    draw_detections,
    fetch_camera_image,
    split_labels,
)


def run_one_search(finder: SimpleTargetFinder, target_text: str) -> None:
    labels = split_labels(target_text)

    print("Target labels:", labels)
    print("Fetching image from the camera...")
    image = fetch_camera_image()

    print("Running the model...")
    detections = finder.find(image, labels)

    output_path = build_output_path(labels)
    draw_detections(image, detections, output_path)

    if detections:
        describe_best_detection(detections[0], image)
    else:
        print("No detection found above the score threshold.")

    print(f"Saved debug image: {output_path}")


def main() -> None:
    print("Loading the model once...")
    finder = SimpleTargetFinder()

    if len(sys.argv) >= 2:
        run_one_search(finder, sys.argv[1])
        return

    print('Type a target like: bottle')
    print('You can also type synonyms like: bottle, water bottle')
    print("Press Enter on an empty line to quit.")

    while True:
        target_text = input("\nTarget: ").strip()
        if not target_text:
            break
        run_one_search(finder, target_text)


if __name__ == "__main__":
    main()
