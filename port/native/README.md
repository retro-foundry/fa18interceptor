# Native port runner

The 2026-10-06 user request starts `fa18_native`, reusing `port/game/` source
with a separate native entry point. This supersedes the earlier restriction
against starting a native runner. It does not restore the deleted `fa18_port`.

```powershell
python scripts/build_native.py
build/native/fa18_native.exe --adf local/media/fa18.adf
```

Gameplay behavior and visuals at equivalent states/events are the acceptance
scope; exact Amiga frame timing is not required. Preserve physics, rules and
source-defined timers while allowing native rendering/presentation cadence.
The accepted native target is a 20 ms frame budget with every frame presented,
without recreating Amiga missed frames. This target is not a measured guarantee
or a change to gameplay physics/timers.
Intro/loading duration may differ. Copper fade is ignored. Strict frame checks
below remain diagnostics, not a requirement to reproduce rendering delays. See
[`../../analysis/native_gameplay_acceptance.md`](../../analysis/native_gameplay_acceptance.md).
Five selected independent demo/carrier checkpoints match both 320x200 gameplay
pages, phase/controls and named player motion/pose/matrices byte for byte;
complete gameplay sequences remain unaccepted. Comparisons use equal pre-input
boundaries and source-defined draw/display roles; physical buffer numbering
may differ after loading. Reuse existing original checkpoints:

```powershell
python tools/native/check_gameplay_checkpoint.py --source build/native-flight/reference-demo2401.dat --iteration 2364
python tools/native/check_gameplay_checkpoint.py --source build/native-flight/reference-carrier-approach.dat --input build/native-flight/carrier-game-input.fa18in --iteration 6289
```

The first independent consecutive gameplay window checks 128 updates: all
128 player-motion/control/phase comparisons match, while 53 complete two-page
drawing comparisons match. Remaining differences are the seconds-driven target
information line (ALT/HDG/SPD cycling), first at tick 273. This window has
different elapsed times, so the HUD page difference at an equal update count
does not by itself establish a timer defect. Verify equivalent elapsed game
time and source transitions; exact original display cadence is not required.
Reuse the retained original prefix:

```powershell
python tools/native/check_gameplay_window.py --source-prefix build/native-flight/gameplay-window-source --source-first 2401 --native-first 2364 --count 128 --out build/native-flight/gameplay-window-comparison
```

The checker retains all failures in `comparison.json` and currently exits 1.
`--frame-capture FIRST+COUNT PREFIX` exports consecutive actual pre-input and
body boundaries as `PREFIX.ITERATION.entry.dat` / `.before.dat` / `.after.dat`.
The entry export requires a flight update before its input/stage; its existing single
iteration form keeps the old filenames. Reference-only `FA18_LOOP_DUMP` accepts
`FIRST+COUNT:PREFIX` for original boundaries. No capture feeds native behavior.
An alternate C0DA38 frame writes its `.after.dat` at the enclosing owner exit,
with JSON `frame_owner_exit: true`; ordinary captures report false and end at
C0F3C0. The normal frame-body checker rejects an alternate exit explicitly.
`check_gameplay_comparison.py --source SOURCE.dat --native NATIVE.entry.dat`
checks verifier strictness against an accepted pair: equivalent buffer
allocation passes, while wrong presentation, publication, HUD/input/motion
or game tick fails.

Destroyed flight records now enter source C22ADE's 15-tick expiry rather than
aborting during scene rendering. C09DD0 clears a matching target and posts
TARGET DESTROYED through C25704; repeated drawing does not restart expiry.
Twenty controlled original descriptor comparisons and a disk-backed native
scene integration check pass. The latter supplies destruction inputs only in
its test entry; it shares the playable runner's `fa18_native_runtime` objects.
This verifies the rendering transition, not complete destruction/collision or
recorded-frame timing. See
[`../../analysis/native_record_expiry_milestone.md`](../../analysis/native_record_expiry_milestone.md).

The default is an SDL window with native stereo sound. `--wav PATH` captures
the same PCM, including in headless runs. A key acknowledges the credits. A first-time
pilot can enter a callsign, edit with Backspace, and confirm with Return.
The original numbered main menu is then displayed. Digits 1-5 select the source
mode and show its transition banner. Digit 6 opens the source-gated mission
list; F1-F4 select the missions enabled by the supplied disk's pilot record.
Digit 7 uses the original next-mission selector. Digit 8 opens flight-log
statistics. Escape returns from the mission list or log to the main menu.
Digit 3 (source mode 2) now continues beyond its banner. Return at each of its
two source prompts reaches record-4 playback and flight. One failure/menu-return
route is exercised; 32 actual input/stage intervals and 21 sampled frame bodies
match original instructions' compared RAM/drawing. Other playback streams,
resets and complete mode outcomes remain unverified. Run
`python tools/native/check_mode_two.py`; see
[`../../analysis/native_mode_two_milestone.md`](../../analysis/native_mode_two_milestone.md).
In the log, SHIFT-2 resets the 39 words, and 1 saves the exact 78 bytes. A reset
pilot goes through enlistment/callsign entry again. Closing the window exits.
Free Flight (digit 2) now runs the complete source bootstrap, delayed scene
selection and viewport/message stages. Return acknowledges the original disk's
code-input message, then the original numbered location and aircraft keys run.
The aircraft selection resets the recorder/root through C10B90 and updates the
records. The view/control update completes startup into C10DAE; P pauses/resumes.
The `scene-setup` screen label also covers this initial flight loop. `=` and `-`
use the source throttle controls, and arrow keys use the source stick controls.
F and C execute source flare/chaff stock, messages and control-effect launch,
motion, drawing and ground-contact expiry. Keyboard depleted-stock selection
and SHIFT-F's mode-6 sound are connected. Recorder $FD function keys also
publish their source event; their inherited selection is dead to publication.
848 source input cases and two controlled actual native input parents pass.
Depleted pending recorder carry remains unfinished. Effect component/face collision
children are connected; 256 source-parent cases and four controlled actual
native parents match original compared RAM/display. These include face hits
and component misses; complete weapon-kill sequences remain unverified.
The runtime integration test uses real Free Flight keys and shares
all playable runtime objects; four actual bodies match original gameplay and
display. Build `fa18_native_countermeasures_test` and run
`python tools/native/check_countermeasures.py` for retained focused evidence.
See [`../../analysis/native_countermeasures_milestone.md`](../../analysis/native_countermeasures_milestone.md).
The preview now draws the source horizon and normal/wide terrain packets into
ordinary host planes, followed by the original scene placement/model streams,
ground descriptors, aircraft hulls and fixed matrix mark. Grounded aircraft
motion and stick recording now execute. Cockpit/HUD marks, tapes, readouts,
panel images, compass and indicator/mode bars are connected. Original cockpit
artwork is loaded from `pix/inst5` and `pix/frnt5`, with the source plane caches
and union mask. Both display pages preserve the immutable instrument images.
Earlier HUD checks omitted those disk assets; populated artwork comparisons now
pass. Eight actual assembled frame bodies now match original C0EFEA-C0F3C0
in compared gameplay state and every display-plane byte, including cockpit
and map. C1C860's caller now preserves the request-dependent all-list versus
one-list sorting decision. Full recorded display/task cadence remains open.
See [`../../analysis/native_frame_body_milestone.md`](../../analysis/native_frame_body_milestone.md).
Lost-target selection cleanup now executes after the HUD and before timers.
The source debug page mark and numeric overlays execute under their original
gates after the counter and before the final message. All non-stack RAM agrees
in 320 original caller-range cases. Stores icons, grid/record markers and
scene-position labels were the remaining identified frame owners. C2B3C2
scene labels are now connected on both frame branches, using original scene
rows and the source point/number layout. 256 complete source comparisons pass,
including 41 visible number cases. Four of six identified frame owners are
connected at that milestone. C2B564's grid/record-marker tree is now connected
as well: **M opens the original map** during Free Flight, with both grid axes,
coordinate labels and aircraft markers. 385 source cases and the real map
checkpoint pass. C30A00 stores icons now run between weapon status and grid
readouts, including the source selection colours and redraw/clipping rules.
1,484 stores and 280 complete normal HUD source comparisons pass; the exhausted
weapon's one-digit zero is correctly blank. All six identified frame owners
are connected. This is a scope inventory, not whole-game progress; full recorded
frame parity remains open. See `../../analysis/native_stores_milestone.md`.
Menu-to-flight selection now publishes its next callback once per update and
starts the banner at the final message boundary. The measured demo startup lead
is 36 ticks. The remaining viewport wait spans a different number of game
updates per PAL frame; source WaitBOVP/task pacing remains open. See
`../../analysis/native_stage_dispatch_milestone.md`.
The C50158 voice updater now runs every PAL frame after the viewport/master
fade, including during menu pauses and display/timer waits. Original voice
programs, period/volume slides and slot release execute; output levels publish
to ordinary native channel state. 7,250 source comparisons and a real wait/
tone-completion test pass. Sample requests (C500D8), repetition/chaining and
audible playback are now connected as well. Signed disk PCM uses original PAL
pitch and stereo routing, published through SDL or optional WAV capture. 256
full source handler cases, four waveform/pitch/routing/block-partition cases
and actual nonzero WAV/SDL publication pass. Bit-exact original audio timing,
fetch/interrupt latency and filtering remain unverified. See
`../../analysis/native_sample_output_milestone.md` and
`../../analysis/native_voice_cadence_milestone.md`.
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
ticks at that milestone, now 36. Remaining message/frame cadence and audible
alignment remain open; per-tick programs and native sample output are connected.
Other selected modes stop at their banner.
Menu entry now executes complete C0FBE0 sound/volume/reset/palette setup.
Its busy pause yields under the nominal PAL-clock conversion; input received
during the pause is retained for the following game input poll. Source tone
mute and volume-fade state are no longer overridden by the frontend.
C1718E's complete mouse-counter/control/viewport/fade callback now runs once
per PAL frame, including pending game timers and display waits, as required by
C17456's vertical-blank server. C17104 supplies original -960..960 control
bounds. SDL relative mouse motion and left/right buttons are connected; E9K
replays accept original device-0/4 `m`/`b` rows alongside keyboard input.
Stable palette publication is connected. Original mouse acquisition cadence
and exact gameplay frame/audio alignment remain open. See
[`../../analysis/native_input_pal_milestone.md`](../../analysis/native_input_pal_milestone.md).

The supplied ADF is read-only. `--save-dir PATH` selects the native save overlay
(default `saves-native`). Its `config` retains the original 78-byte format.
Voice programs, sample requests and audible output have their native owners;
complete original frame/audio timing remains open.

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
python tools/native/check_audio.py --runner build/native/fa18_native.exe
python tools/native/check_samples.py --runner build/native/fa18_native.exe
python tools/native/check_frame_body.py --runner build/native/fa18_native.exe
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
composition as C0EFD4. C0DA38's full-viewport selection and early frame exit are
now connected: either selection outcome skips the remaining frame children and
returns to outer display presentation. Eighteen source cases and a disk-backed
native frontend/original frame-prefix comparison pass. See
[`../../analysis/native_scene_exit_milestone.md`](../../analysis/native_scene_exit_milestone.md)
and
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

Digit 4 now reaches native mode 125's source prompts and sustained flight.
A normal input run exercises Escape/menu return and re-entry, with 2,211
scene/HUD frames. 55 actual input/stage intervals and 35 sampled bodies match
original instructions' compared RAM/display. Complete outcomes and sequences
remain unaccepted. The existing mode checker shares the playable runtime:

```powershell
python tools/native/check_mode_two.py --mode 125 --out build/native-flight/restore-check/original-check
```

See [`../../analysis/native_mode_125_milestone.md`](../../analysis/native_mode_125_milestone.md).

Next-mission digit 7 now selects the original pilot log's mode 6 and runs
briefing/context/aircraft setup into flight, with 401 scene/HUD frames in a
normal input run. 38 actual input/stage intervals and 27 sampled bodies match
compared original RAM/display. Full mission outcomes and variants remain open.

```powershell
python tools/native/check_mode_two.py --mode 6 --out build/native-flight/mission-six/original-check
```

See [`../../analysis/native_mode_six_milestone.md`](../../analysis/native_mode_six_milestone.md).

Mission-list F1 now enters normal mode 3 without recorder playback. Both source
aircraft choices reach flight (969 scene/HUD frames), each passing 43 actual
input/stage intervals and 29 sampled bodies against compared original
RAM/display. Full mission completion/combat and variants remain open.

```powershell
python tools/native/check_mode_two.py --mode 3 --out build/native-flight/mission-three/original-check
python tools/native/check_mode_two.py --mode 3 --aircraft 2 --out build/native-flight/mission-three/original-two
```

See [`../../analysis/native_mode_three_milestone.md`](../../analysis/native_mode_three_milestone.md).
