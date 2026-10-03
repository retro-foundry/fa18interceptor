# Status

Updated 2026-10-03.

## Summary

| Area | State |
| --- | --- |
| Native game | Runs in an SDL2 window at 50 Hz; three sealed native recordings cover demo flight, successful carrier landing, and qualification failure |
| Current proof | The 454 registered entries pass shadow, sandbox, sealed RAM and poison on all three native recordings. Complete postflight scheduler adapters pass 180,224 full-register/full-SR/all-RAM cases, covering every owned boundary without exclusions. Active dispatch/mode-nine readable-C replay matches 6,005 shadow / 6,005 sandbox calls with timing disabled; nine peers are cold and independently proven structurally. Generic zero-comparison rejections are retained. Archived UAE runs are historical evidence. |
| Translation | 624 routines, 34,309 instructions (seeded from the native recordings); ~70% of CPU cycles in translated code |
| Recreated C source | 454 registered game entries; 554,025 matching shadow / 413,303 sandbox calls over three native recordings, zero mismatches, poison identical. Latest complete family: C09E06 postflight mode dispatch, eight mode handlers and two helpers, retaining the shared completion/readiness tails. See `CURRENT_PORT_HANDOFF.md`. |
| Live C timing | There are 246 timing-step entries. Local scheduler timing passes 359 instructions / 11,488 DMA cases; fresh combined timing passes 14,599 / 467,168 with sealed reset per fixture. The isolated family matches all 36,236 frames and sealed RAM. ALL remains 416/361; Copper fade is deferred. See `analysis/routines/native_c_postflight_scheduler.md`. |
| Kickstart replacement | 2,661,668 RAM-to-ROM transitions inventoried; the 2,157,736 observed `VBeamPos`, 21,331 `WaitBlit`, 16,526 `WaitBOVP`, 32,022 `OwnBlitter`/`DisownBlitter`, 81,120 Exec `Disable`/`Enable`, 37,669 Exec `GetMsg`, and 36,236 potgo.resource `WritePotgo` entries now use C with sealed RAM unchanged. Other OS calls and cold boot remain (`analysis/routines/fc5ece_vbeam_pos.md`, `analysis/routines/fc5a58_wait_blit.md`, `analysis/routines/fc5e58_wait_bovp.md`, `analysis/routines/fc64bc_fc64d4_blitter_ownership.md`, `analysis/routines/fc1428_fc1436_exec_interrupts.md`, `analysis/routines/fc1bea_exec_get_msg.md`, `analysis/routines/fe44f2_potgo_write.md`) |
| Bus timing | Modelled (`port/machine/bus.c`): within ~0.1-0.5% of cycle-exact UAE per scene; residual 1-colour-clock errors still make long replays drift |

The 2026-10-02 planning review found that all 187 timed entries together match
500 demo frames, while C30918 alone reproduces the frame-416 difference.
Other HUD combinations also fail; removing the seven narrowed HUD entries from
ALL leaves the same first difference. The targeted gauge checkpoint is now
corrected, with ALL's first difference unchanged. The next work is a
complete readable parent batch; C1D10C, C1E540
and the two workspace helpers are now complete; the scene-placement pair
and follow-up placement parent, then C0F5F8 and the context/bootstrap batch,
raise coverage to 432 entries; C22C80/C1C63E raised it to 434, and complete
C29042 raised it to 435; update/input/display owners raised it to 438, and
four complete input-event owners raised it to 442; complete command dispatch
owners raised it to 444; the complete postflight scheduler family now raises
it to 454. The visible checkpoint
also confirms a
fade reset and completion two machine frames late. The initializer correction
removes the frame it introduced; the inherited frame remains. A 17-entry HUD
original-code control also removes that frame, moving the first RGB difference
to 425; neither half of that group suffices. This control is diagnostic,
not a completed port correction. The user has deferred the remaining minor
Copper-fade difference for later.
`CURRENT_PORT_HANDOFF.md` retains the evidence, scope
and the work selection/validation schedule.

The complete C1E540 placement-ordering domain, CPU adapter and source timing
are implemented and registered. Normal liveness and original-byte fixtures
prove the live outputs; full isolated live and normal registered gates pass.
The earlier memory-only proof remains separately reproducible and is no
longer the integration claim. See
`analysis/routines/native_c_placement_order_domain.md`.

The complete C1D10C template-placement domain and normal CPU adapter match
8,192 original-instruction CPU/RAM fixtures and 492 shadow / 1,046 sandbox
recorded whole-call comparisons without relaxing production liveness. Its
parent and C1D722 timing are implemented and registered at 423/624. Their
seven-entry isolated live group matches all 36,236 frames and sealed RAM. See
`analysis/routines/native_c_template_placements_domain.md`.

The complete primary/alternate scene-placement pair C1CB14/C1CB26 now
preserves the distance refresh, countdown/skip and descriptor-result paths,
including the C1ED3C bypass and signed countdown overflow. Domain C, normal
CPU adaptation and source timing are proven separately. Descriptor consumers
remain explicit children. See `analysis/routines/native_c_scene_placements.md`.

The complete C1CCBC parent covers selected selectors, alternate placements and
the final relative-point pass. C1D0A4 now shares the exact position body with
C1D0B6, preserving its signed workspace index. Normal CPU/RAM, direct child
full-SR and live source timing proofs are separate. See
`analysis/routines/native_c_followup_placements.md`. C0F5F8's complete normal
integration and source timing now pass, including its hexadecimal $4650
(18,000) offset limit and real source callbacks. See
`analysis/routines/native_c_post_input_tick.md`. The complete C1C860 context
refresh, C08F26 bootstrap and C0F920/C0F992 callbacks now pass their independent
readable-C and live timing gates. The proof tool automatically isolates a
recorded child when its parent absorbs the batch comparison, while still
rejecting unproven entries. See `analysis/routines/native_c_scene_bootstrap.md`.
The complete C22C80 record-update and C1C63E update-stage parents now pass
independent readable-C, source-timing and live gates. The stepped proof runner
captures child returns before local save frames, preserving that boundary
through cold original-byte continuations. See
`analysis/routines/native_c_record_update_stage.md`.
C29042 is complete and registered, including 229 cold internal instructions
and all nine mode policies. Its CPU/RAM, readable body, timing and live proofs
are in `analysis/routines/native_c_selector_origin.md`. The older typed
adjustment callback now consumes the normalizer's changed triple and preserves
the selected shift; its focused GNU contract passes.
C0EFD4's complete 210-instruction update sequence, C0F3C4 pending input and
C0D730 display owner now pass CPU/RAM, readable-body and timing checks. The
display alternative unlinks the enclosing frame; its C and timing adapters
preserve that exit. The proof tools now retain source input replay contracts
and complete cold byte dispatches before classification. See
`analysis/routines/native_c_update_sequence.md` and
`analysis/figures/native_update_sequence_checkpoint.json`.
The complete C16EAE/C16BF2/C16C56/C13D34 input-event batch passes independent
CPU/RAM, readable-body and live timing checks; C13D34 is cold in recordings.
See `analysis/routines/native_c_input_events.md` and
`analysis/figures/native_input_events_checkpoint.json`.
The complete C1AC28/C1AD74 command owners now pass independent readable-C,
full CPU/RAM and source-timing checks, including cold actions and reset/error
exits. See `analysis/routines/native_c_command_dispatch.md` and
`analysis/figures/native_command_dispatch_checkpoint.json`.
The complete postflight scheduler family, including its shared tails and
cold mode paths, now passes independent readable-C CPU/RAM, DMA timing and
live integration gates. See `analysis/routines/native_c_postflight_scheduler.md`
and `analysis/figures/native_postflight_scheduler_checkpoint.json`.
Next audit remaining complete command/context peers and postflight owners;
C0F090/C0F132 remain internal labels.

## The game program

- One Amiga Hunk executable (`F-18 Interceptor` on the ADF): 185 hunks,
  286 KB CODE, 1.5 KB DATA, 11.5 KB BSS, loaded into Slow RAM at `$C00000+`
  (`analysis/hunk_runtime_resolved.json`).
- It is compiled C (Lattice-style `LINK A6` frames and stack arguments) plus
  hand-written assembly for the renderer, so most routines map to one C
  function each.
- C startup at `$C0DEB0`; `main()` at `$C0E27E`; the main loop at `$C15D96`
  calls the update `$C0EFD4` each iteration. The loop is CPU-paced: one
  iteration lasts as many video frames as the 68000 needs.
- The game uses Kickstart heavily (interrupt servers, graphics.library blitter
  ownership, `WaitBOVP`); about half of all executed instructions are in ROM.

## Emulator and recordings

- **Native recordings** replace the emulator runs, which are obsolete and
  archived read-only in `captures/uae/`. Record with
  `fa18_recomp --window --record OUT.fa18in` (CMake build), seal with
  `python scripts/seal_native_run.py NAME --state START.bin --input OUT.fa18in`
  into `captures/native/NAME/`. Three are sealed (demo01,
  qual_carrier_success, qual_fail_crashes) and the proof uses only them.
- Input is keyed to main-loop iterations (entries to the update `$C0EFD4`),
  not frames: live input waits for the next iteration and is logged as
  delivered (FA18_LOOP_INPUT_V1, `port/recomp/loop_input.h`). Replays are
  exact for a given machine model and translation, and shadow mode keeps
  that timing, so every proof run of a native recording must end exactly as
  sealed (the check verifies it). They are not exact across timing-model
  changes: the game integrates elapsed time per pass, so its state follows
  machine timing even when the input lands on the same iteration.
- Engine9000 fork (UAE core), `scripts/engine9000_bridge.py`: kept for the
  machine-layer reference (traces, custom-register logs).

## Native machine and translation

- `port/machine/`: bus and memory map, blitter (area, fill, descending, line;
  busy timing), Copper, bitplane display, CIAs, interrupts, UAE savestate
  loader, keyboard/mouse/replay input. Blitter output matches the real Chip
  RAM exactly for replayed register writes.
- `tools/recomp/recomp.py`: whole-program translation seeded from traces and
  the fallback log, decoded with Musashi's own tables. Kickstart ROM runs on
  the Musashi interpreter.
- Translated and interpreter-only runs end with byte-identical RAM and
  registers over 3,000 frames; gcc and MSVC builds match each other.
- `port/machine/bus.c`: CPU bus timing, shared by interpreter and translated
  code (next section).

## Bus timing

The recordings were made with UAE in cycle-exact mode (`cpu_cycle_exact` in
each capture's `config.uae`). The game's main loop is CPU-paced, so a
recording only replays natively if native CPU time matches UAE's closely.

What is modelled:

- **DMA slots per line** (colour clocks): memory refresh, bitplane fetch
  (lowres and hires fetch order; planes 5-6 take the CPU's slots) and the
  Copper (two fetches per instruction from the WAIT position; overflow runs
  on the next line). Lines not yet drawn this frame use the previous frame's
  map; after a restore, one Copper frame is simulated on a scratch copy.
- **CPU accesses**: Chip RAM, Slow RAM (on the chip bus) and custom
  registers wait for a free slot; ROM does not. Accesses follow the 68000's
  microcycle order: internal cycles first for taken branches, `-(An)` and
  indexed modes, and JMP/JSR forms; a jump's last two fetches read the
  target, after its stack accesses. CIA accesses wait for the E clock.
- **Blits** get a per-cycle timeline from UAE's cycle diagrams (fill adds an
  idle step; line mode is `-C-D`). With BLTPRI the CPU only gets idle steps;
  without it the CPU takes the third blitter cycle it waited for. That is the
  closest fit to the traces; UAE's pipelined rule is not reproduced.
- **Musashi's 68000 cycle table** is corrected in `tools/musashi/m68k_in.c`
  (regenerate `m68kops.c` with `m68kmake`): long ALU operations from
  registers, ADDQ to An, ADDA.W #imm, ANDI.L, bit operations on bits 0-15,
  and exact MULS, DIVS and DIVU timing, all measured against UAE traces
  (`scripts/musashi_timing_audit.py`).
- **Frames and input** follow Engine9000: a frame ends when line 3 starts;
  the first frame after a UAE restore runs two frames of machine time; key
  codes are libretro `RETROK_*` values; mouse motion is released on JOYxDAT
  reads and at vertical blank (UAE `readinput` and `mouseupdate`).

Measured per instruction against cycle-exact traces
(`scripts/recomp_timing.py`), scenes are within 0.01-0.5%, and one
full-screen area blit about 1%. That is not enough for 10,000-frame replays:
run060 first differs at frame 94, where native reaches a ROM beam-wait loop
about 10 colour clocks early and takes one extra pass. Exact replays need
UAE's cycle-exact 68000 and pipelined blitter and DMA arbitration, ported
from `tools/engine9000-public/ami9000/sources/src` (the CE CPU handlers,
`custom.c` `dma_cycle`, `blitter.c`).

Diagnostics: `FA18_WAIT_LOG`, `FA18_MAP_LOG` and `FA18_BLIT_LOG` (bus waits,
line slot maps, blits), `FA18_WATCH=lo-hi` (writes to a range),
`FA18_TRACE=N` (instruction trace with cycle counts), `FA18_STEAL` and
`FA18_ECLOCK_PHASE` (model variants).

## Recreated C source

| File | Routines |
| --- | --- |
| `render_line.c` | `setup_line`, `draw_line` (`$C2FA7E`), `reset_line_style` |
| `render_polygon.c` | polygon edge (`$C305AA`), bounds, `prepare_polygon`, `draw_polygon`, mask compositing and clearing |
| `polygon_clip.c` | clip stages against the four view planes, `clip_and_draw_polygon` (`$C2469E`): clip, project, draw |
| `faces.c`, `view_marks.c`, `plot.c` | faces from the transformed vertex table; view marks and the ring (not yet called); pixel plots |
| `messages.c` | the cockpit message line: choice, flashing, timeout, posting |
| `draw_stream.c` | an object's draw-stream commands: segments, grids, faces, derived points |
| `clip.c` | view-plane crossings, the shared edge-end decision, the corner-edge projection |
| `flight_recorder.c`, `post_input.c` | the game's own flight recorder; the post-input stage sequence |
| `plane_tests.c`, `tracking.c` | face-stream plane-side test, back-face test; turning angles toward a direction |
| `render_buffers.c`, `clip.c`, `matrix.c`, `view_transform.c`, `vertex_tail.c`, `attitude.c` | buffer clears and plane blits; view-plane clipping, matrices, vertex transforms, attitude |
| `render_state.c`, `render_page.c`, `render_span.c` | blit starts, state blocks, draw page, span bounds |
| `fixed_math.c` | `sin_cos`, `y_rotation_matrix`, `rounded_divide`, `attenuate_offset` |
| `audio.c` | voice output, master volume fade, voice programs, channel interrupts |
| `text.c`, `numbers.c` | glyph plotting, packed BCD, the date line |
| `control_records.c` | control-record fields, selection, rate class, player reset, level-list filing |
| `interrupts.c`, `fault.c` | the audio interrupt server; the fault hook and fatal error |
| `post_input.c`, `notify.c`, `stages.c`, `view.c` (incl. `aim_view`), `player_input.c`, `screen_frame.c` | stage callbacks, cadence, zoom, mouse buttons, frame lists |

Every routine passes the shadow proof (all live registers, flags, memory and
custom-chip writes identical to the original on every call) and the poison
check. Ranked candidates: `python tools/recomp/port_candidates.py` (45 ready).

## Analysis assets

- Memory map: `analysis/memory_map.md`.
- 425 routine reports in `analysis/routines/`.
- 1,176 byte-exact assembly slices in `source_amiga/observed/`
  (`python scripts/verify_reconstructions.py`), 51,256 bytes.
- Raw P-code for 126 captures in `pcode/raw/` (16% of CODE bytes). It is
  evidence of what ran, read by the coverage scripts and cited by routine
  reports; the translation and the C port do not read it
  (REVERSE_ENGINEERING.md, section 4).
- Coverage accounting: `analysis/coverage.json`.

## The earlier port

`fa18_port` (the top-level `port/*.c`, built by `port/CMakeLists.txt`) was a
bottom-up port of about 400 routine slices with 208 contract tests. It never
rendered the flight view (it stopped at frame 273, blocked on CPU-paced
timing) and is superseded. Its modules are reused where they pass the shadow
proof; so far, packed BCD and the rounded divide.
