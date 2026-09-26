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
| `$C2FF48` | Polygon wrapper selects line or descending area-blit routes (`analysis/routines/c2ff48_display_submission_wrapper.md`) | Not ported: the required run060+ settled fill fixture and exact edge/mask rule are still missing |
| Display page boundary | Observed cockpit and M-map pages are four 40-byte × 200-row planes with RGB4 palette state (`analysis/routines/native_planar_display_adapter.md`) | Native `FA18PlanarPage` → `FA18IndexedFrameBuffer` decoder and `FA18Palette` RGB444 application ported; page selection and palette programming remain open |
| run075 frames 200-233 menu page | Baseline menu screenshot is identical to oracle frame 200; frames 201-233 remain pixel-identical, with the frame-230 key event changing menu state only | Native menu page is rendered and compared for every frame 200-233; live playback stops at frame 234 until its changed page/state is reconstructed |
| `$C330FE` | Byte-mask update over source glyph bytes and strided display longwords (`analysis/routines/c330fe_strided_long_mask_update.md`) | Proved set/clear mask algebra ported through bounded `FA18GlyphMaskLane` and `FA18PlanarPage`; glyph records and lane placement remain open |
| `$C33058` | Static-text glyph lanes; run060 success-text entry proves four plane destinations and set/clear mask selection (`analysis/routines/c33058_static_text_glyph_lanes.md`) | Bounded post-glyph-lookup four-lane submission ported through `FA18StaticGlyphSubmission`; glyph/layout producers and general placement remain open |

## Progress

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

The native live gate currently covers run075 frames 200 through 284. Frame
285 remains locked until its changed display page is reconstructed.

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
