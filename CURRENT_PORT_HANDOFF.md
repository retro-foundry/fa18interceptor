# C port handoff

Updated 2026-10-01. This is the current work state. Older notes remain in git
history (the preceding handoff is in commit 9f2d5ca6); ignored gate logs may
also remain under build/recomp/.
PORT.md describes the architecture and source conventions.

## Objective and order

Recreate readable C for the whole game, proven against the original source and
sealed native recordings. Work in related batches. The order is Stage D game C,
Stage F native backend, then only the Stage E Kickstart services still needed.
The user explicitly deferred OS work and asked for larger routine batches.

## Verified baseline

- 413 of 624 translated game entries are registered in port/game/glue/ports.c.
  The latest full gate for that registered set matched 727,968 completed shadow
  calls and 1,214,836 sandbox calls across three native recordings, with zero
  mismatches and identical poison frames. Ported parents absorb some formerly
  counted child calls, so the aggregate call totals need not rise monotonically.
  This gate was rerun after the bridge refactor and boundary-trace additions;
  build/recomp/ports_report_*.json again describe this 413-entry baseline.
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

## Completed C awaiting live timing

These sources and glue compile, but none of these entries is in ports.c.
Recorded-call comparison alone does not authorize activation.

| Entry | Source and recorded-call proof | Live ON blocker |
| --- | --- | --- |
| C2AB34 / C2AB5A (wide/normal map packet) | port/game/map_packet.c and glue/glue_map_packet.c use the shared port/map_packet_* core. Latest temporary 416-entry full gate: zero mismatches; the pair matched 622 completed shadow calls and 8,058 sandbox calls. Multiple polygons per packet and full register effects are implemented. | demo01 first RGB difference at frame 350 with a 20,000-cycle charge. A fixed-charge sweep moved the first difference at best to frame 416, where 29,453 pixels differed; blit count also changed. Drawing many polygons before charging cycles at return loses intermediate chipset timing. |
| C2B05A (record region probe) | port/game/record_region_probe.c and glue/glue_record_region_probe.c. Latest temporary 416-entry full gate: zero mismatches; this entry matched 3 completed shadow and 17 sandbox calls. The earlier isolated 414-entry proof had 18 sandbox matches. Fourteen source calls take interrupts. Source D2-D5 save restores into D2/D4-D6; preserve that mapping. | With 20,000 cycles, demo01 final RAM hash, 20,833-frame endpoint, and 555,658 blits matched, but 14 RGB frames differed, first at 19,445. Charging the 31,541 sandbox mean moved the endpoint to 20,835. The earlier isolated sandbox measured 22,150 to 35,192 cycles; live costs and interrupt sites are recorded below. |
| C2FD8C (active plane submission) | port/game/active_planes.c and glue/glue_active_planes.c; see analysis/routines/c2fd8c_first_active_plane_submission.md and adjacent reports. A focused sandbox probe matched 2,078 calls. | Shadow busy-poll counters differed on five calls because sandboxed custom writes do not start live blits. Live demo01 first differed at RGB frame 297. Source blitter wait and event timing remain unresolved. |

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
   In the source,
   instruction boundaries can service Copper, blitter, and interrupts inside
   these routines. In the current bridge, run_glue in
   port/recomp/recomp_ports.c charges a single fixed value after the whole C
   call. Measure source event/cycle boundaries and make the C path advance
   through equivalent observable boundaries. The bridge has no C continuation
   today, so an interrupt inside a C call needs an explicit resumption design.
   Recheck the map pair, region
   probe, and active planes as one timing batch; keep their gameplay and
   register logic source-backed. A new fixed average charge has already failed.
2. Keep porting independent game-source groups while timing work proceeds.
   python tools/recomp/port_candidates.py -n 40 currently lists C279D0
   (renderer packet; typed groundwork in port/projection_grid.c and reports
   under analysis/routines/c279d0_*), C1D10C (terrain/scene template path;
   typed groundwork under port/terrain_* and analysis/routines/c1d10c_*),
   the three inactive entries above, and three OS trampolines. Inspect
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
- Compare live --ports on RGB444 frame output with the native shadow RGB444
  output for each affected recording, not just final RAM, frame count, or
  blit totals. The full gate writes build/recomp/frames_shadow_NAME.bin.
  --ports-only ADDRESS applies only to registered entries. An inactive entry
  needs temporary registration for a probe and must be removed if live output
  differs. Demo01 is 20,833 frames and its RGB stream is about 3.4 GB.

  For demo01, after the full gate (run from the repository root in Git Bash):

  ```sh
  ./build/recomp/fa18_recomp.exe --state captures/native/demo01/state.bin \
    --input captures/native/demo01/input.fa18in --to-end \
    --rom local/system/kick13.rom --ports on \
    --rgb444 build/recomp/frames_on_demo01.bin
  cmp build/recomp/frames_shadow_demo01.bin build/recomp/frames_on_demo01.bin
  ```

  Repeat with each affected native recording and require exact equality.
- Increase the registered count and update this file only after every gate
  passes. Commit a coherent source batch with its evidence.
- scripts/build_recomp.sh does not track generated-code header dependencies.
  After changing recomp_runtime.h, touch affected generated C before rebuilding.

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
