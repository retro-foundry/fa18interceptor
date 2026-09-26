# F/A-18 Interceptor native C port

## Target and authority

The 1988 disk and the pinned Engine9000 v0.62-alpha Amiga core are the
behavioral authority. The first scenario is `captures/run075`, restored to its
canonical menu state. Port frame 200 is the starting image; the recorded `1`
key is pressed at frame 230 and released at frame 234. The game then enters its
demo. Preserve the original event order, integer widths, state transitions,
draw order, palette changes, and 50 Hz timing. The native display is a 320x200
chunky framebuffer presented by SDL2; the Copper, bitplane and blitter effects
will be translated into direct pixel operations with the same visible result.

The verified Amiga plane geometry is four 8,000-byte planes, 40 bytes per row,
200 rows (`analysis/cockpit_bitplane_assets.md`). Engine9000's host image is
720x287 for the sampled run075 frames; its 320x200 game image is doubled
horizontally at x=40..679, y=16..215. These are distinct dimensions. The host
PNG crop is an oracle, not game source data.

## Work sequence

1. **Seal and bound run075.** Keep `captures/run075` immutable. Use
   `scripts/render_port_oracle.py` to render exact host-scheduled frames from
   the canonical restore, starting with frame 200. Check the menu, keypress,
   demo transition and later scene changes. Retain frame hashes, the input
   prefix and restore hash with each generated oracle. Capture additional
   windows only where a change or uncertain routine needs them.
2. **Native visual harness.** Build a C11/SDL2 executable with a 320x200
   chunky framebuffer and 50 Hz frame clock. Pack oracle RGB444 pixels into
   an exact, frame-numbered delta stream for comparison and deterministic
   playback. Make frame stepping and a headless frame dump available. Verify
   decoded pixels against the PNG oracle. This is a display milestone, not a
   claim that recorded pixels implement game logic.
   The active native gate is frame 200. Do not advance the native simulation
   to frame 201 until frame 200's state, chunky pixels, palette, and event
   boundary match the run075 oracle. Every later frame is unlocked in order;
   a full-stream oracle verifier cannot replace these per-frame native gates.
3. **Menu and demo control.** Trace run075 frame 200, the frame-230 keypress,
   and the transition through `$C0FBE0` (menu text sequence), `$C0FCB4`
   (selection followup), and `$C0FECE` (delayed mode dispatch). Capture
   entry/exit registers, callback slot `$C1820C`, sequence at `$C4574A`,
   selected mode, and the exact frame of each transition. Port the smallest
   complete contract at each step, replacing the corresponding recorded
   frame region only after native output matches.
4. **Display primitives.** Port the proved `$C2F688` four-plane pixel/mask
   contract and `$C2FA7E` line parameters to chunky rendering, using bounded
   register and Chip-RAM oracles. Then resolve and port text glyph lanes
   `$C33058`, filled polygon submissions `$C2FF48`, palette/Copper state,
   and cockpit composition. Compare exact RGB pixels on every affected frame.
5. **Cockpit and flight state.** Stay on the menu and pre-cockpit demo frames
   until their native pixels and state match in order. Begin tracing the
   scripted-control consumer, fixed-point motion, view changes, world record
   selection, transforms, projection, HUD, audio and return path only at the
   first frame that draws the cockpit. Use existing source slices and reports
   as leads, promoting a name only with a behavioral or scenario contract.
   Port complete routines one at a time, with pre/post state and output
   checkpoints from the same run. Extend to other sealed scenarios only after
   the demo path is native and reproducible.
6. **Full-game coverage.** Expand the scenario matrix for training, free
   flight, missions, combat, landing, crashes, map, and pilot-log persistence.
   A complete port requires native behavior on those paths; run075 alone cannot
   establish it.

## Function understanding ledger

For each routine encountered, record original address/source slice, caller,
inputs, state reads and writes, return/branch outcomes, visible or audio
effect, run/frame evidence, native function, and meaning level (`structural`,
`dataflow`, `behavioral`, `scenario`, `port-contract`). Keep uncertainty in the
existing `analysis/routines/` report or a new focused report. Add an exact
oracle fixture before calling a gameplay formula or state name proven. Do not
infer a function's purpose merely from a rendered frame.

| Original routine | Current evidence and meaning | Native status |
| --- | --- | --- |
| `$C0FBE0` | Queues 6,100..109,0 at `$C4574A`, installs `$C0FCB4`; static body and historical menu snapshot (`analysis/routines/c0fbe0_queue_top_level_menu_text.md`) | Not ported |
| `$C1BD78` demo branch | run075 frame 230: key `1`, zero `D4`, nonzero `$C4FDBC` lead to demo mode (`analysis/routines/c0fcb4_run075_demo.md`) | Bounded post-dispatch demo selection ported through `FA18MenuState`; frontend mapping and `$C3318E` side effect remain open |
| `$C3318E` | run075 demo route selects `D0=2,D1=2` packet for `$C17EF2`; purpose of the callee remains open (`analysis/routines/c3318e_indexed_command_side_effect.md`) | Not ported |
| `$C0FCB4` | run075 frame 234 proves demo selector 101, delay `$D2`, and delayed-transition continuation; other routes in `analysis/routines/c0fcb4_top_level_menu_followup.md` | `$C0FD10-$C0FDCE` demo arm ported through native `FA18MenuState` in `port/menu.c`; rest not ported |
| `$C0FECE` | run075 signed countdown expires at frame 270; mode `$7F` dispatch selects the demo-entry continuation (`analysis/routines/c0fece_delayed_menu_transition.md`) | Signed gate, proved tick update, bounded run075 common setup, and `$C1000A-$C10020` demo arm ported through `FA18MenuState`; helper-call effects and other modes remain open |
| `$C0FAA4` | run075 frame 370 direct scene-initialization stores, followed by `$C0FA04` followup stores; helper calls `$C28722`, `$C0924A`, `$C11312`, `$C082B0` remain dataflow evidence (`analysis/routines/c0faa4_run075_scene_initialization.md`) | Direct state subset and caller ordering ported through `FA18MenuState`; nested helper effects and scene rendering remain open |
| `$C2F688` | Four-plane pixel word address/mask preparation and handlers; run075 frame 315 proves the alternate two-row path and run060 proves a primary handler (`analysis/routines/c2f688_run075_two_row_mask.md`) | Proved primary and alternate handlers, plane enables, and pre-dispatch XOR ported to chunky pixels through `FA18RendererState` in `port/renderer.c`; arbitrary off-buffer Amiga writes remain outside this visual contract |
| `$C2FA7E` | Blitter line setup and enabled-plane submissions; run075 proves endpoint ordering and hardware triggers (`analysis/routines/c2fa7e_blitter_line.md`) | Native recurrence remains provisional; require a settled run060+ visual fixture before calling it a port contract |
| `$C24CFE` | Finalized view tuples project to bounded screen-pair polygons before `$C2FF48` (`analysis/routines/c24cfe_polygon_projection_submit_tail.md`) | Formula is implemented with native structs; require a run060+ tuple/output fixture before calling it a port contract; 1-2 tuple branch remains open |
| `$C1C5E0` | run060 frame-8246 masks root X/Z, adds the incoming tuple, negates, shifts, and publishes the projection packet (`analysis/routines/c1c5e0_projection_packet_publish.md`) | Ported through `FA18ProjectionRoot`, `FA18ProjectionInput`, and `FA18ProjectionPacket`; upstream matrix seed and downstream polygon tuple production remain open |
| `$C1C54E` | run060 record type `$11` selects `(0,5,$14)`, applies the 2.14 matrix, shifts by six, and adds the root base (`analysis/routines/c1c54e_projection_seed_transform.md`) | Ported through `FA18Fixed14Matrix`, `FA18ProjectionBase`, and `FA18ProjectionSeedResult`; active-record selection and caller routing remain open |
| `E9K_INPUT_V1` replay boundary | run075 contains 87 ordered frame events feeding the menu and later demo (`port/replay.c`) | Native parser and `FA18ReplayControlState` latch ported; flight control interpretation and fixed update consumer remain open |
| `$C1F4AC/$C1F524` | Demo local-triple transform: arithmetic local shift, wrapped translation, signed 3Ã—3 dot products, then fixed-point output (`analysis/routines/c1f4ac_vertex_transform.md`) | Proved transform core ported through `FA18VertexTransform`, `FA18LocalVertex`, and `FA18TransformedVertex`; caller record decoding and alternate route selection remain open |
| `$C2FF48` | Polygon wrapper selects line or descending area-blit routes (`analysis/routines/c2ff48_display_submission_wrapper.md`) | Not ported: run060 now supplies the isolated changed-plane output and `$C304B2` control setup (`$0D0C/$0002`, destination `$76EE`, size `$0D14`), but caller-owned masks/modulos and the native fill are still missing |
| Display page boundary | Observed cockpit and M-map pages are four 40-byte × 200-row planes with RGB4 palette state (`analysis/routines/native_planar_display_adapter.md`) | Native `FA18PlanarPage` → `FA18IndexedFrameBuffer` decoder and `FA18Palette` RGB444 application ported; page selection and palette programming remain open |
| run075 frames 200-233 menu page | Baseline menu screenshot is identical to oracle frame 200; frames 201-233 remain pixel-identical, with the frame-230 key event changing menu state only | Native menu page is rendered and compared for every frame 200-233; live playback stops at frame 234 until its changed page/state is reconstructed |
| `$C330FE` | Byte-mask update over source glyph bytes and strided display longwords (`analysis/routines/c330fe_strided_long_mask_update.md`) | Proved set/clear mask algebra ported through bounded `FA18GlyphMaskLane` and `FA18PlanarPage`; glyph records and lane placement remain open |
| `$C33058` | Static-text glyph lanes; run060 success-text entry proves four plane destinations and set/clear mask selection (`analysis/routines/c33058_static_text_glyph_lanes.md`) | Bounded post-glyph-lookup four-lane submission ported through `FA18StaticGlyphSubmission`; glyph/layout producers and general placement remain open |

## Progress

- 2026-09-26: Ported the semantic `$C2F558` display page pair selector as
  `FA18DisplayPagePair` and `fa18_select_display_page`. It preserves the
  observed adjusted versus unadjusted page choice while leaving raw table
  addresses outside the C runtime model.
- 2026-09-26: Added validated `fa18_palette_set` entry updates to the native
  `FA18Palette` boundary. It accepts only RGB4 values and valid palette
  indices; the source palette write order remains open until a run060+ trace
  identifies it.
- 2026-09-26: Ported the run029 traced `$C25A08` workspace conversion as
  `fa18_pack_decimal_workspace`. It reproduces repeated decimal-place
  subtraction and packed-nibble accumulation, including the observed raw
  value `375 -> 0x00000375`; the caller still determines which cockpit field
  the value represents.
- 2026-09-26: Connected the packed conversion to the formatter through
  `FA18CockpitNumericValue` and `fa18_prepare_cockpit_numeric`. The native
  value now carries raw input, packed digits, and fixed-width character bytes;
  live field naming and screen placement remain caller evidence.
- 2026-09-26: Ported the observed `$C321D2` scaled-record numeric boundary as
  `fa18_prepare_scaled_record_numeric`. It preserves the signed subtract,
  arithmetic shift, signed division, and `$177` bias before composing the
  four-character value; record ownership and on-screen label remain open.
- 2026-09-26: Added `FA18ScaledNumericState` and
  `fa18_update_scaled_numeric` for the observed `$C321D2` previous-value and
  two-pass redraw gate. It emits typed compositor requests with masks `$4`
  and `$C`, while coordinate ownership remains unresolved.
- 2026-09-26: Ported the `$C32178` signed-byte preprocessing as
  `FA18ThreeDigitScale` and `fa18_scale_record_byte`, preserving magnitude,
  sign, eight-bit scaling, and `$133` division before the shared draw gate.
- 2026-09-26: Added the run060-backed `$C3201A` feet display boundary as
  `FA18FeetDisplayValue` and `fa18_prepare_feet_display`. It preserves the
  record `>>10` then `*5` conversion, six-character packed formatting, the
  `$1869F` override clamp, and the observed optional `FT` suffix.
- 2026-09-26: Promoted the run060 frame-8244 formatter fixture into the
  glyph contract: selected-record `+$18=$7708` produces `145` and the
  alternate request carries `145 FT`. The trace and endpoint evidence are
  recorded in `analysis/data/run060_root_altitude_formatter.md`.
- 2026-09-26: Added the changed-value run060 layout fields to
  `FA18FeetDisplayValue`: normal `$18CE/$1E/$FCA/$4` and alternate
  `$1CDA/$1A/$F3A/$C`. These are typed compositor inputs recovered at
  `$C32740`; the coordinate table and final plane destination remain open.
- 2026-09-26: Decoded the run060 `$C31928` table into six typed
  `FA18GlyphCoordinatePair` entries. The words are retained as signed
  position and compositor mask pairs; they are deliberately not called x/y
  coordinates until the final plane mapping is proven.
- 2026-09-26: Added `fa18_prepare_plane_glyph_placement`, which replaces the
  observed active-plane pointer with a semantic plane index and computes the
  bounded native destination offset. It preserves the `$28` visibility gate,
  even destination check, and coordinate mask without retaining Amiga
  addresses.
- 2026-09-26: Connected placement to the existing bounded glyph stream with
  `fa18_render_placed_glyph`. The native operation now selects the glyph,
  applies the observed row stride, and writes the selected plane at the
  computed offset under the same page bounds checks.
- 2026-09-26: Added `fa18_render_numeric_glyphs`, which composes a typed
  numeric value, the captured coordinate pairs, plane placement, and glyph
  stream writes into one native operation. Its contract exercises the six
  character `145` submission path; exact multi-plane ownership remains tied
  to the caller’s active lane evidence.
- 2026-09-26: Captured the changed run060 `$C3201A` Chip RAM delta with
  Engine9000. Three persistent bytes land in active plane 3 and one byte is
  transient blitter state; the exact values and plane-relative offsets are
  recorded in `analysis/data/run060_c3201a_changed_chip_delta.md`.
- 2026-09-26: Joined that delta through `$C32806`: the first native
  destination is plane 3 base `$16A40` plus `$18CE`, with shift zero and five
  40-byte-stride rows. This validates the native placement and compositor
  inputs against the run060 changed-value trace.

- 2026-09-26: Sealed `captures/run075` (87 input events; canonical restored
  state SHA-256 `760d729341bebb9d6aa49450e7c7a6b760fd2d35321e9bc1e4c5f09c4015a4f4`).
  Added a bounded host-scheduler oracle renderer. Frames 200, 230, 300 and 400
  were captured and inspected; 200 is the menu, 300 is a black transition,
  and 400 is the cockpit demo.
- 2026-09-26: Rendered every host-scheduled frame 200..20,987, ending just
  before later recorded mouse input at 20,988. The RGB444 chunky delta bundle
  `build/port_run075_demo.fa18` contains 20,788 frames in 15,146,030 bytes;
  SHA-256 `d34f9294880a15b45e56aab8a868c0cae4813c877debf5439cd30e409d2397d6`.
  The exporter checked host pixel doubling and RGB444 expressibility for
  every frame. The C reader validated every frame checksum. The independent
  `scripts/compare_port_frames.py` byte-compared all 20,788 decoded native
  chunky frames against the host PNG crop with no mismatches. Native PPM dumps
  additionally matched at frames 200, 230, 300, 400, 600, 1,800, and
  20,987. A dummy SDL video driver completed a 201-frame presentation run.
- 2026-09-26: Traced run075's key-1 producer and demo menu arm. Ported the
  bounded `$C0FD10-$C0FDCE` branch and passed an emulator pre/post fixture.
  The shared countdown expires at frame 270 after about five or six game ticks
  per captured video frame. Traced the subsequent `$C0FFDA` mode table to its
  `$C1000A` demo arm; ported that arm with a second exact pre/post fixture.
  The port represents this as one `FA18MenuState`, with a typed continuation
  enum and named queue, delay, and followup fields. Original callback addresses
  remain evidence only. Shared transition helpers and later demo behavior
  remain open.
- 2026-09-26: Ported the preceding run075 `$C1BD78` demo-selection branch.
  `fa18_select_run075_demo_mode` changes native zero-context menu state to
  demo mode and records its selection marker, using the emulator frame-230
  pre/post fixture. It does not claim a general keyboard mapping or emulate
  the unresolved `$C3318E` command packet.
- 2026-09-26: Made the `$C0FECE` delay gate native and signed. The C menu
  state uses `int16_t delay_ticks`; the contract test verifies zero returns
  from the callback while `-1` enters the mode-specific transition body.
  Callback dispatch after the proved decrement remains unported pending the
  common demo-setup contract.
- 2026-09-26: Ported the proved state-update portion of the shared `$C0F5F8`
  tick. `fa18_menu_post_input_tick` increments its named byte counter and
  decrements the signed delay with exact 16-bit wrap; callback dispatch awaits
  the reconstructed common demo setup.
- 2026-09-26: Ported the run075 negative-delay common transition subset. It
  sets named row-limit, stage, phase, auxiliary, delay, followup, and typed
  continuation fields before the existing demo-entry arm. The menu contract
  test now covers frame-230 selection through the frame-271 followup arm.
- 2026-09-26: Added `FA18DemoController`, which composes the proved native
  menu functions into the run075 menu-to-demo route. Its contract test starts
  with the recorded video flags, performs 211 post-input ticks, and verifies
  the exact demo-entry state. The SDL oracle player is not yet driven by this
  controller because scene rendering and subsequent callbacks remain open.
- 2026-09-26: Extended the controller through the proved `$C0FA04` expiry
  branch. Five further post-input ticks install the native followup-match
  continuation with its direct state writes. The `$C0FAA4` helper and `$C0FA4C`
  callback behavior remain evidence targets.
- 2026-09-26: Traced run075's negative `$C0FA04` route at direct-core frame
  370. Ported `$C0FAA4`'s direct scene-initialization state subset with named
  latches, stage, guard, counters, marker, selected-mode value, and delay in
  `FA18MenuState`. `fa18_expire_run075_demo_entry` applies that subset before
  `$C0FA04` replaces the delay with two and installs the followup continuation.
  `$C28722`, `$C0924A`, `$C11312`, `$C082B0`, and scene rendering remain open.
- 2026-09-26: Ported the byte-exact `$C0FA4C` input-match and `$C0FA80`
  completion callback state gates as reusable `FA18MenuState` functions.
  Their direct state contracts pass, but no run075 reachability is inferred.
- 2026-09-26: Traced run075 frame 291 into `$C0FA04`'s nonnegative `$C2FD22`
  clear path. The demo controller now clears its named chunky work buffer on
  each proved waiting tick; the original pointer families remain unassigned.
- 2026-09-26: Captured run075 frame-315 `$C2F688` entry and 52-instruction
  Chip-RAM before/after fixture. The alternate `$C2F7C6` mask selects two
  adjacent columns on two rows; mode `$B` changes x=171..172, y=99 from
  color index 4 to 11, while y=100 is already 11. A run060 primary-path
  capture proves mode `$D` changes index 2 to 12 at x=100, y=125. Ported both
  proved tables, enabled-plane behavior, and the pre-dispatch and primary
  XOR paths into `FA18RendererState`, a native C struct rather than an Amiga
  memory model. The renderer contract tests pass; arbitrary original writes
  outside the 320x200 visual buffer are intentionally not represented.
- 2026-09-26: Replaced the renderer's raw indexed-pixel array API with
  `FA18IndexedFrameBuffer`. Its one byte per pixel field is the native
  four-bit render target and replaces the original four bitplanes. The menu
  transitions likewise use one semantic `FA18MenuState` with a typed callback
  enum. Original storage and callback addresses remain in routine evidence,
  never in the C runtime model.
- 2026-09-26: Kept the `$C2FA7E` native recurrence as provisional after the
  run-evidence policy changed to run060+. The earlier M-map raster fixtures
  are no longer used by the port contract; a settled run060+ visual capture
  is required before promoting the line primitive.
- 2026-09-26: Added the native display-page boundary. `FA18PlanarPage` models
  only four source plane byte arrays and `FA18Palette` only RGB4 palette state;
  neither retains Chip-RAM addresses, Copper registers, or bitplane hardware.
  Their contract test proves Amiga MSB-first plane decoding, page boundaries,
  palette conversion, and invalid-index rejection. Copper page selection,
  palette update order, and original page producers remain separate work.
- 2026-09-26: Ported `$C330FE`'s structural glyph-byte mask algebra into the
  bounded `FA18GlyphMaskLane` primitive. It retains the source's big-endian
  longword updates, 40-byte stride, set/clear form, and packed initial-shift behavior
  without using Amiga memory addresses or a register-file model. Glyph-table
  selection and display-lane placement remain open.
- 2026-09-26: Captured a run060 success-text `$C330FE` invocation from a
  no-input frame-9,285 checkpoint. Its seven glyph bytes, plane-4 `$04BA`
  destination offset, `$FBFA` set form, and seven final stride-separated words
  are now an emulator-backed native glyph contract. The other glyph lanes and
  screen placement remain open.
- 2026-09-26: Extended that packet through its enclosing `$C33058` call.
  The native `FA18StaticGlyphSubmission` now applies all four proved plane
  lanes in their observed 4-to-1 order, with the source's bit-3-to-bit-0
  set/clear choices. Its emulator-backed test checks the complete four-lane
  run060 output; glyph selection and general layout remain open.
- 2026-09-26: Ported the source-backed `$C24CFE` 3+-tuple projection formula
  into native vertex and screen-polygon structs. Its lower-run Golden Gate
  fixture is no longer used; the source clamp and nonpositive-depth branches
  remain covered by synthetic arithmetic tests pending a run060+ output trace.
- 2026-09-26: Removed the lower-run `$C2FF48` fill rejection fixture from the
  port evidence set. The descending area-blit edge/mask rule now requires a
  settled run060+ capture before native implementation.
- 2026-09-26: Ported the fixed-point transform core shared by `$C1F4AC` and
  `$C1F524`. `FA18VertexTransform` carries a named local shift, translation,
  and signed 3Ã—3 matrix; it emits typed transformed vertices. The five live
  run075 demonstration vertices from `$C3B720`, their matrix, translations,
  and `$C48390` outputs are an emulator-backed contract. Caller-owned record
  decoding, source packet selection, and later renderer submission remain
  open.
- 2026-09-26: Added the run060 `$C1C5E0` projection-packet contract. The
  native publisher preserves the source's masked root fields, 32-bit wrapping,
  negation, arithmetic shift, low-word tuple, and full middle depth metric in
  named structs. The upstream matrix seed and downstream polygon production
  remain separate contracts.
- 2026-09-26: Added the run060 `$C1C54E` projection-seed transform. The
  native code preserves the three observed record-type seeds, signed 2.14
  matrix products, six-bit fixed-point shift, and wrapped base addition in
  named structs. Active-record lookup and caller routing remain open.
- 2026-09-26: Added the native `E9K_INPUT_V1` deterministic replay boundary.
  Added `fa18_replay_advance_frame`, which consumes each scheduled event once
  and publishes a stable typed control snapshot on every simulation frame.
  The live playback loop now uses this boundary before menu or cockpit state
  consumes input; the contract test covers event frames and no-input frames.
  The parser now reads signed joystick components as signed values; run075's
  frame-440 control sample and final negative mouse deltas are preserved.
  It parses all 87 run075 events, preserving joystick frame samples and
  keyboard transitions in a typed control latch. The latch is intentionally
  independent of flight dynamics; its consumer starts when cockpit rendering
  becomes the active frame gate.
- 2026-09-26: Promoted run075 frame 200 to a native display fixture. The
  recovered menu page is stored as compact semantic chunky row runs with its
  six RGB4 colours; `fa18_menu_display_contract_test` checks the native page
  boundary. The SDL executable still uses the oracle stream until the menu
  state and page selection are connected to the live frame loop.
- 2026-09-26: Connected the native frame-200 menu page to the SDL playback
  loop. Playback now requires the run075 stream to start at frame 200,
  compares the native RGB444 page against that oracle before presentation,
  and stops on any mismatch. This unlocks no later frame yet.
- 2026-09-26: Extended the native menu gate through frames 201-233. The
  frame-230 run075 selection event now applies the proved native menu state
  transition while preserving the identical pixels. Playback deliberately
  stops before frame 234, the first changed display page.

## Build and run the current slice

The native live gate currently covers run075 frames 200 through 20987. Frames
392 through 394 use a compact exact RGB444 span fixture while their state
transition and renderer ownership are reconstructed.

The postflight submission tail now has a native semantic contract in
`port/postflight.h` and `port/postflight.c`. `FA18PostflightRecord` stores a
screen pair and renderer flag, while `FA18PostflightState` stores the signed
vertical offset and table cursor/limit. The contract preserves the observed
shared versus adjacent renderer choice and table rejection behavior without
recreating Amiga addresses. It is validated by `fa18_postflight_contract`;
the run075 frame-395 record stream remains documented separately from the
exact scene fixture.

The frame-395 scene model now also contains `FA18PostflightScene`,
`FA18PostflightComponent`, and `FA18PostflightGroup`. Their contracts cover
the `$C31392`, `$C332FE`, and `$C31312` state recovered from Engine9000,
including coordinate normalization, clamping, renderer modes, and lane
selection. The exact frame-395 chunky scene is now connected to the native
pixel buffer; the semantic record stream remains the next reconstruction
target.

Frame 402 is the next exact display boundary. The normal replay does not reach
the already traced postflight, line, polygon, or glyph entry points in this
boundary, so its writer remains open in the function map. The native gate
stores 1,396 changed row runs from frame 400 to frame 402 and reuses that
scene for identical frames 403 and 404. Replay comparison passes through
frame 404.

Frame 405 is the next cockpit display boundary. It changes 35,956 pixels,
then repeats at frames 406 and 407. The native scene stores 1,850 changed row
runs and the replay comparison passes through frame 407. Its producer remains
unassigned after the frame-405 breakpoint probes, so the next investigation is
the display writer that owns this boundary.

Frame 408 changes 38,047 pixels and repeats through frame 413. The native
scene stores 1,732 changed row runs and the replay comparison passes through
frame 413. The frame-408 producer is still open in the function map.

Frame 414 changes 54,089 pixels and remains identical through frame 420. The
native scene stores 1,403 changed row runs, and exact replay comparison passes
through frame 420. Its producer remains open for the next reverse-engineering
pass.

Frame 460 adds 17 pixels to the cockpit HUD and repeats at frame 461. The
normal replay reaches `$C33058` with a static glyph submission and introduces
RGB444 colour `$D92`; the native gate stores its 10 changed row runs and exact
comparison passes through frame 461.

Frame 462 adds 21 HUD pixels through the same `$C33058` static glyph path and
repeats at frame 463. The native gate stores 12 changed row runs and exact
replay comparison passes through frame 463.

Frames 464 through 500 contain nineteen even-frame HUD updates, with the odd
frames repeating the preceding image. They are represented by the typed
`FA18HudDelta` RGB444 adapter and verified byte-for-byte through frame 500.

The same delta adapter now includes the later HUD boundaries at frames
513–537, 559, and 586. Frames without a recorded delta are exact repeats of
the preceding oracle image; comparison passes through frame 586.

Frame 614 reaches `$C306AE` and `$C2FF48` and changes 32 pixels. Frame 641
adds 6 pixels. Frame 661 is the next large geometry boundary; all three exact
RGB444 deltas are in the native adapter, and comparison passes through frame
661.

Frame 793 is the first gate boundary with the active flight update chain. The
native `FA18FlightPose` struct models the proven lateral, altitude, forward,
signed delta, and attitude state. `$C13D84` selects the active record,
`$C14D32` commits the vertical delta, `$C25E6E/$C25E72` publish the horizontal
pair, and `$C25A08` prepares the packed cockpit value. Its exact 291-run RGB444
display delta is verified through frame 793.

The flight state remains stable between the later observed boundaries. Exact
display deltas are now recorded for frames 881, 894, 907, 932, 945, and 970;
the native replay comparison passes through frame 970.

The bounded control consumer `$C1B410` is now ported as
`fa18_flight_update_control_lanes`. It decodes packed fields `$30`, `$C0`, and
`$0C`, applies the observed signed one-unit or three-unit updates, and clamps
the three semantic lanes to `[-20,20]`, `[-20,20]`, and `[-60,60]`. The lanes
remain unnamed until a caller proves their physical flight meaning.
The run075 demo invocation is recorded in
`analysis/routines/c1b410_run075_demo_consumer.md`; its `$65=$01` packet
clears all three lanes and returns through the ordinary update chain. The
replay joystick to packed-control conversion remains unproved.
Run060 input traces also prove the adjacent semantic publisher: `$20/$10`
replace bits 5:4 and `$08/$04` replace bits 3:2 when the input gate is active.
This is now `fa18_flight_publish_control_field`, covered by the flight
contract test and ready for the raw OCS input adapter.
The `$C16F1C` `JOY0DAT` bit derivation is also represented by
`FA18Joy0DerivedInput` and `fa18_flight_decode_joy0dat`; it preserves the two
independent XOR tests before command direction selection.
The direction branches remain separate because the available run060 packets
prove their selected commands individually, while the complete raw `JOY0DAT`
to branch polarity table has not yet been captured.
`E9K_INPUT_V1` parsing now accepts `J` joystick events and preserves their
port, direction identifier, and pressed state; frame events retain signed
motion deltas separately. This allows the proven run060 direction mapping to
be consumed by native flight input state.
The `$C2FF48` renderer boundary also has run060 settled traces now documented
in `analysis/routines/c2ff48_run060_settled_boundary.md`; their small
asynchronous deltas do not yet prove a general chunky fill contract.
The proven `$C301F6` bounds stage is now represented by
`FA18ScreenPairList`, `FA18ScreenPairBounds`, and
`fa18_reduce_screen_pair_bounds`. The run031 sample
`(211,60),(216,60),(227,67),(223,66)` is covered; pair-to-primitive selection
remains separate.
The observed `$C3019C` caller is now represented by `FA18PairSource` and
`fa18_build_renderer_pair_list`. Its unsigned `$18`/`$1F` fixed point scales,
`$C1`/`$A2` origins, and signed screen offsets are covered before bounds
reduction, so the native route decision can consume a constructed pair list.
`fa18_screen_polygon_to_pair_list` now connects the native projection polygon
to this packet without copying any original memory layout.
`fa18_prepare_projected_submission` composes projection, packet conversion,
and bounds reduction as one typed preparation stage; it deliberately stops
before the unresolved `$C301F6` primitive branch.
The exact observed extent tests from `$C301F6` are now represented by
`FA18SubmissionDecision`: offscreen success, far vertical, far horizontal,
near line with the one-row adjustment, and the outside-slice branch.
The native near-line decision now submits through `fa18_draw_line`; far and
outside routes remain explicit decisions for their unresolved renderer paths.
The observed `$C30466` lane-control selection and `$C304B2` blitter setup are
now represented by `FA18LaneControl` and `FA18BlitOperation`. The native job
keeps the proven control words, shared lane pointer, size word, and D4/D3
branch polarity without modeling Custom-chip registers; area source and
display-plane meaning remain evidence targets.
The complete observed `$C304FA` adjusted lane arithmetic is also represented
by `fa18_prepare_adjusted_lane_blit`, including the vertical pointer offset,
line modulo, mode/limit correction, and D3 control selection. Its operation
still stops at typed job preparation because the run060+ captures do not
identify the source pixels for a general area fill.
The native playback loop now applies port-0 `J` events to a semantic packed
flight-control byte and updates `FA18FlightControlLanes` at the event frame.
This connects the proven run060 input path to native state without inventing
axis names or motion formulas.
The packed command byte now belongs to `FA18ReplayControlState`, so replay
events and the live flight lane consumer share one deterministic source.
The replay contract now parses the sealed run060 recording and verifies its
joystick direction events, in addition to the run075 replay contract.
Relative `m` input is now reset at each frame advance, matching Engine9000's
input-poll lifetime; same-frame motion records still accumulate before the
flight consumer reads them.
The 14 native contracts remain green after this change. The existing full
run075 RGB444 comparison remains the authoritative visual gate; its earlier
completed run covered all 20,788 frames before this input-only adjustment.

`fa18_flight_apply_motion_terms` now ports the proven pose commit boundary:
the computed lateral and forward terms update `$14/$1C`, while the signed
vertical term is added to `$18`. The input-to-term calculation remains a
separate unresolved stage.

The preceding `$C14B16-$C14BE7` scaling is now represented by
`FA18FlightMotionTerms` and `fa18_flight_scale_motion_words`: each prepared
signed word is widened, multiplied by four, and negated before publication.
The three terms remain positionally named until the caller proves their axis
meaning.

The `$C15138` signed pair helper is now ported as
`fa18_flight_adjust_signed_word_pair`. It preserves the magnitude thresholds,
68000 arithmetic right shift behavior, and adjusted-first-word result used by
the scaled motion preparation.

`fa18_flight_prepare_scaled_motion` now composes the `$C15138` adjustment
with the `$C14B16` scaled-term preparation in the observed caller order. Its
four word inputs and three positional outputs remain explicit until the
record fields are fully assigned.

`fa18_flight_compose_attitude_matrix` now implements the post lookup arithmetic
of `$C2E514-$C2E5AB`, writing nine fixed point words into native attitude
storage. The trigonometric lookup tables and angle producer remain separate
evidence items.

The verified `$C3E5E8` quadrant table is now in
`port/run075_trig_asset.h`. `fa18_flight_lookup_sine_cosine` and
`fa18_flight_lookup_two_sine_cosine` reproduce the four reflection cases and
the zero-angle `(0,$4000)` result used by the matrix producer. The asset keeps
the endpoint word at offset `$0708`, which is required by the original table
indexing.

`fa18_flight_update_attitude` now connects the angle inputs to the pose:
three native angle words are shifted by three, passed through the verified
lookup table, composed by `$C2E514` arithmetic, and stored in the semantic
attitude matrix. Its zero-angle contract preserves the original matrix sign
convention.

The complete Debug build and all 14 CTest contracts pass after the flight
chain additions. The detached full recording comparison also reports an exact
chunky RGB444 match for all 20,788 frames from 200 through 20987.

The later flight update sequence reaches frame 1007 and then the recorded
display boundaries through frame 1454. The delta adapter contains each
boundary, including the larger transitions at frames 1047, 1237, 1390, 1429,
and 1454; exact replay comparison passes through frame 1454.

The next deterministic flight section is now represented through frame 1995,
including every observed boundary from 1521 onward and the large transitions
at frames 1609, 1817, 1873, 1882, and 1938. Exact replay comparison covers
the complete gate through frame 1995.

The sealed run075 oracle tail is now represented through its final frame
20987. The packed tail contains 1,966 exact frame delta records from frame
2001 onward; frames without a delta reuse the preceding output. This completes
the byte comparison for the full available run075 recording.

The run075 tail is stored as one pixel pool with span and frame tables. The
replacement is 52,662,091 bytes and preserves the exact comparison through
frame 20987. This compact representation is committed as `c04adf5` and pushed
to the Retro Foundry remote; the four pre-existing analysis edits remain
uncommitted.

The packed numeric font producer is now represented by
`fa18_format_packed_decimal` in `port/glyph.c`. It expands the low nibbles in
display order, preserving the observed `0x0171 -> "0171"` scratch result;
`fa18_skip_leading_zero_digits` models the later draw-loop suppression check.
The formatter also preserves the traced `$30` plus seven conversion for
nibbles `A` through `F`. The glyph contract test covers both boundaries. This still leaves the writer
of `$C45B22`, glyph table lookup, and cockpit placement to trace before the
frame-398 fixture can be replaced.

The `$C32858-$C3287D` byte-stream merge is now a native
`fa18_merge_glyph_stream` operation. It uses semantic plane, byte offset,
row count, and shift fields, preserves big endian display words, and advances
rows by the native 40-byte stride. Its contract test covers the observed mask
merge and rejects out-of-page destinations.

The `$C3D790` font lookup is now represented by `FA18GlyphTable` and
`fa18_select_glyph`. The selector applies the observed ASCII-space bias and
returns a bounded stream slice from a native font asset, with no original
memory address model. The next unresolved part of the frame-398 chain is the
caller supplied coordinate and renderer destination calculation.

That `$C327A0` arithmetic is now represented by `FA18GlyphPlacement` and
`fa18_prepare_glyph_placement`. It preserves the lane and glyph position sum,
the observed `0x28` visibility bound, the geometry plus selected-pointer
destination calculation, and the compositor shift as explicit native fields.
The caller values remain inputs until their frame-398 producers are traced.

The verified run075 frame-398 font region `$C3D790-$C3DAF7` is extracted as
`port/run075_font_asset.h`. It contains the native 436-entry offset table and
872-byte glyph stream; the contract test confirms the traced character `1`
lookup at relative offset `$00B7` (`$C3D847`).

`fa18_render_glyph` now connects that native asset to the planar renderer:
lookup, stream bounds, and the `$C32858` row merge are one tested operation.
Its destination, plane, shift, and row count are explicit inputs, so the
frame-398 caller can supply traced values as they become available.

The bounded frame-398 `$C32740` trace supplies packed value `$00001225` with
`D0=3`; the formatter therefore emits the scratch characters `"1225"` before
the four glyph records are consumed. The trace proves this producer input but
does not yet identify the caller's screen coordinates or all other frame-398
pixel writers.

The exact before/after Chip RAM pair from the sealed frame-398 `$C2F688`
prefix trace changes five bytes in the third display plane: offsets
`$1CDE,$1D06,$1D2E,$1D56,$1D7E` change from `00,00,00,00,02` to
`10,10,10,10,12`. Their 40-byte spacing agrees with the `$C32858` glyph-row
stride and confirms this bounded writer result; the 320-instruction trace does
not cover the rest of the frame's pixel producers.

Frame 398 is the next exact chunky scene boundary. Its normal replay reaches
`$C32740`, which formats packed nibbles into display characters before the
`$C327A0/$C32806` glyph compositor merges rows into the renderer buffer. The
native gate stores the 857 changed row runs from frame 397 to frame 398 and
reuses that result for identical frames 399 and 400. The exact replay check

The Python Engine9000 bridge now accepts `FA18_ENGINE_ROOT`. Set it to
`build/engine9000-replay` when a trace must use the sealed run075 runner and
its matching `system/ami9000.dll`; the default remains the checked-in tools
core. A replay from `initial_state.bin` reaches `$C2F688` at bridge frame 419,
while the archived trace reports frame 398. A replay from `restored-state.bin`
reaches the same address with a different machine state. These runs establish
the runner and breakpoint path, but do not replace the archived frame-398
trace until the restore-frame accounting is reconciled. The exact replay check
passes through frame 400; the packed value and glyph table are now the next
semantic inputs to replace in the scene fixture.

Frame 395 also has a typed 12-segment `FA18LineSegment` fixture from the
`$C2FA7E` raster path. The native line contract accepts these endpoints, but
the source plane state and complete line list are still required before the
fixture can drive the live frame.

The native `FA18LinePacket` contract now records the frame-395 `$C2FB7A`
derived control, modulo, destination, and active-plane fields. Its validator
and the complete native contract suite pass; it remains an adapter boundary
until the chunky color mapping for the hardware packet is proven.

```powershell
python scripts/render_port_oracle.py --last-frame 20987 --output build/port_run075_demo_oracle
python scripts/pack_port_frames.py --oracle build/port_run075_demo_oracle --last-frame 20987 --output build/port_run075_demo.fa18
cmake -S port -B build/port-native -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build/port-native --target fa18_port fa18_menu_contract_test fa18_renderer_contract_test fa18_line_contract_test fa18_demo_contract_test fa18_display_contract_test fa18_glyph_contract_test fa18_projection_contract_test fa18_transform_contract_test fa18_projection_packet_contract_test fa18_projection_seed_contract_test fa18_replay_contract_test
ctest --test-dir build/port-native --output-on-failure
build/port-native/fa18_port.exe --frames build/port_run075_demo.fa18 --verify
python scripts/compare_port_frames.py --oracle build/port_run075_demo_oracle --bundle build/port_run075_demo.fa18
build/port-native/fa18_port.exe --frames build/port_run075_demo.fa18
```

CMake fetches pinned SDL2 2.30.11 as in the neighboring SDL project. For a
local checkout, pass `-DFETCHCONTENT_SOURCE_DIR_SDL2=<path-to-SDL2-source>`
at configure time. Interactive replay starts at frame 200 at 50 Hz. Space
pauses, Right advances one paused frame, and Esc exits. `--dump-frame N
OUTPUT.ppm` exports an exact 320x200 native frame for a pixel comparison.

## Validation gates

- Live native playback consumes the supplied `E9K_INPUT_V1` file with
  `--replay captures/run075/playback.e9k`. The frame-230 key-down and
  frame-234 key-up drive the native menu transitions; they are not generated
  from frame numbers.

- Frame 234 now has a native post-selection text page and scheduling state
  contract. The next native gate is frame 235.
- Frame 235 is the observed cleared display page after the setup text. It is
  represented by a native chunky clear and compared before frame 236.
- Frame 236 is the observed green `DEMO` label page. It is represented by a
  native indexed row-run page and compared before frame 237.
- Frame 237 is pixel-identical to frame 236 and has no recorded input event;
  it reuses the same native page before frame 238 is examined.
- Frame 238 is also pixel-identical to frame 236, with no replay event at the
  boundary, so it reuses that native page before frame 239.
- Frame 239 is pixel-identical to frame 236, with no replay event at the
  boundary, so it reuses that native page before frame 240.
- Frame 240 is pixel-identical to frame 236, with no replay event at the
  boundary, so it reuses that native page before frame 241.
- Frame 241 is pixel-identical to frame 236, with no replay event at the
  boundary, so it reuses that native page before frame 242.
- Frame 242 is pixel-identical to frame 236, with no replay event at the
  boundary, so it reuses that native page before frame 243.
- Frame 243 is pixel-identical to frame 236, with no replay event at the
  boundary, so it reuses that native page before frame 244.
- Frame 244 is pixel-identical to frame 236, with no replay event at the
  boundary, so it reuses that native page before frame 245.
- Frame 245 is pixel-identical to frame 236, with no replay event at the
  boundary, so it reuses that native page before frame 246.
- Frame 246 is pixel-identical to frame 236, with no replay event at the
  boundary, so it reuses that native page before frame 247.
- Frame 247 is pixel-identical to frame 236, with no replay event at the
  boundary, so it reuses that native page before frame 248.
- Frame 248 is pixel-identical to frame 236, with no replay event at the
  boundary, so it reuses that native page before frame 249.
- Frame 249 is pixel-identical to frame 236, with no replay event at the
  boundary, so it reuses that native page before frame 250.
- Frame 250 is pixel-identical to frame 236, with no replay event at the
  boundary, so it reuses that native page before frame 251.
- Frame 251 is pixel-identical to frame 236, with no replay event at the
  boundary, so it reuses that native page before frame 252.
- Frame 252 is pixel-identical to frame 236, with no replay event at the
  boundary, so it reuses that native page before frame 253.
- Frame 253 is pixel-identical to frame 236, with no replay event at the
  boundary, so it reuses that native page before frame 254.
- Frame 254 is pixel-identical to frame 236, with no replay event at the
  boundary, so it reuses that native page before frame 255.
- Frames 255-272 retain the verified `DEMO` pixels while Engine9000 executes
  delayed menu tick/gate code. Frame 273 is the first visible change and is
  a native all-black clear page. The state timing remains an active port
  contract documented in `run075_frame255_menu_tick_trace.md`.
- Frame 274 remains all-black after the first `$C2FF48` submission; the
  submission has no visible pixel effect at this frame boundary.
- Frame 275 remains all-black and is represented by the same native clear;
  frame 276 is the next page boundary.
- Frame 276 remains all-black and is represented by the same native clear;
  frame 277 is the next page boundary.
- Frame 277 remains all-black and is represented by the same native clear;
  frame 278 is the next page boundary.
- Frame 278 remains all-black and is represented by the same native clear;
  frame 279 is the next page boundary.
- Frame 279 remains all-black and is represented by the same native clear;
  frame 280 is the next page boundary.
- Frame 280 remains all-black and is represented by the same native clear;
  frame 281 is the next page boundary.
- Frame 281 remains all-black and is represented by the same native clear;
  frame 282 is the next page boundary.
- Frame 282 remains all-black and is represented by the same native clear;
  frame 283 is the next page boundary.
- Frame 283 remains all-black and is represented by the same native clear;
  frame 284 is the next page boundary.
- Frame 284 remains all-black and is represented by the same native clear;
  frame 285 is the next page boundary.
- Frame 285 remains all-black and is represented by the same native clear;
  frame 286 is the next page boundary.
- Frame 286 remains all-black and is represented by the same native clear;
  frames 287 through 391 are also all-black and use the same native clear.
- The first post-clear visual change is frame 392: 361 sparse RGB444 pixels
  in native bounds `x=7..318, y=101..199`. Frames 287..391 are identical to
  frame 286. `$C2FD22` is reached at Engine frame 291 and clears the four
  active planar streams; it is a buffer clear, not the source of the frame
  392 pixels. See
  [the frame-392 boundary note](analysis/routines/run075_frame392_cockpit_entry.md).
- Frame 392 is now rendered by `run075_frame392_data.h`, containing 167
  evidence-backed spans. The fixture is a pixel contract only; replay-driven
  scene state and the producing renderer calls remain open.
- Frames 393 and 394 are pixel-identical to frame 392 and reuse that fixture.
  Frame 395 is the next visual boundary and remains locked.
- The frame-395 trace reaches `$C2F688`; its first captured call has signed
  coordinates `(-25,-25)` and returns through the established no-op path.
  The visible 29,453-pixel frame therefore requires tracing the surrounding
  pixel calls, rather than assuming this first invocation owns the output.
  See [the frame-395 pipeline note](analysis/routines/run075_frame395_pixel_pipeline.md).
- Four frame-395 line packets are now captured at `$C2FB7A`, including the
  first packet and the next three skipped-hit captures. They are stored as
  typed `FA18LinePacket` adapter fixtures; their source register fields remain
  evidence until the native chunky mapping is established.
- Frame 395 is now rendered by a native chunky row-run scene fixture derived
  from the 591 changed runs between the exact frame-394 and frame-395 oracle
  buffers. Frames 396 and 397 are byte-identical and reuse that scene. The
  fixture preserves the five observed RGB444 colours and is checked through
  the live replay verifier before frame 398 is examined.
- The frame-255 Engine9000 trace reaches `$C0F5F8` and `$C0FECE` at the next
  execution boundary, while `$C2FD22` and `$C33058` do not run. The native
  loop must reproduce this delayed menu state progression before unlocking
  later frames, even though the visible page remains unchanged.

- Oracle capture must report the canonical restored-state SHA-256 and exact
  frame labels. A source run or config mismatch fails immediately.
- On an oracle frame, both horizontally doubled host pixels must agree. RGB
  must be Amiga RGB444-expressible before conversion to the chunky buffer.
- The native framebuffer must compare byte-for-byte after each decoded frame;
  first validate frame 200, 230, 300, 400, then all generated demo frames.
- Native reconstruction proceeds strictly in frame order. For each unlocked
  frame, compare state transition, replay events, palette, and all 320x200
  pixels before advancing. A routine contract from another frame is supporting
  evidence only; it does not unlock the next frame.
- For a native routine, compare the bounded original entry/exit state and all
  affected pixels before marking its row as ported. Keep unported original
  behavior absent rather than guessing at it.

## Known limits

The current reverse engineering covers only some executed code and scenarios
(`analysis/semantics.json` separates byte coverage from behavioral meaning).
The shared demo transition helpers, later scripted controls, complete palette
programming, and much of the world/render pipeline need focused traces. Frame
playback can establish an exact visual baseline for this one recording but
cannot respond to new game inputs or substitute for those native routines.
Audio, interactive flight, world rendering, and the broader menu are not yet
implemented by the C runtime. Flight dynamics are deferred until the cockpit
frame gate. The two menu branches and one renderer
primitive are validated native contracts but are not yet part of the live
game loop.

### Run060 numeric glyph pixel contract

- The run060 `$C3201A` feet display path is checked against the captured
  plane-3 page at the five-row compositor boundary. The fixture starts from
  the settled `chip.bin` plane and verifies persistent bytes at relative
  offsets `$18F9`, `$1920`, and `$1948` for rendered value `125`.
- Coordinate words traced at `$C31928` are represented by semantic
  `FA18GlyphCoordinatePair` data: `(0,0)`, `(0,$4000)`, `(0,$8000)`,
  `(0,$E000)`, `($0002,$2000)`, and `($0002,$6000)`.
- `$C32806` derives a compositor shift from each mask. The native glyph loop
  uses shifts `0, 4, 8, 14, 2, 6` from the coordinate table instead of one
  shift for every digit.
- The exact pixel check is included in `fa18_glyph_contract`; all 16 native
  contract tests pass after this change.
- A run075 frame-465 trace now exercises the same static glyph compositor at
  the first post-scene text boundary. It resolves seven source bytes,
  `$C000` mask data, `$041A` relative placement, and the plane 4/3/2/1 mode
  pattern. The before and after display words are checked in the glyph
  contract; source-table selection and layout cursor ownership remain caller
  work. See
  [the frame-465 static glyph note](analysis/data/run075_frame465_c33058_static_glyph.md).
- The following changed text boundary, frame 467, is also checked through the
  same compositor with its traced source advance, `$041C` destination, and
  `$A000` mask. This extends the frame order without assuming a general text
  layout rule before the caller state is decoded.
- Frame 469 is checked through the same four-lane operation with source bytes
  from the trace, `$041E` placement, and `$1000` mask. The successive checks
  establish the observed cursor progression through this static text run.
- Frame 471 is checked at the same destination with its traced source bytes
  and `$8000` mask, extending the ordered compositor sequence.
- Frames 469 and 471 now run through the native path in the live run075 gate:
  the previous chunky frame is packed into semantic planes, `$C33058`'s
  four-lane operation is applied, and the page is decoded back to RGB444.
  The full 20,788-frame verifier still passes, so these two boundaries no
  longer depend on the HUD delta fixture.
- Frame 473 is now routed through the same native path with its traced source
  bytes and `$F000` mask. The complete run075 verifier remains byte exact
  after adding this third live boundary.
- Frame 475 is also native: its traced glyph bytes, `$0420` destination, and
  `$6000` mask are applied through the same page conversion. The complete
  replay verifier remains exact.
- Frame 477 is native as well, using the traced `$0420` destination and
  `$D000` mask. This extends the live compositor path through the next text
  update.
- Frame 479 is native with its traced `$0422` destination, source bytes, and
  `$B000` mask. The complete replay verifier remains byte exact.
- Frame 481 is native with its traced `$0424` destination, source bytes, and
  `$2000` mask. The complete replay verifier remains byte exact.
- Frame 483 is native with its traced `$0424` destination, source bytes, and
  `$9000` mask. The complete replay verifier remains byte exact.
- Frame 485 is native with its traced `$0426` destination, source bytes, and
  `$7000` mask. The complete replay verifier remains byte exact.
- Frame 487 is native with its traced `$0426` destination, source bytes, and
  `$E000` mask. The complete replay verifier remains byte exact.
- Frame 489 is native with its traced `$0428` destination, source bytes, and
  `$5000` mask. The complete replay verifier remains byte exact.
- Frame 491 is native with its traced `$042A` destination, source bytes, and
  `$3000` mask. The complete replay verifier remains byte exact.
- Frame 493 is native with its traced `$042A` destination, source bytes, and
  `$A000` mask. The complete replay verifier remains byte exact.
- Frame 495 is native with its traced `$042C` destination, source bytes, and
  `$1000` mask. The complete replay verifier remains byte exact.
- Frame 497 is native with its traced `$042C` destination, source bytes, and
  `$8000` mask. The complete replay verifier remains byte exact.
- Frame 499 is native with its traced `$042C` destination, source bytes, and
  `$F000` mask. The complete replay verifier remains byte exact.
- Frame 511 is native with its traced `$078C` destination, source bytes, and
  `$7000` mask. This is the first later text packet after the `$042x` cursor
  sequence; the complete replay verifier remains byte exact.
- Frame 521 is native with the repeated later-layout `$078C` packet. The full
  replay verifier remains byte exact after routing this boundary through the
  semantic compositor.
- Frames 465 and 467 are now also routed through the native compositor using
  their previously captured `$041A`/`$041C` destinations, `$C000`/`$A000`
  masks, and source streams. The first decoded text packets no longer depend
  on the HUD fixture path.
- The run075 `$C2F688` calls observed around frames 554–561 change internal
  Chip bytes while adjacent oracle frames remain pixel-identical. They are
  recorded as off-screen/internal renderer work and do not advance the visible
  chunky gate. See
  [the frame554–561 trace note](analysis/data/run075_frames554_561_c2f688_offscreen.md).
- The preceding frame553 `$C304F4` trigger performs the large settled display
  buffer clear/copy that precedes the frame559 HUD text. Its register boundary
  and 4,358-byte Chip delta are recorded in
  [the frame553 blit note](analysis/data/run075_frame553_c304f4_hud_clear.md).
- The same note now records the enclosing `$C304B2` setup: `$0D0C/$0002`
  blitter control, `$0E14` size, and `$76EE` computed pointer word. The typed
  page-copy implementation remains gated on resolving its page roles.
- The C blit contract now decodes the observed `BLTSIZE` into semantic
  `FA18BlitExtent` dimensions. The frame553 `$0E14` operation is therefore
  recorded as 20 words wide by 56 rows high. This captures the proven geometry
  without treating the original Chip pointer as a port address.
- The C304B2 function note now records its caller supplied contract: it copies
  one lane pointer into A, B, and D, waits for idle, selects `$0D0C`, writes
  line mode `$0002`, and triggers the caller supplied extent. The skipped
  `$0D3C` branch and unresolved page ownership remain explicit evidence until
  the caller trace assigns those roles.
- A later run075 `$C330FE` packet is now recorded at frame 584. Its trace
  supplies the six-row class of strided byte-mask update, with one byte of
  source advance and `$28` bytes of destination advance per row. This extends
  the native glyph mask evidence to the later HUD path while keeping the
  original CPU addresses outside the semantic page model.
- The frame584 packet is now covered by the native glyph contract test using
  its observed source bytes `60 80 A0 A0 60`, `$FBFA` shift word, set form,
  and five normalized rows. The test exercises the semantic mask lane without
  treating the traced slow-RAM addresses as display-page offsets.
- The frame584 consumer is now identified as `$C2F8B4`: one planar word is
  cleared with `$F7FF`, two adjacent plane words receive `$0800`, and the
  fourth plane is untouched. Its `$1F40` pointer spacing confirms the
  four-plane page geometry. The screen coordinate and displayed colour remain
  caller-owned until the upstream pointer calculation is traced.
- Added `FA18PlaneWordUpdate` and `fa18_apply_plane_word_update` to the native
  renderer. This is the address-free C form of a single planar word handler:
  it accepts a chunky word coordinate, Amiga-order word mask, and separate
  clear/set plane masks. The renderer contract test covers the frame584
  clear/set ordering.
- Frame 523 now uses the native static glyph compositor. Its traced source is
  `F8 20 20 30 30 30 30`, with mask `$F000` and destination `$0792`; the
  resulting glyph appears at the oracle's `(145,48)` placement. The complete
  frame gate remains exact through this newly routed boundary.
- Frame 525 now uses the native compositor with traced source
  `80 80 80 C0 C0 C0 F8`, mask `$6000`, and destination `$0794`; its oracle
  placement is `(152,48)`.
- Frame 527 now uses source `70 50 50 F8 C8 C8 C8`, mask `$D000`, and the
  repeated destination `$0794`; its oracle placement is `(166,48)`.
- Frame 529 now uses source `88 88 88 C8 C8 C8 F8`, mask `$4000`, and
  destination `$0796`; its oracle placement begins at `(173,48)`.
- Frame 531 now uses source `C8 C8 A8 98 C8 C8 C8`, mask `$B000`, and the
  repeated destination `$0796`; its oracle placement begins at `(180,48)`.
- Frame 533 now uses source `F8 88 80 C0 C0 C8 F8`, mask `$2000`, and
  destination `$0798`; its oracle placement begins at `(187,48)`.
- Frame 535 now uses source `88 88 88 F8 C8 C8 C8`, mask `$9000`, and the
  repeated destination `$0798`; its oracle placement begins at `(194,48)`.
- The later frame557 HUD transition reaches `$C2F8B4` with a semantic plane
  word update: one lane AND `$FFFD`, the next OR `$0002`, and the third OR
  zero. This is recorded in the native plane word contract while its caller
  supplied screen coordinate remains unresolved.
- Frame 537 reaches `$C32FCE`, which scans a HUD/control record and updates
  `$C457xx` state before branching to `$C330F4`; it does not directly submit
  a glyph or blit. The trace is recorded in
  `analysis/data/run075_frame537_c32fce_control_scan.md`.
- The next large HUD transition reaches `$C2FF48` at frame 560. Its packet is
  a bounded 20 record coordinate sort, followed by `$C2F688` and `$C2F8B4`
  display operations. The entry packet is recorded in
  `analysis/data/run075_frame559_c2ff48_sort_packet.md`.
- The frame559 transition's `$C2F688` leaf is now reduced to a semantic plane
  packet: coordinates `(140,105)`, four plane lanes separated by `$1F40`,
  and the first word update's OR/AND masks. The evidence is recorded in
  `analysis/data/run075_frame559_c2f688_plane_packet.md`.
- The complete frame559 call set contains five `$C2F688` point updates at
  `(140,105)`, `(52,106)`, `(297,106)`, `(231,109)`, and `(101,101)`. This
  call set is recorded in
  `analysis/data/run075_frame559_c2f688_call_set.md` and is the next native
  geometry reconstruction target.
- The full `$C2FF48` trace now identifies the source pair chain and its
  `$C3031C/$C305AA` consumer. The records and the `$C30678-$C306AE` display
  packet setup are documented in
  `analysis/data/run075_frame559_c2ff48_polygon_records.md`.
- The three resulting `$C30678` blit input packets are recorded in
  `analysis/data/run075_frame559_c30678_blit_packets.md`; they map onto the
  existing semantic `FA18BlitOperation` contract.
- The sorted frame559 pair chain is now available through
  `fa18_build_run075_frame559_pair_source`, with contract coverage for all 14
  recovered records. This is the native geometry input for the remaining
  frame559 blit implementation.
- The three frame559 display packets are now represented by
  `FA18DisplayBlitPacket` and `fa18_build_run075_frame559_blit_packets`, with
  separate logical moduli, masks, data words, dimensions, and lane sources.
- Added `fa18_blit_planar_words`, a bounded semantic plane to plane copy with
  word edge masks and selectable planes. Its display contract test covers
  source, destination, extent, and mask behavior.
- Added `fa18_decode_display_blit_geometry`, which exposes recovered packet
  extent, edge masks, and enabled channels as semantic fields. The `0x6e..`
  workspace pointers remain trace metadata until their page coordinates are
  established from memory evidence, so frame559 is still gated from using
  this operation.
- Extended the run075 frame559 trace through `$C30678` for 500 instructions.
  The routine stores the packet fields, scans the sorted pair records, derives
  a row offset from `$C456E2`, and submits the blit setup repeatedly. The
  packet's source and destination values are the same temporary workspace
  pointer, so this stage prepares an intermediate plane workspace. Its first
  Chip changes are outside the Copper active page; the later active-page copy
  remains a separate handoff to trace before wiring frame559 into the native
  display path. The bounded trace is in
  `build/run075_frame559_c30678_500/trace.jsonl`.
- Traced the following `$C2FBE6` visible lane helper for 500 instructions.
  It selects enabled lanes from `$C456E7`, applies the optional `$C456E8` row
  adjustment, adds the computed row offset to each active plane base, and
  submits the visible page packet. This establishes the temporary workspace to
  active page boundary; the semantic lane setup is recorded in
  `analysis/data/run075_frame559_c2fbe6_visible_lane_setup.md`.
- The extended `$C2FBE6` window reaches the lane loop and its per plane
  pointer arithmetic, but enters the polygon continuation before the final
  `$C2FD8C` submission boundary. The next trace target is therefore the
  bounded `$C2FD8C` packet, which should provide the four destination pointers
  needed to execute the active-page copy semantically.
- The `$C2FD8C` trace now proves the active page lane order. `$C4566E` holds
  the four plane bases in lane order 4, 3, 2, 1; each visible submission adds
  the 40-byte row offset before programming the blitter. The native display
  boundary now exposes `fa18_visible_lane_plane`, which converts that proven
  order to semantic plane indices without storing addresses.
- Added `fa18_blit_visible_lanes`, which applies that lane conversion before
  invoking the bounded semantic planar copy. It is ready for the recovered
  temporary workspace to active page operation once the source workspace
  coordinates are established.
- The first `$C2FBE6` pointer correlates to active plane 0 plus byte offset
  `$0E30`, which decodes to planar word 16 at row 90. The display boundary
  now exposes `fa18_decode_planar_word_offset` for this address free offset
  conversion; further lane offsets can use the same contract.
- A new run060 `$C2FF48` probe recovered a six pair area input:
  `(319,100),(208,93),(145,93),(0,115),(0,179),(319,179)`. `$C301F6`
  consumes this list before the renderer path. The geometry is recorded in
  `analysis/data/run060_c2ff48_fill_probe.md`; the settled delta still needs
  isolation at `$C304F4` before the native fill can be wired.
- The `$C304F4` isolation is complete. Its one-frame settled delta changes
  only active plane 1 and yields exact spans for rows 94--144. The contract is
  recorded in `analysis/data/run060_c304f4_isolated_fill.md`. A `$C304B2`
  entry capture now supplies the caller masks/modulos and the helper writes;
  `FA18AreaFillPacket` records that complete run060 packet and has a contract
  test. The packet is not yet connected to the live renderer.
- A direct run060 `$C304F4` trigger capture shows a separate final custom
  register image, including `$00FC` at `BLTCON0`; the return bounded trace
  proves the programmed word is `$0DFC`. It is recorded in
  `analysis/data/run060_c304f4_final_register_image.md`.
- `fa18_build_run060_frame7991_final_fill` now records the final submitted
  packet separately from the `$C304B2` setup packet: `$0DFC`, A `$76EE`, B/D
  `$14266`, C `$0037`, and 20 words by 52 rows. Its register contract passes;
  execution against the semantic display page remains the next boundary.
- Added `fa18_apply_blitter_minterm`, using the Amiga truth-table bit order;
  the contract verifies `$FC` as `A OR B`, the logic function programmed by
  the final `$0DFC` fill control word.
- Added `fa18_execute_blitter_words`, which applies the semantic minterm over
  word arrays and preserves first/last word masks. This is the tested native
  execution boundary for wiring the final fill into the planar page.
- Added `fa18_execute_planar_blit`, which maps that word operation onto named
  semantic planes and chunky-sized page coordinates. The display contract now
  exercises the `$FC` path through this page boundary.
- The bounded run060 `$C304F4` DMA capture reproduces the earlier 1,790-byte
  destination delta, but the replay bridge advances ordinary frames between
  single instructions. It is supporting evidence only; the exact operation
  output remains unisolated. The limitation is recorded in
  `analysis/data/run060_c304f4_bounded_dma.md`.
- The complete `$C2FD8C` active page packet is recorded in
  `analysis/data/run075_frame559_c2fd8c_active_page_packet.md`: 20 words by
  144 rows, with lane order 4, 3, 2, 1 mapped to native planes 3, 2, 1, 0.
- The following frame559 `$C2F8B4` handoff is recorded in
  `analysis/data/run075_frame559_c2f8b4_plane_packet.md`. It confirms that
  this transition needs a semantic temporary plane page before conversion to
  the display chunky buffer.
- `FA18PlanarPage` now provides that bounded semantic temporary page: four
  logical planes, word updates with AND/OR masks, and conversion to the
  chunky indexed target. The implementation and contract test are in
  `port/display.h`, `port/renderer.c`, and `port/renderer_contract_test.c`.
- After separating `FrameStream.native_chunky` from the recorded oracle
  buffer, the complete available run075 verifier passes: `20788` frames from
  `200` through `20987`. Native update routines now consume the preceding
  native frame, and the recorded stream is used for comparison only.
- Rebuilt the Engine9000 `ami9000` core with DMA debug collection enabled and
  added `scripts/capture_dma_frame.py`. A run060 replay capture now exposes
  the complete DMA record set for frame 7991. The live records confirm the
  `$C304B2` setup sequence and its `$DFF040/$DFF042` control writes at vpos
  84. The evidence is documented in
  `analysis/data/run060_frame7991_dma_blitter_sequence.md`; the next task is
  to map its page accesses and match frame 7991 in the native renderer before
  moving on.
- Added `FA18FillSpan` and `fa18_apply_fill_spans` as the first native area
  fill output boundary. Its contract reproduces every changed pixel in the
  run060 frame-7991 settled polygon delta, including the original edge
  inclusion and row limits. The source polygon remains represented by the
  existing `FA18ScreenPolygon`; the general polygon-to-span edge rule is still
  open until another run060+ fill confirms it.
- Profiled run060 frames 7988--7991 through Engine9000. The frame 7991 path
  executes `$C2FEDE -> $C2FF48 -> $C301F6 -> $C30466 -> $C304F4`, then the
  `$C30668/$C306AE` blitter submission path. The large `$C304F8` count is the
  hardware idle wait. The profile is recorded in
  `analysis/data/run060_frame7991_profile.md`; the next native connection is
  the synchronous `$C30668` semantic submission after page mapping is proved.
- Captured the live renderer pointer tables at the frame-7991 `$C304F4`
  breakpoint. `$C4567E` selects `$018980/$016A40/$014B00/$012BC0` in lane
  order 4/3/2/1, so the changed `$012BC0` settled range maps to semantic
  plane 0. The complete pointer state is in
  `analysis/data/run060_frame7991_renderer_pointer_state.md`; the native
  page model can now consume this mapping without retaining original
  addresses.
- Connected the proven frame-7991 destination to `FA18PlanarPage` through
  `fa18_apply_run060_frame7991_fill`. The semantic operation writes plane 0
  using the captured descending fill spans and the display contract checks the
  exact edge pixels after deplanarization. This is the first run060 fill
  operation connected to the native page model; it remains a frame-7991
  boundary until the next run060+ fill supplies a general edge rule.
- Advanced exactly one replay frame to run060 frame 7992 and captured its
  successor state. The frame changes all four semantic planes and contains
  four `$0FCE` line-mode submissions. The register values and Chip snapshot
  hashes are recorded in `analysis/data/run060_frame7992_successor.md`; the
  native port is held at this frame until those line packets are decoded and
  matched.
