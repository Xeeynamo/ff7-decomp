#!/bin/bash
# Optimized Docker build wrapper for FF7 decomp
# This script adds Go caching for 3x faster builds

set -e

# Create Go cache directories if they don't exist
mkdir -p ~/.cache/ff7-go-mod ~/.cache/ff7-go-build

# Run Docker with all necessary volume mounts including Go caches
exec docker run --rm --platform=linux/amd64 \
    -v "$(pwd)":/ff7 \
    -v ff7_venv:/ff7/.venv \
    -v "$(pwd)/build":/ff7/build \
    -v "$HOME/.cache/ff7-go-mod":/gocache/mod \
    -v "$HOME/.cache/ff7-go-build":/gocache/build \
    -e "GOMODCACHE=/gocache/mod" \
    -e "GOCACHE=/gocache/build" \
    ff7-build:latest -lc "cd /ff7 && $*"
