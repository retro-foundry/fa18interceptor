# Native port runner

Complete mission-three bodies (2026-10-09): all 4,965 unique native bodies
match original instructions with zero gameplay/display differences under
the existing frame-body contract. Compact complete-RAM capture retains
14,895 snapshots in 5.61 MB gzip, preserving the whole 42,706-frame run,
final RAM and earned pilot with zero gameplay heap violations. Release/Debug
reproduce the previous complete 40-body window; affected CTests pass.
Independent drawing history, audio timing and visible performance remain
open. See [frame-body evidence](../../analysis/native_complete_frame_body_milestone.md).

Combined cockpit history (2026-10-09): all eight complete planes across the
first 40-observation drawing episode follow actual radar, bitmap and message
writes (2,560,000 XOR bytes). The first 38 differing plane-0 observations are
explained; 1,873 remain open. All three omitted-owner histories are rejected;
earlier message and whole-plane-1/2 reports are unchanged. See
[combined drawing evidence](../../analysis/native_combined_cockpit_history_milestone.md).

Remaining drawing audit (2026-10-09): original radar writes explain the first
17 differing plane-3 observations. Full-flight audits retain 1,911 unresolved
observations for plane 0 and 2,785 for plane 3 and exit failure; whole-plane
acceptance stays strict. Plane-1/2 reports remain identical. See
[remaining drawing evidence](../../analysis/native_remaining_plane_audit.md).

Complete mission-three plane histories (2026-10-09): all bytes of planes 1
and 2 on both pages are accounted for across all 4,967 independent flight
observations (79,472,000 bytes per plane). Every differing observation has a
validated actual-input radar/message paint history; a missing late history
is rejected. Complete record cores stay exact. Runtime is unchanged; whole
planes 0/3 and broader completion remain open. See [plane evidence](../../analysis/native_complete_plane_history_milestone.md).

Complete retained audio payloads (2026-10-09): all 8,281 original Demo handler
requests resolve in final RAM to native payloads on the same channels (12
original payloads). Full PCM/event coverage and native owned-span hashes pass;
four corrupt catalogs are rejected and the 48-request startup gate still passes.
Fetch-time contents, handoff ordering and waveform timing remain unproved.
Runtime is unchanged. See [payload evidence](../../analysis/native_complete_audio_payload_milestone.md).

Bounded message pages (2026-10-09): ten actual C322EE inputs/returns match
original instructions; immutable glyphs and source colour/clear rules predict
all 640,000 owner page bytes. Five native bodies and live original text returns
pass. Current whole-flight message rules still pass all 4,964 transitions and
nine controls. Runtime is unchanged; cross-runtime complete frame histories
and broader drawing remain open. See [message evidence](../../analysis/native_message_owner_pages_milestone.md).

Bounded radar history (2026-10-09): ordered original crosshair, cache erasure
and marker writes predict all 70 complete owner page sets across 35 observations.
Actual instrument bitmap redraws explain all requested plane XOR bytes
(944,000), including the later background-bit interaction. Negative controls
and the existing five-frame delta check pass. Runtime is unchanged; complete
flight drawing remains 287/4,967, with message differences still open. See
[paint history evidence](../../analysis/native_radar_paint_history_milestone.md).

Free Flight model crash fix (2026-10-09): valid command $90 at C3BA04
now derives the original six-point block through C20F78; its shared $94
entry is also connected. 184 complete command cases and 32 actual-model
camera poses match original instructions. Release/Debug model, flight-start
and preallocation checks pass; canonical Release is refreshed. Precise
reported flight route is unsealed. See [crash evidence](../../analysis/native_free_flight_six_point_block_fix.md).

Current whole-flight allocation/cockpit check (2026-10-09): Release/Debug
preserve all qualification/mission-three traces, final RAM, counters and
earned saves, with zero gameplay heap violations. Five bounded complete
page deltas match selected-radar marker history. Corrected original owner
returns match all pages in both bounded windows (5/5 and 11/11); the previous
end address included the next HUD child and is now rejected. Bounded pixel
interaction is now explained above; broader drawing stays open. Runtime is unchanged. See
[current evidence](../../analysis/native_cockpit_radar_milestone.md).

Original voice ownership (2026-10-09): reference tracing now resolves the
loaded sound hunks, correcting false empty voices in a separate launch with
a 480-byte relocation. Complete original Demo and separate-launch execution,
PCM and register writes remain exact. All 48 initial music requests match
native sample bytes/order/period/volume. Original right-channel onset follows
left by 67 samples; native starts together, so onset/whole-flight sound remain
open. Native executable and gameplay storage are unchanged. See
[voice and handoff evidence](../../analysis/native_original_voice_layout_milestone.md).

Native PCM filter (2026-10-09): playable output now uses the original A500
fixed and enabled LED cascades, with 56 bytes of fixed state and no gameplay
allocation. Release/Debug match every sample in the complete 13,230,014-frame
original cold-start response. The complete native Demo also matches original
filter processing, preserving every audio event and game-data byte. All
33,800 intro/Free Flight/final-combat frames preserve game state and agree
across builds, with zero heap violations; music transitions pass. Visible
Free Flight presents all 6,500 frames, with 0.518 ms maximum audio work; one
406 ms SDL input stall still fails the 20 ms frame gate. Original game
onset/handoff alignment, broader drawing/performance and deferred state
cleanup remain separate. See [filter integration evidence](../../analysis/native_pcm_filter_milestone.md).

Original filter evidence (2026-10-09): the existing LED interface reports
filter enabled at all 21,069 Demo boundaries, preserving complete original
PCM/execution and every earlier event row. Two independent cold starts also
produce 13,230,014 stereo frames matching unchanged A500/LED filter functions
exactly, including the reference mixer gain; non-silent output and every
sample are checked. Wrong filters and corrupted LED traces are rejected.
Native executable is unchanged; filter integration, onset/handoff alignment
and whole-flight sound acceptance remain open. See [filter evidence](../../analysis/native_original_filter_state_milestone.md).

Native PCM averaging (2026-10-09): playable output now uses original source
interval averaging instead of sample holding, with fixed integer storage.
196,608 intervals match unchanged original accumulator functions in both
builds. Intro, Free Flight and final combat preserve complete game state;
Debug/Release agree on the intentionally changed PCM. Complete Demo requests,
stops, voices and stream phases remain exact, with zero heap violations.
Canonical Release is refreshed. Original onset/handoff alignment and Amiga
filter acceptance remain open; broader completion and state cleanup scopes
are unchanged. See [averaging evidence](../../analysis/native_pcm_averaging_milestone.md).

`--audio-trace PATH` records bounded sample/stop events and complete per-frame
voice/stream state using preallocated buffers. Complete Demo PCM/RAM/counters
and every trace row agree in Release/Debug, with zero heap violations; tracing
and preallocation checks pass. Original onset/handoffs and filter acceptance
remain open. See [trace evidence](../../analysis/native_audio_trace_milestone.md).

The complete original demo now has a validated PCM recording: all 21,069
reference calls preserve original execution and complete sample coverage.
Their 406,753-row audio-write/voice trace also preserves full state and PCM.
LED/filter pin is known at the retained endpoints only.
Native onset/handoffs and interpolation/filter fidelity remain open; the native
executable is unchanged. See [audio evidence](../../analysis/native_original_audio_recording_milestone.md).

Gameplay storage is reserved before the playable frame loop. Project heap and
buffered-file open/close calls are guarded; SDL uses a fixed 32 MiB startup arena,
PCM a fixed 512 KiB ring, and pilot/capture I/O direct OS handles. Exhaustion
fails explicitly. Use `--memory-report PATH` to record guard and arena evidence.
Release/Debug preserve complete native PCM/state/saves. OS/driver heaps are
outside the measured scope; original RNG/timing remain unchanged. See
[evidence](../../analysis/native_preallocation_milestone.md).

Visible qualification/mission-three playback now finishes with real audio,
exact RAM/counters/earned save, no resets and all 42,706 frames presented.
Recorded controls are isolated by an opt-in diagnostic. Two SDL poll stalls
exceed the 20 ms work budget, so performance acceptance remains open. All rows
remain counted. Other missions/views/combat are separate. See
[visible evidence](../../analysis/native_visible_replay_input_milestone.md).

Complete mission-three drawing localization (2026-10-09): all 4,967 observations
match rows 0..127 of both complete pages. Optional adjoining bands preserve
every earlier trace, page, final RAM byte, counter and save in Release/Debug.
The first row-191 difference is sliding target-info text at the same offset.
Strict pages remain 287/4,967; later cockpit drawing and broader completion
remain open. See [localization evidence](../../analysis/native_mission_drawing_bands_milestone.md).

Complete mission-three message timing (2026-10-09): all 4,964 real transitions
obey original elapsed/countdown, delay/redraw, full text and colour-cache rules,
including 1,362 paused HUD periods. Nine priority-message producer fields match
at all 4,967 observations. Both first HDG events follow two sampled-second
changes (original tick168/native185); this is assessed under the accepted
cadence policy. Release/Debug agree; nine wrong results are rejected. Optional
read-only fields preserve all earlier traces, pages, final RAM, counters and
saves. Strict pages remain 287/4,967; other drawing and completion work remain
open. State cleanup stays deferred. See [message evidence](../../analysis/native_mission_message_timing_milestone.md).

Ground-strip startup fix (2026-10-09): native now calls original C2527C,
generating all 40 disk-backed strip corner records before gameplay. Missing
corners caused empty ground draws and later placement skips, consistent with
the Free Flight disappearing-road report. Initializer RAM and all 240 corners
match original instructions; the omitted-call regression is rejected.
Release/Debug mission-three gameplay remains exact; first missing-ground-line
pages now match and strict drawing improves 267 -> 287/4,967. The next strict
difference is observation 22,303. Free Flight callback/restart, connected model,
frontend and new CTests pass. Other drawing remains open; state cleanup stays
outside this goal. See [ground evidence](../../analysis/native_ground_bounds_startup_milestone.md).

Native raster addressing (2026-10-09): negative-X lines now use the original
logical-shift starting offset and aligned word address. The connected raster
checks pass all existing cases and 32 new negative-X cases per checkpoint.
Release/Debug complete mission-three gameplay, traces and RAM remain identical
to the preceding evidence; strict drawing stays 267/4,967. A bounded capture
localizes the first visible difference to a skipped ground segment and a
different placement cache; its earlier cause remains open. Canonical Release
is refreshed. State cleanup remains deferred outside this goal. See
[drawing evidence](../../analysis/native_negative_line_and_drawing_window_milestone.md).

User scope update (2026-10-09): migrating remaining byte-array state into named
C structures is deferred to a separate follow-up outside the active
complete-C-port goal. It improves ownership and clarity; the existing arrays
are ordinary PC memory and do not establish a missing mission. This overrides
older completion notes. Independent whole-flight comparisons, recorded
audio/filter fidelity and visible gameplay performance remain in scope; the
uninterrupted campaign requirement remains waived.

The successful independent mission-three flight now passes complete gameplay
state in Release/Debug: all 4,967 observations and 79,472 record cores match.
An exact original probe proves two extra recorder observations were interrupt
resumptions; every observation remains compared at its real update call. Both
native builds agree on traces, RAM, counters and earned saves/menu return.
Strict drawing matches 267/4,967 and remains open. This supersedes the earlier
state-parity failure below; runtime and Escape are unchanged. See
[execution evidence](../../analysis/native_independent_mission_execution_milestone.md).

The original and independent native now complete mission three with gear
raised after takeoff, lowered before runway landing, grade and menu return.
The complete original recording reproduces without its controller; native
Release/Debug agree. Whole-flight state parity remains unaccepted after a
repeated original entry at tick 360. Strict comparisons and verified snapshots
retain the failure. Gameplay and Escape are unchanged. See
[success recording](../../analysis/native_independent_mission_success_milestone.md).

Independent mission-three failed flight now matches all 5,216 aircraft cores
and camera/control/target state through its first crash/reset callback. The
original recording reproduces all 22,467 boundaries without its input pilot.
Native uses an ordinarily enlisted, earned pilot at the same scene level zero;
Release/Debug agree. Drawing matches 259/326 and 67 failures remain unassessed.
Successful all-mission whole flights and other completion work stay open.
Gameplay and original Escape are unchanged. See [recording evidence](../../analysis/native_independent_mission_recording_milestone.md).

Independent complete qualification now matches all 43,648 record cores and
camera/control/target fields across 2,728 first-flight boundaries; 2,722
complete drawings match. Nine result/restart callback runs preserve all
1,611 distinct gameplay states. Six message differences follow the original
expiry/blink cadence at a one-step phase offset, with 83 observed states,
267 original-owner checks and four rejection probes. Release/Debug agree.
Initial pilot contexts differ, and second-flight/menu observation limits
remain explicit. Gameplay and original Escape restart are unchanged;
all-mission independent flights and other completion items remain open.
See [qualification flight evidence](../../analysis/native_independent_qualification_trace_milestone.md).

Read-only `--flight-trace PATH` records all sixteen complete cores, camera/control
state, HUD/clocks and both complete drawing-page hashes at C0EFD4 pre-input.
It shares the 512 MiB capture budget with requested raw RAM. Independent original
whole-demo evidence matches all32,736 cores of the first flight and its complete
gameplay restart sequence; strict drawing and later fixed-offset failures remain
explicit. See [trace evidence](../../analysis/native_independent_demo_trace_milestone.md).
The reported ALT/HDG timing is now assessed across all 374 transitions in the
selected-target episode. Both runs obey original clock, countdown, context,
redraw and full-text rules; Release/Debug agree and wrong results are rejected.
V2 corrects V1's mouse field label and observes actual target selection and HUD
gates. Strict drawing and all-mission whole flights remain open. See
[HUD timing evidence](../../analysis/native_demo_hud_timing_milestone.md).

Initial startup and qualification-entry sorting are verified through the actual
connected frontend. The original C0F812 LINK -14 frame and complete C08F26 call
match lists, retained output and compared RAM. Release/Debug agree on 22 combined
intervals, 60 lists and 40 factor probes; normal frontend/demo regressions pass.
Optional read-only startup observation leaves sorting/gameplay unchanged. See
[startup evidence](../../analysis/native_initial_startup_sort_milestone.md).

Current acceptance (2026-10-09): the user waived an uninterrupted campaign.
Repeated individual qualification and mission tests, using actual earned saves
and checking objectives, landing, results and menu return, replace that gate.
Cold starts are allowed. The continuous-campaign driver remains optional
diagnostic tooling; older references to its sixth flight as required work are
superseded. Independent original whole flights and other completion work remain.

Fresh Release and Debug earned cold tours pass with all nine stages identical,
including every counter and 78-byte save. Release qualification and final source
gates also pass in original/new-pilot contexts; ten selected checks pass overall.
Repeated individual mission acceptance is met. See
[individual mission evidence](../../analysis/native_individual_mission_acceptance.md).

Actual Demonstration drawing order now matches its original caller; its native
menu transition sorts all three lists. Mode assertions corrected a fixture that
had selected qualification under the Demonstration label. Qualification entry
also passes. Release/Debug agree on 12 intervals, 30 lists and 20 factor probes;
frontend/demo regressions pass. Initial startup is now verified above; other completion
items stay open. See [mode audit](../../analysis/native_menu_sort_mode_audit_milestone.md).
The earlier sorting summary below is historical.

Menu/restart retained sorting now has direct original-caller comparisons:
eleven intervals, 27 sorted lists and eighteen incoming-factor probes agree.
Release and Debug pass; gameplay and playable binaries are unchanged. Reached
paths replace the factor through templates or uncached distances. Initial
startup and rare callers remain open. See
[menu/context evidence](../../analysis/native_menu_context_sort_milestone.md).

PCM playback now retains frontend-owned host buffer spans rather than resolving
an addressed game byte for every sample. Three ordinary-key audio/state/pixel
replays and the full earned tour preserve their previous results. Original
sample/voice comparisons and ownership checks pass. Voice/request state still
uses the source arena; full typed-state migration and original recorded-audio
fidelity remain open. See [PCM ownership evidence](../../analysis/native_pcm_buffer_ownership_milestone.md).

A newly enlisted pilot now earns qualification and all six menu missions,
including final aircraft objective, carrier wire landing, saved sixth result,
menu return and cold Next Mission wrap. The submarine remains active at
objective admission; a visible explosion is not required. Gameplay is unchanged.
The new per-mission gates compare sampled original instructions and playable
saved bytes. `fa18_native_new_pilot_tour` starts with an empty save directory
and carries only actual game saves across cold starts. Independent original
whole flights and uninterrupted single-process tour checks remain open; see
[native_new_pilot_tour_milestone.md](../../analysis/native_new_pilot_tour_milestone.md).

Earlier acceptance notes below describe the preceding milestones.

Final mission availability is now earned by the ordinary rescue/cruise save
chain from the original ADF pilot. The active final success gate loads that
saved log and completes aircraft objective, carrier landing, save, menu and
cold wrap. Completions advance 5 -> 6; sampled original comparisons and the
playable replay agree. See
`../../analysis/native_final_mission_earned_availability_milestone.md`.
Gameplay is unchanged; independent original whole flights remain open. The
older patrol diagnostic retains its unearned log.

Intercept Incoming Cruise Missile (F5/internal mode seven) now completes
interception, carrier deck/wire landing, stop/save, messages, Escape/menu and
cold reload through ordinary keys. `fa18_native_mission_7_sequence` compares
42 intervals and 171 bodies with original RAM/drawing, including 20 combat
bodies, 80 consecutive landing/result bodies and one config write. The playable
replay delivers all 1,533 host events and agrees on saved bytes/menu. Starting
availability is earned by the normal-key rescue save; its existing gate now
reproduces exact retained rescue inputs and log bytes. See
`../../analysis/native_cruise_mission_sequence_milestone.md`. All six menu
missions have successful routes. Independent original whole flights remain open. Gameplay and comparison masks are
unchanged; passing RAM remains temporary inside the existing 480 MiB cap.
Debug/Release gates agree on cruise inputs, saved bytes and result sequence.
Nine selected Release checks and three Debug checks pass; the playable Release
executable is unchanged and build-cache use remains 2.00 GiB after pruning.

Search and Rescue (F4/internal mode six) now completes pod deployment, near-site
objective, carrier deck/wire landing, stop/save, messages, Escape/menu and cold
reload through ordinary keys. `fa18_native_mission_6_sequence` compares 44
input/stage intervals and 193 bodies with original RAM/drawing, including 40
rescue bodies, 80 consecutive landing/result bodies and one config write.
The playable replay delivers 870 host events and saves identical bytes. Both
Debug and Release pass with matching inputs and saved results. The fresh pilot
is the original ADF log; full port-earned tour availability remains open.
See `../../analysis/native_rescue_mission_sequence_milestone.md`. Gameplay and
comparison masks are unchanged; independent full flights remain open and
internal mode seven is accepted above. Passing RAM stays temporary inside the
existing 240-pair / 480 MiB cap and pruning hooks remain active.

The final Carrier Sub mission (F6/internal mode 8) now completes its aircraft
objective, carrier wire landing, stopped result, save, messages and Escape/menu
return. Cold Next Mission wraps from saved mode eight to mode three. The serial
`fa18_native_mission_8_sequence` gate compares 42 flight intervals/239 bodies
plus one cold-wrap interval/body with original RAM/drawing, including all hit
windows, 64 landing bodies and one config write. The playable runner delivers
2,626 flight events plus four wrap events and agrees on saved bytes/menu/wrap.
C230B0's returned selection word is preserved into the generic scheduler;
original FF04 replaces the adapter's incorrect 0004 completion word. Predicates,
countdowns and comparison masks are unchanged. Class-20 surface record 14 remains active
at objective admission. The original availability-only pilot fixture was
unearned. See `../../analysis/native_final_mission_sequence_milestone.md`.
The active gate now uses earned availability above; independent complete-flight
parity remains open. The
earlier `fa18_native_final_patrol_diagnostic` remains accepted; capture partitions
retain the 480 MiB cap and builds/CTest keep the 4 GiB pruning hooks.

Mode four now completes normal-key escort, carrier arrestor landing, stopped
result, config save, result messages, Escape/menu return and cold reload from
the earned region pilot. All 43 input/stage intervals and 159 sampled bodies
match original compared RAM/drawing, including 64 consecutive landing bodies
and one config write. Completions advance 1 -> 2 and grade 0 -> 1. The playable
runner delivers the same 3,883 host replay events and agrees on the saved log
and menu. JSON distinguishes `host_replay_events`/`host_replay_pending` from
game-input replay counters. The serial gate is `fa18_native_mission_4_sequence`.
See `../../analysis/native_mission_four_sequence_milestone.md`. Gameplay and
comparison masks are unchanged; independent complete flights remain open.
The user's mission six is the Carrier Sub mission (F6/internal mode 8), while
internal mode 6 is Search and Rescue. Verify original counter/outcome/save
behavior against the user's historical context; see
`../../analysis/native_final_mission_completion_context.md`. Decompile any
remaining instruction translations encountered on the active path.

Region flight now passes with an earned level-zero pilot log. Normal menu
reset, qualification and mission-three success/save/menu return reproduce all
78 bytes; the playable runner agrees on both saved results. Qualification
matches 50 original input/stage intervals/174 bodies, mission three 44/164 and
region flight 57/49, including the existing spawn/zone/NPC-missile/reset guards.
The serial gates are `fa18_native_region_flight` and
`fa18_native_region_pilot_progression`. The host replay loader now accepts the
complete 1,652-event mission flight. See
`../../analysis/native_region_pilot_progression_milestone.md`. Successful mode
four is accepted above; whole-port acceptance remains open.

The native record loop now passes C2DEE0/C2DFF6's final matrix product cursor
to the following C2436A sight update. All 576 component cases match the
original output/cursor/RAM. The normal-key mode-four diagnostic matches all
26 input/stage intervals and 59 sampled bodies, including its crash
continuation and reset. The serial `fa18_native_mission_four_combat_reset`
gate passes in Debug and Release. See
`../../analysis/native_matrix_sight_reference_milestone.md`. This reset flight
has no hit/objective/save and still crashes; the successful route is accepted above.
The earlier level-two region scenario misses spawn/zone/NPC-missile coverage
while matching all 58 intervals/47 bodies. The earned-pilot gate above now
restores that coverage, preserving the original keys and strict guards.

Modes three and five now complete result messages and Escape/menu restart
after their normal-key successful flights. All 44/42 input-stage intervals
and 162/169 sampled bodies match original compared RAM/drawing, with 64
consecutive landing bodies per mode and one original config write. Completion
count and grade stay at 4 and 2 through bootstrap, with all 78 log bytes
unchanged and cold reload verified. The serial gates are
`fa18_native_mission_3_sequence` and `fa18_native_mission_5_sequence`. Gameplay
is unchanged. Debug/Release fixture builds and all five selected checks pass
in each configuration. See `../../analysis/native_mission_result_sequence_milestone.md`.
Other mission successes, independent complete flights, further play after
this result menu and remaining state/audio/performance work remain open.

Mode five now has normal-key formation, radar shoot-downs, objective, carrier
arrestor landing, stopped-aircraft success, result save and cold reload
acceptance. All 34 input/stage intervals and 155 sampled bodies match original
compared RAM/drawing, including 59 consecutive touchdown-to-result bodies.
Completion count advances 3 -> 4 and grade 1 -> 2 without a reset. The serial
gate is `fa18_native_mission_five_success`. Gameplay is unchanged. See
`../../analysis/native_mission_five_success_milestone.md`. Result messages and
restart have the sequence gate above; other successes and independent complete
flights remain open.

The optional validation pilot now completes formation and both radar kills,
then reaches original mode-five objective phase FF at tick 19,304. All 29
input/stage intervals and 118 sampled bodies match original compared
RAM/drawing, including 32 formation-window and 40 combat-window bodies. The
serial gate is `fa18_native_mission_five_objective`. This bounded gate excludes
return, landing and saved success; those have the full mission gate above. See
`../../analysis/native_mission_five_objective_milestone.md`.

The native validation pilot now reaches C0A002's proximity countdown and
C0A12E's paired aircraft restoration using normal keys. All 24 input/stage
intervals and 107 sampled bodies match original compared RAM/drawing, including
64 consecutive window bodies. All 201 native proximity scans are observed;
original execution reports 12 restoration calls for each aircraft, with the
native restored values checked against the original table. The serial CTest
is `fa18_native_mission_five_forced_return`. This gate excludes complete mission
success; independent complete flights remain open. See
`../../analysis/native_mission_five_forced_return_milestone.md`.

Independent-camera depth sorting now receives the actual final scaled view
coefficient through the native projection caller. All 640 complete component
cases and 48 actual map/moving-camera bodies match original output and compared
RAM/drawing, including both factor signs and preserved-factor sorting. The
serial `fa18_native_view_depth` CTest uses bounded capture windows and per-body
timing sidecars. Source ownership and remaining startup/menu contracts are in
`../../analysis/native_context_depth_milestone.md`; full game acceptance stays
open.

Normal-key mode-five combat, enemy-expiry accounting and the natural reset
now pass 26 input/stage intervals and 115 sampled bodies against original
compared RAM/drawing, including all 63 consecutive combat-window bodies.
Control normalisation and depth sorting publish their actual retained output
to ordinary host renderer state for early model expiry. The gate also checks
64 candidate-reference, 128 complete depth-sort and 256 distance/factor cases.
Masks, distance arithmetic and gameplay rules are unchanged. The flight still
crashes; this remains a reset regression alongside the successful mission gate. See
`../../analysis/native_mission_five_combat_reset_milestone.md` for source
ownership, remaining caller contracts and comparison scope. The keyboard pilot
remains validation-only; passing RAM is temporary and bounded.

Mode three now has a normal-key objective/landing/taxi/result/save/reload gate.
All 36 input/stage intervals and 184 sampled bodies match original compared
RAM/drawing, including 96 consecutive landing bodies and the actual config
write. Completion count advances 3 -> 4 and mode grade 1 -> 2, with no crash
reset. The validation pilot reads flight state and sends ordinary keys; gameplay
and the playable executable are unchanged. Debug/Release builds and checks
pass. Captures stay temporary and bounded. See
`../../analysis/native_mission_three_success_milestone.md` for the source
conditions, terrain landing followed by taxi, and comparison limits.
Successful modes four, six, seven and eight and independent complete flights
remain open, alongside the remaining state/audio/performance work.

Carrier qualification now has two repeatable source sequence gates: the
original ADF log and a reopened unqualified saved pilot. Each completes takeoff,
landing, success, config write, restarted flight and fresh log reload, matching
50 input/stage intervals and 174 bodies against original compared RAM/drawing.
All 96 consecutive landing bodies and the actual result DOS Write are checked;
the new pilot earns qualification 0 -> 1. Gameplay and the playable executable
are unchanged. Debug/Release builds and six affected Release CTests pass.
Passing RAM remains temporary and bounded. See
`../../analysis/native_qualification_sequence_milestone.md`. Other mission-list
successes, independent full flights and remaining state/audio/performance work
remain open; the complete-port goal stays active.

The mode-eight gun shoot-down now passes 317 input/stage intervals and 275
bodies against original compared RAM/drawing, including all 101 consecutive
bodies from first damage through enemy expiry accounting and eventual
inactivation. The native map clipping owner publishes the retained output used
by early model expiry, fixing the former C4F6DE cache mismatch. All 256 focused
clipping residue/drawing cases match. No collision, damage, motion or timer
rule changes. Debug/Release playable and fixture builds and eight affected
CTests pass, including gun/radar/infrared shoot-downs. Gun kill is now a CTest
gate; passing captures remain temporary and bounded. See
`../../analysis/native_gun_kill_milestone.md`. Successful complete missions,
independent full flights and the complete-port goal remain open. This entry
supersedes older gun acceptance limitations below.

Mode eight now also has a required normal-input infrared shoot-down: 55
input/stage intervals and 802 bodies match original compared RAM/drawing,
including every body in the 634-body impact-to-inactivation interval. The
shared observer checks the selected projectile kind and pilot-log hit word.
No gameplay rules or playable executable changed. Debug/Release builds and
four affected CTests pass, including radar kill. Captures remain temporary
and bounded. See `../../analysis/native_infrared_kill_milestone.md`.
Gun shoot-downs and full successful missions remain open; this supersedes
older infrared acceptance limitations below.

Mode eight now has a required normal-input radar shoot-down: impact, 15-tick
expiry, enemy-aircraft expiry accounting and inactivation of the struck record.
All 634 consecutive bodies in that interval match original compared RAM/drawing;
the full probe passes 57 input/stage intervals and 802 bodies. The observer's
body serials require every boundary, and passing RAM is deleted immediately
after comparison. Captures stay temporary and bounded. Debug/Release builds
and eight affected CTests pass. See `../../analysis/native_radar_kill_milestone.md`.
Gun/infrared shoot-downs, successful missions, independent complete flights and
the remaining state/audio/performance work stay open.

Mode eight now has a required normal-input radar hit: 57 input/stage intervals
and 188 sampled bodies, including the exact hit body, match original compared
RAM/drawing. Inactive C22AC0 retains its caller's actual placement result;
32 focused inactive-record contracts pass. Weapon probes check consumption
before natural reset and restored stores afterward. Mode-eight combat retains
its long-flight guards with a bounded pull-up. Hit, combat-eight and all three
weapon source comparisons run in CTest with temporary passing RAM.
Debug/Release builds and thirteen affected checks pass across targeted runs.
See `../../analysis/native_radar_hit_milestone.md`. Complete kills/missions,
independent flights, other contracts, audio, typed state and performance stay open.

Normal-input mode-six failure now has a required three-loss/reset-exhaustion/
menu-return/Free-Flight-relaunch scenario; 86 input/stage intervals and 237
sampled bodies match original compared RAM/drawing. Mode two's missing C28722
reset connection is fixed: all seven streams, wrap and Escape return match
30 intervals/206 bodies. Both comparisons are registered in CTest and keep
passing RAM temporary. Debug/Release builds and seven affected checks pass.
See `../../analysis/native_natural_outcomes.md`. Successful missions and
independent complete sequences remain open, alongside the other full-port work.

Nine childless postflight callbacks now retain preceding input results. The
expanded comparison passes 355 full bodies, 372 recorder parents, 84 keyboard
parents and 168 separate input/stage parents against original compared
RAM/drawing and defined returns. Three controlled postflight outcomes pass
nine intervals/25 bodies and saved-log persistence. Native Debug/Release and
eight affected checks pass.
See `../../analysis/native_postflight_input_return.md`; other contracts and
whole-game mission/audio/state/performance acceptance remain open.

Smoothing publication/restart, context entry and viewport completion now
preserve actual preceding input results. Sixty separate input/stage parents,
247 full bodies, 264 recorder parents and 84 keyboard parents match original
compared RAM/drawing and defined returns. Stage results are checked separately
from later body/message outputs. Native Debug/Release and eleven affected checks pass. See
`../../analysis/native_setup_input_return.md`; other callback contracts and
whole-game mission/audio/state/performance acceptance remain open.

The smoothing cancel child now calls the existing native cancel/reset
composition. Its formerly unavailable MC_CANCEL handler is exercised with a
validation-only source cancel marker at a naturally reached mode-four stage.
The run resumes flight and returns to the menu; all 44 actual input/stage
intervals and 29 sampled bodies match original compared RAM/drawing. Native
Debug/Release and eight affected checks pass. See `../../analysis/native_smoothing_cancel.md`; other
stage contracts and whole-game mission/audio/state/performance remain open.

The complete demo now has a bounded performance checkpoint: all 10,910 frames
call SDL presentation in a normally paced hidden Direct3D window, with measured
work peaking at 6.9374 ms. Headless work peaks at 1.6407 ms. Host pacing intervals
sometimes exceed 20 ms; visible display and full mission/combat performance
remain open. Timing instrumentation preserves complete RAM, pixels and counters
in ordinary Free Flight. Native Debug/Release and five affected checks pass.
See `../../analysis/native_frame_performance.md` for measurement scope.

The viewport-message stage and idle frame now preserve actual preceding input
outputs. Fourteen idle bodies match original returns, with twelve followed
directly by recorder input. The expanded suite matches 199 full bodies,
216 recorder parents, 84 keyboard parents and twelve separate input/stage
parents against compared RAM/drawing and defined returns. Ten affected checks
and native/reference builds pass. See `../../analysis/native_idle_input_return.md`.
Other stage/reset/HUD contracts and whole-game mission/audio/state/performance
acceptance remain open.

Context camera calculations now call the existing matrix/observer owners.
Actual record/preset/map outputs and preserving requests compose subsequent
depleted input. Zoom text and skipped-message decimal policies also reach
their actual native HUD outputs. 187 full bodies match compared RAM/drawing;
204 recorder parents and 84 keyboard parents match RAM and defined returns.
Two added idle bodies keep their inherited output explicitly unresolved.
All 12,576 selected command parents match RAM/returns; ten affected checks and
native/reference builds pass. See
`../../analysis/native_context_input_return.md`. Idle/stage/reset contracts and
whole-game mission/audio/state/performance acceptance remain open.

Indexed selection, function-key throttle outputs and recorder-$FD relative
changes now compose later depleted input. Expanded frames also connect source
release/model/vertex paths and fix negative throttle-request ordering and its
recorder gate. 175 full bodies, 192 recorder parents and 72 keyboard parents
match original compared RAM/drawing and defined returns. All 10,688 selected
command parents match RAM/returns; nine affected checks and native/reference builds pass. See
`../../analysis/native_indexed_input_return.md`. Context/reset/HUD contracts and
whole-game mission/audio/state/performance acceptance remain open.

Fire selection, successful countermeasure events and nested eject publication
now expose their actual outputs to subsequent depleted recorder input.
Depleted/modified gates preserve prior output. 163 full bodies, 180 recorder
parents and 60 intervening keyboard parents match original compared RAM/drawing
and returns. The radar/weapon runtime probes now require actual action owners
and values in the correct mode. All 8,256 selected command parents and nine
affected checks pass; native/reference builds succeed. See
`../../analysis/native_fire_countermeasure_input_return.md`. Indexed/context
contracts and whole-game mission/audio/state/performance acceptance stay open.

Radar/weapon commands now expose actual range, block and mode outputs to
subsequent depleted recorder input; target/throttle/hook/ECM actions preserve
their preceding output. Weapon $80-$10 follows the original signed branch to
$30. 151 full bodies, 168 recorder parents and 48 intervening keyboard parents
match original compared RAM/drawing and returns. All 4,416 selected command
parents and nine affected checks pass. See
`../../analysis/native_weapon_radar_input_return.md`. Whole-game acceptance,
other action families, audio, typed state and measured performance stay open.

View commands now compose their actual detail, origin level, record type, mode
and masked zoom-flag outputs into subsequent depleted recorder input. 139 full
bodies, 156 recorder parents and 36 intervening keyboard parents match original
compared RAM/drawing and returns; all 2,176 selected command parents pass RAM
and return checks. Other action families and whole-game acceptance stay open.
See `../../analysis/native_view_action_input_return.md`.

Control commands now compose their actual preservation/HUD-mode/gear-gate
outputs through skipped publication into subsequent depleted recorder input.
127 full bodies, 144 recorder parents and 24 intervening keyboard parents match
original RAM/drawing. See `../../analysis/native_control_action_input_return.md`.
Remaining return contracts and whole-game acceptance stay open.

Intervening wait/modifier/empty/queue-only commands now preserve their actual
prior or selector output; accepted queue publication supplies its signed index.
115 full bodies, 132 recorder parents and twelve intervening keyboard parents
match original RAM/drawing. 512 focused command parents pass RAM; 471 defined
returns match. Remaining action-owned outputs stay explicit. See
`../../analysis/native_intervening_command_input_return.md`.

Lost-target cleanup now preserves skipped outputs and publishes its actual view
mode or signed translated-queue index through the existing publication owner.
103 complete bodies and 120 input parents match original RAM/drawing, including
twelve selected cleanup frames in ordinary Free Flight. See
`../../analysis/native_selection_cleanup_input_return.md`. Earlier HUD/command
outputs and whole-game acceptance remain open.

Grid/aircraft-marker rendering now composes its actual point depth, heading,
shape offset, clipped-segment height, line or number-text output into first
depleted recorder input. Inactive grids preserve the preceding result. 91 full
bodies and 108 input parents match original RAM/drawing; two snapshots each
pass 401 grid/marker and 128 clipped-segment return/RAM cases. See
`../../analysis/native_grid_marker_input_return.md`. Remaining return contracts
and whole-game acceptance stay open.

HUD marker lines now publish their actual size or defined clipped-start X
delta to first depleted recorder input. Seventy-nine full bodies and 96 input
parents match original compared RAM/drawing. Two runtime snapshots each pass
128 line-return/RAM cases; focused HUD checks pass 492 RAM cases and 252 returns.
The raster check is now included in native CTest. See
`../../analysis/native_hud_marker_input_return.md`. Active grid/aircraft-marker
return composition and whole-game acceptance remain open.

Small-text odd destinations now preserve their selected glyph through the
original release fault return. Focused HUD checks match 480 RAM cases and 237
defined returns, including 27 odd-destination cases in cockpit/context views.
See `../../analysis/native_small_text_fault_return.md`.

Scene-position labels now publish their actual row-Z load, matrix product or
number-text result to first depleted recorder flare/chaff commands. Skipped
labels preserve the preceding context HUD readout. Sixty-seven complete bodies
and 84 recorder input parents match original compared RAM/drawing; 256 label
returns and 450 HUD RAM cases with 207 defined returns pass. See
`../../analysis/native_scene_label_input_return.md`. Remaining drawing/command
contracts and whole-game acceptance stay open.

The actual numeric debug overlay now returns its final four-plane text
character/glyph selection to first depleted recorder flare/chaff commands.
Inactive overlays preserve the preceding result. Fifty-five complete bodies
and 72 recorder input parents match original compared RAM/drawing; 256 gated
debug returns match, including clipped/odd-window cases. See
`../../analysis/native_debug_text_input_return.md`. Remaining drawing/command
contracts and whole-game acceptance stay open.

HUD bar destinations and small-text character/glyph results now supply the
first depleted recorder flare/chaff command. The periodic cockpit redraw also
returns its source pass count. Forty-three actual bodies and 60 recorder input
parents match original compared RAM/drawing; focused HUD checks match 405 RAM
cases and 117 defined returns. Unfinished drawing and intervening-command
returns remain explicit missing contracts. See
`../../analysis/native_hud_input_returns.md`. Whole-game acceptance stays open.

The periodic page-top clear now returns its actual pattern to the next first
depleted recorder flare/chaff command when the later label/debug passes skip.
The current suite matches 31 flight bodies and 48 recorder input parents,
including twelve new clear-derived cases reached through normal game counters.
See `../../analysis/native_page_clear_input_return.md`; remaining drawing and
intervening-command returns still need their own source contracts.

The final message renderer now returns its defined character/glyph byte to
the first depleted pending flare/chaff command. Nineteen flight bodies and
36 recorder input parents match original compared RAM/drawing, including
twelve independently verified first-depleted returns. Inactive message frames
do not assign a new return; drawing/intervening-command producers remain unfinished.
See `../../analysis/native_message_input_carry_milestone.md`.

The reported demo outside-view clipping after takeoff is fixed. Attached
projection now consumes the original model vertex, as C1F2EE does. All 223
independent takeoff boundaries match both drawing pages, camera state and
player motion/pose. Three outside-camera cases join the ten-case frame-body
suite. Extended missions also fix a coarse matrix clamp, matching 232 input/
stage intervals and 726 bodies across modes five through eight. See
`../../analysis/native_outside_camera_milestone.md`. Full gameplay acceptance
remains unfinished.

Latest input connection: Delete's source C06BF0 callback reset now executes
existing C1748C/C17456 domain owners in flight and menus. Check it with
`python tools/native/check_mode_two.py --mode 125 --callback`; 57 input/stage
intervals and 37 sampled bodies match original compared RAM/display. Eligible
pilot fixtures now establish source file readiness before saving; six fresh
mission/ejection/weapon probes match 264 intervals and 211 bodies. See
`../../analysis/native_callback_reset_milestone.md`. Whole-game acceptance
remains unfinished.

The 2026-10-06 user request starts `fa18_native`, reusing `port/game/` source
with a separate native entry point. This supersedes the earlier restriction
against starting a native runner. It does not restore the deleted `fa18_port`.

```powershell
python scripts/build_native.py
build/native/fa18_native.exe --adf local/media/fa18.adf
```

Builds and native CTest runs automatically prune disposable replay/test output
against a 4 GiB build-cache budget. Active compiler files, fetched dependencies,
logs/reports, compressed original RAM and canonical recordings are protected.
The limit cannot delete protected files to make space. To run cleanup manually:

```powershell
python scripts/prune_build_artifacts.py
```

Mode, postflight and independent-window comparisons now use temporary RAM by
default, retaining reports and the failed case. `--keep-captures` explicitly
retains raw debug output, still subject to the build-cache policy. The window
checker accepts retained original `PREFIX.ITERATION.dat.gz` without expanding
the cache. Use `--frame-capture-entry-only` for independent pre-input windows;
it writes one snapshot per boundary rather than three. Native capture ranges
default to 512 MiB; larger deliberate captures require `--capture-budget-mib N`.
See [`../../analysis/workspace_artifact_retention.md`](../../analysis/workspace_artifact_retention.md).

For aligned mode-fixture diagnostics, `FA18_MODE_END_TICK` selects a positive
unsigned terminal tick; `FA18_MODE_FINAL_DATA` writes the final RAM as
`PREFIX.before.dat`. These environment variables apply only to
`fa18_native_mode_two_test`. Default endpoints and acceptance guards remain
intact, so a shortened diagnostic may exit unsuccessfully after writing its
snapshot. Clear both variables before acceptance runs. The fixture services
960 stereo PCM frames at 48 kHz after each tick, as the playable backend does.
Use temporary capture storage and prune after manual runs. See
[`../../analysis/native_region_flight_diagnostics.md`](../../analysis/native_region_flight_diagnostics.md).

Region coverage uses an earned level-zero pilot log, reproduced through normal
menu reset/callsign entry, qualification and mission-three completion. The
original level-two disk log fills both aircraft admission slots before takeoff.
`python tools/native/check_mode_two.py --mode 4 --flight` loads the retained
78-byte log before flight and compares all sampled original boundaries without
changing its keys or coverage guards. The serial CTests are
`fa18_native_region_flight` and `fa18_native_region_pilot_progression`; the latter
reproduces the log and checks qualification/mission-three source boundaries and
playable saved results. Host `--replay` storage grows to accept complete consumed
input such as the 1,652-event mission-three flight. See
[`../../analysis/native_region_pilot_progression_milestone.md`](../../analysis/native_region_pilot_progression_milestone.md).

Gameplay behavior and visuals at equivalent states/events are the acceptance
scope; exact Amiga frame timing is not required. Preserve physics, rules and
source-defined timers while allowing native rendering/presentation cadence.
The accepted native target is a 20 ms frame budget with every frame presented,
without recreating Amiga missed frames. The measured demo checkpoint above
covers frame work on one host; broader performance acceptance remains open.
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
Completed bodies in a range also write `PREFIX.ITERATION.timing.json`, with
their actual PAL interval, saved tick and owner-exit status. The runner's final
JSON summary describes the last body; use each sidecar for window comparisons.
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

For complete flights, `--frame-delta FIRST+COUNT PATH` captures the same
actual input/body boundaries as an ordered changed-block stream, with
full reconstructed-MiB hashes and a terminal count. It uses fixed storage
allocated before gameplay. This diagnostic requires recorded `--input`,
uses the normal capture budget, and runs separately from raw frame, flight
and audio traces. `tools/native/frame_delta.py` decodes complete snapshots;
`check_mission_frame_delta.py` checks trace preservation and actual original
body execution. Captures never feed the game.

Destroyed flight records now enter source C22ADE's 15-tick expiry rather than
aborting during scene rendering. C09DD0 clears a matching target and posts
TARGET DESTROYED through C25704; repeated drawing does not restart expiry.
Twenty controlled original descriptor comparisons and a disk-backed native
scene integration check pass. The latter supplies destruction inputs only in
its test entry; it shares the playable runner's `fa18_native_runtime` objects.
This verifies the rendering transition, not complete destruction/collision or
recorded-frame timing. See
[`../../analysis/native_record_expiry_milestone.md`](../../analysis/native_record_expiry_milestone.md).

Mode-4 throttle/stick flight also
exercises region spawn/orientation, zone exits, NPC missile launches and the
player's hit/restart sequence (5,970 scene/HUD frames). 56 actual input/stage
intervals and 47 sampled bodies match original compared RAM/display with
unchanged exclusions. Run `python tools/native/check_mode_two.py --mode 4 --flight`;
see [`../../analysis/native_region_flight_milestone.md`](../../analysis/native_region_flight_milestone.md).
Whole flights and other combat/outcome branches remain unverified.

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
Comma and period use the original rudder controls; releasing either clears
the rudder input. Numeric keypad view controls now accept live SDL keys and
legacy replay identities. Keypad 2 selects the outside view, 4/6 cycle view
angles, and 8 returns to the cockpit under the normal flight context. The
physical keypad remains distinct from the numbered menu keys, with either
Num Lock state. See [`../../analysis/native_host_keys_milestone.md`](../../analysis/native_host_keys_milestone.md).
F and C execute source flare/chaff stock, messages and control-effect launch,
motion, drawing and ground-contact expiry. Keyboard depleted-stock selection
and SHIFT-F's mode-6 sound are connected. Recorder $FD function keys also
publish their source event; their inherited selection is dead to publication.
2,512 source input cases pass, including 1,536 claimed recorder stock cases.
Twenty-four controlled recorder input parents match after normal Free Flight
startup, including successful flare followed by depleted chaff. C1C23C ignores
the inherited event after input is claimed; a first depleted recorder command
with an unclaimed queue still requires its real carry producer. Effect component/face collision
children are connected; 256 source-parent cases and four controlled actual
native parents match original compared RAM/display. These include face hits
and component misses; complete weapon-kill sequences remain unverified.
The runtime integration test uses real Free Flight keys and shares
all playable runtime objects; four actual bodies match original gameplay and
display. Build `fa18_native_countermeasures_test` and run
`python tools/native/check_countermeasures.py` for reports and focused evidence.
Passing captures are temporary; only failed cases retain raw RAM by default.
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

Set `FA18_TRACE_DRAWING_BANDS=1` with `--flight-trace PATH` to add five adjoining
full-width hash bands covering both complete pages. Full-page hashes remain
unchanged and authoritative. `FA18_TRACE_MESSAGE_FIELDS=1` independently adds
message-owner inputs. Both options are read-only diagnostics.

Use `--recorded-input-only` for a windowed recording comparison to suppress
physical gameplay controls while `--input` or `--replay` supplies them. SDL
events are still polled every frame and window close still works. Without this
option, physical input participates normally. The visible performance checker
uses it for independently earned mission recordings; keep the window open
through intermediate menus until it closes automatically and reports the result.

Add `--frame-times PATH` to an ordinary windowed run to write per-frame timing
CSV. The parent directory must already exist. Input, game, audio, conversion,
presentation and host wait are separate columns. `--hidden` is an optional
window diagnostic; headless reports omit conversion/presentation. CSV writing
is outside measured work but included in the next frame's start interval.
Asset loading/device creation precede the loop. Every loop frame is reported.

```powershell
build/native/fa18_native.exe --frame-times build/flight-times.csv
python tools/native/report_frame_times.py build/flight-times.csv --output build/flight-times.json --require-budget --require-presentation
```

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

Mission-list F2 now runs normal mode 4 through briefing/context setup into
flight (2,507 scene/HUD frames). 39 actual input/stage intervals and 27 sampled
bodies match compared original RAM/display. Complete outcomes/combat and other
variants remain open.

```powershell
python tools/native/check_mode_two.py --mode 4 --out build/native-flight/mission-four/original-check
```

See [`../../analysis/native_mode_four_milestone.md`](../../analysis/native_mode_four_milestone.md).

Mission-list F3 now runs normal mode 5 through briefing/context setup into
flight (2,312 scene/HUD frames). 39 actual input/stage intervals and 27 sampled
bodies match compared original RAM/display. Complete outcomes, record-restoration
branches and combat remain open.

```powershell
python tools/native/check_mode_two.py --mode 5 --out build/native-flight/mission-five/original-check
```

See [`../../analysis/native_mode_five_milestone.md`](../../analysis/native_mode_five_milestone.md).

Mission-list F5 now runs source mode 7 with an eligible saved pilot (2,289
scene/HUD frames). The original locked pilot still rejects F5; validation
changes only the saved availability flag and reopens through the normal loader.
42 input/stage intervals and 29 bodies, including C11078/C110A4, match compared
original RAM/display. Complete outcomes/combat remain open; standalone mode-7
placement-driver snapshots still fail at command $C3 and remain unaccepted.

```powershell
python tools/native/check_mode_two.py --mode 7 --out build/native-flight/mission-seven/original-check
```

See [`../../analysis/native_mode_seven_milestone.md`](../../analysis/native_mode_seven_milestone.md).

Mission-list F6 now runs source mode 8 with an eligible saved pilot (2,458
scene/HUD frames). The original pilot's availability byte $19 remains locked;
validation changes only that saved byte and reopens through the normal loader.
38 actual input/stage intervals and 27 sampled bodies match compared original
RAM/display. The reached C0A364 scheduler and saved A4=C29702's transition sort
choice are connected. Complete mission outcomes, combat and other variants
remain open.

```powershell
python tools/native/check_mode_two.py --mode 8 --out build/native-flight/mission-eight/original-check
```

See [`../../analysis/native_mode_eight_milestone.md`](../../analysis/native_mode_eight_milestone.md).

Normal mode-8 Shift-E now follows the source ejection action into failure/menu
return. The native route reaches 739 scene/HUD frames; 46 actual input/stage
intervals and 43 sampled bodies match compared original RAM/display. Source
flag/queue, programmed sound, clone/orientation and lifetime model/vertex
construction are connected. Other outcomes and combat remain unfinished.

```powershell
python tools/native/check_mode_two.py --mode 8 --eject --out build/native-flight/ejection/original-check
```

See [`../../analysis/native_ejection_milestone.md`](../../analysis/native_ejection_milestone.md).

Normal mode-8 Return/Space exercises both missile stocks and gun ammunition.
The missing $98 geometry command is connected, and the control-point renderer
now consumes the original child's returned plane destination for its test/clear.
138 actual input/stage intervals and 112 sampled bodies match compared original
RAM/display. Successful hits, complete combat and outcomes remain open.

```powershell
python tools/native/check_mode_two.py --mode 8 --weapon 1 --out build/native-flight/weapons/1-fixed
python tools/native/check_mode_two.py --mode 8 --weapon 2 --out build/native-flight/weapons/2-fixed
python tools/native/check_mode_two.py --mode 8 --weapon 3 --out build/native-flight/weapons/three-fixed
```

See [`../../analysis/native_weapon_firing_milestone.md`](../../analysis/native_weapon_firing_milestone.md).

The shared runtime now connects player readiness, mode-four result view,
mode-five record restoration and the reached result messages/log updates.
Controlled terminal fixtures match nine actual input/stage intervals and 25
sampled bodies against original RAM/display. These fixtures seed terminal
conditions only in validation; they do not establish normal-input mission
success. Result comparisons explicitly retain the shared C1643A save boundary;
its disk/status gates remain open. All 26 affected native tests pass.

```powershell
python tools/native/check_postflight_schedule.py
```

See [`../../analysis/native_postflight_schedule_milestone.md`](../../analysis/native_postflight_schedule_milestone.md).

Sustained native mode-six/eight throttle/stick/target/fire input now resumes
the readable guidance countdown after C06C02. The target marker consumes the
projection child's returned coordinates, attitude longs are signed, matrix
extraction retains its scaled divisor, and requested settling uses previous
roll. 114 input/stage intervals and 465 sampled bodies match compared original
RAM/display, including three original guidance-child returns in mode eight.
No new exclusions were added. These are connected sampled comparisons; full
missions and independent sequences remain open.

```powershell
python tools/native/check_mode_two.py --mode 6 --combat --out build/native-flight/combat-probe/validated-6
python tools/native/check_mode_two.py --mode 8 --combat --out build/native-flight/combat-probe/validated-8
```

See [`../../analysis/native_guidance_limits_milestone.md`](../../analysis/native_guidance_limits_milestone.md).

Configuration saves now compose complete source C1643A status/readiness
decisions, and enlistment runs C162E4's source refresh/read/create flow through
existing Amiga host file services. The ADF remains read-only and writes go to
the overlay. The postflight checker executes complete original game file
owners at its OS boundaries, with no game-routine bypass. Nine intervals and
25 bodies match; 77 result/config parents and 2,560 CPU contract cases pass.
Recorded carrier qualification still saves success, restarts and reloads.
All 28 selected native tests and 12 host/loading checks pass. Whole gameplay,
audio and performance acceptance remain open.

See [`../../analysis/native_config_owner_milestone.md`](../../analysis/native_config_owner_milestone.md).
