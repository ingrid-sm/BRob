#!/bin/zsh

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PYTHON_BIN="/Users/ingrid.sm/Desktop/Projects/robot/.venv/bin/python3"

cd "$SCRIPT_DIR"

"$PYTHON_BIN" -m uvicorn vision_api:app --host 0.0.0.0 --port 8001
