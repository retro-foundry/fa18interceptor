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
| Recreated routines (`port/game/`) | 378; 731,823 calls matching in shadow and 920,240 in the sandbox pass over three native recordings; poison-clean. Ported parents contain formerly counted nested calls. |
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

`$C0D74A`/`$C0D752` (`display_records.c`, `276be504`) now share the complete
four-candidate matrix preparation, corner projection, and seven-way record
selection body. Across the three native recordings they matched 6,129
completed shadow calls and 10,857 sandbox calls. The full proof and 10-frame
parity check passed. Their nested `$C2E758` register replay was separated from
its C operation so the parents run the projection once. The original extended
branches also exposed a count/offset error in the older isolated selector C;
`c43e2d3e` corrects it and its source-backed contract case passes.

`$C2374C` `consume_selected_fire_request` (`selected_fire.c`, `2bdb54d5`),
`$C28722` `initialize_scene_from_mode` (`scene_dispatch.c`, `f1937a36`), and
`$C0FAA4` `initialize_scene_state` (`stages.c`, `e68c7d7d`) are committed and
included in the Numbers row. The scene selector handles normal streams and
the `$7D` special record; the initializer runs it, root setup, message reset,
and final setup in source order. The selector matched eight completed shadow
calls in focused probes; the initializer matched its completed demo call and
both demo sandbox calls. The full three-recording proof and 10-frame parity
check passed on this batch.

`$C0FA04` `finish_post_input_followup` (`stages.c`, `679ef67b`) is committed
and included in the Numbers row. Its completed demo shadow call and all eight
demo sandbox calls matched, including the seven buffer clears. The full
three-recording proof and 10-frame parity check passed. Its parent `$C0F992`
still depends on the untranslated `$C08F26` cold-scene bootstrap; that stage
also calls `$C1C63E` and `$C1C860`, so work further down that chain is needed
before the parent can become C.

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
   `$C2FD8C` has an inactive C draft in `port/game/active_planes.c` and
   `glue/glue_active_planes.c`. It submits four
   active cockpit planes, then runs `$C0D752`, the direct `$C301F6` polygon
   submission and optional `$C30466` composite, followed by `$C0D74A` and
   a 22-byte record copy or clear. Its blitter waits and busy-poll counters
   need source-accurate C. A demo01 shadow probe found five counted-wait
   mismatches: the shadow runner holds custom-register writes during the C
   trial, so its C blits never become busy. A targeted demo01 sandbox run
   compared 2,078 calls with zero mismatches, but the sandbox also holds
   custom writes. A live `--ports on --ports-only C2FD8C` comparison against
   `--ports off` first differs in RGB at frame 297, even when the cycle charge
   is lowered from 65,000 to the sandbox mean of 24,025. The draft is
   deliberately absent from `ports.c`; resolve live write/busy timing and
   the provisional 56-cycle poll in `fa18_machine_count_blitter_polls` before
   registering it. Evidence is in
   `analysis/routines/c2fd8c_first_active_plane_submission.md`,
   `c2fdf4_remaining_active_plane_submissions.md`, and
   `c2fede_selected_table_display_stage.md`; the isolated orchestration is
   `port/selected_table_display_stage.c`.
   `$C13D84` is now registered (`indexed_record_update.c`,
   `glue_indexed_record_update.c`). Its indexed selection, phase and control
   updates, signed response routes, damping, child `$C26428` call and final
   +$6E result are in C. The glue replays the child's live register effects.
   Focused proof over the three native recordings matched 4,501 completed
   shadow calls and 4,636 sandbox calls with zero mismatches; 120 shadow
   calls were interrupted before comparison. The full 378-routine gate passed:
   731,823 shadow matches, 920,240 sandbox matches and identical poison
   frames. All 10 parity frames 393-402 remained pixel-exact. `$C26EBE` is
   the next ready large candidate. Its bounded opening scan through
   `$C270AA` is drafted in `candidate_record_scan.c`: class and header
   filters, two three-axis distance bounds, and the immediate record writes.
   It is inactive and has only a strict C syntax check; the later geometry
   branches still need porting before registration and proof. The side-result
   and signed terminal-height blocks `$C278D6-$C279C6` are drafted in the
   same file; the central geometry and level/volume walk remain.
   `$C2D408` is now registered (`record_matrix_update.c`,
   `glue_record_matrix_update.c`). The class-$30 tracking route, nonclass
   velocity/depth paths and post-transform orientation are C. Focused proof
   over all three native recordings matched 3,975 completed shadow calls and
   4,877 sandbox calls with zero mismatches. The full 377-routine gate passed:
   736,396 matching shadow calls, 927,924 sandbox matches and identical
   poison frames. All 10 parity frames 393-402 were pixel-exact.
   `$C2DEE0` is now registered (`matrix.c`, `glue_transform_matrix.c`). It
   builds the nine-long signed product, converts it through the original
   table/division angle branches, and returns the three shifted angles with
   the caller-visible registers. Focused proof across the three native runs:
   5,114 completed shadow matches and 5,328 sandbox matches, zero mismatches.
   `$C2DB18` is now registered (`matrix_route.c`, `glue_matrix_route.c`). It
   selects the active record's angle tuple, publishes the transformed angles,
   and builds the final row-scaled matrix. The full targeted proof over the
   three native runs matched 3,353 completed shadow calls and 3,353 sandbox
   calls, zero mismatches. The full combined gate passed with 375 routines,
   754,388 matching shadow calls and 962,808 sandbox calls, with identical
   poison frames. All 10 parity frames 393-402 were pixel-exact.
   `$C2D99C` is now registered as the selector for the two matrix routes.
   Focused proof over all three native recordings matched 4,082 completed
   shadow calls and 5,810 sandbox calls with zero mismatches. The summary
   above remains the last full combined gate; include this selector in the
   next larger batch gate.
   The post-input parent `$C0F992` depends on `$C08F26`; its deeper
   `$C1C63E`/`$C1C860` calls are still translated.
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
