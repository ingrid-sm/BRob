from __future__ import annotations

from typing import List, Optional

from fastapi import FastAPI
from fastapi import HTTPException
from pydantic import BaseModel

from local_config import CAMERA_BASE_URL, PHOTO_MODE, USE_FLASH
from simple_target_finder import (
    Detection,
    SimpleTargetFinder,
    build_output_path,
    describe_best_detection,
    draw_detections,
    fetch_camera_image,
    split_labels,
)


app = FastAPI(title="BRob Beginner Vision API", version="1.0.0")

finder: Optional[SimpleTargetFinder] = None


class FindRequest(BaseModel):
    target: str
    save_debug_image: bool = True


class DetectionResponse(BaseModel):
    label: str
    score: float
    box_xyxy: List[int]


class FindResponse(BaseModel):
    ok: bool
    target: str
    labels: List[str]
    best_detection: Optional[DetectionResponse]
    detections: List[DetectionResponse]
    image_width: int
    image_height: int
    debug_image_path: Optional[str]


@app.on_event("startup")
def startup() -> None:
    global finder
    finder = SimpleTargetFinder()


@app.get("/health")
def health() -> dict:
    return {
        "ok": True,
        "camera_base_url": CAMERA_BASE_URL,
        "photo_mode": PHOTO_MODE,
        "use_flash": USE_FLASH,
        "model_loaded": finder is not None,
    }


def convert_detection(detection: Detection) -> DetectionResponse:
    return DetectionResponse(
        label=detection.label,
        score=detection.score,
        box_xyxy=detection.box_xyxy,
    )


@app.post("/find", response_model=FindResponse)
def find_target(request: FindRequest) -> FindResponse:
    global finder
    assert finder is not None

    labels = split_labels(request.target)
    try:
        image = fetch_camera_image()
    except Exception as error:
        raise HTTPException(status_code=503, detail=f"Camera fetch failed: {error}")
    detections = finder.find(image, labels)

    debug_image_path = None
    if request.save_debug_image:
        debug_image_path = build_output_path(labels)
        draw_detections(image, detections, debug_image_path)

    if detections:
        describe_best_detection(detections[0], image)

    return FindResponse(
        ok=True,
        target=request.target,
        labels=labels,
        best_detection=convert_detection(detections[0]) if detections else None,
        detections=[convert_detection(item) for item in detections],
        image_width=image.width,
        image_height=image.height,
        debug_image_path=debug_image_path,
    )
