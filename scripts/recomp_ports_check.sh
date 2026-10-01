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
# The recordings are captures/native/*/ (FA18_LOOP_INPUT_V1). Archived
# Engine9000 captures are not a proof fallback. QUICK=1 probes the first
# recording only. Fails on any mismatch.
set -e
cd "$(dirname "$0")/.."
EXE=./build/recomp/fa18_recomp.exe
ROM=local/system/kick13.rom
OUT=build/recomp
POISON="$OUT/frames_poison.bin"
cleanup_frames() {
  rm -f "$POISON" "$OUT"/frames_shadow_*.bin
}
trap cleanup_frames EXIT HUP INT TERM

# One line per recording: name, then the runner's input arguments.
RUNS=""
for d in captures/native/*/; do
  [ -f "$d/input.fa18in" ] || continue
  n=$(basename "$d")
  RUNS="$RUNS$n|--state $d/state.bin --input $d/input.fa18in --to-end
"
done
if [ -z "$RUNS" ]; then
  echo "no sealed native recordings in captures/native/" >&2
  exit 1
fi
[ -n "${QUICK:-}" ] && RUNS=$(printf '%s\n' "$RUNS" | head -1)
rm -f "$OUT"/ports_report_*.json
FIRST=$(printf '%s\n' "$RUNS" | head -1)
FIRST_RUN=${FIRST%%|*}

run_one() {
  run=$1
  args=$2
  rgb_args=""
  [ "$run" = "$FIRST_RUN" ] && rgb_args="--rgb444 $OUT/frames_shadow_$run.bin"
  $EXE $args --rom $ROM --ports shadow --ports-report "$OUT/ports_report_$run.json" \
    $rgb_args --ram-out "$OUT/ram_shadow_$run.bin" >/dev/null
  $EXE $args --rom $ROM --ports sandbox --ports-report "$OUT/ports_report_sandbox_$run.json" >/dev/null
  if [ -f "captures/native/$run/run.json" ]; then
    python -c "
import hashlib, json, sys
want = json.load(open('captures/native/$run/run.json'))['replay']['final_ram_sha256']
got = hashlib.sha256(open('$OUT/ram_shadow_$run.bin', 'rb').read()).hexdigest()
if got != want: sys.exit('$run: the shadow run does not end as sealed (machine or translation changed?)')
"
  fi
}

# Recordings are independent and write disjoint outputs. Run them on separate
# cores; QUICK still launches only its single selected recording.
run_all() {
  pids=""
  while IFS='|' read -r run args; do
    [ -n "$run" ] || continue
    run_one "$run" "$args" &
    pids="$pids $!"
  done
  status=0
  for pid in $pids; do
    if ! wait "$pid"; then status=1; fi
  done
  return "$status"
}
printf '%s\n' "$RUNS" | run_all

run=${FIRST%%|*}; args=${FIRST#*|}
$EXE $args --rom $ROM --ports shadow --poison --rgb444 "$POISON" >/dev/null 2>&1
cmp -s "$OUT/frames_shadow_$run.bin" "$POISON" || {
  echo "POISON: frames differ: a register or flag declared dead is read"; exit 1; }
rm -f "$POISON"
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
python scripts/prune_build_artifacts.py --quiet
