#!/bin/sh
# Stage D proof over the sealed recordings:
#  1. SHADOW: every call of every recreated routine is compared with the
#     generated routine (live registers and flags, memory outside the dead
#     stack, custom-register writes), in each scenario below.
#  2. POISON (run075): overwrite everything liveness declares dead after each
#     compared call; the frames must not change.
# QUICK=1 runs run075 only. Fails on any mismatch.
set -e
cd "$(dirname "$0")/.."
EXE=./build/recomp/fa18_recomp.exe
ROM=local/system/kick13.rom
# name, start state, frames (the recordings are sealed; see analysis/qualification_run_audit.md)
SCENARIOS="run075:restored-state.bin:${FRAMES:-3000}
run024:initial_state.bin:27437
run060:restored-state.bin:10085
run062:restored-state.bin:2470"
[ -n "${QUICK:-}" ] && SCENARIOS="run075:restored-state.bin:${FRAMES:-3000}"

rm -f build/recomp/ports_report_*.json
for s in $SCENARIOS; do
  run=${s%%:*}; rest=${s#*:}; state=${rest%%:*}; frames=${rest#*:}
  [ -f "captures/$run/$state" ] || { echo "skip $run (not present)"; continue; }
  extra=""
  [ "$run" = run075 ] && extra="--rgb444 build/recomp/frames_shadow.bin"
  $EXE --state "captures/$run/$state" --rom $ROM --replay "captures/$run/playback.e9k" --frames "$frames" \
    --ports shadow --ports-report "build/recomp/ports_report_$run.json" $extra >/dev/null
done
$EXE --state captures/run075/restored-state.bin --rom $ROM --replay captures/run075/playback.e9k \
  --frames "${FRAMES:-3000}" --ports shadow --poison --rgb444 build/recomp/frames_poison.bin >/dev/null 2>&1
cmp -s build/recomp/frames_shadow.bin build/recomp/frames_poison.bin || {
  echo "POISON: frames differ: a register or flag declared dead is read"; exit 1; }
cp build/recomp/ports_report_run075.json build/recomp/ports_report.json
python -c "
import glob, json
runs = {p.split('ports_report_')[1][:-5]: {r['entry']: r for r in json.load(open(p))}
        for p in sorted(glob.glob('build/recomp/ports_report_run*.json'))}
first = next(iter(runs.values()))
total = 0
for entry, r in first.items():
    cells = []
    for run, rows in runs.items():
        x = rows.get(entry, {'calls': 0, 'matched': 0, 'mismatched': 0})
        total += x['matched']
        cells.append(f\"{run} {x['matched']:>7}\" + ('!' if x['mismatched'] else ' '))
    print(f\"{entry} {r['name']:30} \" + '  '.join(cells))
never = [r['name'] for e, r in first.items() if not any(rows.get(e, {}).get('calls') for rows in runs.values())]
if never: print('never called:', ', '.join(never))
print(f'{len(first)} routines, {total} matching calls over {len(runs)} recordings; poison: frames identical')
"
