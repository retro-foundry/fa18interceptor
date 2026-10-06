# Native port runner

The 2026-10-06 user request starts `fa18_native`, reusing `port/game/` source
with a separate native entry point. This supersedes the earlier restriction
against starting a native runner. It does not restore the deleted `fa18_port`.

```powershell
python scripts/build_native.py
build/native/fa18_native.exe --adf local/media/fa18.adf
```

The default is an SDL window. A key acknowledges the credits. A first-time
pilot can enter a callsign, edit with Backspace, and confirm with Return.
The original numbered main menu is then displayed. Digits 1-5 select the source
mode and show its transition banner. Digit 6 opens the source-gated mission
list; F1-F4 select the missions enabled by the supplied disk's pilot record.
Digit 7 uses the original next-mission selector. Digit 8 opens flight-log
statistics. Escape returns from the mission list or log to the main menu.
In the log, SHIFT-2 resets the 39 words, and 1 saves the exact 78 bytes. A reset
pilot goes through enlistment/callsign entry again. Closing the window exits.
Free Flight (digit 2) now runs the complete source bootstrap, delayed scene
selection and viewport/message stages. Return acknowledges the original disk's
code-input message, then the original numbered location and aircraft keys run.
The aircraft selection resets the recorder/root through C10B90 and updates the
records. The view/control update completes startup into C10DAE; P pauses/resumes.
The `scene-setup` screen label also covers this initial flight loop. `=` and `-`
use the source throttle controls, and arrow keys use the source stick controls.
The preview now draws the source horizon and normal/wide terrain packets into
ordinary host planes, followed by the original scene placement/model streams,
ground descriptors, aircraft hulls and fixed matrix mark. Grounded aircraft
motion and stick recording now execute. Cockpit/HUD marks, tapes, readouts,
panel images, compass and indicator/mode bars are connected. Original cockpit
artwork is loaded from `pix/inst5` and `pix/frnt5`, with the source plane caches
and union mask. Both display pages preserve the immutable instrument images.
Earlier HUD checks omitted those disk assets; populated artwork comparisons now
pass. Complete frame ordering remains pending.
The record/context slice repeats during setup; headless statistics expose its
`record_updates`, `scene_frames`, `terrain_polygons`, `model_calls` and
`hud_frames` counts. The banner CRACKED BY A-HA is original disk message $47;
the earlier crash-message description was incorrect. Source timer requests use
seconds/microseconds from the native runner's deterministic PAL frame clock.
Qualification (digit 5) now continues through carrier construction and its
briefing into active flight. Return acknowledges the code prompt, then Space
leaves the qualification briefing. F10 applies the source throttle level;
Down pulls back. A bounded carrier-start/takeoff probe passes source comparisons.
Carrier landing, qualification-success save and restart are demonstrated with
original consumed keys. Complete recorded frame parity remains unverified.
Demonstration Flight (digit 1) now loads the original `text/textply` and
`text/textctl` recorder assets at startup, then flies through the source demo
stages, NPC guidance and recorded launches. All 4,892 demo updates complete;
startup/record/launch/asset comparisons pass. Functional scenario wiring is
3/3, while full native frame parity remains 0/3 accepted. All startup sound
resources and linked voice descriptors now load from the ADF or execute the
source square-wave/noise generators. All 15 sample buffers, availability flags
and resulting random seed match original startup. This restores the source
210-update demo banner and reduces the observed startup lead from 97 to 37 game
ticks. Remaining message/frame cadence and per-tick audio programs/output remain open.
Other selected modes stop at their banner. Audible output remains unconnected.
Menu entry now executes complete C0FBE0 sound/volume/reset/palette setup.
Its busy pause yields under the nominal PAL-clock conversion; input received
during the pause is retained for the following game input poll. Source tone
mute and volume-fade state are no longer overridden by the frontend.
C1718E's viewport/fade tail now runs once per PAL frame, including pending
game timers and display waits, as required by C17456's vertical-blank server.
Stable palette publication is connected. The full mouse-counter callback and
audible output remain open.

The supplied ADF is read-only. `--save-dir PATH` selects the native save overlay
(default `saves-native`). Its `config` retains the original 78-byte format.
Voice programs and audible output still need their asynchronous host owner.

```powershell
build/native/fa18_native.exe --headless --frames 1800 --ppm build/credits.ppm
python tools/native/check_frontend.py --runner build/native/fa18_native.exe
python tools/native/check_menu.py --runner build/native/fa18_native.exe
python tools/native/check_menu_start.py --runner build/native/fa18_native.exe
python tools/native/check_viewport.py --runner build/native/fa18_native.exe
python tools/native/check_flight_start.py --runner build/native/fa18_native.exe
python tools/native/check_records.py --runner build/native/fa18_native.exe
python tools/native/check_raster.py --runner build/native/fa18_native.exe
python tools/native/check_models.py --runner build/native/fa18_native.exe
python tools/native/check_hud.py --runner build/native/fa18_native.exe
python tools/native/check_cockpit_assets.py --runner build/native/fa18_native.exe
python tools/native/check_sound_resources.py --runner build/native/fa18_native.exe
python tools/native/check_qualification.py --runner build/native/fa18_native.exe
python tools/native/check_demo.py --runner build/native/fa18_native.exe
```

Build ownership is `port/recomp/CMakeLists.txt` -> `port/native/CMakeLists.txt`.
`FA18_NATIVE_ONLY=ON` returns before creating the CPU/machine targets. The
explicit native target links SDL and ordinary disk/Hunk loading, ILBM decoding,
game message sequencing, command selection, mission/log menu owners and text
plotting. It never links
Musashi, generated translations, game glue, or the chipset model. A link map
supports the omission check. No ROM, savestate, captured page, or frame replay
supplies game behavior or pixels.

The original ADF provides splash pixels, palette, fonts and text tables.
`advance_main_loop_message_sequence` calls native children with `MessageWorking`
values; `begin_top_level_menu` / `finish_top_level_menu` execute the complete
source menu owner around its host pause and publish the existing selectors.
`plot_glyph8` writes ordinary bitplane buffers, which SDL presents directly.
Source data addresses currently index checked host storage; fully typed game
state is still future work. Source callback identifiers describe the recreated
menu transitions; they never execute source instructions or CPU adapters.
`--data-out PATH` exports host buffers and source data for checkpoint inspection;
it supplies no runtime state. Initial player pose, camera and template-gate
banks match a focused original checkpoint. Full record state, update timing and
active gameplay remain unverified; see
[`../../analysis/native_flight_start_milestone.md`](../../analysis/native_flight_start_milestone.md).
Bootstrap record updates and context refresh now run through native children.
The focused record oracle matches original non-stack RAM at its tested boot
checkpoints, including an optional original Free Flight checkpoint. The setup
record slice repeats, but full input/view/timer ordering and remaining active
children are open. See
[`../../analysis/native_bootstrap_records_milestone.md`](../../analysis/native_bootstrap_records_milestone.md).

Authority: C0E2E8/C0E078 splash load, C0E53C/C0E78A busy delay, C11446/C11478
credits and acknowledgement, C115BA-C1175A tour/name flow, C0FBE0 menu,
C32CEE message sequencing, C330FE glyphs, and C1643A flight-log saving.
Menu continuation reuses C1AD74/C1BC72/C1BD78 command selection,
C0FCB4 mode routing, C1017E available-mission queue, C24E8A/C24F76 summary
formatting, and C0FE36/C16406 log actions/reset. See
[`../../analysis/native_menu_milestone.md`](../../analysis/native_menu_milestone.md).
The source busy pause is converted to nominal PAL ticks once; DMA/loading,
CPU-paced message update frequency and fade timing are not reproduced.
Static settled splash, credits and menu comparisons are separate from timing
parity. Copper fade remains excluded from acceptance, per the user.

Native preview ownership is `native_flight_tick` -> `native_scene_project` /
`native_scene_draw` -> the shared matrix/projection, active-plane, map-packet,
clip and polygon owners -> `port/game/native/raster.c`. The native branches
replace blitter line/fill/composite submissions with direct host plane writes;
the reference branches retain their hardware calls. `check_raster.py` compares
160 polygons and the horizon/map buffers against original opcodes at each of
two native selection checkpoints. Those opcodes, ROM and chipset run only in
the validation executable. See
[`../../analysis/native_terrain_preview_milestone.md`](../../analysis/native_terrain_preview_milestone.md).

Scene placements now call `port/game/native/model.c` for source distance/LOD,
static/flat vertices and draw commands. Existing `draw_stream.c` supplies
geometry; `circle.c` has a native span-mask backend. The model oracle compares
reached descriptors at three actual runner checkpoints and 48 circle cases,
without treating source CPU/ROM/chipset dependencies as native runtime code.
The initial static-scenery milestone is documented in
[`../../analysis/native_scene_objects_milestone.md`](../../analysis/native_scene_objects_milestone.md).

The followup list now includes aircraft descriptors and cached record hulls,
with compact/extended derived vertices, history/expiry drawing and source grid/
setup-control ordering. The model check compares all non-stack data as well as
planes, and independently checks the complete followup, grid and control parents
at three runner checkpoints. See
[`../../analysis/native_aircraft_rendering_milestone.md`](../../analysis/native_aircraft_rendering_milestone.md).
The view/control update, input recorder, indexed aircraft controls and root
motion now execute source owners directly. Throttle motion and stick ramp/release
are checked at seven native checkpoints against original C12098/C1C63E non-stack
RAM. See [`../../analysis/native_flight_controls_milestone.md`](../../analysis/native_flight_controls_milestone.md).
Unconnected flight command/dynamics children fail explicitly when reached.
Nineteen cockpit/HUD instrument and panel owners now execute through direct
host drawing. Three checkpoints each pass 95 original-instruction cases across
centered, panned and clipped views. See
[`../../analysis/native_hud_milestone.md`](../../analysis/native_hud_milestone.md).
Postflight dispatch and the cockpit message line are now connected, with source
warning selection before the HUD and the final text sequence after flight work.
Notification cadence executes before record updates. Three checkpoints each
pass 115 HUD/message cases.
The frame timer now yields across host PAL ticks, using the original disk's
update-rate table. Pending polls repeat neither physics/drawing nor final text.
The source game counter advances and freezes while paused; periodic region,
zone-check, page-clear and cockpit-redraw paths execute. Seven view/record
checkpoints, 36 timer/readout cases and two periodic record passes match original
non-stack RAM. See [`../../analysis/native_clock_milestone.md`](../../analysis/native_clock_milestone.md).
Complete frame ownership and exact recorded cadence remain open.
C12950 now consumes control/sound actions in active and inactive frame branches,
through typed C locals/arguments and existing audio consumers. The native run
executes 2454 owner calls; 720 source cases match memory and 1469 exact sound
requests. Native sample loading/output remains unfinished. See
[`../../analysis/native_control_actions_milestone.md`](../../analysis/native_control_actions_milestone.md).
Full flight and the record-expiry
transition remain unfinished. Setup correctness is separate from frame cadence
and recorded gameplay acceptance.

Scene ordering C0F048-C0F124 now comes from the shared update-sequence owner,
with the native child consumer supplying the existing host rendering paths.
Bias/flagged/range gates and stage markers therefore use the same source
composition as C0EFD4. Alternate page presentation C0DA38 remains unconnected
and fails explicitly if reached. See
[`../../analysis/native_scene_ordering_milestone.md`](../../analysis/native_scene_ordering_milestone.md).

Positive paired model strips C1F584-C1F6F8 now render during the native pullback
probe. `check_strips.py` checks 96 source cases and actual reached descriptor,
plane and record comparisons. Dynamics warning publication uses C25704.
The subsequent terrain fix and short takeoff check are documented below. See [`../../analysis/native_model_strips_milestone.md`](../../analysis/native_model_strips_milestone.md).

Negative terrain visibility indices now resolve original image words and retain
C2AF40's exact source selector. `check_map_limits.py` passes 384 source cases and
the repaired tick-7294 terrain checkpoint. `check_strips.py` also checks a short
native takeoff: ground flag, height and source bookkeeping, plus original
view/record comparisons. The subsequent display/reset integration is documented below;
recorded-flight acceptance is not established. See
[`../../analysis/native_map_visibility_milestone.md`](../../analysis/native_map_visibility_milestone.md).

Outer display C1612C and draw-page selection C2F558 now execute in Free Flight.
Publication/palette waits resume on host PAL ticks without repeating physics,
HUD or text. The source activity loop decrements once after four waits; the
short pullback completes one postflight reset and resumes C10DAE. Both host
pages render through the selected source plane table. `check_postflight_entry.py`
checks native wait cadence/reset plus 128 resumable and three postflight source
cases. The blocking owner passes 1024 complete CPU/RAM contracts; affected
native checks and twelve reference CTests pass. Exact beam/input timing and
full recorded-flight acceptance remain open. See
[`../../analysis/native_outer_display_milestone.md`](../../analysis/native_outer_display_milestone.md).

Free Flight key presses/releases now queue into the original C0F3C4 input
owner before stage/view/record work. C16EAE/C16BF2/C16C56 polling and complete
C1AD74/C1AC28 dispatch preserve recorder drains and command publication/clearing.
`check_input.py` compares 144 original input/command cases and four native
wait/press/release checkpoints. Headless counters expose `input_passes`,
`input_events` and `input_queued`. Physical gameport acquisition, modifier
timing and inherited countermeasure arguments remain open. See
[`../../analysis/native_input_milestone.md`](../../analysis/native_input_milestone.md).

Qualification mode 9 now shares the connected input, flight and display owners.
C0FECE's carrier setup, C0FB70/C0FBB6 briefing and C0A2F0 landing schedule run
directly. Carrier command C207FE and C1FF0A result-word accumulation preserve
source followup drawing state. Four native checkpoints demonstrate carrier
startup and short takeoff; original startup, view/record and reached rendering
comparisons pass. See
[`../../analysis/native_qualification_milestone.md`](../../analysis/native_qualification_milestone.md).

Sealed `FA18_LOOP_INPUT_V1` keyboard recordings now run with `--input PATH` and
an optional `--iterations N` bound. They start on the first native main-menu
update after cold startup; `--replay E9K` can supply intro navigation. The first
recording column controls delivery, while video-frame metadata is informational.
Raw key identity and same-update edge order are preserved. Timer/display waits
retain the update iteration. Crash and carrier prefixes reach active
qualification; complete outcomes and frame parity remain open. See
[`../../analysis/native_loop_replay_milestone.md`](../../analysis/native_loop_replay_milestone.md).

The sealed crash prefix now passes the C17F8C collision-sound gate and reaches
two native postflight resets by iteration 2150. Its original view/record state
comparisons and six collision-child cases pass. The subsequent C0F920 return to
the menu remains unconnected; loaded samples and full outcome/frame parity are
still open. See
[`../../analysis/native_collision_milestone.md`](../../analysis/native_collision_milestone.md).

C0F920/C08F26 now return a completed flight sequence to the native menu,
clearing source renderer work banks and recorder/mode state. The entire sealed
crash input runs to its stated three-crash/menu outcome, and selecting
qualification again reaches the carrier briefing. Four original reset cases
and affected native regressions pass. Native host ticks differ from the sealed
video frames, so this is functional scenario completion; frame parity remains
open. See
[`../../analysis/native_sequence_return_milestone.md`](../../analysis/native_sequence_return_milestone.md).

For original gameplay comparisons, `--input` also accepts `FA18_GAME_INPUT_V1`
from `fa18_recomp --ports off --game-input-out PATH`. These rows mark keys the
game consumed at C1AD74; sealed loop recordings mark hardware injection, whose
delivery can differ. Native turn/hook state matches a focused original
checkpoint with consumed keys; startup flags/countdown and carrier touchdown
integration remain open. See
[`../../analysis/native_game_input_milestone.md`](../../analysis/native_game_input_milestone.md).

Carrier touchdown now runs C083E2's source mission reset. The aircraft stops on
the deck with original motion, orientation, contact and reset state through
iteration 6288. Eight original child comparisons pass. The bounded check reuses
existing reference evidence:

```powershell
python tools/native/check_touchdown.py --game-input build/native-flight/carrier-game-input.fa18in --source-approach build/native-flight/reference-carrier-approach.dat
```

See [`../../analysis/native_carrier_touchdown_milestone.md`](../../analysis/native_carrier_touchdown_milestone.md).

Qualification success now queues the original result messages, updates and saves
the 78-byte pilot record, then restarts through the original viewport/bootstrap
callbacks. All 8038 consumed-input updates complete; the saved qualification
word is 1 and reloads exactly. Two of three scenarios have functional outcomes.
Full frame parity and post-result timing/state alignment remain open. To reuse
the same original evidence for the native result/save/reload check:

```powershell
python tools/native/check_qualification_result.py --game-input build/native-flight/carrier-game-input.fa18in --source-end build/native-flight/reference-carrier-end.dat
```

See [`../../analysis/native_qualification_result_milestone.md`](../../analysis/native_qualification_result_milestone.md).
