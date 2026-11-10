#!/usr/bin/env sh
# Generated with OpenAI Codex. Requires a desktop display or xvfb-run.
set -eu
cd "$(dirname "$0")/.."
mkdir -p build
for body in 0 1 2 3 4; do
 ./build/heliograph --near --body "$body" --frames 40 --capture "build/planet-$body.png" > "build/planet-$body.log" 2>&1
 if grep -E 'WARNING:|ERROR:' "build/planet-$body.log"; then exit 1; fi
 done
./build/heliograph --smoke --capture build/transit-arrival.png > build/smoke.log 2>&1
grep -E 'RUN:|WARM:|SMOKE' build/smoke.log
if grep -E 'WARNING:|ERROR:' build/smoke.log; then exit 1; fi
