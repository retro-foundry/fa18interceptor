#!/bin/sh
# Stage D proof over the recordings:
#  1. SHADOW: every call of every recreated routine is compared with the
#     generated routine run live (live registers and flags, memory outside
#     the dead stack, custom-register writes). Shadow keeps the plain run's
#     timing, so a native recording ends exactly as sealed; that is checked.
#  2. SANDBOX: the same recordings compared the older way, which also covers
#     calls with an interrupt or hardware access inside (audio, joystick).
#  3. POISON (first recording): overwrite everything liveness declares dead
#     after each compared call; the frames must not change.
# The recordings are captures/native/*/ (FA18_LOOP_INPUT_V1). Until there are
# any, the archived Engine9000 runs in captures/uae/ stand in (UAE=1 forces
# them). QUICK=1 runs the first recording only. Fails on any mismatch.
set -e
cd "$(dirname "$0")/.."
EXE=./build/recomp/fa18_recomp.exe
ROM=local/system/kick13.rom
OUT=build/recomp
rm -f $OUT/ports_report_*.json

# One line per recording: name, then the runner's input arguments.
RUNS=""
if [ -z "${UAE:-}" ]; then
  for d in captures/native/*/; do
    [ -f "$d/input.fa18in" ] || continue
    n=$(basename "$d")
    RUNS="$RUNS$n|--state $d/state.bin --input $d/input.fa18in --to-end
"
  done
fi
if [ -z "$RUNS" ]; then
  echo "(no native recordings: using the archived Engine9000 runs)"
  RUNS="run075|--state captures/uae/run075/restored-state.bin --replay captures/uae/run075/playback.e9k --frames ${FRAMES:-3000}
run024|--state captures/uae/run024/initial_state.bin --replay captures/uae/run024/playback.e9k --frames 27437
run060|--state captures/uae/run060/restored-state.bin --replay captures/uae/run060/playback.e9k --frames 10085
run062|--state captures/uae/run062/restored-state.bin --replay captures/uae/run062/playback.e9k --frames 2470"
fi
[ -n "${QUICK:-}" ] && RUNS=$(printf '%s\n' "$RUNS" | head -1)

FIRST=""
printf '%s\n' "$RUNS" | while IFS='|' read -r run args; do
  [ -n "$run" ] || continue
  $EXE $args --rom $ROM --ports shadow --ports-report "$OUT/ports_report_$run.json" \
    --rgb444 "$OUT/frames_shadow_$run.bin" --ram-out "$OUT/ram_shadow_$run.bin" >/dev/null
  $EXE $args --rom $ROM --ports sandbox --ports-report "$OUT/ports_report_sandbox_$run.json" >/dev/null
  if [ -f "captures/native/$run/run.json" ]; then
    python -c "
import hashlib, json, sys
want = json.load(open('captures/native/$run/run.json'))['replay']['final_ram_sha256']
got = hashlib.sha256(open('$OUT/ram_shadow_$run.bin', 'rb').read()).hexdigest()
if got != want: sys.exit('$run: the shadow run does not end as sealed (machine or translation changed?)')
"
  fi
done

FIRST=$(printf '%s\n' "$RUNS" | head -1)
run=${FIRST%%|*}; args=${FIRST#*|}
$EXE $args --rom $ROM --ports shadow --poison --rgb444 $OUT/frames_poison.bin >/dev/null 2>&1
cmp -s "$OUT/frames_shadow_$run.bin" $OUT/frames_poison.bin || {
  echo "POISON: frames differ: a register or flag declared dead is read"; exit 1; }
cp "$OUT/ports_report_$run.json" $OUT/ports_report.json
python -c "
import glob, json
def load(pattern):
    return {p.split('ports_report_')[1][:-5]: {r['entry']: r for r in json.load(open(p))}
            for p in sorted(glob.glob('$OUT/ports_report_' + pattern))}
shadow = {k: v for k, v in load('*.json').items() if not k.startswith('sandbox_')}
sandbox = load('sandbox_*.json')
first = next(iter(shadow.values()))
total = 0
for entry, r in first.items():
    cells = []
    for run, rows in shadow.items():
        x = rows.get(entry, {'matched': 0, 'mismatched': 0})
        total += x['matched']
        cells.append(f\"{run} {x['matched']:>7}\" + ('!' if x['mismatched'] else ' '))
    print(f\"{entry} {r['name']:30} \" + '  '.join(cells))
def compared(runs, e): return sum(rows.get(e, {}).get('matched', 0) for rows in runs.values())
never = [r['name'] for e, r in first.items() if not any(rows.get(e, {}).get('calls') for rows in shadow.values())]
only_sandbox = [r['name'] for e, r in first.items() if not compared(shadow, e) and compared(sandbox, e)]
unproven = [r['name'] for e, r in first.items() if not compared(shadow, e) and not compared(sandbox, e)]
if never: print('never called:', ', '.join(never))
if only_sandbox: print('compared only in the sandbox pass:', ', '.join(only_sandbox))
if unproven: print('never compared:', ', '.join(unproven))
print(f'{len(first)} routines, {total} matching calls over {len(shadow)} recordings '
      f'(+{sum(compared(sandbox, e) for e in first)} in the sandbox pass); poison: frames identical')
"
