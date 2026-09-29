# Handoff

One page. History is in git. Updated 2026-09-29.

## Goal

Recreated, readable C source for the whole game ([PORT.md](PORT.md)). The
translated game runs natively; hand-written C replaces translated routines
one at a time, each proven on every call.

## Numbers

| Check | Result |
| --- | --- |
| Recreated routines (`port/game/`) | 347; 850,259 calls matching in shadow and 1,019,096 in the sandbox pass over three native recordings; poison-clean |
| Native recordings (`captures/native/`) | demo01, qual_carrier_success, qual_fail_crashes; each replays byte-identically under the proof |
| Ready to recreate next | `python tools/recomp/port_candidates.py` |
| run075 frames 393-402 from the frame-392 snapshot | 10/10 exact |
| run060 replay | game RAM identical through frame 93; pixels exact to frame 540; drifts after |
| run062 replay | frame 2475 exact |
| Translated vs interpreter-only, 3,000 frames | identical RAM and registers |

## Next

0. **Record native sessions** (the user plays): menu, demo, a normal
   flight, a success, a crash, a failure, the post-flight screens.
   `build/recomp-cmake/Release/fa18_recomp.exe --state captures/uae/run075/restored-state.bin
   --rom local/system/kick13.rom --window --frames 0 --record local/NAME.fa18in`,
   then `python scripts/seal_native_run.py NAME --state ... --input local/NAME.fa18in`.
   Once some exist the check uses only them; re-record after any change to
   machine timing or the translation (the check then says the run no
   longer ends as sealed).
1. **Keep recreating routines.** Work bottom-up from `port_candidates.py`,
   following PORT.md, "Recreating a routine". The polygon path is C from
   the faces (`$C09952`, `$C099F6`) down through the clipper and
   `draw_polygon`; the clipper's register replay is reusable for its other
   callers (`port/game/glue/glue_clip.h`). The cockpit and HUD are now
   mostly C (`hud_readouts.c`, `hud_bars.c`, `hud_marks.c`,
   `message_line.c`, `postflight_hud.c`). The HUD stage's parts `$C33370`
   (tapes), `$C33B38` (status mark) and `$C33CD2` (transform) are registered;
   `$C33370`'s glue replays every step in order (the tape forms, record type
   $10, only approximately: the recordings never show them). `$C332BC` is registered: its glue runs
   each step's recreated routine in order, so draw_postflight_hud's C itself
   is a transcription that does not run under the proof. The remaining
   HUD work is the radar `$C31226`. The stores icons `$C30A00`/`$C30AE2`
   are now C; their glue preserves stray high bits of the caller's D4. Glue helpers
   for routines that end in drawing are in `glue_text.h`: the small-text
   line is probed before the C (the last glyph's cell) and replayed after
   it; pixel, line and blit replays read only the plot state, so they run
   after the C in order, with CURRENT_COLOUR set to the value then in force.
   `$C1E328` (display-list sort) is registered. Its glue reconstructs the
   stack byte read by the sort after its entry MOVEM has overwritten the
   caller's stack slot.
   `$C2F1C0` (filled circle), `$C2EC90` and its projection variants,
   `$C0CF98` (scaled circle stream), `$C12098` (view controls), and
   `$C1B27E` (recorder playback and input ramps) are now registered. The
   recorder's end marker advances its restarted byte cursor by one. Its
   throttle hold path clears the function-key level. `$C13176` was compared
   in the sandbox pass; `$C3316E` was called but not compared in these
   recordings.
   `$C1FB82` (backface dispatch) is registered. Face loops ($C21060, $C20C38, $C20C22, $C20A52, $C20A40) have C and glue
   but stay unregistered: the dispatcher at $C1F942 returns to callers whose
   liveness is unknown, so D7's high word counts as live, and it comes from
   the previous face's draw. The glue replays only the last face (BLTSIZE
   before it is kept by the machine, fa18_bltsize_at_draw_start). Next step:
   the dispatcher's callers' liveness, or a replay of every face. The `$C0004E` family are stack
   trampolines, not game logic; leave them.
2. **Exact UAE timing (dropped for now).** Native recordings make the port
   independent of UAE replays; the bus model stays as it is. Tools: `scripts/recomp_timing.py` (per
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
- The Engine9000 runs in `captures/uae/` are obsolete; keep them read-only.
- No Ghidra on this account. The `pcode/raw/` exports are frozen evidence;
  nothing in the port reads them (REVERSE_ENGINEERING.md, section 4).
- `scripts/check_native_build.py`, `scripts/native_frame_count.py` and
  `port/native_data_allowlist.txt` are user-owned; do not edit them.
- A recreated routine is done only when `scripts/recomp_ports_check.sh`
  passes and parity is unchanged. Commit per verified batch.
