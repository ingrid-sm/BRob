from __future__ import annotations

from dataclasses import dataclass
from io import BytesIO
from pathlib import Path
from typing import List
import time

import requests
import torch
from PIL import Image, ImageDraw
from transformers import Owlv2ForObjectDetection, Owlv2Processor

from local_config import CAMERA_BASE_URL, MODEL_ID, PHOTO_MODE, SCORE_THRESHOLD, TOP_K, USE_FLASH


@dataclass
class Detection:
    label: str
    score: float
    box_xyxy: List[int]


def pick_device() -> str:
    if torch.cuda.is_available():
        return "cuda"
    if hasattr(torch.backends, "mps") and torch.backends.mps.is_available():
        return "mps"
    return "cpu"


def build_photo_url(mode: str, use_flash: bool) -> str:
    camera_base_url = CAMERA_BASE_URL.rstrip("/")
    flash_value = 1 if use_flash else 0
    return f"{camera_base_url}/photo?mode={mode}&flash={flash_value}"


def try_fetch_camera_image(mode: str, use_flash: bool, timeout_seconds: int) -> Image.Image:
    url = build_photo_url(mode, use_flash)
    print(f"Fetching camera image: {url}")
    response = requests.get(url, timeout=timeout_seconds)
    response.raise_for_status()
    return Image.open(BytesIO(response.content)).convert("RGB")


def fetch_camera_image() -> Image.Image:
    attempts = [
        (PHOTO_MODE, USE_FLASH, 20),
        ("fast", False, 12),
    ]

    last_error = None

    for mode, use_flash, timeout_seconds in attempts:
        try:
            return try_fetch_camera_image(mode, use_flash, timeout_seconds)
        except Exception as error:
            last_error = error
            print(f"Camera fetch failed with mode={mode}, flash={use_flash}: {error}")
            time.sleep(1.0)

    raise RuntimeError(f"Could not fetch an image from the ESP32-CAM. Last error: {last_error}")


class SimpleTargetFinder:
    def __init__(self) -> None:
        self.device = pick_device()
        self.processor = Owlv2Processor.from_pretrained(MODEL_ID)
        self.model = Owlv2ForObjectDetection.from_pretrained(MODEL_ID).to(self.device)
        self.model.eval()

    def find(self, image: Image.Image, labels: List[str]) -> List[Detection]:
        inputs = self.processor(text=[labels], images=image, return_tensors="pt")
        inputs = {key: value.to(self.device) for key, value in inputs.items()}

        start_time = time.perf_counter()
        with torch.no_grad():
            outputs = self.model(**inputs)
        inference_ms = (time.perf_counter() - start_time) * 1000.0

        target_sizes = torch.tensor([[image.height, image.width]], device=self.device)
        result = self.processor.post_process_object_detection(
            outputs=outputs,
            target_sizes=target_sizes,
            threshold=SCORE_THRESHOLD,
        )[0]

        detections: List[Detection] = []

        for score, box, label_index in zip(result["scores"], result["boxes"], result["labels"]):
            x1, y1, x2, y2 = box.tolist()
            detections.append(
                Detection(
                    label=labels[int(label_index)],
                    score=float(score),
                    box_xyxy=[round(x1), round(y1), round(x2), round(y2)],
                )
            )

        detections.sort(key=lambda item: item.score, reverse=True)
        detections = detections[:TOP_K]

        print(f"Device: {self.device}")
        print(f"Inference time: {inference_ms:.1f} ms")

        return detections


def draw_detections(image: Image.Image, detections: List[Detection], output_path: str) -> None:
    debug_image = image.copy()
    draw = ImageDraw.Draw(debug_image)

    for detection in detections:
        x1, y1, x2, y2 = detection.box_xyxy
        draw.rectangle([x1, y1, x2, y2], outline="lime", width=4)
        draw.text((x1 + 6, y1 + 6), f"{detection.label} {detection.score:.2f}", fill="lime")

    debug_image.save(output_path)


def describe_best_detection(detection: Detection, image: Image.Image) -> None:
    x1, y1, x2, y2 = detection.box_xyxy
    center_x = (x1 + x2) / 2.0
    center_y = (y1 + y2) / 2.0
    error_x = center_x - (image.width / 2.0)
    error_y = center_y - (image.height / 2.0)

    print("Best detection:")
    print(f"  label: {detection.label}")
    print(f"  score: {detection.score:.3f}")
    print(f"  box: {detection.box_xyxy}")
    print(f"  center: ({center_x:.1f}, {center_y:.1f})")
    print(f"  error from image center: x={error_x:.1f}, y={error_y:.1f}")


def split_labels(target_text: str) -> List[str]:
    labels = [part.strip() for part in target_text.split(",")]
    labels = [label for label in labels if label]
    if not labels:
        raise ValueError("Please provide at least one label.")
    return labels


def build_output_path(labels: List[str]) -> str:
    safe_name = labels[0].replace(" ", "_").replace("/", "_")
    return str(Path(__file__).with_name(f"output_{safe_name}.jpg"))
