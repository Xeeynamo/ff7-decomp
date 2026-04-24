#!/bin/bash
# Optimized Docker build wrapper for FF7 decomp
# Uses Docker volumes for Go caching (faster on macOS than host mounts)

set -e

# Run Docker with volume mounts for source, venv, build output, and Go cache
exec docker run --rm --platform=linux/amd64 \
    --name ff7-work \
    -v "$(pwd)":/ff7 \
    -v ff7_venv:/ff7/.venv \
    -v ff7_build:/ff7/build \
    -v go_cache:/gocache \
    ff7-build:latest -lc "cd /ff7 && $*"
