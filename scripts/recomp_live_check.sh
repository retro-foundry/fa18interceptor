#!/bin/sh
# Compare live recreated-C drawing/RGB444 with fresh source (`--ports off`),
# ignoring only source-table Copper fade with unchanged palette indices.
# streams and check the sealed final RAM during the same ON replay. Recordings
# run independently in parallel; temporary outputs are always removed.
# Set PORTS_ONLY to validate an isolated registered batch.
set -e
cd "$(dirname "$0")/.."
EXE=./build/recomp/fa18_recomp.exe
ROM=local/system/kick13.rom
OUT=build/recomp

cleanup() {
  rm -f "$OUT"/frames_off_check_*.bin "$OUT"/frames_on_check_*.bin
  rm -f "$OUT"/ram_on_check_*.bin
  rm -f "$OUT"/ram_off_check_*.bin "$OUT"/indices_off_check_*.bin "$OUT"/indices_on_check_*.bin
}
trap cleanup EXIT HUP INT TERM

RUNS=""
for d in captures/native/*/; do
  [ -f "$d/input.fa18in" ] || continue
  [ -f "$d/run.json" ] || {
    echo "missing seal: $d/run.json" >&2
    exit 1
  }
  n=$(basename "$d")
  RUNS="$RUNS$n|--state $d/state.bin --input $d/input.fa18in --to-end
"
done
if [ -z "$RUNS" ]; then
  echo "no sealed native recordings in captures/native/" >&2
  exit 1
fi
[ -n "${QUICK:-}" ] && RUNS=$(printf '%s\n' "$RUNS" | head -1)

run_one() {
  run=$1
  args=$2
  reference="$OUT/frames_off_check_$run.bin"
  actual="$OUT/frames_on_check_$run.bin"
  ram="$OUT/ram_on_check_$run.bin"
  reference_ram="$OUT/ram_off_check_$run.bin"
  reference_indices="$OUT/indices_off_check_$run.bin"
  actual_indices="$OUT/indices_on_check_$run.bin"
  only_args=""
  [ -n "${PORTS_ONLY:-}" ] && only_args="--ports-only $PORTS_ONLY"
  $EXE $args --rom $ROM --ports off --rgb444 "$reference" --index8 "$reference_indices" --ram-out "$reference_ram" >/dev/null
  $EXE $args --rom $ROM --ports on $only_args --rgb444 "$actual" --index8 "$actual_indices" --ram-out "$ram" >/dev/null
  python scripts/compare_recomp_frames.py "$reference" "$actual" "$reference_indices" "$actual_indices" --reference-ram "$reference_ram" || return 1
  python - "$run" "$ram" <<'PY'
import hashlib, json, sys
from pathlib import Path
name, ram_path = sys.argv[1:]
seal = json.loads((Path("captures/native") / name / "run.json").read_text())
want = seal["replay"]["final_ram_sha256"]
got = hashlib.sha256(Path(ram_path).read_bytes()).hexdigest()
if got != want:
    sys.exit(f"{name}: live ON final RAM differs from the sealed recording ({got} != {want})")
PY
  rm -f "$reference" "$actual" "$ram" "$reference_ram" "$reference_indices" "$actual_indices"
  echo "$run: live ON frames match source OFF with Copper fade excluded; final RAM matches seal"
}

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
