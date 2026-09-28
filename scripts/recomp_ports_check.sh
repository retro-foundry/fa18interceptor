#!/bin/sh
# Stage D proof: run run075 from the menu for 3,000 frames in SHADOW mode.
# Every call to every recreated routine is compared with the generated
# routine (registers, flags, memory, custom-register writes). Fails on any
# mismatch; writes build/recomp/ports_report.json.
set -e
cd "$(dirname "$0")/.."
./build/recomp/fa18_recomp.exe --state captures/run075/restored-state.bin \
  --rom local/system/kick13.rom --replay captures/run075/playback.e9k \
  --frames "${FRAMES:-3000}" --ports shadow --ports-report build/recomp/ports_report.json >/dev/null
python -c "
import json
rows = json.load(open('build/recomp/ports_report.json'))
for r in rows:
    print(f\"{r['entry']} {r['name']:32} calls {r['calls']:>8} matched {r['matched']:>8} \"
          f\"mismatched {r['mismatched']} unchecked {r['hardware'] + r['incomplete']}\")
never = [r['name'] for r in rows if not r['calls']]
if never: print('never called in this run:', ', '.join(never))
"
