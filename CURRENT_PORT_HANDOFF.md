# C port handoff

Updated 2026-10-01. This is the current work state. Older notes remain in git
history (the preceding handoff is in commit 44063a95); ignored gate logs may
also remain under build/recomp/.
PORT.md describes the architecture and source conventions.

## Objective and order

Recreate readable C for the whole game, proven against the original source and
sealed native recordings. Work in related batches. The order is Stage D game C,
Stage F native backend, then only the Stage E Kickstart services still needed.
The user explicitly deferred OS work and asked for larger routine batches.
After the C279D0 batch, the latest instruction is to return to game timing
parity. That timing investigation is the immediate priority.

## Verified baseline

- 419 of 624 translated game entries are registered in port/game/glue/ports.c.
  The latest full gate for that registered set matched 703,353 completed shadow
  calls and 1,110,694 sandbox calls across three native recordings, with zero
  mismatches and identical poison frames. Ported parents absorb some formerly
  counted child calls, so the aggregate call totals need not rise monotonically.
  build/recomp/ports_report_*.json describe this 419-entry baseline. GNU and
  MSVC builds pass. In addition to active planes and the earlier audio batch,
  24 registered glyph, input, page, notification, command/audio, buffer,
  polygon, face, postflight and followup entries now use source-timed steps.
  These isolated bridges match fresh source OFF output on all 36,236 frames. The
  combined instruction oracle now matches 6,713 instructions and 214,816 cases
  with DMA contention enabled. See
  analysis/routines/native_c_registered_timing_batch.md.
  The new four-entry map/region batch is independently exact across all
  36,236 live frames. Its 735 instructions also match 23,520 fixtures with
  DMA bus contention enabled. See analysis/routines/native_c_map_region_activation.md.
  C279D0 is also newly registered: its independent live output matches all
  36,236 frames, and its 271 instructions pass 8,672 DMA-contention fixtures.
  Readable whole-call C separately matches 3,258 shadow and 1,967 sandbox calls,
  plus 4,096 structural cases including partial writes and edge clamps. See
  analysis/routines/native_c_grid_projection_activation.md.
  Eleven additional renderer, clipping, record-view and transform entries now
  use source timing and independently match all 36,236 live frames. Their
  1,094 instructions pass 35,008 DMA-contention fixtures. The broader DMA
  oracle also found and removed an extra stack read in C17B08's older timing
  bridge. See analysis/routines/native_c_renderer_timing_batch.md.
  A further 22 sound-start/random and screen-frame/corner entries now use
  source timing and match all 36,236 live frames together. Their 1,026 source
  instructions pass 32,832 DMA fixtures. The sound correction moves the
  combined first difference from frame 297 to frame 416. Isolated ON final
  RAM also matches each recording's sealed SHA-256. See
  analysis/routines/native_c_sound_frame_timing_batch.md.
  Fifteen more polygon, line, projected-segment and cell-template entries now
  use source timing and match every live frame and sealed final RAM together.
  Their 1,062 instructions pass 33,984 DMA fixtures. ALL still first
  differs at frame 416 by 361 pixels.
  See analysis/routines/native_c_drawing_cell_timing_batch.md.
  Twenty additional startup, number-field, orientation and direction-tracking
  entries now match every live frame and sealed final RAM in isolation.
  Their 1,293 instructions pass 41,376 DMA fixtures; 103 registered entries
  now have source timing. Fresh ALL traces match through terrain-refresh
  entry and identify display-list sorting and condition updates as the next
  timing targets. See analysis/routines/native_c_startup_timing_batch.md.
- The current recordings are captures/native/demo01,
  captures/native/qual_carrier_success, and
  captures/native/qual_fail_crashes. Each has state.bin, input.fa18in, and
  run.json with a sealed final RAM hash. Archived captures/uae runs, including
  run075, are historical evidence and are not an acceptance gate.
- The headless GNU and MSVC Release builds pass. The typed port's eight
  affected map/detail contract tests passed with the map source changes.
- Only .vscode/ is untracked; it belongs to the user. Leave it alone.
- C2AA9C, C2AB34, C2AB5A and C2B05A are now registered with resumable source
  timing. The map parent reuses port/map_packet_depth_stage.c; its normal/wide
  children reuse the established map packet core. Their readable whole-call
  glue is checked separately from the instruction steps by
  python tools/recomp/check_map_region_glue.py. This isolated registry variant
  matched 281 shadow and 3,988 sandbox calls without changing the normal runner.
- The region probe additionally matches original registers and RAM on 4,096
  direct structural oracle cases, including signed coordinates, sloped edges,
  endpoint exclusions and randomized high register halves. The oracle found
  and corrected the old C's near-endpoint exit: $C2B1E4/$C2B1FE abandon the
  directory walk, not just one segment. See
  analysis/routines/c2b05a_record_region_probe.md.
- The registered all-native path is not yet frame-faithful. Complete
  source-timed bridges now give exact isolated output for the
  C31226/C3129A/C31312 postflight group and C305AA polygon edge across all three
  sealed recordings. C0FA04 is also source-timed and exact across the three
  recordings. The complete registered ON demo now matches through frame 415
  and first differs at one-based frame 416 by 361 pixels. The preceding
  renderer batch was at frame 297 by 5,440 pixels. Source timing for the
  sound-start/random family removes that early frame difference; the new
  screen-frame/corner family is also exact in isolation through all recordings.
  No fixed mean fees were substituted. The latest 15-entry drawing/cell batch
  resolves the independent C1D3F4/C2FF48/C2EE4A differences at frames 415/416,
  matching all 36,236 frames and sealed final RAM in isolation. ALL retains
  the same frame-416 difference. The remaining bounded ranking finds
  C3201A/C31F4C/C20A40 at frame 424 by 34,144, C26EBE at frame 441 by
  12,238 and C0D04C/C20D68 at frame 484 by 17. The latest 20-entry batch
  removes message-reset, scene-initialization and long-table timing debt:
  fresh startup instruction rows match through C10174 before C1C860 in
  machine frame 296. C1017A after that call is 10,254 cycles early. A fresh
  inner trace first differs at C1C99C after C1E328 (-9,950 cycles), with
  another -310 across C09A78. These observed differences identify complete-
  call debt, not substitute charges or proof of the inner pixel cause.
  See analysis/routines/native_c_startup_timing_batch.md.
  Shadow/sandbox matches do not establish live ON fidelity for the whole
  registered set.
- Plane shadow now replays the live source's ordered DMACONR inputs on saved
  entry RAM. It independently checks native outputs and write sequences, and
  fails extra/reordered/missing reads. All five former counter failures now
  compare and pass. Report fields busy_input_calls/reads expose this proof
  input model. Two intentionally incorrect bridge copies are rejected by
  tools/recomp/check_shadow_busy_inputs.py. Full ON replay separately proves
  the actual timing. No mismatch or hardware classification was suppressed.

## Newly activated grid projection packet

C279D0 now implements the complete three-table packet in
port/game/grid_projection_packet.c, with CPU effects and resumable timing in
separate glue files. It preserves source-selected depth gates, sparse-matrix
projection, negative record kinds, partial triangle writes, coordinate clamps,
and both pixel helpers. All 36,236 isolated live frames are exact.

The structural oracle found a word-overflow distinction at C27BD6/C27C98:
BLE after ADD.W tests the signed unwrapped sum, whereas subsequent CMP.W
instructions compare wrapped depth. That case is retained in 4,096 original-
instruction fixtures, including 12 partial-write and 13 edge-clamp cases.
Custom writes are held in these structural fixtures; the separate live replay
proves actual drawing and event timing.

The generic tools/recomp/check_whole_call_glue.py ENTRY... now supplies
independent readable-C checks for future batches. It reuses cached objects and
an isolated registry, running the three recordings concurrently. The earlier
check_map_region_glue.py delegates to it with its four default entries.

## Previous map and region batch

The previous map/region live blockers are resolved. C2AB34/C2AB5A share the
source-timed packet bridge, C2AA9C supplies the depth-stage frame and pass
sequence, and C2B05A retains the source's interrupt boundaries and unusual
D2/D4-D6 restore. Domain observers remain mathematical values; CPU effects
stay in glue. No new gameplay or placeholder behavior was introduced.

The first map bridge passed CPU-only instruction fixtures yet accumulated
bus delays from extra reads on CLR memory instructions. A boundary trace found
the first difference at C2AD22 in frame 297. Removing those reads made the
600-frame map trace identical and the complete isolated live batch exact.
The instruction oracle's new --bus option checks memory-access timing under
DMA contention so this distinction is covered before expensive full replays.

## New timing evidence

- Optional FA18_BOUNDARY_TRACE=PATH and FA18_BOUNDARY_RANGE=LO-HI (hex,
  exclusive HI) produce instruction/flow-target and chipset-service CSV rows,
  with CPU registers, SR, cycles, next event, frame/line and blit/interrupt
  state. tools/recomp/summarize_boundary_trace.py reports costs and event sites.
- On the complete sealed demo, 17 original region calls cost 20,214-47,376
  live cycles including waits and interrupts. Fourteen enter $FC0D14 and
  span the next frame. The first starts in frame 19,443 and interrupts at
  $C2B19E after 2,560 cycles. The traced replay matched sealed final RAM.
- First-600-frame traces include 22 wide-map calls, 5 normal-map calls and
  22 active-plane calls. Wide-map calls starting at frames 299, 327, 349,
  366 and 384 each include 416 blits before returning.
- Commands, artifact hashes, source interrupt sites and interpretation limits
  are in analysis/routines/native_c_call_boundary_timing.md. Filtered rows
  omit chipset events inside children and interrupt handlers outside the
  selected address range; start/end frames still prove those crossings.

## Next work

1. Continue game timing parity, as requested after the C279D0 source batch.
   The sound/frame, drawing/cell and new 20-entry startup/math batches are
   independently exact. ALL still first differs at frame 416 by 361 pixels.
   Read analysis/routines/native_c_startup_timing_batch.md for selectors,
   proof and fresh traces. Startup now matches through C10174 before
   C1C860. The inner trace first differs after C1E328 display-list sorting
   (-9,950 cycles), then C09A78 adds -310. Target C1E328 with C1E4A6 and
   C1D91A, plus C09A78/C09A98 and their shared C09AB8 predicate. Other
   fixed terrain children include C1CA82/C2F66E. Inspect source contracts
   before assigning debt to an inner instruction or claiming a pixel cause.
   C1D3F4 and its three lookup/filing children now use source timing.
   C1D10C and C1E540 remain unregistered. Group shared bodies and exits,
   preserve calls, interrupts, DMA waits and frame crossings, and run --bus
   fixtures before full live checks. Do not substitute measured fixed fees.
   Use `python scripts/probe_recomp_timing.py ENTRY... --frames 500` or
   `--rank-fixed 35 --differences-only`; it reuses one source stream and
   removes scratch streams. Remaining isolated failures include
   C3201A/C31F4C/C20A40 (424), C26EBE (441), C0D04C/C20D68 (484).
   Subset bisection is not monotonic. Require combined evidence before
   claiming whole-game parity.
2. Resume additional unregistered game functions in related batches.
   The user explicitly asked for progress in the function count; avoid another
   standalone audit of already registered fixed-charge routines as the main
   batch. C279D0 is complete and activated. Start with C1D10C (648 instructions;
   terrain_* groundwork), including its placement emission and cache tail.
   Inspect indirect-call parents C0F5F8 and the C1CB14/C1CB26 siblings for explicit child contracts;
   the candidate tool excludes them. Preserve the original-source authority.
   Group shared bodies and children, prove readable whole-call C independently
   of timing steps, then run the full gate and isolated live checks.
   Reuse FA18Port's resumable step and source range for calls that cross
   chipset events, children or frames; helpers live in glue_step.h. Use
   --bus instruction fixtures with varied horizontal phases before full
   replay. Keep unported children explicit rather than counting partial
   functions or graph tails. The three C0004E/C000B4/C000BA candidates are
   Kickstart trampolines and remain deferred.
3. Once game source is complete, build the native backend from plain C memory,
   drawing, and audio. Reassess which Kickstart services remain; do OS work
   last as requested.

## Gate for a registered batch

- Read original instructions with python tools/recomp/port_info.py ADDRESS,
  then the corresponding analysis/routines report and typed port code. Preserve
  exact word arithmetic, high register halves, MOVEM.W sign extension, memory
  writes, and child effects. Reuse narrow register-effect helpers; never run a
  child with side effects twice.
- Build headless on this Windows workspace with
  & 'C:\Program Files\Git\bin\bash.exe' scripts/build_recomp.sh
  and build MSVC with
  cmake --build build/recomp-cmake --config Release -j 8
- Probe the new batch, then run
  & 'C:\Program Files\Git\bin\bash.exe' scripts/recomp_ports_check.sh
  over all three native recordings. It checks shadow, sandbox, sealed final
  RAM, and poison frames. Inspect per-entry calls, incomplete calls, and every
  mismatch. QUICK=1 is only a first-recording probe.
- Compare live `--ports on` RGB444 output with fresh `--ports off` source
  output for every affected recording, not just final RAM, frame count or blit
  totals. Set `PORTS_ONLY` to the comma-separated registered batch and run
  `& 'C:\Program Files\Git\bin\bash.exe' scripts/recomp_live_check.sh`.
  The same ON replay now also checks sealed final RAM, saving three extra
  replays per batch. Recordings run concurrently; temporary RGB/RAM outputs
  are removed. The latest completed batch leaves build/ at 0.177 GiB. An
  inactive entry needs temporary registration for a probe and must be removed
  if live output differs. A stepped SHADOW stream is not the live source
  oracle because source-first hardware-input replay can alter its timing.
- Increase the registered count and update this file only after every gate
  passes. Commit a coherent source batch with its evidence.
- Rank registered fixed-charge timing debt with
  `python scripts/probe_recomp_timing.py ENTRY... --frames 500`. Each positional
  argument is one entry, a comma-separated group, or `ALL`. The bounded probe
  reuses a single source stream and removes its scratch streams by default.
  `--rank-fixed N --differences-only` selects the N largest accumulated drifts
  from the latest demo gate report before probing them.
- Independently prove readable whole-call C with
  `python tools/recomp/check_whole_call_glue.py ENTRY...`; registered timing
  steps are disabled only in the temporary proof registry.
- The headless Ninja graph tracks source and header dependencies and shares
  objects with structural oracles and mutation builds. An unchanged build is
  subsecond. Do not manually touch generated C after a header change.

## References and constraints

- PORT.md: architecture and source conventions.
- port/game/glue/ports.c: actual registered set and ON cycle charges.
- scripts/recomp_ports_check.sh: native proof driver. Archived UAE replay and
  scripts/recomp_parity.py are not current gates.
- captures/native/*/run.json: sealed frame endpoints and final RAM hashes.
- Existing reports under analysis/routines/: original behavior evidence.
- Do not infer mechanics, constants, or object meaning without original source
  or capture evidence. Do not edit the user-owned scripts/check_native_build.py,
  scripts/native_frame_count.py, port/native_data_allowlist.txt, or .vscode/.
