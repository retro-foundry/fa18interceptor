# Handoff

For whoever picks this up next. History is in git; this page is the state,
the process, and the traps. Updated 2026-09-30.

## Goal

Recreated, readable C source for the whole game ([PORT.md](PORT.md)). The
translated game runs natively; hand-written C replaces translated routines
one at a time, each proven on every call.

## Numbers

| Check | Result |
| --- | --- |
| Recreated routines (`port/game/`) | 367; 796,515 calls matching in shadow and 1,029,188 in the sandbox pass over three native recordings; poison-clean. Ported parents contain formerly counted nested calls. |
| Native recordings (`captures/native/`) | demo01, qual_carrier_success, qual_fail_crashes; each replays byte-identically under the proof |
| Ready to recreate next | `python tools/recomp/port_candidates.py` |
| run075 frames 393-402 from the frame-392 snapshot | 10/10 exact |
| run060 replay | game RAM identical through frame 93; pixels exact to frame 540; drifts after |
| run062 replay | frame 2475 exact |
| Translated vs interpreter-only, 3,000 frames | identical RAM and registers |

## How to work

The loop, per routine or small group:

1. `python tools/recomp/port_candidates.py -n 40` lists routines whose
   callees are already C, ranked by glue burden. Read the target with
   `python tools/recomp/port_info.py C2005C`: its instructions, its observed
   call sites, and the registers, high words and flags live after it.
2. Check `analysis/routines/` and `analysis/` for a report on the address
   before inventing any meaning or any global name. Most addresses that look
   unnamed are already documented somewhere in `analysis/`.
3. Write the C in the right `port/game/` file, as original source would be
   written (PORT.md, Conventions).
4. Write the glue in `port/game/glue/`: read the inputs from registers and
   memory, call the C, rebuild every live register, flag and high word the
   original leaves, then `glue_return()`. Register it with
   `python tools/recomp/register_ports.py "comment" C2005C=name:cycles`.
5. Build with `sh scripts/build_recomp.sh` and probe with
   `QUICK=1 sh scripts/recomp_ports_check.sh` (first recording only, about
   2.5 minutes).
6. Commit each verified routine or pair as it lands, not once at the end.
7. Before updating any count or calling the batch done, run the **full**
   `sh scripts/recomp_ports_check.sh` over all three recordings and
   `python scripts/recomp_parity.py --start 392 --frames 10`.

Port several routines per full proof run; do not run the full proof per
routine. But see the first trap below: the quick probe is not a substitute
for it.

## Recently ported

All of these are registered and matching over all three recordings:

- `$C10C68` `queue_post_input_context_command` (`stages.c`) - once the
  countdown expires, queues the heading marker and context command, then
  installs the next callback. All 25 recorded calls matched. It now contains
  the two formerly direct heading-formatter calls.
- `$C1C40C` `build_template_bit_gates` (`template_gates.c`) - clears and
  populates three 128-row bit-gate tables from the original signed-relative
  template directories. All three recorded calls matched.
- `$C25070` `refresh_post_input_heading` (`target_heading.c`) - scans the
  post-input record list, transforms the selected record's point, tracks its
  heading and formats the three output digits; both demo calls matched in the
  preceding full proof before `$C10C68` contained them.
- `$C28B34` `dispatch_scene_records` and `$C28AFE`
  `initialize_scene_record` (`scene_dispatch.c`) - copy scene pointers,
  filter entries, create and orient control records, then aim a newly
  created record. The dispatcher matched 15 direct shadow calls in focused
  probes before its parent was registered; the parent matched 14 calls in
  the full proof. The full report lists the direct dispatcher as uncalled
  because those calls are now inside the ported parent.
- `$C3003A` `draw_panel_mark` (`hud_bars.c`) - the panel blit, mark polygon,
  line and individual pixels. Its glue replays the entire chain's register
  effects; 3,133 shadow calls matched across the three recordings.
- `$C11788` `advance_postflight_reset` and `$C11830`
  `restart_postflight_scene` (`stages.c`) - the failure-side callback resets
  the scene, decrements its repeat byte and either schedules another reset
  or clears the render buffers and schedules the failure message. The second
  callback optionally sets view mode zero before placing the scene root.
- `$C1FF0A` `test_stream_face` - three vertex offsets into the clipper input
  and the face test; the 18 bytes that follow are skipped when it passes.
  `$C1FB82`'s dispatch is C as `face_test_passes` (`plane_tests.c`).
- `$C2005C` `draw_tested_face`, `$C20100` `draw_indexed_face_list`,
  `$C20002` `draw_tested_parallelogram` - a vertex offset list (or a
  parallelogram from three offsets), the face test on the kind word, and the
  polygon drawn in the kind's colour or the one that follows. `$C20100` runs
  that face once per offset from a base pointer. They share
  `tested_face_tail` (`draw_stream.c`).
- `$C21060` `draw_quad_list`, `$C20C38`/`$C20C22` `draw_face_grid`
  /`draw_face_grid_plain` - these already had C and glue but were blocked on
  D7's high word. Fixed by replaying **every** face's clipper call rather
  than only the last (`glue_batch57.c`, `glue_batch58.c`).
- `$C1D3F4` `expand_cell_templates` (`control_records.c`) - expands a cell of
  the template table into the per-level record lists.
- `$C28800` `aim_record_at_view` (`control_records.c`) - points a record at
  what a selector word names, then turns it toward that target.
- `$C0924A`/`$C09266`/`$C092A0` `reset_scene_context` /
  `reset_scene_recorder` / `place_scene_root` (`scene_setup.c`) - three entry
  points into one sequence that places the player's record for the scene.

Recreated but **not** registered, each with the blocker written in its glue:

- `$C20A52`/`$C20A40` the two face lattices (`glue_batch58.c`) - they draw a
  second run of faces back from the far edge, and there the winning write of
  D7's high word is an earlier face's. Replaying that face exactly needs the
  BLTSIZE and LINE_LAST_ROW from before *that* draw, and the machine keeps
  only the last (`fa18_bltsize_at_draw_start`). A per-draw log of those two
  is not enough on its own: an intermediate face's replay also reads
  `CLIP_OUTPUT` and `POLY_VERTICES`, which by then hold the last face's data.

## The problem that was just fixed

**Symptom.** Routine after routine ended in a call whose registers the glue
could not rebuild. Calling that callee's glue again would have redrawn or
re-stepped, so the analysis dead-ended after the C was already written.

**Cause.** Most callee glue does the C call and the register rebuild in one
function, so there is no way to get the registers without the work.

**Fix.** Split such a glue into the C call plus a `*_registers()` helper that
any caller's glue can replay on its own. This is what the clipper, the line
and the plotters had always done (`glue_clip.h`, `glue_text.h`); it just had
not been applied further. Split so far, all declared in `glue_text.h`:

- `record_orientation_registers` out of `glue_C2D954` (`glue_batch23.c`)
- `track_direction_registers` out of `glue_C123FA` (`glue_batch41.c`)
- `world_registers` exported from `glue_batch24.c`
- `scene_setup_registers`, `view_mode_zero_registers`, and
  `clear_render_buffers_registers` split from their glue so the postflight
  callbacks can replay their register effects without repeating the work.
- `prepare_polygon_to_row_registers`, `blit_lane_registers`, and
  `mark_polygon_registers` split the mark polygon's drawing chain so the
  panel-mark glue can replay every call's register effects in order.

**Effect.** `$C28800` and the `$C0924A` trio then landed in minutes each
instead of hours. Do the split first when a target ends in a fused callee;
it is much cheaper than re-analysing that callee from every caller.

A helper is safe to replay only if everything left in it is pure or
idempotent. `record_orientation_registers` recomputes the same matrix from
the same angles, so replaying it is harmless; `set_record_orientation`, the
part that is not, stays on the work side.

## Traps, and how to avoid them

1. **`QUICK=1` runs one recording only.** It has twice passed a routine the
   full run then rejected (`$C20A40`, `$C092A0`; the latter is never called
   in demo01 at all). Use it to iterate, never to conclude. Nothing is done
   until the full three-recording proof passes.
2. **A mismatch only in a register's high word means look backwards.**
   `SET_W` preserves the upper half, so a high word can carry from far
   earlier than the routine's own code - through callees, or from the
   caller. Most mismatches this session were exactly this. Find the last
   full-width write to that register and reproduce it.
3. **`MOVEQ` and `MOVEM.W` write whole registers.** `MOVEQ #0,D6` at the top
   of `$C1D4E4` is why D6 after it is that routine's search bound, not the
   bitmap offset its caller had just computed in D6 - 1,648 mismatches came
   from assuming otherwise. `MOVEM.W` into data registers sign-extends.
4. **Do not `call_port` a callee whose glue does work.** It will draw or step
   twice. Split it (see above) or leave the routine unregistered with the
   reason written down.
5. **Transcribe control flow literally, including what looks like a bug.**
   `$C0924A`'s retry path does not put its record pointer back, so a retry
   writes its pose through the record the rejected entry had named. The C
   keeps that.
6. **Check `analysis/` before naming a global.** Nine addresses in the scene
   setup looked undocumented; `analysis/routines/c093be_positive_scene_pose.md`
   and `c09498_negative_scene_pose.md` named and bounded almost all of them.
   Every entry in `globals.h` carries evidence - keep it that way.
7. **Verify your edits landed.** A silent string-replace failure left one
   commit claiming two glue splits when only one had applied. When scripting
   edits, assert the pattern was found before writing, and re-read the file.
8. **`$C0004E`, `$C000B4`, `$C000BA` are trampolines into Kickstart**, not
   game logic. They would raise the routine count without recreating any
   source. Leave them.

## Next

0. **Record native sessions** (the user plays): menu, demo, a normal
   flight, a success, a crash, a failure, the post-flight screens.
   `build/recomp-cmake/Release/fa18_recomp.exe --state captures/uae/run075/restored-state.bin
   --rom local/system/kick13.rom --window --frames 0 --record local/NAME.fa18in`,
   then `python scripts/seal_native_run.py NAME --state ... --input local/NAME.fa18in`.
   Once some exist the check uses only them; re-record after any change to
   machine timing or the translation (the check then says the run no
   longer ends as sealed).
1. **Keep recreating routines**, bottom-up from `port_candidates.py`.
   Cheapest first, given the splits that now exist: `$C25070` and `$C28B34`
   both end in `$C2D954` and can use `record_orientation_registers` as it
   stands. After that, split `$C2E758`'s glue to unblock the
   `$C0D74A`/`$C0D752` pair.
   Sibling routines are worth seeking out: `$C0924A`/`$C09266`/`$C092A0` are
   three entry points into one body, and the grid and lattice pairs share
   one implementation, so one transcription registers several routines.
2. **Exact UAE timing (dropped for now).** Native recordings make the port
   independent of UAE replays; the bus model stays as it is. Tools:
   `scripts/recomp_timing.py` (per instruction against a trace),
   `scripts/recomp_state_diff.py` (first frame where game RAM differs),
   `scripts/recomp_outcome.py` (pixels at chosen frames).
3. **Kickstart calls.** Inventory, then replace with C (PORT.md stage E).
   Reference for the shim: the Amiga Developer CD v2.1 at `D:\amiga-dev`
   (outside the repo, on this machine). Its includes, autodocs and FD/LVO
   files give each library call's offset, registers and behaviour, which is
   what a C shim for the game's Kickstart calls needs.

### Where the rest of the port stands

The polygon path is C from the faces (`$C09952`, `$C099F6`) down through the
clipper and `draw_polygon`; the clipper's register replay is reusable for its
other callers (`port/game/glue/glue_clip.h`). The cockpit and HUD are mostly
C (`hud_readouts.c`, `hud_bars.c`, `hud_marks.c`, `message_line.c`,
`postflight_hud.c`). The HUD stage's parts `$C33370` (tapes), `$C33B38`
(status mark) and `$C33CD2` (transform) are registered; `$C33370`'s glue
replays every step in order (the tape forms, record type $10, only
approximately: the recordings never show them). `$C332BC` is registered: its
glue runs each step's recreated routine in order, so `draw_postflight_hud`'s
C itself is a transcription that does not run under the proof. The remaining
HUD work is the radar `$C31226`. The stores icons `$C30A00`/`$C30AE2` are C;
their glue preserves stray high bits of the caller's D4. Glue helpers for
routines that end in drawing are in `glue_text.h`: the small-text line is
probed before the C (the last glyph's cell) and replayed after it; pixel,
line and blit replays read only the plot state, so they run after the C in
order, with `CURRENT_COLOUR` set to the value then in force. `$C1E328`
(display-list sort) is registered; its glue reconstructs the stack byte read
by the sort after its entry MOVEM has overwritten the caller's stack slot.
`$C2F1C0` (filled circle), `$C2EC90` and its projection variants, `$C0CF98`
(scaled circle stream), `$C12098` (view controls) and `$C1B27E` (recorder
playback and input ramps) are registered. The recorder's end marker advances
its restarted byte cursor by one; its throttle hold path clears the
function-key level. `$C13176` was compared in the sandbox pass; `$C3316E` was
called but not compared in these recordings.

## Commands

```sh
sh scripts/build_recomp.sh                                 # headless build
QUICK=1 sh scripts/recomp_ports_check.sh                   # first recording only (probe)
sh scripts/recomp_ports_check.sh                           # shadow + sandbox + poison proof
python scripts/recomp_parity.py --start 392 --frames 10    # parity
python tools/recomp/port_candidates.py -n 40               # what to port next
python tools/recomp/port_info.py C2FA7E                    # what a routine needs
python tools/recomp/register_ports.py "comment" C2005C=name:cycles
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
- A recreated routine is done only when the full `scripts/recomp_ports_check.sh`
  passes and parity is unchanged. Commit per verified batch.
