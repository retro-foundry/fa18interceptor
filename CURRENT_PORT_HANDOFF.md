# C port handoff

Updated 2026-10-01. This is the current work state. Older notes remain in git
history (the preceding handoff is in commit ebbfc3da); ignored gate logs may
also remain under build/recomp/.
PORT.md describes the architecture and source conventions.

## Objective and order

Recreate readable C for the whole game, proven against the original source and
sealed native recordings. Work in related batches. The order is Stage D game C,
Stage F native backend, then only the Stage E Kickstart services still needed.
The user explicitly deferred OS work and asked for larger routine batches.

## Verified baseline

- 414 of 624 translated game entries are registered in port/game/glue/ports.c.
  The latest full gate for that registered set matched 721,715 completed shadow
  calls and 1,169,611 sandbox calls across three native recordings, with zero
  mismatches and identical poison frames. Ported parents absorb some formerly
  counted child calls, so the aggregate call totals need not rise monotonically.
  build/recomp/ports_report_*.json describe this 414-entry baseline. GNU and
  MSVC builds pass. In addition to active planes and the earlier audio batch,
  18 registered glyph, input, page, notification, command/audio, buffer and
  polygon entries now use source-timed steps. Their isolated ON replay matches
  fresh source OFF output on all 36,236 frames. The combined instruction oracle
  matches 616 instructions and 19,712 cases. See
  analysis/routines/native_c_registered_timing_batch.md.
- The current recordings are captures/native/demo01,
  captures/native/qual_carrier_success, and
  captures/native/qual_fail_crashes. Each has state.bin, input.fa18in, and
  run.json with a sealed final RAM hash. Archived captures/uae runs, including
  run075, are historical evidence and are not an acceptance gate.
- The headless GNU and MSVC Release builds pass. The typed port's eight
  affected map/detail contract tests passed with the map source changes.
- Only .vscode/ is untracked; it belongs to the user. Leave it alone.
- The register bridge refactor for the map pair and region probe is complete.
  The latest temporary 416-entry gate matched 719,426 completed shadow calls
  and 1,160,055 sandbox calls with zero mismatches, sealed final RAM unchanged,
  and identical poison frames. The map pair accounted for 622 shadow and
  8,058 sandbox matches; the region probe for 3 shadow and 17 sandbox matches.
  Temporary registration was removed. Its reports are retained under
  build/recomp/bridge_gate_416_*.json; these are not the registered baseline.
- The region probe additionally matches original registers and RAM on 4,096
  direct structural oracle cases, including signed coordinates, sloped edges,
  endpoint exclusions and randomized high register halves. The oracle found
  and corrected the old C's near-endpoint exit: $C2B1E4/$C2B1FE abandon the
  directory walk, not just one segment. See
  analysis/routines/c2b05a_record_region_probe.md.
- The registered all-native path is not frame-faithful. The glyph/input/page/
  notification/command/buffer/polygon timing batch fixes the former C330FE
  blocker. A current 500-frame demo replay now first differs at one-based frame
  416; C31226 (`dispatch_postflight_renderer`) alone produces the same frame
  and 361 differing pixels. The sealed carrier-success endpoint is 12,353
  source frames versus 31,346 with all registered ports ON. Shadow/sandbox
  matches do not establish live ON fidelity for the whole registered set.
- Plane shadow now replays the live source's ordered DMACONR inputs on saved
  entry RAM. It independently checks native outputs and write sequences, and
  fails extra/reordered/missing reads. All five former counter failures now
  compare and pass. Report fields busy_input_calls/reads expose this proof
  input model. Two intentionally incorrect bridge copies are rejected by
  tools/recomp/check_shadow_busy_inputs.py. Full ON replay separately proves
  the actual timing. No mismatch or hardware classification was suppressed.

## Completed C awaiting live timing

These sources and glue compile, but none of these entries is in ports.c.
Recorded-call comparison alone does not authorize activation.

| Entry | Source and recorded-call proof | Live ON blocker |
| --- | --- | --- |
| C2AB34 / C2AB5A (wide/normal map packet) | port/game/map_packet.c and glue/glue_map_packet.c use the shared port/map_packet_* core. Latest temporary 416-entry full gate: zero mismatches; the pair matched 622 completed shadow calls and 8,058 sandbox calls. Multiple polygons per packet and full register effects are implemented. | demo01 first RGB difference at frame 350 with a 20,000-cycle charge. A fixed-charge sweep moved the first difference at best to frame 416, where 29,453 pixels differed; blit count also changed. Drawing many polygons before charging cycles at return loses intermediate chipset timing. |
| C2B05A (record region probe) | port/game/record_region_probe.c and glue/glue_record_region_probe.c. Latest temporary 416-entry full gate: zero mismatches; this entry matched 3 completed shadow and 17 sandbox calls. The earlier isolated 414-entry proof had 18 sandbox matches. Fourteen source calls take interrupts. Source D2-D5 save restores into D2/D4-D6; preserve that mapping. | With 20,000 cycles, demo01 final RAM hash, 20,833-frame endpoint, and 555,658 blits matched, but 14 RGB frames differed, first at 19,445. Charging the 31,541 sandbox mean moved the endpoint to 20,835. The earlier isolated sandbox measured 22,150 to 35,192 cycles; live costs and interrupt sites are recorded below. |

The map pair also blocks the small parent C2AA9C. Do not add any of these
entries, the parent, or mid-function graph tails to the count to show progress.
MapPacketHooks and RecordRegionProbeHooks now carry only walk observations,
mathematical values and the map polygon child callback. Register replay lives
in glue. The observations establish completed-call parity; they are not
resumable execution boundaries.

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

1. Resolve the shared timing boundary for long C calls, using the new CSV
   observations above. The register bridge refactor is already complete.
   FA18Port now supports an optional one-instruction step and source range;
   its dispatcher retains caller PC/SP across children, interrupts and frames.
   Active planes, the audio group, and the 18-entry registered timing batch
   now have exact isolated live proof. Shared glue helpers live in
   glue_step.h. The fading bridge uses step_start=C24FE6 for an existing shared
   early RTS, without adding a registry entry. Extend the mechanism to the
   inactive map/region stages.
   In the source,
   instruction boundaries can service Copper, blitter, and interrupts inside
   these routines. In the current bridge, run_glue in
   port/recomp/recomp_ports.c charges a single fixed value after the whole C
   call for ordinary ports. Measure source event/cycle boundaries and make the C path advance
   through equivalent observable boundaries. The registered stepped bridges
   have resumable C continuations today; the map and region observation hooks
   still need explicit source-backed checkpoints.
   Recheck the map pair and region
   probe as a related timing batch; keep their gameplay and
   register logic source-backed. A new fixed average charge has already failed.
2. Continue the registered timing audit at C31226. It alone reproduces the
   all-registered first difference at demo frame 416 (361 pixels). Preserve
   its postflight renderer child order and instruction/event boundaries; a
   partial bridge that exits into an external generated tail is not a valid
   sandboxed replacement. C305AA was observed as a later fixed-charge blocker,
   but must be rechecked only after C31226 is exact. Keep porting independent
   game-source groups while timing work proceeds.
   python tools/recomp/port_candidates.py -n 40 currently lists C279D0
   (renderer packet; typed groundwork in port/projection_grid.c and reports
   under analysis/routines/c279d0_*), C1D10C (terrain/scene template path;
   typed groundwork under port/terrain_* and analysis/routines/c1d10c_*),
   the inactive map/region entries above, and three OS trampolines. Inspect
   fixed-target indirect callers too; the candidate tool omits them. Group
   routines that share source logic or already completed children. The three
   C0004E/C000B4/C000BA candidates are Kickstart trampolines; defer them.
   A read-only indirect-call audit also found the game-source parents
   C0F5F8 (post-input tick; existing port/post_input_tick.c and routine report)
   and C1CB14/C1CB26 (display-list traversal). Their callbacks need explicit
   child contracts. The short C53Cxx/C53Fxx leaves found by this audit are
   library-vector trampolines and remain deferred OS work.
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
  The recordings run concurrently and temporary streams are removed. An
  inactive entry needs temporary registration for a probe and must be removed
  if live output differs. A stepped SHADOW stream is not the live source
  oracle because source-first hardware-input replay can alter its timing.
- Increase the registered count and update this file only after every gate
  passes. Commit a coherent source batch with its evidence.
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
