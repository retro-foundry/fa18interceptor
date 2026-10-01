#!/bin/sh
# Compare live recreated-C RGB444 output with fresh source (`--ports off`)
# streams. Recordings run independently in parallel; temporary streams are
# always removed. Set PORTS_ONLY to validate an isolated registered batch.
set -e
cd "$(dirname "$0")/.."
EXE=./build/recomp/fa18_recomp.exe
ROM=local/system/kick13.rom
OUT=build/recomp

cleanup() {
  rm -f "$OUT"/frames_off_check_*.bin "$OUT"/frames_on_check_*.bin
}
trap cleanup EXIT HUP INT TERM

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

run_one() {
  run=$1
  args=$2
  reference="$OUT/frames_off_check_$run.bin"
  actual="$OUT/frames_on_check_$run.bin"
  only_args=""
  [ -n "${PORTS_ONLY:-}" ] && only_args="--ports-only $PORTS_ONLY"
  $EXE $args --rom $ROM --ports off --rgb444 "$reference" >/dev/null
  $EXE $args --rom $ROM --ports on $only_args --rgb444 "$actual" >/dev/null
  if ! cmp -s "$reference" "$actual"; then
    echo "$run: live ON RGB444 frames differ from source OFF" >&2
    return 1
  fi
  rm -f "$reference" "$actual"
  echo "$run: live ON RGB444 frames match source OFF"
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
