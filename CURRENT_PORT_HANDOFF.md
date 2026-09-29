# Handoff

One page. History is in git. Updated 2026-09-29.

## Goal

Recreated, readable C source for the whole game ([PORT.md](PORT.md)). The
translated game runs natively; hand-written C replaces translated routines
one at a time, each proven on every call.

## Numbers

| Check | Result |
| --- | --- |
| Recreated routines (`port/game/`) | 180; 1,222,313 calls matching over run075, run024, run060 and run062; poison-clean |
| Ready to recreate next | `python tools/recomp/port_candidates.py` |
| run075 frames 393-402 from the frame-392 snapshot | 10/10 exact |
| run060 replay | game RAM identical through frame 93; pixels exact to frame 540; drifts after |
| run062 replay | frame 2475 exact |
| Translated vs interpreter-only, 3,000 frames | identical RAM and registers |

## Next

1. **Keep recreating routines.** Work bottom-up from `port_candidates.py`,
   following PORT.md, "Recreating a routine". The polygon path is C from
   the faces (`$C09952`, `$C099F6`) down through the clipper and
   `draw_polygon`; the clipper's register replay is reusable for its other
   callers (`port/game/glue/glue_clip.h`). Next on the list: `$C11BFC`,
   `$C345A0`, `$C123FA`, `$C27456`. `$C1FB82` (backface predicate) is
   postponed until its callers are C. The `$C0004E` family are stack
   trampolines, not game logic; leave them.
2. **Exact replay timing (deferred).** Bus timing is modelled (STATUS.md,
   "Bus timing"). For end-to-end replays, port UAE's cycle-exact 68000 and
   blitter/DMA arbitration. Tools: `scripts/recomp_timing.py` (per
   instruction against a trace), `scripts/recomp_state_diff.py` (first frame
   where game RAM differs), `scripts/recomp_outcome.py` (pixels at chosen
   frames).
3. **Kickstart calls.** Inventory, then replace with C (PORT.md stage E).

## Commands

```sh
sh scripts/build_recomp.sh                                 # headless build
sh scripts/recomp_ports_check.sh                           # shadow + poison proof
python scripts/recomp_parity.py --start 392 --frames 10    # parity
python tools/recomp/port_candidates.py -n 40               # what to port next
python tools/recomp/port_info.py C2FA7E                    # what a routine needs
python tools/recomp/recomp.py --trace build/recomp/run075_f392_trace10/trace.jsonl \
    --seeds build/recomp/fallback_3000.json --edges port/recomp/generated/recomp_edges.json
cmake -S port/recomp -B build/recomp-cmake && cmake --build build/recomp-cmake --config Release
```

Snapshots and traces come from `scripts/engine9000_bridge.py`
(`--frames N`, `--trace-frames N`); oracle frames are cached in
`build/recomp/oracle/`. Regenerating the translation needs the frame-392
snapshot in `build/recomp/run075_f392/`.

## Standing rules

- Never present an emulator frame as native output.
- No Ghidra on this account. The `pcode/raw/` exports are frozen evidence;
  nothing in the port reads them (REVERSE_ENGINEERING.md, section 4).
- `scripts/check_native_build.py`, `scripts/native_frame_count.py` and
  `port/native_data_allowlist.txt` are user-owned; do not edit them.
- A recreated routine is done only when `scripts/recomp_ports_check.sh`
  passes and parity is unchanged. Commit per verified batch.
