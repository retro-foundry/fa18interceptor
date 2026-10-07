# Current playable port handoff

Latest connected batch: digit 4 now enters native mode 125's prompts and
sustained flight, with Escape/menu return and re-entry also exercised.
55 actual input/stage intervals and 35 sampled bodies match original
instructions' compared RAM/display. C0FECE's restore children, model command
$120 (C1FEE4) and record scheduler C0A334 are connected. Seven affected native
integration tests, mode-2 source regressions and current frontend checks pass.
This is sampled route coverage; whole-mode outcomes and whole-game acceptance
remain open. See `analysis/native_mode_125_milestone.md`.

Latest connected batch: digit 3 now runs native source mode 2 through its two
prompts, aircraft-record-4 control stream, flight failure and menu return.
32 actual input/stage intervals and 21 sampled frame bodies match original
instructions' compared RAM and drawing bytes. The checks exposed and fixed
the transition's saved-pointer sort choice and filled-circle address offsets.
Native page planes now retain the original descending 320x200 layout; the
host surface's lower 56 rows remain outside the game bitmap.
This is one exercised route, not whole-mode or whole-game acceptance. See
`analysis/native_mode_two_milestone.md` and `tools/native/check_mode_two.py`.

Latest batch, 2026-10-07: keyboard F/C reaches native source flare/chaff launch,
motion, ground-contact expiry and drawing through C1518C. Complete original
input/control-effect parent comparisons pass 688/192 cases; four actual
keyboard-driven Free Flight bodies match compared gameplay and all display
bytes. Active/crash/map, queued input, native integration, frontend/omission and
reference host/loading checks pass. A follow-up connects effect component/face
collision children: 256 full source-parent cases and four controlled actual
native parents match compared RAM/display (face hits and component misses).
$FD function-key selection now proceeds: the inherited value is dead to its
publication path. Complete input comparisons pass 848 cases and two controlled
actual native input parents. Depleted pending recorder carry remains unfinished.
See `analysis/native_countermeasures_milestone.md`. Whole gameplay remains open.

Latest timing clarification: exact Amiga frame timing is not a completion gate
for the native port. Preserve gameplay physics/rules and source-defined timers;
allow native rendering/presentation cadence. Compare equivalent gameplay states
and events, and keep strict recorded-frame results as diagnostics. The historical
0/3 strict sequence result below is not a whole-game completion measure. Do not
reproduce original rendering delays solely to align the timed HUD page at an
equal update count. Copper fade remains excluded. See `NEW_PORT_HANDOFF.md`.
The accepted native target is a 20 ms frame budget with every frame presented;
Amiga missed frames need not be reproduced. Performance has not been measured
as part of this clarification, and gameplay physics/timers remain authoritative.

Latest direction, 2026-10-06: build a native runner from `port/game/`, starting
with intro -> credits -> pilot entry -> menu. This supersedes the incremental
emulation-removal work below and the former restriction against a new runner.
`fa18_native` is owned by `port/native/` and `port/game/native/`; shared loading
stays in `port/amiga/`. Build with `python scripts/build_native.py`. See
`port/native/README.md` for scope, source authority, validation and launch.
The emulator runners remain reference tools. Full native gameplay remains open.
Latest user scope: gameplay frames must match; intro/loading/preflight duration
may differ and loading can be faster. Copper fade remains excluded. The old
36-update startup lead is not by itself a blocker. See
`analysis/native_gameplay_acceptance.md`: original start-2401 and native
C0EFEA/update-2364, both game tick 222, match both 320x200 gameplay pages and
player motion/pose/matrices byte for byte. `check_gameplay_checkpoint.py`
reuses the original dump. Five selected checkpoints across demo/carrier now
match both pages, phase/controls and named player motion/pose/matrices: demo
tick 222; carrier updates 5101/5501/6001/6289, ticks 412/812/1312/1600. This is
5/5 selected checkpoints, not complete sequences. Independent comparisons use
the new pre-input `.entry.dat` boundary and C2F558/C1612C draw/display roles;
carrier physical page 1 equals native 0. Tests reject wrong roles, active-table
publication, HUD bits, motion, key-release phase and ticks. Existing frame-body
captures at C0EFEA/C0F3C0 remain unchanged. All original evidence was reused.
Complete gameplay-sequence acceptance remains 0/3. The first consecutive independent
window now has 128/128 matching player motion/controls/phases, and 53/128
matching complete drawing page pairs. At tick 273 the original target-info
line has switched ALT -> HDG while native still shows ALT. Other drawing bytes
match. C25312's seconds-driven INFO_REQUEST/C459C4 cycling exposes different
gameplay pacing (18,000 vs 8,560 ms across this window). Optional native/source
capture ranges and `check_gameplay_window.py` retain every difference for reuse.
The one bounded original prefix agrees with existing final RAM/registers.
Next: correct connected gameplay timer/poll/display cadence from source,
without guessed speed factors, capture-fed clocks or intro timing requirements.
Both select original C2502E entry 15 (67 ms); source rendering exceeds that
limit while native approaches it. No timer arithmetic/rate-index bug was found.
Latest scene branch: C0DA38's full-viewport selection now propagates its early
exit through C0D730 -> scene -> flight -> frontend. Both accept/reject paths
skip the remaining HUD/timer/counter/final-message children and still resume
outer display presentation. Eighteen signed-threshold/source-mode cases match
original RAM/result/frame unwind; a disk/input-backed native frontend fixture
matches the complete original C0EFEA-to-owner-exit prefix (5,253 instructions,
zero gameplay/display differences). Capture JSON's `frame_owner_exit` explicitly
distinguishes this boundary from C0F3C0. Active/crash/map bodies, expiry regression,
frontend/omission and twelve reference host/loader tests pass. This branch is
1/1 complete; original recordings were not rerun. See
`analysis/native_scene_exit_milestone.md`. Full gameplay acceptance stays 0/3;
independent HUD cadence remains open. Five retained demo boundaries also match
the named motion/rates/orientation/matrices for all sixteen flight records.
Latest gameplay branch: C22ADE destruction-to-expiry is connected through the
actual scene traversal, including C09DD0 target cleanup and C25704's TARGET
DESTROYED message. The timer starts at 15, bit $0200 clears and $0400 sets;
repeated rendering preserves expiry. Twenty controlled source descriptor cases
across demo/setup checkpoints match returns, records, rendering and non-stack
data. A native-only integration test starts from the disk/input and exercises
the scene using its visible record, with controlled destruction inputs confined
to the test. Production/test share the same native runtime objects. Frontend,
CPU/chipset omission and active/crash frame-body comparisons pass. This branch
is 1/1 complete; full frame acceptance remains 0/3 and the independent HUD timer
difference remains open. No full original replay was repeated. See
`analysis/native_record_expiry_milestone.md`.
Latest PAL input batch: complete C1718E now runs through host counter samples,
including signed delta wrap, bounded controls and input ticks before its
existing viewport/fade tail. C17104/C1712C/C17456 initialize original -960..960
bounds and the native callback. SDL/E9K mouse and buttons reach source input
owners. 128 startup + 6,400 full callback source cases, actual wrap/clamp/buttons/
waits, eight frame bodies, affected gameplay and reference checks pass. This
connection is 1/1 complete. See `analysis/native_input_pal_milestone.md`.
Latest assembled-frame batch: C1C860's native adapter now sorts all lists only
for nonzero context requests, otherwise one list as in C1C870/C1C98A/C1E328.
Actual crash-flight comparison exposed the old always-all cursor difference.
`--frame-capture ITERATION PREFIX` exports actual C0EFEA/C0F3C0 boundaries;
eight demo/wait/readout/crash/carrier/cockpit/map bodies match original bytes
in compared state and every display byte. This is 8/8 sampled bodies, not full
recording acceptance. Explicit scratch/async voice/busy exclusions are in
`analysis/native_frame_body_milestone.md`. Copper fade excluded; startup lead
36 ticks and full frame parity 0/3 remain. Next: original WaitBOVP/task pacing.
Latest sample batch: C4FFB4's misleadingly named clear helper now requests
native channel service. Complete C500D8 repetition/chaining/stop behavior feeds
native signed PCM streams and SDL stereo output; `--wav PATH` captures the
same PCM in headless runs. 256 full handler comparisons, four disk-backed
pitch/routing/block-partition cases, nonzero WAV/SDL publication, PAL voices
and affected gameplay/reference checks pass. Review Free Flight cockpit/audio
at `build/native-flight/sample-freeflight6100.png` and `.wav`. See
`analysis/native_sample_output_milestone.md`. Three native audio boundaries
are connected; bit-exact audio fetch/filter/timing remains unverified. Startup
lead stays 36 ticks; full frame parity stays 0/3, Copper fade excluded.
Latest voice batch: C50158 now runs each host PAL frame after C1718E's
viewport/master fade, including menu pauses and suspended display/timer polls.
Its complete delay/program/output/slide owner publishes ordinary native channel
levels; ended tones now release their real slots. C5002A's interrupt-5 node
priority -120 establishes this ordering. 7,250 source comparisons, actual
wait/volume/tone completion and affected gameplay/reference checks pass. See
`analysis/native_voice_cadence_milestone.md`. The PAL voice-update connection is
1/1 complete; C500D8 sample completion/chaining and audible output remain open.
Startup lead remains 36 ticks; full frame parity stays 0/3, Copper fade excluded.
Latest dispatch correction: menu C0FCB4 now publishes C0FECE without ticking it
again in the same update. Menu messages run afterward at the final C32CEE
boundary. Actual recorded selection agrees with source countdown $D2, cleared
key claim and started banner; the next update first decrements it to $D1.
The demo lead falls 37 -> 36 ticks. A bounded original prefix, identical to the
existing checkpoint, isolates most of the remaining gap to C0FA4C's viewport
wait: original 80 updates in 34 PAL frames, native 43 updates in 45 PAL frames
before this correction. Native next-whole-PAL WaitBOVP pacing remains open;
do not replace it with a guessed fixed pass count. Menu/demo/carrier/crash
checks pass. See `analysis/native_stage_dispatch_milestone.md`. This correction
is 1/1 verified; six identified missing frame owners remain connected. Full
frame parity stays 0/3 accepted; Copper fade excluded.
Latest cockpit stores batch: C30A00 now runs in the actual HUD between weapon
status and grid readouts, using original stocks, symbols and clipping. Complete
HUD checks also fixed the exhausted missile's one-digit zero: source leaves it
blank. 1,484 stores and 280 complete normal HUD source comparisons pass in all
non-stack RAM. Native demo, live cockpit/map, carrier save/restart/reload and
12 reference contracts pass. See `analysis/native_stores_milestone.md`.
All six identified missing frame owners are connected (100% of that inventory,
not whole-game progress). Full frame parity remains 0/3 accepted, startup lead
37 ticks, Copper fade excluded. Async voices/output and full timing/state parity
remain open; reuse reference recordings rather than repeating full replays.
Latest grid batch: C2B564's complete X/Z grid and aircraft-marker child tree
now runs after selection cleanup, before timers. Map entry's C0F4A6 voice
release is connected; M opens the map in actual native Free Flight. 385 full
source comparisons pass, including the live checkpoint and 224 visible
fixture cases. All five legacy owners pass 128 cases each. Native demo final
RAM is unchanged; carrier save/restart/reload and 12 reference contracts pass.
See `analysis/native_frame_markers_milestone.md`. Five of six identified
missing frame owners are connected (about 83% of that inventory); C30A00
stores icons remain. Full frame parity is still 0/3 accepted, startup lead
37 ticks, Copper fade excluded. Do not repeat original full replays needlessly.
Latest labels batch: C2B3C2 now runs after the counter on the drawing branch
and at the idle join, before overlays/final message. It uses original scene
data with connected native projection and C32A44 number layout. All non-stack
RAM agrees in 256 complete source cases, including 41 visible number cases;
128 legacy complete-call cases also pass. The 4,892-update native demo's final
RAM is unchanged, and 12 reference contracts pass. See
`analysis/native_frame_labels_milestone.md`. Four of six identified missing
frame owners are connected (about 67% of that inventory). Stores icons and
grid/record markers remain; full frame parity is 0/3 accepted and startup lead
37 ticks. Copper fade remains excluded.
Latest frame-tail batch: C12242 lost-selection cleanup now runs after the HUD,
before timers. C2F49C page marks and C31B76 numeric overlays run after the
counter under C0F386's original gates, before the final message. 64 cleanup
and 256 overlay source caller-range cases match all non-stack RAM. The native
demo completes; its checkpoint image and all final RAM except the stage marker
are unchanged. Crash/re-entry, frontend/menu and reference contracts pass.
See `analysis/native_frame_tail_milestone.md`. Three of six identified missing
frame owners are now connected (50% of that inventory, not whole-game effort).
Stores, grid/record markers and scene labels remain open. Full frame parity
remains 0/3 accepted, startup lead 37 ticks; Copper fade is still excluded.
Latest cadence batch: C1718E's viewport/master-fade tail now runs on each host
PAL frame, including menu delays, timer polls and display waits. It previously
ran once per game update. C17456 registers interrupt 5 through Exec vector
-$A8 at C53B00; the reference machine raises that vertical-blank bit each PAL
frame. Native RGB4 stable publication is connected as well. 128 source
sequences (6,400 ticks) and a real display-wait/fade check pass. Frontend/menu,
carrier/save/restart, crash/re-entry and populated cockpit regressions pass;
the native demo completes and its final record parent agrees. See
`analysis/native_viewport_cadence_milestone.md`. The viewport/fade tail is 1/1
connected; full mouse-counter input and audible output are still open. Demo
startup tick lead remains 37; native full frame parity remains 0/3 accepted.
Latest startup batch: native menu entry now runs the complete C0FBE0 owner,
including source sound selection, master-volume target, work-bank/message
reset and palette-table loading. Its C0E78A busy pause yields under the existing
nominal PAL-clock convention. Input/game updates stop during the pause and
queued keyboard selections execute afterward. Eighteen source comparisons
match all non-stack RAM before/after the pause. Source tone/fade overrides are
removed; menu volume/state now match original startup. Frontend/menu, complete
native demo, carrier/save/restart, crash/re-entry and twelve reference tests
pass. See `analysis/native_menu_start_milestone.md`. C0FBE0 setup is 1/1 parent
connected and verified; startup lead remains 37 ticks and frame parity is 0/3.
Next: asynchronous voice updates/output and remaining frame-tail drawing.
Latest timing batch: native C17510/C1756A/C1787A startup sound resources and
linked voice descriptors are connected. All 15 sample buffers, availability
flags $07/$F7 and the generated-noise random seed match original startup.
23 source cases, 31 demo cases and two record parents pass. Native demo update
2,400's lead falls from 97 to 37 ticks by restoring the original 210-update
banner delay. The complete native demo, carrier qualification/save/restart,
crash/menu/qualification re-entry and frontend/menu regressions pass.
See `analysis/native_flight_sound_milestone.md`. Startup sound initialization is
3/3 source parents complete; audible output remains open. Voice updates/output
and remaining message/viewport/frame/result cadence remain open. Frame parity is 0/3.
Latest correction: native startup now loads original pix/inst5 and pix/frnt5
and publishes C16982's caches/mask. Earlier HUD checks used empty asset pointers;
the user's missing-cockpit observation was correct. The cockpit bitmap now
renders. The first page is moved off immutable image Hunk 62. Both assets' eight
rendering planes, headers and mask match original startup; four cache cases and
230 populated HUD cases pass, along with frontend/menu/display regressions and
twelve reference CTests. See `analysis/native_cockpit_assets_milestone.md`.
Cockpit artwork is 2/2 verified; native frame parity remains 0/3 accepted.
Latest comparison update: original C1AD74 consumed-key exports now distinguish
game input from sealed hardware-injection rows. Native accepts the separate
FA18_GAME_INPUT_V1 format and matches turn/hook aircraft state through iteration
5508, except the existing region bit/countdown differences. Export preserves
original RAM/registers/timing. Corrected carrier input reaches the unconnected
C083E2 touchdown mission reset, now connected and verified at iteration 6288.
Its old helper's wrong player-phase address is corrected. Aircraft landing,
ground/contact, speed and reset fields agree apart from the two startup debts;
qualification result callbacks C11078/C110A4 and viewport/restart
C0F946/C0F974/C0F992 are now connected. The carrier recording completes with
consumed keys, persists qualification success and restarts; fifteen source
parent cases and save/reload pass. Two of three scenarios now have functional
outcomes; full frame parity remains open, including post-result timing/state.
Native demonstration flight now reads original textply/textctl recorder assets,
connects C0FA04/C0FA4C/C0FA80, NPC dispatch/guidance and selected-fire launches,
and completes all 4,892 recorded updates. Four native record checkpoints,
31 startup/launch cases, source asset comparisons and regressions pass.
Functional scenario wiring is 3/3; native full frame parity is still 0/3 accepted.
The demo reaches active flight early; original audio-availability flags and
message/viewport cadence remain open. See `analysis/native_demo_milestone.md`.
Next: original startup/audio availability and full frame/result cadence. Evidence:
`analysis/native_game_input_milestone.md`.
Touchdown evidence: `analysis/native_carrier_touchdown_milestone.md`.
Result evidence: `analysis/native_qualification_result_milestone.md`.
The first functional intro/menu milestone is implemented: original splash,
credits and settled menu pixels match source OFF reference frames exactly;
first-tour callsign editing and 78-byte save/reload pass; SDL dummy presentation
runs. Native link-map omission check passes. Both reference builds and their
twelve tests pass. Native settled-screen timing is not frame-parity evidence.
Native menu continuation is now connected: original digit/function-key mode
selection, mission availability, pilot-log summary, Escape returns, SHIFT-2
reset and 78-byte save/reload. All five mode banners, the mission list and
pilot log match source settled pixels exactly. Both compiler reference builds,
the existing frontend checks and focused menu checks validate this batch.
Evidence: `analysis/native_menu_milestone.md`.
Native Free Flight now runs the C08F26 storage/pose/template-gate prefix,
C0FECE countdown/scene constructors, C101FC/C10228 viewport transition and
C10678 mode messages. Initial player pose, camera and all three template-gate
banks match a focused original checkpoint. Full record state still differs;
the final bootstrap record update and context refresh are now connected.
The record/context slice now repeats during setup (12 updates in the focused
run). Native record composition matches original C1C63E non-stack RAM at native
boot/setup checkpoints and an original Free Flight checkpoint, including active
slot dispatch and its inverse matrix. Complete input/view/timer ordering and
other active children remain open. The original disk's $47 message reads
**CRACKED BY A-HA**; the earlier crash-message diagnosis was wrong. See
`analysis/native_bootstrap_records_milestone.md`.
Free Flight now consumes message completion at C1072E, accepts Return through
the original typed-code check, and takes location/aircraft keys through the
source command-selection/publication owners. C10B90 resets the recorder/root
and updates records; the root now becomes aircraft kind $11. Camera-origin
normalization and matrix children run directly. The tested path reaches
C10C08 after aircraft selection, or C10DAE after P pause/resume. Five bounded
C1C63E checkpoint comparisons match original non-stack RAM; intro/menu and
twelve reference CTests pass. Evidence: `analysis/native_setup_selection_milestone.md`.
Other modes still stop at their banner. No native flight runs yet.
The native setup preview now draws the source horizon and terrain packets,
with direct host plane line/fill/composite operations. Its real caller is
native_flight_tick -> native_scene_project/native_scene_draw -> the shared
source owners. 160 polygon cases and complete horizon/map submissions match
original plane buffers at location and aircraft checkpoints. Existing setup,
record and frontend checks plus twelve reference CTests pass. Copper fade is
excluded; no full sealed replay was repeated. Evidence:
`analysis/native_terrain_preview_milestone.md`. Rough startup estimate about
80% (previously 70%), scoped to Free Flight startup wiring: terrain now renders,
but aircraft/scene objects, cockpit/HUD, full update ordering and active flight
remain open. Native data is still addressed storage rather than typed state.
The native setup now also draws scene placement/model commands, ground
descriptors and the fixed matrix mark. The C1EE14 static model driver calls
existing direct draw-stream geometry and host raster operations; no instruction
adapters enter this runtime path. Three native setup checkpoints execute
246/10,334/17,005 descriptor calls. Their reached descriptor returns, transformed
vertices and plane buffers match focused original comparisons; 48 circle cases
also match. Frontend and Free Flight selection/pause/resume checks pass.
Evidence: `analysis/native_scene_objects_milestone.md`. Rough scoped startup
estimate now 85% (previously 80%): scenery is connected, aircraft records,
cockpit/HUD and active flight remain open. No full replay repeated.
Aircraft descriptor/record drawing C1ED4C/C1F000 and C1CCBC followup composition
now execute, along with C279D0 grid and C1518C setup control records. Three native
checkpoints execute 290/22,484/31,567 descriptors; 10/34/14 reached descriptors
and all three complete parents match original non-stack RAM. Compact/extended
hull tails and circle masks also pass. Native frontend/menu/setup checks and
twelve reference CTests pass. Evidence:
`analysis/native_aircraft_rendering_milestone.md`. Rough startup estimate now
90% (previously 85%), scoped to Free Flight startup wiring. Cockpit/HUD and
active flight remain open; no full replay repeated.
Native view controls C12098 now run before the record pass, exposing C1B27E
input recording, C13D84 indexed controls and C149BE root motion. Their source
normalization, attenuation, region probe, timer and touchdown tone children are
connected. Aircraft selection reaches C10DAE without needing P; throttle moves
the root with positive speed, and arrow press/ramp/release execute. Seven
native checkpoints match original C12098/C1C63E non-stack RAM. Source clock
requests use the same deterministic host clock in both validation paths.
Evidence: `analysis/native_flight_controls_milestone.md`. Rough startup estimate
now 95% (previously 90%), scoped to Free Flight startup wiring. This demonstrates
grounded motion and stick recording, not takeoff or full flight parity.
Native cockpit/HUD now connects nineteen instrument/panel owners: marks, tapes,
numeric readouts, threat lights, frame/image, compass and indicator/mode bars.
Their native branches perform direct host plane operations. Three native
checkpoints exercise 780/1307/1707 HUD frames; 95 original-instruction cases at
each checkpoint match all non-stack RAM, including planes. Evidence:
`analysis/native_hud_milestone.md`. Rough Free Flight startup wiring estimate
now 97% (previously 95%). Complete frame ordering/cadence and full flight remain
open; this is not recorded-run acceptance.
Next: complete frame ownership, remaining HUD/input and flight-record children.
The C32CEE text sequence now follows flight work, with C11B44 notification
cadence before the record pass and C11BFC warning selection before HUD drawing.
C31226 postflight dispatch and C322EE message-line drawing are connected.
Three checkpoints each pass 115 HUD/message cases; frontend and setup checks
pass. C25312 timer polling and C2548A sampling now execute, followed by the
source game-counter increment. Polling yields across PAL ticks without
repeating physics, HUD or final text; the disk table controls the threshold.
C28996 periodic region work and reached C28E28 zone checks execute. Seven
view/record checkpoints, 36 timer/readout oracle cases, two periodic record
passes, focused yielding and setup/pause/resume checks pass; twelve reference
CTests pass. See `analysis/native_clock_milestone.md`. Rough startup wiring
estimate now about 98% (previously 97%), excluding full flight/frame acceptance.
Game update cadence is distinct from the host PAL clock. Complete frame
ownership and remaining record children are still open.
C12950 control/sound actions now run before the HUD and in the inactive branch,
using typed C locals/arguments and existing audio consumers. The 7000-frame run
executes 2454 action-owner calls; source event/action/pending state is consumed.
720 original-instruction cases match non-stack RAM and 1469 exact sound calls;
the seven view/record checks and link omission pass. Startup wiring remains
roughly 98%; native samples/output and complete frame ownership are still open.
See `analysis/native_control_actions_milestone.md`.
Native scene drawing now uses the shared C0F048-C0F124 owner: bias gating,
flagged-only controls and range-dependent child order follow original source.
1024 complete parent-contract cases, three renderer checkpoints, setup/pause
checks and twelve reference CTests pass. The C0DA38 alternate display route
now fails explicitly pending its page-presentation contract. See
`analysis/native_scene_ordering_milestone.md`. Startup remains roughly 98%.
Native pullback now renders positive C1F584-C1F6F8 paired model strips, with
source interpolation, reverse lanes and wrapping arithmetic. Reached C25704
record warning messages are connected. 96 strip cases and a 7150-tick runner
checkpoint pass; its 29 reached descriptors include a positive strip group.
View/record comparisons also pass at 7150 and 7280. See
`analysis/native_model_strips_milestone.md`. The same pullback next fails the
wide terrain pass at tick 7294, height 1800; takeoff remains unproven. Resolve
that reached path next. Startup estimate remains roughly 98%.
Negative/wrapped terrain visibility indices now read the original loaded image;
the source X selector word is preserved. 384 source cases and repaired tick
7294 terrain buffers pass. The 7150 checkpoint explicitly proves short takeoff:
ground flag clear, height 264160 versus 1800, takeoff bookkeeping set; original
view/record and reached rendering comparisons pass. Rough startup wiring now
99%, excluding recorded-flight acceptance. The probe then reaches postflight
MC_STAGE_SETUP/C0F4A6 free_all_voices, still missing from native setup. See
`analysis/native_map_visibility_milestone.md`. No full sealed replay repeated.
C1612C outer display now resumes across host PAL waits, publishing the completed
page while C2F558 selects the next draw page. Its activity counter decrements
only after the original four palette waits. The native 7400/7404 pair preserves
all gameplay/plane state while activity goes 16 -> 15. By tick 8200, activity is
zero, C11788 has completed one reset and C10DAE/record updates have resumed.
128 resumable source cases, 1024 blocking CPU/RAM contracts, three postflight
source cases and the affected native/reference checks pass. See
`analysis/native_outer_display_milestone.md`. This bounded display/reset batch
is complete (100% of that milestone); startup remains roughly 99%, and full
frame ownership/recorded-flight acceptance remain open. No full replay repeated.
The source C0F3C4 input owner now runs before C0F5F8/view/record work. Native
keys queue through C16EAE/C16BF2/C16C56 and C1AD74/C1AC28; source recorder
drains, command publication and clearing execute. Four native checkpoints
prove that a queued press waits for input polling without repeating gameplay,
then applies the original right-direction bit and releases. 144 original
non-stack-RAM cases, six affected native checks, frontend/menu and twelve
reference tests pass. See `analysis/native_input_milestone.md`. This input
batch is complete (100% of keyboard/source-owner integration); physical
gameport acquisition, modifier timing and inherited countermeasure arguments
remain open. No full sealed replay repeated or whole-game estimate inferred.
Qualification (digit 5 / mode 9) now proceeds through carrier construction,
briefing acknowledgement and the existing context chain into C10DAE. A short
F10/pullback probe clears the ground flag and increases speed/height. C207FE
carrier surface gating and C1FF0A accumulated face-test values are connected;
C0A2F0 qualification landing scheduling follows its original gates. Four native
checkpoints, three sets of 20 original startup/briefing/scheduler cases, complete
view/record and reached rendering comparisons pass, plus 48 flag/face-result
cases, affected native regressions and twelve reference tests. See
`analysis/native_qualification_milestone.md`. This startup/short-takeoff batch
is complete (100% of that scope); full landing/outcomes and recorded-run parity
remain open. No full replay repeated. Demo/remaining modes still stop at banners.
Native recorded-input delivery is now connected: `--input` consumes sealed
FA18_LOOP_INPUT_V1 raw keys by main-loop iteration, anchored at the main menu
after ordinary cold startup. Same-update ordering and suspension counters pass;
bounded crash (1960) and carrier (4760) prefixes reach active qualification.
Original view/record and reached carrier rendering comparisons pass. This
delivery milestone is 100%; original cadence and full recorded parity remain
open. The later crash prefix reaches an unconnected C17F8C collision-sound child.
See `analysis/native_loop_replay_milestone.md`.
C17F8C collision sound and C06C02 release fault are now connected. The sealed
crash prefix reaches two postflight resets by iteration 2150 (288 record
updates); actual original view/record and six collision-child cases pass.
This bounded collision/reset milestone is 100%. The next missing stage is
C0F920, returning the sequence to the main menu; full recorded parity remains
open. See `analysis/native_collision_milestone.md`.
C0F920/C08F26 sequence reset is now connected, with C2FD22 clearing the actual
renderer work banks. The entire sealed crash input completes 3082 iterations /
168 events, three postflight resets and return to the main menu; selecting
qualification again reaches its carrier briefing. Four original reset cases
match all non-stack RAM, including dirty work banks. Existing qualification and
frontend/link checks pass. Sequence return is 100% of this functional scope.
One of three sealed input scenarios runs to its stated outcome; no complete
native recorded-frame parity has been accepted. See
`analysis/native_sequence_return_milestone.md`.
Next: carrier-success/demo outcomes, complete C0EFD4 ownership, remaining HUD/record children and C0DA38
alternate presentation. Exact input-callback/beam timing remains unproven.
Resolve input/view/timer ordering; missing reached model children still fail
explicitly rather than supplying substitute geometry.
Source sound requests now consume the original mute/absent-voice gates; native
sample loading/output remains open. Source data still uses
checked address-indexed host buffers, pending typed-state migration.

Updated 2026-10-06 after active-runner emulation metering, following the user's source-ownership correction, cleanup and
explicit instruction to prevent another costly detour. Read this handoff and
`AGENTS.md` before continuing. This file supersedes earlier resume instructions;
previous handoffs and removed source are preserved in git history.

The user's current comparison policy (confirmed 2026-10-06) ignores Copper
fade when comparing frames. Fade-only palette/brightness differences are
excluded; geometry, other rendering behavior and gameplay state still need to
match. Treat strict RGB hashes as diagnostics, not sufficient evidence of a
failure under this policy.

Latest measurement correction (schema 2): source-instruction adapters are now
counted at `step_begin`, where they fetch/decode an original opcode. Retained C
entry scheduling is excluded. The old schema omitted that work and overstated
CPU removal: cached **38.4011%** and old bounded **69.8507%** are superseded.
The corrected **800-frame, three-recording** raw CPU minimum is **0.4470%**;
per scenario demo **2.5530%**, carrier **0.4470%**, crash **2.4463%**. This is a
partial probe, not a full-suite result or completion percentage. The corrected
full-suite minimum is unmeasured. Accepted share unavailable: source comparisons
still fail. Memory/chipset/boot cutover **0%**, subsystem deletion **0/4**.
No gameplay behavior changed in this meter batch; old demo RGB/index/RAM and
all old profile fields match, MSVC/GNU profiles match, twelve CTests and profiling
invariance pass. No full replay repeated. Evidence:
`analysis/emulation_removal_adapter_meter_probe.json/.md`; the canonical
`analysis/emulation_removal_meter.json/.md` now contain this schema-2 partial
probe. Historical schema-1 full evidence remains in git/build history and must
not be used as the current estimate. Continue native game/parent ownership and
report future deltas using schema 2. The connected chain still removes 892
instruction cases, with explicit parity/timing/stack/matrix debt below.

Previous connected batch: native C1C63E retains game `RecordUpdateStageFrame`
and calls the C22C80 record loop directly. All 112 update-stage CPU cases are
deleted; the shared C22C80/C1C63E instruction body is now **100% removed**.
This connected chain has removed 892 instruction cases, retaining thin entries,
guest data and other original children. 4,096 original-child core cases match
all state; 4,096 production cases match registers/PC/SR and RAM outside old
CPU stack scratch. Strict all-RAM still fails (case 0 at C7FED1). Both compilers/
runners, twelve CTests and profiling pass. The MSVC/GNU demo matches. Bounded
recordings reach 43/15/75 native stage -> record-loop calls, with zero C22C80
CPU entry dispatch. Previous-build non-fade/index differences start at
381/446/263 (64/1,588/12,536 pixels); final RAM differs. Timing/parity stay open.
No full replay repeated. Bounded raw demo **69.8507%**, delta **-0.0008 pp**;
full raw **38.4011%** cached, accepted share unavailable, axes **0%**, gate **0/4**.
Raw meter excludes CPU-style adapters; repair that measurement before using it
for further progress estimates. Inventory unchanged. Evidence:
`analysis/emulation_removal_update_stage_batch.json/.md`.

Previous connected batch: C22C80 now runs a retained game `ControlRecordsFrame`
and calls the retained record-dynamics C owner directly. All 226 outer CPU
instruction cases are deleted; the sibling C1C63E's 112 cases are unchanged.
Together with the previous batch, **780/780 cases of these two targeted CPU
bodies (100%) are removed**. Thin entry adapters, guest memory and other
children's runtime boundaries remain, so this is not whole-plan completion.
4,096 original-child core cases match all state. 4,096 production continuation
cases match registers/PC/SR and RAM outside old CPU stack scratch C7FD00..C7FEFF;
strict all-RAM fails case 0 at C7FED5. IRQ/event timing is held in those proofs.
GNU/MSVC both runners, twelve CTests, profiling and the MSVC/GNU demo pass.
Three 800-frame recording probes reach 129/30/150 direct C22C80 -> C25B66 calls
and zero C25B66 CPU-entry dispatch. Non-fade comparisons versus the previous
batch fail at 346/446/263; final RAM differs. Timing/parity remain open. No full
replay repeated. Bounded demo raw **69.8515%**, delta **-0.0191 pp**;
full **38.4011%** cached, accepted share unavailable, axes **0%**, gate **0/4**.
Raw counters exclude CPU-style port steps and cannot represent whole-plan
completion. Counts remain 25/590/84/691. Evidence:
`analysis/emulation_removal_record_loop_batch.json/.md`.
Next: C1C63E parent ownership and native root/control/render children; preserve
explicit timing, non-fade, stack and matrix acceptance debt.

Previous connected batch: C25B66 now schedules retained game C dynamics and
composes its five native child owners directly. Its 554 CPU instruction cases
are deleted (42.3547% of the shared family); 754 other cases are unchanged.
A frame-end return fix prevents the interpreter consuming a pending C return.
16,384 held-child cases match full state and all 554 boundaries; six focused
runner frame pairs pass. GNU/MSVC both runners, twelve CTests, profiling and
an MSVC/GNU bounded demo pass. Original-child live matrix fixtures still fail
case 2. Three 800-frame probes complete but non-fade changes versus the previous
commit begin at 591/446/263 (39,478/2,989/187,594 pixels); final RAM differs.
These parity gates and unmodeled parent event timing remain open. No full replay
repeated. Bounded demo raw CPU **69.8706%**, delta **-0.0073 pp**;
full raw **38.4011%** cached, accepted share unavailable; other axes **0%**,
deletion **0/4**, counts 25/590/84/691. Raw counters exclude CPU-style port
steps, so they do not measure this body deletion or whole-plan completion.
Evidence: `analysis/emulation_removal_flight_parent_batch.json/.md`.
Continue outer C22 record-loop C ownership and direct parent integration.

Previous connected batch: native C2C392 now calls `normalize_record_vector` ->
existing `magnitude3` directly in game C. Its normal path no longer yields to
C2574A/C1D974 CPU entries. 12,400 production continuation cases match complete
register/PC/SR/RAM state with held events, including 396 calls to each child
and zero corresponding CPU-entry dispatch. A constructed real-runner frame
case reaches both edges and matches source RAM/RGB/indices. Its interrupts
are masked; outer instruction/event timing remains open. All six frame pairs,
GNU/MSVC runners, twelve CTests, profiling and the MSVC/GNU demo pass. Three
800-frame recordings match previous outputs/profiles and do not reach this
normalization path. Batch delta **0.0000 pp**, bounded demo **69.8779%**,
cached full raw minimum **38.4011%**, accepted share unavailable; other axes
**0%**, deletion **0/4**. Counts remain 25/590/84/691. Other callers still use
the normalization adapters. No full suite repeated. Evidence:
`analysis/emulation_removal_normalization_batch.json/.md`. Normal gameplay
children of this autopilot owner are now C calls; continue outer flight/loop
ownership, keeping the fault and unknown-transfer boundaries explicit and
the zone-exit timing regression below unresolved.

Previous connected batch: native C2C392 now calls six game steering functions
directly with C values/results, removing their CPU dispatch from this owner.
49,600 production continuation cases match all registers/PC/SR/RAM with held
events; 17,118 direct calls cover all six children. Six source-constructed
real-runner frame pairs start at original C22D88 -> C25B66, stop at the original
autopilot return and match RAM/RGB/indices. IRQs are masked in those frame
fixtures; sealed gameplay and parent event timing are separate open gates.
Three 800-frame recording RGB/index/RAM and full profiles are unchanged; an
MSVC/GNU demo matches. Both runners/compilers build, twelve CTests and profiling
checks pass. Steering CPU adapters remain for other original/unknown callers;
counts stay 25 C-owned / 590 CPU rows / 84 deferred. Bounded demo **69.8779%**,
batch delta **0.0000 pp**, cached full raw minimum **38.4011%**, accepted share
unavailable. Other cutover axes **0%**, deletion **0/4**. No full suite repeated.
Evidence: `analysis/emulation_removal_steering_calls_batch.json/.md`. Continue
native normalization and flight parent ownership; retain the zone-exit timing
regression below as unresolved acceptance debt.

Previous connected batch: C25B66's C25BA6 zone-exit call now selects native
`update_dynamics_record_zone_exit` -> `advance_record_zone_exit`. Its C frame
retains fault/placement phases across original runtime children. C28E28's CPU
entry/registry/guard and sole wrapper file are retired. **16,384 production
continuation cases** match all registers/PC/SR/RAM, covering **79/79** source
instructions. Both runners/compilers build; twelve CTests, GNU profiling and
an MSVC/GNU bounded demo pass. Native calls in 800 demo/carrier/crash frames
are **2/0/2**, CPU-entry calls zero, aggregate CPU counts unchanged. Demo/carrier
outputs and all three final RAMs match the preceding build. Crash rendering
has a **185-pixel non-fade regression**, first at frame **556**; source first
differences still **619/446/263**. Rendering acceptance is open; parent
instruction/event timing is unmodeled. Component correctness does not close
that integration gap. This removes an entry/stepping dependency, not new
metered CPU work: delta **0.0000 pp**. Counts **25 C-owned / 590 readable CPU
rows**, 84 deferred entries. Bounded demo raw CPU **69.8779%**; full minimum
**38.4011%** is cached, accepted percentage unavailable; other cutover axes
**0%**, subsystem deletion **0/4**. No full replay suite repeated. Evidence:
`analysis/emulation_removal_zone_exit_batch.json/.md`. Follow up the rendering
timing debt before claiming native-flight acceptance; remaining child calls
and the outer C25B66 CPU/event parent are still removal work.

Previous dependency removal: C25B66's C25C6A call now selects the native
forty-arm C2C392 record autopilot. The original owner has **987 instructions**,
including **361 cold instructions** missed by the old 626-instruction body.
A retained C frame owns response limits and continuation phases; original
normalization/steering children run through the existing dispatcher.
49,600 component cases match all registers, PC, SR and RAM (864/987 boundaries);
12,400 production-continuation cases match the same state (841/987). Both cover
all 361 cold instructions. Unknown/modified action targets retain the exact
original transfer; arbitrary fault-target behavior is not claimed proven.
Both GNU/MSVC runners build; twelve CTests, GNU profiling, the parent DMA
oracle and continuation IRQ/SP/lifetime/reset fixture pass. GNU/MSVC demo
frames/indices/RAM match. In 800 demo frames, 27 calls are native, CPU-entry
calls are zero, and aggregate CPU instructions decrease **2,076**. Demo raw
CPU work avoided is **69.8779% (+0.0397 pp)**; carrier/crash are unchanged.
Source parity remains open: first non-fade differences **619/446/263** (demo
previously 565). Parent instruction/event timing remains unmodeled, alongside
outer CPU/guest state and source child dependencies. There are **24 C-owned
entries**, **591 readable CPU rows**, **84 deferred entries** and **691 direct
opcode bindings**. Full raw CPU **38.4011%** is cached; bounded three-recording
minimum **48.0797%** is unchanged; accepted CPU percentage remains unavailable.
Native memory/chipset/boot **0%**, subsystem deletion **0/4**. No full replay
suite was repeated. Evidence: `analysis/emulation_removal_autopilot_batch.json/.md`.
Next: native normalization/steering child calls, then C28E28 in the same
flight parent. This remains actual game ownership work, not a fixed-cycle
adapter repair.

Previous dependency removal: C25B66's live matrix call at C25D9E selects the
game C owner directly, with copied arguments retained across IRQ service.
C2D408's CPU entry adapter and registry row are removed. Six bounded
before/after pairs (three recordings, parent and combined selections) match
all 800 RGB/index frames and final RAM. Combined dispatches decrease by
68/15/75; instruction/device counts remain identical. The DMA parent oracle
and boundary lifetime/IRQ/reset fixture pass, both GNU/MSVC runners build,
twelve CTests pass, GNU profiling is invisible and an MSVC/GNU demo matches.
There are twelve C-owned entries and 602 readable CPU rows. This is a live
call-site dependency removal; the flight parent, guest memory, result publisher
and existing 9,500-cycle atomic matrix timing remain CPU/chipset dependent.
Bounded CPU-work delta **0.0000 pp**, cached full raw CPU **38.4011%**;
memory/chipset/boot cutover **0%**, subsystem gate **0/4**. Source parity remains
open at demo/carrier/crash frames **565/446/263** (Copper fade excluded).
Evidence: `analysis/emulation_removal_matrix_dispatch_batch.json/.md`.
Prioritize native flight ownership and ordered events next; do not restore
the removed child CPU adapter. The plan remains incomplete.

Latest validated prerequisite replaces C265E8's fixed scan charge with
source timing. All 65 instructions / 2,080 DMA cases and isolated 800-frame
comparisons pass; both builds, twelve CTests and GNU profiling checks pass.
Combined carrier moves 374 -> **446**, crash 213 -> **263**, demo stays 565.
No full rerun: cached raw CPU **38.4011%**, new delta unmeasured; native
memory/chipset/boot **0%**, gate **0/4**. This removes a timing error, not a
CPU dependency. Evidence: `analysis/emulation_removal_slot_timing_batch.json/.md`.
The user's concern about slow removal progress is valid: prioritize connected
native call ownership and deletion next, rather than further adapter expansion.

Latest connected timing batch repairs the C310AA/C12242 compass/selection
interaction. All 39 instructions / 1,248 DMA cases match; three paired
800-frame drawing/RAM probes and every shadow/sandbox call pass. Both builds,
twelve CTests, GNU profiling checks and an 800-frame MSVC/GNU paired demo pass.
Combined demo first non-fade difference moves from 316 to **565** (683 pixels);
carrier/crash still differ at 374/213. No full replay rerun: raw CPU **38.4011%**
is the cached full-suite figure, new delta unmeasured; memory/chipset/boot
**0%**, gate **0/4**. Evidence:
`analysis/emulation_removal_selection_timing_batch.json/.md`.

Historical failure before the slot scan timing repair was the crash
**C265E8,C2D408** interaction: a
220-frame minimized pair first changes indices at 213; neither member alone
differs by 220. Reused isolated matrix captures first differ at demo 584,
carrier 446, crash 263. Preserve native side/depth ownership, ordered events
and original arithmetic; do not tune fixed averages or restore removed CPU
child adapters as game behavior. Continue toward the native frame entry and
remaining removal phases; source timing bridges do not close any removal axis.

Latest timer prerequisite: C25482's fixed 30-cycle charge is source-timed.
All four instructions / 128 DMA cases match, three isolated 800-frame drawing/
RAM comparisons match OFF, and all 27/15/36 shadow/sandbox calls pass. Both
builds, twelve CTests and GNU profiling checks pass. Message/image/timer group
matches demo through 800. ALL still differs at 316; neither half of the full
603-entry registry now reproduces it alone. Minimize the interacting paths
next. No full rerun: cached raw CPU **38.4011%**, new delta unmeasured;
memory/chipset/boot **0%**, gate **0/4**. Evidence:
`analysis/emulation_removal_timer_timing_batch.json/.md`.

Latest image prerequisite: C30EAA's fixed 3,000-cycle blit adapter is now
source-timed and reuses the existing C30F46 plane-stream tail. Its 62 source
instructions / 1,984 DMA oracle cases match. Three isolated 800-frame drawing/
RAM comparisons pass; sandbox passes, shadow has zero mismatches with hardware
interruptions classified separately. The message-plus-image pair matches demo
through 800, but ALL still differs at 316. A 603-entry short subdivision now
isolates C25482's timer charge. Both builds, twelve CTests and GNU profiling
checks pass. No full rerun: cached raw CPU **38.4011%**, new delta unmeasured;
memory/chipset/boot **0%**, gate **0/4**. Evidence:
`analysis/emulation_removal_image_timing_batch.json/.md`.

Latest timing prerequisite: C11BFC's fixed 3,000-cycle message adapter is
now source-timed. All 256 instructions / 8,192 DMA oracle cases match; each
800-frame isolated recording matches OFF drawing/RAM and every shadow/sandbox
call (42/15/74, none incomplete). Both builds and twelve CTests pass. Combined
ON still differs at demo frame 316; fresh short subdivisions isolate C30EAA's
fixed image-blit adapter as another reproducer. Address that real rendering
path next. No full replay rerun: raw CPU **38.4011%** is the cached measurement,
new full-suite delta unmeasured; memory/chipset/boot **0%**, gate **0/4**.
Evidence: `analysis/emulation_removal_message_timing_batch.json/.md`.

The stores-icon prerequisite batch replaces the active `C30A00` / `C30AE2`
fixed-cycle adapters with source-derived steps in
`port/game/glue/glue_hud_stores_step.c`. Runner entry -> machine frame -> recomp
hook -> active port registry reaches these steps during recorded flight. This
removes their fixed-cycle timing error, not their PC/register/bus dependency.
All 90 instructions / 2,880 opcode-oracle cases match; parent and independent
child shadow/sandbox checks pass; all three full isolated recordings match RGB
and sealed final RAM. Both toolchains build and all twelve CTests pass.

Combined ON matches demo01 through frame 564 under strict RGB comparison;
assess subsequent differences with the fade exclusion. The full meter rerun
in `analysis/emulation_removal_meter_after_hud_stores.json/.md` gives raw minimum
CPU-work removal **38.4011%** (+0.0090 percentage points); final-RAM/iteration
acceptance remains open. Memory/chipset/boot cutover remain **0%**, deletion
gate **0/4**. This is validated integration scaffolding, not Phase 1 completion.

Fade exclusion is implemented, not just documented: headless `--index8`
records the selected palette index alongside RGB444. The shared comparator in
`scripts/compare_recomp_frames.py` requires matching indices and excludes only
same-index colours from the source's C08510 fade table. It preserves checks for
upper-palette/other colours, drawing during black frames and unequal frame
counts. The meter, live gate, timing probe and image report use it (NumPy is
required for these Python comparisons).

The full fade-aware report excludes millions of actual fade differences, but
combined ON still fails drawing/state parity: first index differences occur at
demo01 frame 316 (64 pixels), carrier 374 (55,901), crashes 213 (1,905), and
ADF GNU/MSVC 1460 (26). Frame 565 changes indices, so it is not fade-only.
All three full isolated stores recordings pass the new frame/RAM gate. Both
toolchains build, all twelve CTests pass, and capture/profiling remain invisible
to RGB/RAM/stdout/stderr in all three CPU modes on both toolchains. Raw CPU,
native-memory/chipset/boot axes and deletion gate are unchanged by diagnostics.

Phase 1's first connected batch is `C2D408` -> `C1342C`: runner entry -> machine
frame -> recomp hook -> active `glue_C2D408` ->
`update_record_nonclass_matrix` -> `load_record_velocity` -> direct
`update_matrix_side_record()`. The child no longer exchanges Musashi registers
with its parent. Before/after compatibility observations carry ordinary C
values, published only at the outer CPU adapter. `glue_matrix_side_record.c`
and the C1342C registry/prototype entries are deleted. Known static callers
and all five suite profiles establish this ownership within that coverage;
`port/game/native_call_graph.json` preserves it for tooling. The game child has
no custom-register/service/interrupt boundary within this existing atomic
parent call. Guest memory and the parent's fixed timing charge remain; this
batch is not an emulation-free frame body. The isolated 800-frame demo executes
56 direct child calls in 70 parent calls; RGB, indices, RAM and existing counters
are byte-identical to the prior ON runner. Parent shadow: 65 matches, five
incomplete, zero mismatches; sandbox: 70 matches, zero incomplete/mismatches.
The full suite executes 2,809 / 3,341 / 312 / 106 / 106 direct calls (three
recordings, GNU/MSVC ADF). Every previous meter counter, output hash, runner
statistic and fade-aware comparison is unchanged. See
`analysis/emulation_removal_meter_after_native_side.json/.md`: raw CPU removal
**38.4011%**, delta **0.0000 pp**; accepted CPU share remains unavailable because
the combined baseline still fails parity. Memory/chipset/boot **0%**, gate
**0/4**. Next: retire proven C-only leaves inside this native child; keep
original generated reference implementations for OFF/source comparisons.

The next validated batch retires all seven CPU entry adapters under C1342C:
C13A2A, C13A8E, C13B5A, C13BA0, C13C0A, C13C64 and C13CDE. Their known original
callers are native C owners, and none independently dispatches in any full-suite
ON profile. Domain behavior and generated reference code are retained. The
800-frame parent source shadow/sandbox checks still give 65/70 matches, zero
mismatches (five shadow comparisons incomplete). Both toolchains build; twelve
CTests and GNU profiling invisibility pass. All five full-suite output hashes,
counters, direct edge counts and comparisons are identical to the prior batch.
See `analysis/emulation_removal_meter_after_matrix_leaves.json/.md`.
Eight C entries now have no CPU adapter; 606 readable CPU registrations remain
(531 translated plus 75 source-only), with reconstruction still 614 entries.
Raw minimum **38.4011%**, delta **0.0000 pp**, cutover axes **0%**, gate **0/4**.
The 36 deleted guest-access sites belonged to unused adapters: **zero** game
state sites were converted. Current access-site count is 8,952.

The user clarified that full replays should run infrequently because of their
cost. Prefer affected source comparisons and bounded runner checks for routine
batches; reuse these full-suite reports. Run full replays for substantial
behavior changes, milestone acceptance or unresolved failures requiring them.
Next connected closure: C2D408 -> C2DD4E -> C2DE96/C2DEA2. Its parent already
passes depth results as C values. Parent fixed timing is a separate open issue:
the isolated 800-frame C2D408 probe first differs from OFF at frame 584 (488
non-fade pixels), despite matching source call results.

The depth closure is now validated and committed as the next batch: C2DD4E,
C2DE96 and C2DEA2 CPU adapters are retired, including `glue_matrix_depth.c`.
The game owner already calls these functions directly with C values; the outer
parent now profiles its actual depth call. The 800-frame isolated probe runs
69 direct depth calls and 56 side calls. Previous counters and RGB/index/RAM
bytes are identical; source shadow/sandbox counts remain 65/70 matches, zero
mismatches (five shadow incomplete). Both toolchains build and twelve CTests
pass. No full replay was repeated. Evidence:
`analysis/emulation_removal_matrix_depth_batch.json/.md`.
Current inventory is 603 CPU registrations plus 11 direct C entries, still 614
readable entries. Reused full-suite raw minimum **38.4011%**, observed bounded
counter delta **0**; accepted CPU share unavailable, memory/chipset/boot **0%**,
gate **0/4**. Next: investigate the parent's source timing boundary before
extending toward its generated callers; additional leaf retirement alone will
not fix its live frame mismatch.

The 600-frame timing trace identifies the actual outer caller as active
`glue_C25B66_step` -> JSR at C25D9E -> C2D408 -> C25DA4. Across 28 calls, OFF
boundary intervals vary from 10,240 to 20,622 cycles (mean 12,532.57), versus
ON's 9,520-9,530. Three OFF calls cross frame end, versus two ON. These intervals
include JSR/return bus settlement, not just child instruction time. Evidence:
`analysis/emulation_removal_matrix_parent_timing.json/.md`. Recover dynamic
source access/arithmetic/child timing and event boundaries; do not tune the
fixed charge to an observed average or restore reference execution as gameplay.
This bounded discovery changes no axis and does not repeat the full suite.

The native matrix-side helper chain now takes its selected record as a C
argument rather than rereading guest CURRENT_RECORD in six helper bodies.
The owner still publishes the original pointer; math/field accesses are
unchanged. Three isolated 800-frame probes preserve RGB/index/RAM, runner
results and all instruction/device/call counts. Native pointer reads decrease
by 56/0/106 (demo/carrier/crashes), only on page C18000. Parent source shadow
matches 65/66 calls (five/ten incomplete), sandbox 70/76, zero mismatches.
Both toolchains and twelve CTests pass; no full replay was rerun. Evidence:
`analysis/emulation_removal_record_arguments_batch.json/.md`.
Access sites now 8,946, zero state sites converted. Reused raw CPU minimum
**38.4011%**, bounded instruction delta **0**, cutover axes **0%**, gate **0/4**.
This makes native call inputs explicit ahead of the remaining timing work.

## What went wrong and must not recur

About fourteen hours of elapsed work on 2026-10-05 expanded a separate gameplay
implementation in the abandoned top-level `port/` tree. Its standalone proofs
were reported as progress toward emulation independence without establishing
that the playable runner called it. This wasted substantial user time and money.
Some shared host components were integrated and remain useful; this does not
excuse the disconnected gameplay work or its misleading progress reports.

The user clarified that `port/game/` is the active port and requested deletion
of the abandoned sources. Commit `0855f5d6` removed 759 tracked source/header
files and the old build definition. Commit `e70a7014` moved the 48 retained
dependencies into `port/game/` and updated builds, includes and active tools.
Do not recover the deleted implementation as a parallel port or resume its
bootstrap/pose/control-owner plan. Git history is reference evidence only.

The unsupported three-to-six-week estimate was withdrawn. The later "0%" answer described
the absence of a verified complete emulation-free gameplay path, not the amount
of work completed; it was misleading as an overall progress percentage. Neither
number is an accepted project estimate. The measured 614/699 (87.84%) figure is
only readable routine reconstruction within the known inventory. It is neither
whole-game discovery coverage nor emulation-independence completion.

## Required checks before substantial implementation

1. Verify the candidate implementation is compiled by
   `port/recomp/CMakeLists.txt` or `scripts/build_recomp.py`, and trace its caller
   from the actual runner entry. A library link alone does not establish use.
2. Identify the specific CPU, guest-bus, generated-code or chipset dependency
   the batch will remove, and the real startup/menu/flight scenario exercising
   that change. Record the active caller and planned integration in this handoff.
3. Make the first batch a small connected change exercised in the playable
   runner. Do not accumulate more disconnected owner modules and expensive
   component proofs while startup/frame integration remains absent.
4. Validate the changed runner path and the relevant original behavior. Report
   separately what is compiled, what is actually exercised, which dependency
   was removed, and which dependencies remain. Standalone proofs alone do not
   complete a runtime milestone.
5. Reuse existing source and evidence. Run checks appropriate to the affected
   behavior; repeat or broaden them only for new changes, failures or unresolved
   concerns. The retired 249-test suite is not the active runner's acceptance
   suite. Do not spend another long sequence validating the wrong target.

Report useful progress regularly during execution. If evidence shows a batch
is disconnected or targets the wrong build, correct the implementation direction
before continuing it. Do not substitute routine counts, passing test counts,
commit counts or elapsed time for a measured runtime result. Give any future
time/effort estimate with its scope, assumptions and measured basis; do not
invent an overall percentage or extrapolate a deadline from function counts.

## Work scope

Work on `port/game/`, `port/game/glue/` and the actual `port/recomp/` runtime.
The goal is the playable game without Kickstart or emulation. Do not build a
parallel gameplay implementation in the top-level `port/` directory.
A component proof counts as runtime progress only when the active runner uses it.

The abandoned `fa18_port` source/build and disconnected native replacement
components were removed. The 48 retained shared source/header files have now
been moved into `port/game/`: disk/Hunk loaders, map-packet code, its projection
type header and three command type headers. No top-level `port/*.c` or
`port/*.h` files remain. Build paths and active includes use the new locations.
The CMake runtime now compiles the map core with the other game sources; the
Amiga loader library compiles disk/Hunk once. The GNU source glob includes all
of these without duplicate explicit source lists. See `port/README.md`.

Historical `analysis/routines/native_*.md` notes without the `native_c_` prefix
and standalone `tools/recomp/check_native_*` component tools may reference the
removed implementation. They are historical evidence, not a resume plan or
runnable acceptance gates for the active game. Inspect their source dependencies
before using them. The disconnected uncommitted shared-origin batch was dropped.

Removal validation: both active runners build with MSVC Release and GNU;
all eleven active CTests pass. The GNU and MSVC ADF-only launcher checks pass
185 Hunk/8,441 relocation construction, both CPU modes, splash, credits and
keyboard-to-demo, with zero ROM accesses or unsupported services. Hash checks
before relocation confirmed all 697 tracked active-tree files, all 48 retained
sources and the three user-owned guard/allowlist files were unchanged.

Relocation validation also passes both MSVC/GNU runner builds, all eleven active
CTests and both ADF-only launcher checks. All 48 moved implementations are
unchanged apart from four loader include paths; the GNU manifest has 450 unique
existing source files. Loader/OFS checks match all 185 hunks, 8,441 relocations
and 22 resource hashes. The retained RGB4 and viewport comparison tools now use
the moved loaders and exclude checkpoint dependencies on deleted components:
16,384 RGB4 and 32,768 viewport construction/merge calls match their frozen host
implementations. These shared host components are used by the active runner.

## Active runner state

`fa18_romfree` loads the original game/resources from an ADF without Kickstart
or a savestate, using reusable host compatibility/loading in `port/amiga/`,
Interceptor configuration in `port/romfree/`, and guest adapters in `port/os/`.
It still uses Musashi, guest RAM and the chipset model. Removing the abandoned
sources does not establish emulation independence.

The last active function milestone (`05fa7453`) statically recompiles 85 deferred
entries in `port/recomp/generated/recomp_static_deferred.c`. They retain explicit
`STATIC_RECOMP`/`TODO(decompile)` markers and per-entry debt in
`recomp_deferred.json`. The inventory includes 539 readable translated entries
(528 CPU entry adapters and eleven direct C entries without CPU adapters)
and 75 additional readable source-only callable entries. Static compilation
still depends on shared CPU/machine state and is not readable decompilation.
See `analysis/routines/static_recomp_deferred.md` and its checkpoint for the
existing validation scope and original call-graph limitations.

Existing ROM-free verification covers clean startup/demo, menu and active
mission entry/restart, and original configuration save/load. Full mission
outcomes/progression, carrier success and active-flight teardown remain open.
See `analysis/routines/romfree_machine_startup.md`,
`analysis/routines/romfree_exact_followup.md` and
`analysis/routines/romfree_performance_followup.md` for evidence and limits.

## Next work and validation

Before sustained implementation, document the real startup/frame call graph
from `port/recomp/recomp_main.c` through machine execution and the registered
`port/game/glue/ports.c` owners. Inspect `port/game/memory.h`, hardware access
and child hooks for the remaining CPU/bus/chipset dependencies. Distinguish the
44 deferred original ADF entries from the 41 reference wrappers; only the actual
required game paths define the cutover work. The known inventory does not prove
the complete original callback/call graph has been found.

Use that evidence to choose the first small dependency-removal batch in the
active runner. The intended milestone is clean launch into active flight with
Musashi absent from that executable; it is not delivered yet. Chipset removal
and complete game-mode acceptance remain additional requirements. Keep each
change connected to the actual game/runtime call graph and record its measured
result before extending the batch. Reuse retained shared code where called.

Build the MSVC runners with `cmake -S port/recomp -B build/recomp-cmake` and
`cmake --build build/recomp-cmake --config Release`. Headless GNU builds use
`python scripts/build_recomp.py` and `python scripts/build_recomp.py --romfree`.
Run `ctest --test-dir build/recomp-cmake -C Release --output-on-failure`.
For changed gameplay use `scripts/recomp_ports_check.sh` and affected live
RGB/RAM comparisons with `scripts/recomp_live_check.sh`, plus targeted original
routine proofs. Keep known combined readable-C timing debt explicit.

Original disk bytes and source behavior remain the authority; emulator/ROM
runs and sealed captures are validation evidence. Do not alter
`scripts/check_native_build.py`, `scripts/native_frame_count.py` or
`port/native_data_allowlist.txt`. The former `fa18_port` guard is retained as
user-owned historical tooling; its removed target is not an active build.
Preserve unrelated `.vscode/` content. Commit completed validated batches as
previously requested. The emulation-independence goal remains unfinished.

## Emulation removal: measured baseline (2026-10-06)

The active objective is `port/EMULATION_REMOVAL_PLAN.md`, with commits after
validated batches and scoped percentages at each step. Phase 0's call graph is
`analysis/emulation_removal_call_graph.md`; the meter is
`tools/recomp/emulation_meter.py` and its results are
`analysis/emulation_removal_meter.json/.md`.

`--profile` preserves routine-count keys and adds `_emulation`. It counts every
interpreted instruction including ROM, every executed generated/static opcode
helper, original-byte execution between labels and the three OS RTE helper
paths. Guest API accesses are attributed to interpreter, generated, residual,
port, OS, chipset or host, with conservative overlapping 4 KiB RAM-page hits.
Direct DMA and host-compatibility memory accesses are outside that page count:
absence does not establish exclusive native ownership. Chipset counts cover
live blits, Copper instructions, bitplane word fetches, CIA events and interrupt
requests. OS dispatches and port calls/steps expose remaining adapter work.

Baseline suite: full sealed demo, carrier-success and crash recordings, plus
the original ADF keyboard-to-demo sequence on GNU and MSVC. Raw CPU-work shares
removed are respectively **85.5191%, 69.8738%, 69.7856%, 38.3921%, 38.3921%**;
minimum **38.3921%**. **All five fail OFF/ON RGB parity**; all three native ON
final RAM seals also fail. ADF OFF/ON update iteration counts differ. These are
observations of the existing port, not accepted removal progress or whole-game
completion. The report's accepted CPU share is null while parity fails.

Memory cutover: **0%**, 0/8,988 guest access sites converted. Native chipset and
native boot: **0%**. Deletable subsystems: **0/4**. This instrumentation batch
removes no dependency and has **0 percentage-point removal delta**. Phase 0's
measurement deliverables are published; its suite acceptance requirement and
Phases 1-6 remain open.

Validation: both active GNU and MSVC runners build; all **12** active CTests
pass, including new profiling visibility checks when local reference inputs
exist. `tools/recomp/check_emulation_meter.py` passes on both toolchains:
120-frame OFF, ON and interpreter runs retain identical stdout/stderr, every
RGB frame and final RAM with profiling enabled or disabled. Both full ADF-only
launcher checks pass construction (185 hunks/8,441 relocations), both CPU modes,
splash, credits and keyboard-to-demo, with zero ROM accesses/unsupported services.
An independent unmodified `9c062b80` GNU build reproduces the full crash
recording and 2,600-frame ADF sequence in OFF and ON byte for byte (all RGB,
RAM and statistics). Their parity failures therefore predate the counters.
`inventory.py` now finds active `game/*.h` citations: 332 translated routines
have module matches instead of none from the obsolete top-level glob.

Next: retain the failing baseline and investigate the first affected live
transition, using existing source timing evidence. Then remove the guest-PC
child dispatcher for a hardware-free, already-C parent/leaf pair actually
exercised by the runner. Many ON paths still execute instruction-shaped steps;
their readable domain C is often used only by comparison. Do not treat a step
count, decreased bus traffic or an uncalled domain module as native integration.
The hot C02776 reference entry is an OS VBeamPos wrapper, not an original ADF
game helper to recreate; `recomp_deferred.json` records that origin.

Reproduce:

```text
python tools/recomp/emulation_meter.py --launcher-checks
python tools/recomp/emulation_meter.py --scenario demo01 --frames 120 --out build/meter-probe.json
python tools/recomp/check_emulation_meter.py
ctest --test-dir build/recomp-cmake -C Release --output-on-failure
```

The full meter deliberately exits 1 after writing its reports when parity
fails. Probe results are explicitly partial, never a substitute for suite or
whole-game acceptance. Disposable RGB/RAM outputs live in `build/` and are
removed after hashing; sealed recordings and user-owned files are untouched.
