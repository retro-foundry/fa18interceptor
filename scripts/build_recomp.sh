#!/bin/sh
# Build build/recomp/fa18_recomp.exe (gcc, headless). Dependency-aware objects
# are shared with the structural oracles; the CMake build adds the SDL window.
set -e
cd "$(dirname "$0")/.."
# Replay streams and instruction traces are disposable and can dwarf the
# compiler output. Bound their cache before every headless rebuild.
python scripts/prune_build_artifacts.py --quiet
python scripts/build_recomp.py "$@"
