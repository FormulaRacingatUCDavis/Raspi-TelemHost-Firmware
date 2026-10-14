#!/bin/bash

REPO_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

cleanup() {
    kill "$PID" 2>/dev/null
    wait "$PID" 2>/dev/null
    mv "$REPO_DIR/data/can/intake"/candump-*.log "$REPO_DIR/data/can/process/" 2>/dev/null
}

trap cleanup EXIT
trap 'exit 0' TERM INT

cd "$REPO_DIR/data/can/intake" || exit 1

candump -l vcan0 vcan1 &
PID=$!

wait "$PID"