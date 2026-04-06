#!/bin/zsh

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"

cd "$SCRIPT_DIR"

python3 -m uvicorn vision_api:app --host 0.0.0.0 --port 8001
