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
- The current recordings are captures/native/demo01,
  captures/native/qual_carrier_success, and
  captures/native/qual_fail_crashes. Each has state.bin, input.fa18in, and
  run.json with a sealed final RAM hash. Archived captures/uae runs, including
  run075, are historical evidence and are not an acceptance gate.
- The headless GNU and MSVC Release builds pass. The typed port's eight
  affected map/detail contract tests passed with the map source changes.
- Only .vscode/ is untracked; it belongs to the user. Leave it alone.
- Ignored build/recomp/ports_report_*.json currently describe a TEMPORARY
  414-entry experiment. Do not quote them as the 413-entry baseline. Rerun the
  full gate after a new registered batch to refresh them.

## Completed C awaiting live timing

These sources and glue compile, but none of these entries is in ports.c.
Recorded-call comparison alone does not authorize activation.

| Entry | Source and recorded-call proof | Live ON blocker |
| --- | --- | --- |
| C2AB34 / C2AB5A (wide/normal map packet) | port/game/map_packet.c and glue/glue_map_packet.c use the shared port/map_packet_* core. Temporary 415-entry full gate: zero mismatches; the pair matched 622 completed shadow calls and 8,058 sandbox calls. Multiple polygons per packet and full register effects are implemented. | demo01 first RGB difference at frame 350 with a 20,000-cycle charge. A fixed-charge sweep moved the first difference at best to frame 416, where 29,453 pixels differed; blit count also changed. Drawing many polygons before charging cycles at return loses intermediate chipset timing. |
| C2B05A (record region probe) | port/game/record_region_probe.c and glue/glue_record_region_probe.c. Temporary 414-entry full gate: zero mismatches; this entry matched 3 completed shadow and 18 sandbox calls. Fourteen shadow calls crossed chipset events. Source D2-D5 save restores into D2/D4-D6; preserve that mapping. | With 20,000 cycles, demo01 final RAM hash, 20,833-frame endpoint, and 555,658 blits matched, but 14 RGB frames differed, first at 19,445. Charging the 31,541 sandbox mean moved the endpoint to 20,835. Observed source calls cost 22,150 to 35,192 cycles. |
| C2FD8C (active plane submission) | port/game/active_planes.c and glue/glue_active_planes.c; see analysis/routines/c2fd8c_first_active_plane_submission.md and adjacent reports. A focused sandbox probe matched 2,078 calls. | Shadow busy-poll counters differed on five calls because sandboxed custom writes do not start live blits. Live demo01 first differed at RGB frame 297. Source blitter wait and event timing remain unresolved. |

The map pair also blocks the small parent C2AA9C. Do not add any of these
entries, the parent, or mid-function graph tails to the count to show progress.
MapPacketRegisterEffects and RecordRegionProbeRegisters currently carry
68000 register state through port/game/, despite PORT.md's glue boundary.
Before changing either routine's behavior, move register replay into glue
and keep game logic behind a small data contract. Preserve the recorded-call
parity while doing that refactor.

## Next work

1. Refactor the register bridge noted above, then resolve the shared timing
   boundary for long C calls. In the source,
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
