#!/usr/bin/env sh
# Generated with OpenAI Codex. CPU captures, no display needed.
set -eu
cd "$(dirname "$0")/.."
mkdir -p build
for body in 0 1 2 3 4; do
 ./build/heliograph --headless --near --body "$body" --frames 3 --capture "build/cpu-$body.ppm"
done
./build/heliograph --headless --smoke --capture build/transit.ppm
