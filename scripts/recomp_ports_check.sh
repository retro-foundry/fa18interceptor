#!/bin/sh
# Stage D proof, two parts, over run075 from the menu (3,000 frames):
#  1. SHADOW: every call of every recreated routine is compared with the
#     generated routine: live registers and flags (tools/recomp/liveness.py),
#     memory outside the dead stack, and custom-register writes.
#  2. POISON: the same run, overwriting everything the liveness table declares
#     dead after each compared call, must render identical frames.
# Fails on any mismatch; writes build/recomp/ports_report.json.
set -e
cd "$(dirname "$0")/.."
RUN="./build/recomp/fa18_recomp.exe --state captures/run075/restored-state.bin \
  --rom local/system/kick13.rom --replay captures/run075/playback.e9k --frames ${FRAMES:-3000}"
$RUN --ports shadow --ports-report build/recomp/ports_report.json \
  --rgb444 build/recomp/frames_shadow.bin >/dev/null
$RUN --ports shadow --poison --rgb444 build/recomp/frames_poison.bin >/dev/null 2>&1
cmp -s build/recomp/frames_shadow.bin build/recomp/frames_poison.bin || {
  echo "POISON: frames differ: a register or flag declared dead is read"; exit 1; }
python -c "
import json
rows = json.load(open('build/recomp/ports_report.json'))
for r in rows:
    print(f\"{r['entry']} {r['name']:32} calls {r['calls']:>8} matched {r['matched']:>8} \"
          f\"mismatched {r['mismatched']} unchecked {r['hardware'] + r['incomplete']}\")
never = [r['name'] for r in rows if not r['calls']]
if never: print('never called in this run:', ', '.join(never))
print('poison: frames identical')
"
