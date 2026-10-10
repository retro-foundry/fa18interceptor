# Current playable port handoff

Complete mission-five gameplay comparison (2026-10-10): all 13,775 original
flight observations, 220,400 complete record cores and camera/control fields
match the independent native replay. Both earn grade one, the third completion
and menu return. The ordinary native prefix reproduces all 27,176 rows exactly;
the original has verified identities for all 75,573 observations. Prefix,
alignment and mutation guards pass, and the prior escort assessment is unchanged.
Native gameplay is unchanged. Full drawing/HUD cadence, broader independent
flights and sound timing remain open. See [complete-flight evidence](analysis/native_mission_five_full_flight_milestone.md).

Successful original mission five (2026-10-10): a touchdown-height standoff
approach completes formation/combat, arrests on the carrier, earns grade one
and the third completion, then returns to the menu. All 75,573 observations,
final RAM and consumed keys replay exactly without the controller, including
the earned prefix. Gear/arrest checks and nine profile/driver guards pass.
Native gameplay is unchanged. Actual update identities, independent native
comparison and sound timing remain open. See [mission-five success evidence](analysis/native_original_mission_five_touchdown_milestone.md).

Original mission-five wire approaches (2026-10-10): three complete recordings
replay exactly, including the earned prefix. Descent input reaches the height
gate, but both finals cross the wire too high; no mission-five grade is earned.
The failed-water diagnostic now preserves valid carrier arrest/result timing.
Two original landing-condition cases, nine driver guards and three profile
guards pass. Source archives and rejected captures remain retained. Native
gameplay is unchanged. Successful return, independent flight comparisons and
sound timing remain open. See [wire approach evidence](analysis/native_original_mission_five_wire_milestone.md).

Original mission five (2026-10-10): all 72,650 observations, final RAM and
consumed keys reproduce without the controller, including the earned
47,814-observation prefix. Formation/combat finish, but the landing earns
no grade and remains rejected. The prior budget failure is retained;
27 rejection/retention checks pass. Native gameplay is unchanged.
Successful carrier return, independent native comparisons and sound
timing remain open. See [failed landing evidence](analysis/native_original_mission_five_failed_landing_milestone.md).

Complete escort bodies (2026-10-10): all 6,954 actual native bodies pass
original instructions, including combat, carrier arrest and postflight.
Ground rendering now publishes the source C09812/C09850 accumulator;
the preceding sweep's 18 cache-word failures remain retained. A new
independent playable replay preserves the complete successful recording,
RAM, pilot and counters exactly. Release/Debug affected checks and the
preceding-code rejection guard pass. Broader independent flight histories
and original sound timing remain open.
See [complete body evidence](analysis/native_escort_complete_bodies_milestone.md).

Live native escort (2026-10-10): ordinary qualification and mission-three
controls earn the prefix, then live-carrier controls earn escort, wire
landing, the second completion and menu return. The playable runner
reproduces all 27,176 trace rows, final RAM and saved pilot exactly with
zero resets. Three original instruction bodies around the first motion
difference pass; failed approaches remain rejected and retained. Patrol
regression passes. Original/native whole-flight and sound timing
acceptance remain open. No playable change.
See [live escort evidence](analysis/native_live_escort_input_milestone.md).

Startup dispatch (2026-10-10): original empty-voice handling contains a
400-iteration wait after DMA stop. The recorded channel gap is 5,438
chip clocks at DMA enable plus 12 at the following fetch, agreeing with
all startup consumed-byte phases. Source/write/timing mutations reject;
previous handoff evidence is unchanged. These measurements are not a
native delay constant; onset/waveform acceptance remains open.
See [dispatch evidence](analysis/native_startup_dispatch_milestone.md).

Startup sample handoffs (2026-10-10): all 468,007 original consumed bytes
follow published buffers, addresses, byte order and periods. All 48
requests match the current native ordered payload/period/volume sequence;
the complete native Demo also preserves PCM/RAM/trace/counters exactly.
Original right-channel phase stays 5,450 chip clocks behind left, while
native starts both together. Onset/waveform acceptance remains open.
A conflicting startup DMA grid is retained as failed; the independent
byte observer passes without claiming separate DMA coverage. No playable
change. See [startup sound evidence](analysis/native_startup_sample_handoffs_milestone.md).

Complete escort comparison (2026-10-10): the successful original recording
has exact JSR/LINK identities for all 47,814 observations and a verified
arrest snapshot. Native consumes every key and reaches the menu without
a reset, but earns no escort grade. All 7,170 flight observations are
compared; strict parity remains rejected and retained. Different clock
inputs already select different initial carrier positions. No playable
change or fitted clock is introduced. Sound timing remains open.
See [complete comparison evidence](analysis/native_escort_wire_full_flight_milestone.md).

Successful original escort (2026-10-10): targeting the live original
arrestor geometry completes escort, earns the second mission completion
and returns to the menu. All 47,814 observations, final RAM and consumed
keys reproduce exactly without the controller, including the earned
prefix. Two wrong-target guards reject. Native runtime is unchanged.
The successful reference is ready for update-identity/native whole-flight
comparison; sound timing remains open. See [escort success evidence](analysis/native_original_escort_wire_milestone.md).

Original escort deck contact (2026-10-10): the validation standoff-height
route reaches the original carrier deck with gear down and hook extended,
but misses the arrestor and earns no escort grade. All 65,000 observations,
final RAM and consumed keys reproduce exactly; the first 46,126 equal the
preceding route. Actual wire geometry is aft of the controller target.
The playable runner is unchanged. Wire targeting, broader full flights
and sound timing remain open. See [deck/wire evidence](analysis/native_original_escort_standoff_milestone.md).

Original escort approach (2026-10-10): the existing validation height gate
reduces landing overshoot, but the original still earns no escort grade.
All 65,000 observations, final RAM and consumed keys reproduce exactly
without the controller; all 45,254 observations before the approach change
equal the preceding recording. Three profile guards reject. The playable
runner is unchanged. Successful escort landing, broader independent flights
and sound timing remain open. See [approach evidence](analysis/native_original_escort_approach_milestone.md).

Original escort steering (2026-10-10): repeated ordinary steering inputs
reach combat success in an independently started original game. All 65,000
observations, final RAM and consumed keys reproduce exactly without the
controller, including the complete earned prefix. The return stops beyond
the carrier; no escort grade is earned. The input option is validation-only;
native runtime is unchanged. Successful landing, independent whole-flight
comparison and sound timing remain open. See [steering recording evidence](analysis/native_original_escort_steering_milestone.md).

Default-host combat (2026-10-10): mission five and the final mission pass
all 43,100 presentations with sound, all 14 camera modes in each mission,
gear raised after takeoff and zero restarts. Maximum work is 16.2690 ms,
with no frame over 20 ms, model fault, heap violation or pool failure.
The earlier mission-five clip with premature G remains rejected and fully
retained; both PAL controls survive, so its two host restarts are not
attributed solely to gear. The checker now verifies the G press follows
takeoff. Strict PAL comparisons stay unchanged. Independent complete flights,
original sound timing and broader default-host outcomes remain open.
See [default-host combat evidence](analysis/native_host_combat_views_milestone.md).

Interactive host clock (2026-10-10): windowed gameplay now acquires actual
microseconds, preserving all scene-selection bits. Headless defaults retain
PAL time; sealed visible comparisons explicitly select PAL diagnostics.
Original timer checks pass 72 cases plus two record passes; the final focused
CTest passes 5/5. A fresh default-host Free Flight run presents all 6,502 frames
with live sound, maximum work 15.3594 ms and zero heap violations/pool failures.
This grounded check does not establish default-host combat or full-flight
parity. Independent whole flights and sound timing remain open; campaign
continuity stays waived and named-state cleanup stays outside this goal.
See [host clock evidence](analysis/native_host_clock_milestone.md).

Escort scene clock (2026-10-10): the actual native C0FECE setup matches
original instructions with its own inputs. A reference-only microsecond
probe reproduces every byte of all 16 original cores; three wrong-clock
probes reject. Native timer quantization zeroes the five bits used for
source scene variation, so timer-resolution fidelity remains open. The
whole native replay preserves counters/RAM/save; full escort parity and
sound timing remain open. See [scene clock evidence](analysis/native_escort_scene_clock_milestone.md).

Escort event replay (2026-10-10): source-declared setup/flight events now own
all 2,028 recorded key edges while every actual native update remains counted.
The complete 3,816-observation comparison runs and fails from initial state;
the native outcome also differs. Twelve event-evidence guards and the replay
CTest checks pass; default RAM/pixels/PCM/counters remain byte-identical to
the previous executable. Initialization rules, successful escort coverage and
sound timing remain open. See [event replay evidence](analysis/native_escort_event_replay_milestone.md).

Escort comparison origin (2026-10-10): all 44,458 original observations map
to 44,455 executed updates; three resumptions and 2,028 key edges are retained.
The native replay is rejected before flight assessment: timed briefing paths
enter flight 246 updates earlier than the shared global input schedule. Full
failed captures remain retained; no offsets or runtime changes are applied.
Mission-event input ownership/initialization, successful escort and sound
timing remain open. See [escort origin review](analysis/native_escort_initialization_review_milestone.md).

Earned original escort recording (2026-10-10): qualification and the complete
first-mission prefix reproduce exactly before escort. All 44,458 observations,
final RAM and consumed controls match an independent unmodified replay. The
route ends in the original FE failure; it earns no escort grade. Eight prefix
guards and the unchanged 4,967-observation assessment pass. Native escort
comparison/successful coverage and sound timing remain open. See [escort
recording evidence](analysis/native_earned_escort_recording_milestone.md).

Visible final combat/cameras (2026-10-10): all 21,550 presentations and
all 14 source camera modes pass with sound and complete headless PCM/RAM/
pixels/save/counter equality. Maximum frame work is 17.7416 ms; none exceed
20 ms. Gear is raised after takeoff; project heap violations are zero.
This covers a partial final-mission clip on this host. Other missions and
sound timing remain open. See [camera performance evidence](analysis/native_visible_combat_views_milestone.md).

Current native sound (2026-10-10): a fresh complete Demo preserves all
12,680 boundaries, 8,248 requests, 554 stops and every PCM/RAM/trace byte.
All 8,281 original request payloads and 48 ordered startup-music requests
match current native assets. Original/native onset and handoff timing
remain open. See [current audio evidence](analysis/native_current_audio_recording_milestone.md).

Complete original sample use (2026-10-10): all 21,069 replay calls retain
5,878,371 actual mixer bytes with complete PCM/execution unchanged. Channel 1
emits both bytes of its extra zero word; channel 2 discards its startup word.
Three initial byte addresses remain explicitly unknown. Thirty guards and
the real original-audio CTest pass; full compressed retention revalidates.
Native onset/handoff/waveform and the extra word's origin remain open. See
[sample-use evidence](analysis/native_original_audio_sample_use_milestone.md).

Original audio endpoints (2026-10-10): stopped replays at calls 93 and
2,177 exactly preserve all matching complete-recording PCM, events and DMA
fetches. Actual channel-2 state is idle; the source startup check now requires
that endpoint. Channel 1 is playing and its zero word remains unresolved.
Seven prefix guards pass; duplicate PCM and raw RAM are removed after
verified compressed retention. Native timing/waveform remains open. See
[endpoint evidence](analysis/native_original_audio_endpoint_milestone.md).

Original startup audio fetch (2026-10-10): the channel-2 zero word at
call 2,178 is discarded by the unchanged source startup state machine.
All 66,560 priming-word/attachment cases and five corrupt-context controls
pass after complete recording revalidation. Both uncatalogued words stay
retained; channel 1 at call 94 remains unresolved. Native onset, handoff
timing and whole-waveform acceptance remain open. Runtime is unchanged.
See [startup-fetch evidence](analysis/native_original_audio_startup_fetch_milestone.md).

Complete sound fetch contents (2026-10-10): all 21,069 original replay calls
retain 2,939,342 actual DMA words, with complete PCM/RAM/state/register/video
and the earlier event log unchanged. Every word matches retained original RAM;
2,939,340 catalogued words also match current native asset bytes. Two original
zero fetches outside the request spans remain explicit for handoff assessment.
Thirteen guards and the real 32-frame CTest pass; raw passing data and the
duplicate WAV are removed after verified compressed retention. Native runtime
is unchanged. Sound ordering/timing/waveform acceptance remains open. See
[fetch-content evidence](analysis/native_original_audio_dma_milestone.md).

Visible complete flight (2026-10-10): qualification and mission three now
pass all 42,706 visible presentations with sound, complete final RAM/counters
and the earned pilot unchanged. Maximum measured work is 16.3617 ms, with no
frame above 20 ms. This covers cockpit view 0 on this host; other missions,
combat and camera views remain open. Earlier input-poll failures are retained
without an inferred cause. Runtime is unchanged; recorded sound timing remains
open. See [visible-flight evidence](analysis/native_visible_complete_flight_milestone.md).

Complete mission-three drawing (2026-10-09): all 4,967 independent flight
observations and all eight planes now pass the original-rule history gate
(317,888,000 XOR bytes), with every complete core/scene row matching and no
unexplained observation. Actual panel, radar and message inputs predict
21,612 complete owner page sets; phase, paint, glyph and omitted-owner controls
all reject their mutations. Runtime is unchanged. Audio timing, visible
performance and broader flight coverage remain open; campaign continuity is
waived and named-state cleanup remains outside the goal. See [full-flight
evidence](analysis/native_complete_cockpit_flight_milestone.md).

Actual panel boundaries (2026-10-09): the complete original flight now
retains 3,602 C30764 calls/returns and preserves all 29,305 previous RAM/register
snapshots, the whole replay, final RAM and counters. All 10,806 drawing-owner
returns restore their callers. The actual redraw counter can change from zero
to three between frame entry and drawing; the full history check now consumes
that call boundary. Runtime is unchanged; full drawing acceptance remains open.
See [panel evidence](analysis/native_actual_panel_boundaries_milestone.md).

NO SIG drawing history (2026-10-09): all eight planes across the next
51-observation episode follow actual writes (3,264,000 XOR bytes), closing
48 more plane-0 differences. The full-flight gate still fails with 1,825
plane-0 and 2,785 plane-3 observations open. Missing selected markers now
get explicit inactive-control assessment; counters, ordinary contacts and
all paint controls remain strict. Source-only omitted writes are rejected.
The selected-target regression stays byte-identical; runtime is unchanged.
See [drawing/control evidence](analysis/native_no_sig_cockpit_history_milestone.md).

Complete original flight capture (2026-10-09): 4,967 independent input
observations, 4,965 bodies and 3,602 actual radar/message calls each now
retain complete RAM/registers in 23.71 MB gzip. All 27,110 source trace
rows, counters and final RAM stay exact; 320 old snapshots match and all
7,204 owner returns restore their caller stacks. Current native full traces
are unchanged. The next 51-body message window passes, but its radar
verifier assumes a selected marker despite NO SIG; drawing acceptance
remains failed. See [full original capture evidence](analysis/native_original_full_frame_capture_milestone.md).

Complete mission-three bodies (2026-10-09): all 4,965 unique native bodies
match original instructions with zero gameplay/display differences under
the existing frame-body contract. Compact complete-RAM capture retains
14,895 snapshots in 5.61 MB gzip, preserving the whole 42,706-frame run,
final RAM and earned pilot with zero gameplay heap violations. Release/Debug
reproduce the previous complete 40-body window; affected CTests pass.
Independent drawing history, audio timing and visible performance remain
open. See [frame-body evidence](analysis/native_complete_frame_body_milestone.md).

Combined cockpit history (2026-10-09): all eight complete planes across the
first 40-observation drawing episode follow actual radar, bitmap and message
writes (2,560,000 XOR bytes). The first 38 differing plane-0 observations are
explained; 1,873 remain open. All three omitted-owner histories are rejected;
earlier message and whole-plane-1/2 reports are unchanged. See
[combined drawing evidence](analysis/native_combined_cockpit_history_milestone.md).

Remaining drawing audit (2026-10-09): original radar writes explain the first
17 differing plane-3 observations. Full-flight audits retain 1,911 unresolved
observations for plane 0 and 2,785 for plane 3 and exit failure; whole-plane
acceptance stays strict. Plane-1/2 reports remain identical. See
[remaining drawing evidence](analysis/native_remaining_plane_audit.md).

Complete mission-three plane histories (2026-10-09): all bytes of planes 1
and 2 on both pages are accounted for across all 4,967 independent flight
observations (79,472,000 bytes per plane). Every differing observation has a
validated actual-input radar/message paint history; a missing late history
is rejected. Complete record cores stay exact. Runtime is unchanged; whole
planes 0/3 and broader completion remain open. See [plane evidence](analysis/native_complete_plane_history_milestone.md).

Complete retained audio payloads (2026-10-09): all 8,281 original Demo handler
requests resolve in final RAM to native payloads on the same channels (12
original payloads). Full PCM/event coverage and native owned-span hashes pass;
four corrupt catalogs are rejected and the 48-request startup gate still passes.
Fetch-time contents, handoff ordering and waveform timing remain unproved.
Runtime is unchanged. See [payload evidence](analysis/native_complete_audio_payload_milestone.md).

Bounded message pages (2026-10-09): ten actual C322EE inputs/returns match
original instructions; immutable glyphs and source colour/clear rules predict
all 640,000 owner page bytes. Five native bodies and live original text returns
pass. Current whole-flight message rules still pass all 4,964 transitions and
nine controls. Runtime is unchanged; cross-runtime complete frame histories
and broader drawing remain open. See [message evidence](analysis/native_message_owner_pages_milestone.md).

Bounded radar history (2026-10-09): ordered original crosshair, cache erasure
and marker writes predict all 70 complete owner page sets across 35 observations.
Actual instrument bitmap redraws explain all requested plane XOR bytes
(944,000), including the later background-bit interaction. Negative controls
and the existing five-frame delta check pass. Runtime is unchanged; complete
flight drawing remains 287/4,967, with message differences still open. See
[paint history evidence](analysis/native_radar_paint_history_milestone.md).

Free Flight model crash fix (2026-10-09): valid command $90 at C3BA04
now derives the original six-point block through C20F78; its shared $94
entry is also connected. 184 complete command cases and 32 actual-model
camera poses match original instructions. Release/Debug model, flight-start
and preallocation checks pass; canonical Release is refreshed. Precise
reported flight route is unsealed. See [crash evidence](analysis/native_free_flight_six_point_block_fix.md).

Current whole-flight allocation/cockpit check (2026-10-09): Release/Debug
preserve all qualification/mission-three traces, final RAM, counters and
earned saves, with zero gameplay heap violations. Five bounded complete
page deltas match selected-radar marker history. Corrected original owner
returns match all pages in both bounded windows (5/5 and 11/11); the previous
end address included the next HUD child and is now rejected. Bounded pixel
interaction is now explained above; broader drawing stays open. Runtime is unchanged. See
[current evidence](analysis/native_cockpit_radar_milestone.md).

Original voice ownership (2026-10-09): reference tracing now resolves the
loaded sound hunks, correcting false empty voices in a separate launch with
a 480-byte relocation. Complete original Demo and separate-launch execution,
PCM and register writes remain exact. All 48 initial music requests match
native sample bytes/order/period/volume. Original right-channel onset follows
left by 67 samples; native starts together, so onset/whole-flight sound remain
open. Native executable and gameplay storage are unchanged. See
[voice and handoff evidence](analysis/native_original_voice_layout_milestone.md).

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
cleanup remain separate. See [filter integration evidence](analysis/native_pcm_filter_milestone.md).

Original filter evidence (2026-10-09): the existing LED interface reports
filter enabled at all 21,069 Demo boundaries, preserving complete original
PCM/execution and every earlier event row. Two independent cold starts also
produce 13,230,014 stereo frames matching unchanged A500/LED filter functions
exactly, including the reference mixer gain; non-silent output and every
sample are checked. Wrong filters and corrupted LED traces are rejected.
Native executable is unchanged; filter integration, onset/handoff alignment
and whole-flight sound acceptance remain open. See [filter evidence](analysis/native_original_filter_state_milestone.md).

Native PCM averaging (2026-10-09): playable output now uses original source
interval averaging instead of sample holding, with fixed integer storage.
196,608 intervals match unchanged original accumulator functions in both
builds. Intro, Free Flight and final combat preserve complete game state;
Debug/Release agree on the intentionally changed PCM. Complete Demo requests,
stops, voices and stream phases remain exact, with zero heap violations.
Canonical Release is refreshed. Original onset/handoff alignment and Amiga
filter acceptance remain open; broader completion and state cleanup scopes
are unchanged. See [averaging evidence](analysis/native_pcm_averaging_milestone.md).

Native audio event comparison (2026-10-09): the playable optional trace records
all 12,680 Demo boundaries, 8,248 requests and 554 stops. Release/Debug agree
on every row and complete WAV/RAM/counters/save, with zero heap violations.
Trace, preallocation/output and PCM ownership CTests pass in both builds.
Native/source onset, handoffs and interpolation/filter acceptance remain open;
different startup/timing contexts are not equated. Canonical Release refreshed.
See [native audio trace evidence](analysis/native_audio_trace_milestone.md).

Original complete demo audio (2026-10-09): all 21,069 ordinary reference calls
and 18,582,858 stereo sample frames are recorded intact at 44.1 kHz. Their
406,753-row audio-write/voice trace also preserves full execution and PCM.
LED pin is on at both retained endpoints; intermediate filter state remains
unproven. Intrusive serialization was rejected. Full nonrecording replay
preserves complete RAM/state/registers/video/audio hash; corrupt/incomplete
checks and the active CTest pass. Native executable is unchanged. Native
onset/handoff alignment and interpolation/filter parity remain open; different
replay contexts are not accepted as matching WAVs.
See [original audio evidence](analysis/native_original_audio_recording_milestone.md).

Gameplay preallocation (2026-10-09): project heap and buffered-file creation/
closure are rejected inside the playable frame loop. SDL uses a fixed 32 MiB
startup arena; PCM uses a fixed 512 KiB ring, with no growing audio queue.
Pilot log and diagnostic capture I/O reuse owned storage/direct OS handles.
Release/Debug preserve complete intro, Free Flight and final-combat PCM, RAM,
pixels, saves and counters. Earned individual missions and the independent
qualification/mission-three replay pass. OS/driver heaps are outside this
measurement; original randomness/timing remain unchanged. Named-state cleanup
is still deferred. See [preallocation evidence](analysis/native_preallocation_milestone.md).

Finished visible recorded flight (2026-10-09): qualification and mission three
complete with real audio, exact final RAM/counters/save, no resets and all
42,706 frames presented. Two SDL polling stalls produce 86.0423/719.6773 ms
work, so the 20 ms performance gate remains failed; every row and compressed
final RAM are retained. Both current builds preserve the full reference flight.
Window position changed during measurement; the cause of each stall remains
unproven. Other missions/views/combat and remaining completion work stay open.
See [visible result](analysis/native_visible_replay_input_milestone.md).

Visible replay diagnostics (2026-10-09): the user-closed attempt and a complete-
frame failed flight remain unaccepted. The latter reset unexpectedly and has
two SDL poll-work failures, 247.5437/81.3995 ms. Recorded-input-only is now an
opt-in comparison mode; window close and all poll/present costs remain checked.
Normal SDL controls and 26 entries/52 bodies match original in Release/Debug.
The rebuilt Release full flight preserves prior RAM, traces, counters and save.
The finished recorded-input-only result is reported above. See
[visible replay evidence](analysis/native_visible_replay_input_milestone.md).

Complete mission-three drawing localization (2026-10-09): all 4,967 observations
match rows 0..127 of both complete pages. Five adjoining diagnostic bands cover
every page byte; strict pages remain 287/4,967 without exclusions. Release/Debug
preserve prior traces, final RAM, counters and earned saves. Four bounded
snapshots locate the first row-191 difference to sliding target-info text, with
the same original/native slide offset. Later cockpit drawing remains open.
See [drawing localization](analysis/native_mission_drawing_bands_milestone.md).

Complete mission-three message timing (2026-10-09): all 4,964 real transitions
obey original elapsed/countdown, delay/redraw, full text and colour-cache rules,
including 1,362 paused HUD periods. Nine priority-message producer fields match
at all 4,967 observations. Both first HDG events follow two sampled-second
changes (original tick168/native185); this is assessed under the accepted
cadence policy. Release/Debug agree; nine wrong results are rejected. Optional
read-only fields preserve all earlier traces, pages, final RAM, counters and
saves. Strict pages remain 287/4,967; other drawing and completion work remain
open. State cleanup stays deferred. See [message evidence](analysis/native_mission_message_timing_milestone.md).

Mission-three radar cadence (2026-10-09): the first remaining strict pixels
at 22,303 are the selected-target marker on opposite incoming blink phases.
Twenty-four actual original owner returns and native bodies, 96 complete
owner comparisons and four rejection probes preserve original increments,
gates, coordinates and colours. All 22,326 reference observations reproduce.
This bounded radar phase is assessed under the accepted rendering cadence;
strict pages remain 287/4,967. Target-info text at 22,309 is the next unassessed
difference. Runtime and counters are unchanged; state cleanup stays deferred.
See [radar evidence](analysis/native_mission_radar_cadence_milestone.md).

Ground-strip startup fix (2026-10-09): native now calls original C2527C,
generating all 40 disk-backed strip corner records before gameplay. Missing
corners caused empty ground draws and later placement skips, consistent with
the Free Flight disappearing-road report. Initializer RAM and all 240 corners
match original instructions; the omitted-call regression is rejected.
Release/Debug mission-three gameplay remains exact; first missing-ground-line
pages now match and strict drawing improves 267 -> 287/4,967. The next strict
difference is observation 22,303. Free Flight callback/restart, connected model,
frontend and new CTests pass. Other drawing remains open; state cleanup stays
outside this goal. See [ground evidence](analysis/native_ground_bounds_startup_milestone.md).

Native raster addressing (2026-10-09): negative-X lines now use the original
logical-shift starting offset and aligned word address. The connected raster
checks pass all existing cases and 32 new negative-X cases per checkpoint.
Release/Debug complete mission-three gameplay, traces and RAM remain identical
to the preceding evidence; strict drawing stays 267/4,967. A bounded capture
localizes the first visible difference to a skipped ground segment and a
different placement cache; its earlier cause remains open. Canonical Release
is refreshed. State cleanup remains deferred outside this goal. See
[drawing evidence](analysis/native_negative_line_and_drawing_window_milestone.md).

Latest completion scope (2026-10-09): the user has moved internal state cleanup
to a separate follow-up outside the active complete-C-port goal. Migrating the
remaining byte-array state into named C structures is unfinished ownership and
clarity work, not a completion requirement for this goal or evidence of a
missing mission. The arrays are ordinary PC memory. This direction supersedes
older notes that include state cleanup in the active completion criteria.
Remaining acceptance work covers independent whole-flight comparisons,
recorded audio/filter fidelity and visible gameplay performance. The earlier
waiver of an uninterrupted campaign remains in effect; repeated individual
mission testing is accepted instead. Do not begin state migration in this goal.

Independent mission-three execution comparison (2026-10-09): the successful
flight now passes complete gameplay-state parity in Release and Debug. All
4,967 source observations match 79,472 complete record cores and camera/control/
target state. The full original trace, final RAM and 1,426 consumed edges are
unchanged: an instruction/entry probe proves two interrupt resumptions were
extra recorder observations, not updates. All observations remain compared at
their actual JSR/LINK call identities. Native traces, RAM, counters, grade and
saves agree across builds; both return to the menu. Strict drawing matches
267/4,967 and remains open. No runtime, clock, physics or Escape change. This
resolves the gameplay-state discrepancy in the earlier success note below.
See `analysis/native_independent_mission_execution_milestone.md`.

Independent mission-three successful recording (2026-10-09): the original
now earns its objective, geared runway landing, grade one and menu return. All
27,110 trace boundaries, consumed controls and final RAM reproduce without
the validation controller. Independent native Release/Debug also succeed and
agree on traces, RAM, counters and saves. Whole-flight state parity remains
unaccepted: the first 360 boundaries match, then the source repeats the same
observed tick/state at loop 22,502 while native advances. Ten verified RAM
snapshots retain that discrepancy; reference entry-resumption accounting is
the next investigation. The strict gate correctly fails. Native geared-input
qualification and canonical replay pass in both builds; the default profile
still passes its original source gate. Gameplay/Escape are unchanged. See
`analysis/native_independent_mission_success_milestone.md`.

Independent mission-three recording (2026-10-09): a complete failed flight
now matches all 5,216 record cores and camera/control/target state across 326
boundaries through C11788 crash/reset. The generated ordinary-key recording
reproduces all 22,467 original trace boundaries, consumed keys and final RAM
without the validation controller. A native pilot is enlisted and qualifies
through ordinary keys; both pilots have scene level zero. The bundled level-two
pilot probe was rejected for parity. Release/Debug traces, RAM, counters and
saves agree. Strict drawing matches 259/326; 67 differences remain unassessed.
This failed route does not qualify a successful mission-three whole flight.
Gameplay and original Escape restart are unchanged; broader completion stays
open. See `analysis/native_independent_mission_recording_milestone.md`.

Independent complete qualification flight (2026-10-09): Release/Debug agree
through landing, results and the original restart callback. All 2,728 first-flight
boundaries match 43,648 complete record cores and camera/control/target state;
2,722 complete drawing boundaries match. All 1,611 distinct gameplay states
across nine result/restart callback runs agree. Six strict message differences
are localized and assessed as one-step expiry/blink phase: 83 observed boundaries
and 267 original message-owner comparisons pass, plus four rejection probes.
Both take the FF -> EF qualification result branch. Initial pilot words differ
(original0/native1); this is not new-pilot proof. The source recording ends
during second-flight setup; a native menu trace gap and later fixed-offset
failures remain explicit. Gameplay and retained plain-Escape restart are
unchanged. All-mission independent flights and the other completion work
remain open. See `analysis/native_independent_qualification_trace_milestone.md`.

Independent whole demo recording now has bounded read-only traces. Its first
flight's 2,046 boundaries match all 32,736 cores and camera/control state;
1,235 complete drawing boundaries match. Twelve actual callback runs through
automatic restart preserve all 2,062 distinct gameplay states. The viewport wait
spans33 PAL ticks in both runs despite different loop counts. Strict drawing and
fixed-offset failures remain reported. See
`analysis/native_independent_demo_trace_milestone.md`. All-mission independent
flights and remaining drawing assessment remain open.

Reported ALT/HDG timing (2026-10-09) is assessed under the agreed cadence policy.
Both independent mode-three recordings obey every elapsed accumulator,
request/page and complete message result across the 374-transition selected-target
episode. Original/native format 56/52 new lines, preserve 318/322 cached or blank
lines and suppress the owner on 46 map-context passes each. Both first HDG events
follow two changed-second events; seven initial page transitions agree at equivalent
countdown events. Four wrong-page/clock/text mutations are rejected. V2 fixes
a diagnostic label: V1's "selected_record" was actually mouse coordinates.
The actual selected-record word and HUD gates are now observed. All prior common
trace bytes, final RAM and counters are preserved; Release/Debug full traces and
assessments agree. Eight selected CTests pass. Strict drawing remains 1,235/2,046;
all-mission whole flights, other drawing differences, recorded audio/filter,
visible performance and typed-state cleanup stay open. See
`analysis/native_demo_hud_timing_milestone.md` and its checkpoint.

Escape behaviour (2026-10-09): the user confirmed that plain Escape restarting
the selected Free Flight mode is original behaviour and requested keeping it.
The proposed SDL Escape-to-menu override and its tests were dropped before
commit. C0F992 preserves the mode for restart; modified Escape follows the
existing abandonment/reset path. The earlier menu-return check used Shift+Escape.
Release/Debug were rebuilt and canonical Release refreshed with the original
input code. Both builds pass host-key/frontend checks and artifact cleanup.
The original-input CTest hit its unchanged 15-second oracle timeout twice;
the same comparison passes with a diagnostic 120-second allowance: 2,512
pending-input cases, 12,576 command parents/returns and 16 menu Delete cases.
No test timeout or gameplay source was changed.

Initial startup sorting (2026-10-09) is now verified, completing that requested
check together with qualification entry. The actual C0F812 stack frame and
complete C08F26 bootstrap match native startup; all three lists and retained
output agree. Ten startup intervals add thirty lists and twenty incoming-factor
probes. Release/Debug agree on all 22 combined intervals, 60 lists, 40 probes and
RAM hashes. No startup sort fault or gameplay change was found. Optional
read-only observation attaches before initialization; normal frontend and demo
regressions pass in both builds. Canonical Release is refreshed. See
`analysis/native_initial_startup_sort_milestone.md` and its checkpoint. Older
initial-startup TODO notes below are superseded for the requested callers.

Latest acceptance direction (2026-10-09): the user explicitly waived an
uninterrupted qualification-plus-six-mission run because the final mission's
spawn variation makes that route difficult. Qualify missions individually with
repeated tests instead, retaining actual earned saves, objectives, landing,
results and menu-return checks. Cold starts between missions are allowed.
The existing continuous-campaign tools and five-mission prefix remain diagnostic
evidence; the unsuccessful sixth mission in that session is no longer a
completion blocker. This supersedes every older uninterrupted-campaign
requirement below. Failed bank/height test-pilot experiments were reverted.
Independent original whole flights, recorded audio/filter
fidelity, visible performance and named-state cleanup remain separate work.

Repeated individual qualification is now accepted: current Release and Debug
each complete qualification and all six cold earned missions, result/menu and
Next Mission wrap. All nine tour stages match exactly, including counters and
78-byte saves. Fresh Release original/new-pilot qualification and final-flight
source gates also pass; ten selected CTests pass in total. Final source gates
compare 42/64 intervals and 239/220 bodies and each replay the actual playable
runner. See `analysis/native_individual_mission_acceptance.md` and its checkpoint.
The complete port remains unfinished. Visible-performance tooling must accept
validated individual geared routes instead of requiring the waived campaign.

Free Flight renderer fix (2026-10-09): the user's location-three crash at
model C3AAC4 / stream C3ABEA / command 4018 was a missing native command.
Original table C1FCE8+$18 points to C206E4, now connected through the existing
model dispatcher to `draw_interpolated_segments` in `draw_stream.c`. It retains
the original fixed-point interpolation, DIVS overflow, rounding, clipped drawing
and command return/countdown. Model errors now print player X/Y/Z in world units
(stored coordinates /256), pose index, mode and stage before aborting.
The model oracle compares 72 complete command cases and 16 complete C3AAC4
poses, reaching eight original C206E4 calls with matching RAM/display/returns.
The user also flew through the affected area again and reported no crash.
The automated 50,000-tick location-three smoke passes; its straight flight
already passed before the fix and is not an exact replay of the reported crash.
See `analysis/native_free_flight_interpolation_fix.md` for validation scope.
Renderer, Free Flight startup and cleanup checks pass in Release and Debug;
both executables and canonical Release are refreshed, including diagnostics.

Current title/music fix (2026-10-09): the title now waits for any key instead
of advancing on a timer. Disk music starts at master 63, ducks to 31 on the
acknowledging key, and stops before gameplay. Empty voice requests now clear
current/next PCM buffers immediately, preventing old music from playing at an
engine's pitch after a stop/start in one host block. Menu return restarts music.
Original owners: C0E3E6/C17B96, C11478, C0FECE and C50134-C50150. The title
hold is explicit user steering and supersedes the previous timed credits path.
`fa18_native_music_transition_[1257]` covers actual title/menu/flight buffers
for Demonstration, Free Flight, qualification and a mission, plus menu return.
The frontend check also holds original title pixels for 9,000 idle ticks.
All ten selected Release and Debug checks pass (four music routes, PCM
ownership, original sample/voice oracles, frontend, menu start and cleanup).
The mission route also checks music absence through its active-flight endpoint.
This corrects the coalesced-stop assumption in the older PCM ownership note.
Canonical Release is refreshed; running games need a restart to load it.
Full original recording/filter fidelity remains open.

Final-campaign diagnosis (2026-10-09): the earned five-mission cold suffix
sets the player's destroyed bit at tick 35318 while component damage is zero.
Five complete bodies around this hit match original instructions: zero gameplay
or display-byte differences. Original C2613A sets record +32 bit one on the same
body; C13D84 then freezes thrust/fuel as part of destruction. This resolves the
suspected motion freeze for this route. It does not qualify an independently
started original flight or an uninterrupted six-mission campaign. See
`analysis/figures/native_final_mission_destroyed_body_comparison.json` and
`analysis/native_continuous_campaign_investigation.md`. Three test-pilot input
experiments also failed and were reverted; only read-only destruction telemetry
is retained. The playable runtime and accepted gear-managed pilot are unchanged.

User-requested native text customization (2026-10-09): message 71 now reads
`SECURITY CLEARANCE GRANTED`. Its 34-character field and original descriptor
remain intact. Release/Debug builds and the frontend check pass; qualification
entry at tick 3420 visibly displays the complete phrase. This banner is an
intentional wording difference from original recordings.

Active completion investigation (2026-10-09): qualification and the first five
missions now complete with gear management in one actual Release playable
process, saving five earned grades and returning to menu at tick 146862.
All 78 saved bytes match the observing driver; no crash reset or pending input.
See `analysis/figures/native_geared_five_mission_prefix_checkpoint.json`.
Final combat remains unsuccessful; no six-mission acceptance is claimed.
The continuous driver and playable-replay checker are under `tools/native/`;
no passing whole-campaign result is claimed. Visible demo presents all 10,910
frames, p99 work 4.0922 ms, but a 543.2488 ms SDL input stall gives maximum
544.8323 ms and fails the 20 ms gate. The visible suite has ended; keep its
CSV/report until the stall investigation is resolved.
Continuation: `analysis/native_continuous_campaign_investigation.md`.

Latest user steering: gear up after takeoff, down before landing. The user
manually changed gear in visible mission three; that old fixed-key route later
crashes, so the visible suite ends without an accepted mission result. Generate
gear-managed routes before further visible mission measurements. The campaign
pilot now records ordinary G commands and requires observed gear-up flight and
gear-down before touchdown. Qualification and the first four missions pass with
this control. A preceding gear-down legacy run completed five missions but
crashed in final combat. Ground kills cannot satisfy mode five's required
airborne-expiry count. Radar interception and the result-camera input handoff
now pass mode five and rescue, saved/menu ticks 103321 and 125454. The cruise
interception missed with fuel available; the current attempt tightens its radar
launch alignment: `build/native-flight/campaign-session-gear-cruise-alignment/`.
No six-mission pass
or complete playable replay is accepted yet. The complete-port goal stays active.

Actual Release playable integration now passes the prefix through rescue in
one `fa18_native` process: qualification plus four missions, all 78 saved bytes
agree with the driver, final menu C0FCB4/mode zero at tick 125454, no crash resets
or pending input. The compact proof is
`analysis/figures/native_geared_campaign_prefix_checkpoint.json`. Debug now
replays the same prefix with identical complete counters and all saved bytes;
the six-mission pass remains open. The latest cruise gun approach earns the original
objective at tick 138725, then crashes during the low-altitude return handoff:
`build/native-flight/campaign-session-gear-cruise-close-gun/`. It is not a saved
completion. The diagnostic targets build in Release/Debug, and the existing
mission-five success comparison passes in both. No failing full-campaign test
is registered in default CTest.

The uninterrupted Release driver and its actual playable five-mission replay
now verify the cruise route, including carrier wire and saved grade. Current
final-flight diagnostics use the actual earned mode-seven save. A cold suffix
is diagnostic only and cannot replace the uninterrupted six-mission check.
Five-mission Debug replay remains open; the four-mission prefix passes Debug.

Latest mode-verified drawing-order audit (2026-10-09):

Actual Demonstration entry now sorts all three lists, matching original saved
A4 `$C47584`. The previous fixture selected qualification for its labelled
Demonstration case; selected-mode assertions now prevent that error. The actual
demo exposed 22 RAM differences despite equal retained words, fixed in the
connected native C0FECE path. Qualification entry also passes. Release and Debug
agree on 12 intervals, 30 lists, 20 factor probes and all RAM hashes across ten
scenarios. Frontend/demo and artifact checks pass; canonical Release is refreshed.
Initial startup remains unverified, and whole flights, uninterrupted campaign,
recorded audio, visible performance and named-state ownership remain open. See
`analysis/native_menu_sort_mode_audit_milestone.md` and its checkpoint. The full
goal remains active; older sorting summaries below are historical.

Latest menu/context sorting evidence (2026-10-08):

Nine ordinary-menu scenarios now compare the renderer's retained sort owner
directly with complete original callers: eleven intervals and 27 sorted lists.
All eighteen original-only incoming-factor probes preserve compared RAM and
retained output; a wrong retained word is rejected. Reached menu/restart paths
replace incoming factors through templates or uncached distance calculations.
Release and Debug each pass three selected checks and agree on interval/RAM
hashes. Gameplay and playable binaries are unchanged. Initial startup before
observers attach, mode nine and other rare callers remain outside this gate;
the unknown-factor TODO and complete-port goal remain active. Passing captures
are temporary, with a 6 MiB peak; build cache is 2.10 GiB. See
`analysis/native_menu_context_sort_milestone.md` and its checkpoint.

Latest native PCM ownership (2026-10-08):

Native PCM playback now retains typed current/next host spans owned by its
frontend. Each original C500D8 request resolves its sample address/length once;
byte playback no longer looks up global game-data addresses. Priming, chaining,
clock, pitch, signed samples and stereo remain unchanged. Intro/selection,
Free Flight and final combat preserve complete WAV, RAM, pixels and counters.
The whole earned tour agrees with its preceding results in Release and Debug.
Eight selected Release and six Debug checks pass; the canonical executable is
refreshed. One headless combat measurement observes mean audio work 15.17 ->
10.55 microseconds. This is mixer evidence on one host, not broader visible
performance or original recorded-audio fidelity. Voice programs/request state
and the rest of the typed-state migration remain open; the full goal stays
active. Build cache is 2.07 GiB, with capture/pruning limits unchanged.
See `analysis/native_pcm_buffer_ownership_milestone.md` and its checkpoint.

Latest earned new-pilot tour (2026-10-08):

A newly enlisted pilot now earns qualification and all six menu missions,
including the final Carrier Sub (F6/internal eight). Ordinary inputs complete
rescue, cruise interception and four final aircraft expiries, carrier wire
landing, save, messages/menu and cold Next Mission wrap. All six grades are one
and completions advance five to six. The submarine remains active at objective
admission; its visible explosion is not required by this original counter path.
Only validation input/evidence changes; gameplay and comparison masks remain
unchanged. See `analysis/native_new_pilot_tour_milestone.md` and its checkpoint.

New rescue/cruise/final gates compare 44/42/64 flight intervals and 191/171/220
bodies, with one original config write each and matching playable saved bytes.
The final gate also compares eleven flare/chaff presses and one cold-wrap
interval/body. `fa18_native_new_pilot_tour` replays the same earned chain from
an empty save directory across cold starts; only the game writes the log.
Independent original whole flights, uninterrupted single-process tour checks,
remaining caller contracts, typed state, audio and wider performance remain
open. The complete-port goal stays active. Passing RAM is temporary inside the
unchanged 240-pair / 480 MiB cap, with 4 GiB build pruning still enabled.

Nine selected Release checks and seven Debug checks pass, including the earned
tour, new mission gates, regression checks and artifact policy/cleanup. Both
configurations agree on inputs, saved bytes and objective/landing/menu/wrap
results. Debug final captures use a 120-second deadline after the original
90-second deadline timed out. The playable executables are unchanged; pruned
build-cache use is 2.01 GiB. Sealed evidence is in
`analysis/figures/native_new_pilot_tour_checkpoint.json`.

Earlier acceptance notes below are historical; this summary supersedes their
pending newly enlisted tour work.

Latest earned final availability (2026-10-08): the active final success gate
uses the actual pilot log saved after normal-key rescue and cruise flights.
The original byte-25 grade guard admits F6/internal mode eight. Ordinary keys
then complete four aircraft expiries, carrier landing, save, messages/menu and
cold Next Mission wrap. Completions advance 5 -> 6 and grade 0 -> 1.
All 42 flight intervals/239 bodies plus one cold-wrap interval/body match
original compared RAM/drawing; the playable replay agrees on saved bytes/menu.
No visible submarine explosion is required on this original counter path.
The cruise gate reproduces exact retained input and all 78 prerequisite bytes.
This removes synthetic eligibility from the active final success gate; gameplay
and masks are unchanged. The older patrol diagnostic remains explicitly
unearned. See `analysis/native_final_mission_earned_availability_milestone.md`.
The ADF pilot contains earlier progress: a newly enlisted complete tour,
independent original whole flights and full-port acceptance remain open.
The complete-port goal stays active; capture/pruning limits remain unchanged.
Six selected Release checks and four Debug checks pass, including both earned
progression gates, final success and the Release frontend/link check. Debug and
Release final inputs, saved bytes, objective/landing/menu and wrap agree.
The playable Release executable is unchanged; pruned build use is 2.00 GiB.

Latest cruise-missile sequence (2026-10-08): Intercept Incoming Cruise Missile
(F5/internal mode seven) completes interception, carrier deck/wire landing,
stop/save, messages, Escape/menu and cold reload through ordinary keys.
All 42 input/stage intervals and 171 sampled bodies match original compared
RAM/drawing, including 20 interception/expiry bodies, 80 consecutive landing/
result bodies and one config write. Completions advance 4 -> 5 and grade 0 -> 1.
The playable replay agrees on saved bytes/menu. The starting availability is
earned by the normal-key rescue save from the original ADF pilot; the rescue
gate now reproduces exact retained inputs and log bytes. It is not a complete
tour from a newly enlisted pilot. Gameplay and comparison masks are unchanged;
no new decompilation is claimed. See `fa18_native_mission_7_sequence`,
`analysis/native_cruise_mission_sequence_milestone.md` and its checkpoint.
All six menu missions now have successful native routes. Complete-tour
acceptance, independent complete original flights, remaining caller contracts,
typed state, audio and wider performance remain open; the full goal stays active.
Passing RAM is temporary inside the unchanged 240-pair / 480 MiB capture cap.
Nine selected Release checks and three Debug checks pass, with matching cruise
inputs, saved bytes and sequence evidence. The playable runner delivers all
1,533 cruise host events; its Release executable/link map remain valid and
unchanged. Build-cache use remains 2.00 GiB after pruning.

Latest rescue sequence (2026-10-08): Search and Rescue (F4/internal mode six)
now completes pod deployment, original near-site objective, carrier deck/wire
landing, stop, save, messages, Escape/menu and cold reload through ordinary
keys. All 44 input/stage intervals and 193 sampled bodies match original
compared RAM/drawing, including 40 rescue bodies, 80 consecutive landing/result
bodies and one config write. Completions advance 3 -> 4 and grade 0 -> 1.
The playable runner delivers 870 host events and agrees on all saved bytes and
the menu. Debug/Release rescue gates pass with matching inputs, outcomes and
saved bytes. Gameplay and comparison masks are unchanged; no new decompilation
is claimed. The fresh pilot comes from the original ADF, rather than a
port-earned complete tour. Captures remain inside the 240-pair / 480 MiB cap
and passing RAM is removed. See the serial `fa18_native_mission_6_sequence`,
`analysis/native_rescue_mission_sequence_milestone.md` and its checkpoint.
Internal mode seven is accepted above; earned tour availability and independent
complete original flights remain open; the complete-port goal stays active.
Eight selected Release checks and three Debug checks pass, including all five
accepted mission sequences in Release and the frontend/link gate. The playable
Release executable is unchanged; cache use remains 2.00 GiB after pruning.

Latest final-mission sequence (2026-10-08): ordinary keys destroy four patrol
aircraft, admit the original FF objective, land on the carrier wire, stop, save,
finish result messages and return to the menu. Completions advance 3 -> 4 and
mode-eight grade 0 -> 1. A cold Next Mission selection wraps to mode three.
Class-20 surface record 14 remains active at objective admission; no explosion is
required by this original counter path. All 42 flight intervals/239 sampled
bodies plus one cold-wrap interval/body match original compared RAM/drawing,
including 101 combat bodies, 64 consecutive landing bodies and one config write.
The playable runner delivers all 2,626 flight events and four cold-wrap events,
agrees on saved bytes/menu/wrap and has no queued input or crash reset.
C230B0's returned selection word is now preserved into C0A364/C0A3A6: original
FF04 replaces the adapter's incorrect 0004 completion word. Original predicates,
countdowns and comparison masks are preserved; no new decompilation is claimed.
The serial gate is `fa18_native_mission_8_sequence`; the earlier three-aircraft
diagnostic remains accepted. See `analysis/native_final_mission_sequence_milestone.md`
and its checkpoint. This historical availability fixture was explicitly
unearned, with no flight-state seeding. The active gate now uses earned final
availability as recorded above; independent complete-flight parity remains open.
Two identical flights partition capture storage inside the existing 480 MiB
cap and remove passing RAM before collecting the next partition. The 4 GiB
build-cache pruning hooks remain active.
Debug/Release playable and fixture builds pass. Eight selected Release checks
and three Debug checks pass, including final success, the earlier patrol
diagnostic, modes three/four/five sequences and the frontend/link gate. Both
configurations agree on inputs, counters, saved result and cold wrap. The
canonical Release executable is refreshed; build-cache use is 2.00 GiB.

Latest mission-four sequence acceptance (2026-10-08): ordinary keys from the
earned region pilot log shoot down record 8, complete the escort objective,
return to the carrier, capture the wire, stop, save, finish result messages,
press Escape and return to the menu with cold log reload. All 43 input/stage
intervals and 159 sampled bodies match original compared RAM/drawing, including
64 consecutive landing bodies and one actual config write. Completions advance
1 -> 2 and grade 0 -> 1. The playable runner delivers all 3,883 recorded host
events, returns to the same menu and saves identical bytes. Its JSON now reports
host replay delivery/pending counts separately from game-input replay counts.
The serial gate is `fa18_native_mission_4_sequence`; see
`analysis/native_mission_four_sequence_milestone.md`. This supersedes the
unsuccessful mission-four diagnostic below. Gameplay and comparison masks are
unchanged; independent complete original flights and full-port acceptance
remain open.
Debug/Release playable and mission fixture builds pass. Seven selected Release
checks and three Debug checks pass, with matching mission input/save hashes.
Passing RAM is removed and build-cache use remains 2.00 GiB. Evidence is in
`analysis/figures/native_mission_four_sequence_checkpoint.json`.

Latest user direction (2026-10-08): decompile any remaining instruction
translation encountered on the active path rather than bypassing it. The user also
clarifies that mission six means the final Carrier Sub mission (F6, internal
mode 8), not internal mode 6/Search and Rescue. The supplied Wikipedia passage
is historical context to verify against original outcome/save execution;
preserve original rules.
Mode 8 reaches the generic admitted/completed-counter scheduler, not `mode_six`.
See `analysis/native_final_mission_completion_context.md` for sources and scope.
The current comparison-build inventory distinguishes 75 already-readable
source-only entries from 84 deferred instruction translations. This inventory
does not measure whole-game completeness or native runtime integration.

Earlier mission-four input diagnostic (2026-10-08): the validation-only pilot
supports `4-success` and `4-sequence`, fires earlier on the closing pass and
can pursue regional enemy record 12 after the initial enemies. Starting with
the earned region pilot log, ordinary keys shoot down record 8: radar hits
advance 0 -> 1 at body 6,434. All 26 input/stage intervals and 81 sampled bodies
match original compared RAM/drawing. The flight still crashes and resets at
tick 15,999 (C11830); completions stay 1, with no objective or config write.
The fixture correctly returns failure. These scenario names request success
validation; they do not establish mission-four acceptance. Gameplay and the
playable executable are unchanged. Successful escort/landing/save remains next.
Debug/Release fixture builds pass. Five selected Release checks pass: existing
mission-four combat reset, mode-three/five sequences and artifact policy/cleanup.

Latest region coverage acceptance (2026-10-08): normal menu reset, qualification
and mission-three completion/save/menu return earn the level-zero region pilot.
Qualification matches 50 original intervals/174 bodies and mission three 44/164,
including both config writes. The playable runner agrees on the saved logs/menu.
Region flight preserves its established keys and strict coverage guards and
matches 57 intervals/49 bodies, with records 12/13 spawned, record 12's zone exit
and NPC missiles 9/13. The serial region gate now compares original boundaries;
`fa18_native_region_pilot_progression` reproduces the exact earned log. The host
replay loader accepts the complete 1,652-event flight with dynamic storage.
See `analysis/native_region_pilot_progression_milestone.md`. Game rules and
comparison masks remain unchanged. The mission-four acceptance above extends
this earned-pilot checkpoint; full-port acceptance remains open.
Debug/Release playable/fixture builds pass, with seven selected Release checks,
three Debug checks and the final Release region/report repeat passing. Evidence
is in `analysis/figures/native_region_pilot_progression_checkpoint.json`.

Earlier validation diagnostics (2026-10-08): the mode fixture services PCM
after each tick and supports bounded final-RAM snapshots through
`FA18_MODE_END_TICK` and `FA18_MODE_FINAL_DATA`. Established inputs and strict
guards remain intact. Runner/fixture RAM matches byte-for-byte at ticks
16,003, 18,500 and 22,000 with the existing region inputs. PCM does not resolve
the missing spawn/zone/NPC-missile coverage. The unsuccessful early pull-up/
rudder experiment is documented rather than adopted. Gameplay and the
playable executable were unchanged at that checkpoint. See `analysis/native_region_flight_diagnostics.md`.

Latest connected matrix/sight fix (2026-10-08): the native dynamics caller
passes C2DEE0/C2DFF6's product cursor to the following record's C2436A sight
update. All 576 complete matrix cases match original output/cursor/non-stack
RAM. The normal-key mode-four diagnostic now matches all 26 input/stage
intervals and 59 sampled bodies, including the crash continuation and natural
reset. The new serial gate is `fa18_native_mission_four_combat_reset`;
Debug and Release pass, and the playable executable is refreshed. Arithmetic,
gameplay rules and comparison masks are unchanged. See
`analysis/native_matrix_sight_reference_milestone.md` and `NEW_PORT_HANDOFF.md`.
This remains an unsuccessful flight: no hit/objective/save, crash tick 22,148
and completions unchanged at 3. Mode-four success is the next ordinary-input
mission task. The earlier level-two region run failed its spawn/zone/NPC-missile
guards despite all
58 input/stage intervals and 47 sampled bodies matching original compared
RAM/drawing. The earned-pilot acceptance above supersedes this regression task.
Original keys and strict guards are preserved; see the milestones.
Other successful missions, independent complete flights, further play
after the accepted result menu, remaining contracts, typed state, audio and
wider performance remain open; the complete-port goal stays active.

Latest mission result sequence acceptance (2026-10-08): modes three and five
complete normal-key success, save, result messages, Escape, scene bootstrap,
menu return and cold log reload. All 44/42 input-stage intervals and 162/169
sampled bodies match original compared RAM/drawing, including 64 consecutive
landing bodies per mode and one actual config write. Completion count stays
4 and grade stays 2; all 78 saved bytes survive restart unchanged. The serial
gates are `fa18_native_mission_3_sequence` and `fa18_native_mission_5_sequence`.
Debug/Release fixture builds and all five selected checks pass in each
configuration; consumed input and saved-result hashes agree.
Gameplay and the playable executable are unchanged; passing RAM is temporary
within the existing cap. See `analysis/native_mission_result_sequence_milestone.md`.
This supersedes open result-message/restart claims for modes three and five
below. Successful modes four/six/seven/eight, independent complete flights,
further play after this result menu, remaining contracts, typed state, audio
and wider performance remain open; the complete-port goal stays active.

Latest mode-five mission success acceptance (2026-10-08): normal keys complete
formation, both radar shoot-downs, the original objective, carrier arrestor
landing, stopped-aircraft admission, result save and cold reload. All 34
input/stage intervals and 155 sampled bodies match original compared
RAM/drawing, including all 59 consecutive touchdown-to-result bodies. Completion
count advances 3 -> 4 and mode-five grade 1 -> 2 without a crash reset. The serial
gate is `fa18_native_mission_five_success`. Debug fixture build and four selected
checks pass; Release fixture build and six selected checks pass. Gameplay and
the playable executable are unchanged. See
`analysis/native_mission_five_success_milestone.md`. This supersedes the earlier
open mode-five return/landing/save claims below. Result message completion and
restart, successful modes four/six/seven/eight, independent complete flights,
remaining contracts, typed state, audio and wider performance remain open;
the complete-port goal stays active.

Latest mode-five combat objective acceptance (2026-10-08): normal keys complete
formation, both radar shoot-downs and expiry increments, then reach original
objective phase FF at tick 19,304. All 29 input/stage intervals and 118 sampled
bodies match original compared RAM/drawing, including 32 formation-window and
40 combat-window bodies. The new serial CTest is
`fa18_native_mission_five_objective`. Debug fixture build and four selected
checks pass; Release fixture build and five selected checks pass. Gameplay and
the playable executable are unchanged. Completion count stays 3, no reset or
result write occurs in the bounded run, and safe return/landing/saved success
remain open. See `analysis/native_mission_five_objective_milestone.md`.
Independent complete flights and other whole-port work remain open; the
complete-port goal stays active. This advances the forced-return milestone below.

Latest mode-five forced-return acceptance (2026-10-08): normal keys hold all
201 native proximity scans and reach paired C0A12E restoration. All 24
input/stage intervals and 107 sampled bodies match original compared
RAM/drawing, including 64 consecutive proximity/restoration window bodies.
Original execution reports 12 calls for each aircraft; the native restored
views match the actual C295E0 table. This is a serial CTest gate. Gameplay and
the playable executable are unchanged. Debug builds and all three selected
checks pass; Release builds and all four selected checks pass. Mode-five mission
completion, result save/cold reload, independent full flights and other whole-port work remain
open. See `analysis/native_mission_five_forced_return_milestone.md`. This
supersedes the earlier formation-pilot investigation below.

Earlier mode-five investigation (2026-10-08): the validation-only `5-formation`
pilot and bounded tick limit reproduce an early frozen attitude. All 24
input/stage intervals and 36 sampled bodies match original compared RAM/drawing,
including contact bit-two transitions. This does not demonstrate a port bug
or a successful mission: the objective countdown stays 200 and completions
stay 3. Gameplay and the playable executable are unchanged. Default mode-three
and numeric mode-five pilots remain the existing acceptance/diagnostic paths.
See `analysis/native_mission_five_formation_checkpoint.md`. Mode-five success
and independent full-flight comparisons remain open. The Release fixture build,
identical bounded replay, both existing mission CTests and cleanup pass.

Latest independent-camera depth acceptance (2026-10-08): native projection
returns the final scaled view coefficient to context depth sorting. C2DACC
replaces C29042's earlier origin output; the flight caller's incoming-factor
contract is resolved. All 640 complete component cases and 48 actual camera
bodies match original output and compared RAM/drawing, including both signs
of the factor and preserved-factor sorting. The new serial CTest uses bounded
consecutive captures with per-body timing sidecars. All three selected Debug
and 17 final Release checks pass. No matrix arithmetic,
camera motion, gameplay rule or comparison mask changes. Startup/menu context
callers without projection, remaining mission successes and full-flight
acceptance stay open. See `analysis/native_context_depth_milestone.md` and
`NEW_PORT_HANDOFF.md` for the current executable and validation.

Latest combat/reset acceptance (2026-10-08): the native control-normalisation
and depth-sort owners now publish their actual retained output into host
renderer state for early model expiry. The normal-key mode-five diagnostic
matches all 26 input/stage intervals and all 115 sampled bodies, including
63 consecutive combat-window bodies, seven gun-hit increments, both enemy
expiry-accounting increments and the natural reset. All 64 candidate-reference,
128 depth-sort and 256 distance/factor component cases pass. The comparison is
now a serial CTest gate; masks and gameplay rules are unchanged. The flight
still crashes and completion count remains 3, so mode-five mission success
remains unaccepted. Debug checks and all 14 affected final Release checks pass.
See `analysis/native_mission_five_combat_reset_milestone.md`
and `NEW_PORT_HANDOFF.md` for executable hash, regressions and next work.

Latest mission-list success acceptance (2026-10-08): mode three takes off,
selects an aircraft with normal T input, reaches the original confirmation
objective, lands safely on terrain, taxis into the original runway polygon and
stops. Completion count advances 3 -> 4 and mode grade 1 -> 2; the actual
78-byte config write and cold reload agree. All 36 input/stage intervals and
184 sampled bodies match original compared RAM/drawing, including 96 consecutive
landing bodies. Debug and Release checks pass. The validation pilot only reads
flight RAM and sends keys; gameplay and the playable executable are unchanged.
Passing RAM is temporary and bounded. See
`analysis/native_mission_three_success_milestone.md`. Modes four through eight,
independent complete flights, mission restart, remaining contracts, typed state,
audio and wider performance remain open; the complete-port goal stays active.
This supersedes older claims that all mission-list successes remain unaccepted.

Latest carrier sequence acceptance (2026-10-08): both the original ADF log and
a reopened unqualified saved pilot complete normal-key takeoff, landing, success,
config write, restart and log reload. Each passes 50 input/stage intervals and
174 bodies against original compared RAM/drawing, including all 96 consecutive
landing bodies and the actual C1643A DOS Write. The new pilot earns qualification
0 -> 1. This adds validation and a retained consumed-key fixture; gameplay and
the playable executable are unchanged. Debug/Release builds and six affected
Release CTests pass. Passing RAM remains temporary and bounded. See
`analysis/native_qualification_sequence_milestone.md`. Mission-list successes,
independent complete flights, remaining contracts, typed state, audio and wider
performance remain open; the complete-port goal stays active.

Latest gun shoot-down acceptance (2026-10-08): mode eight passes 317 input/stage
intervals and 275 bodies against original compared RAM/drawing, including all
101 consecutive bodies from first damage through expiry accounting and eventual
inactivation. The connected map clip owner now preserves its actual output for
early model expiry; C4F6DE's former $00C2/$FFFF mismatch is fixed. All 256 focused
clipping residue/drawing cases match. No collision, damage, motion or timer rule
changes. Debug/Release playable and fixture builds and eight affected CTests
pass, including gun/radar/infrared shoot-downs. Gun kill is now a CTest gate.
See `analysis/native_gun_kill_milestone.md` and `NEW_PORT_HANDOFF.md` for source
ownership, hashes and limits. The resolved failure is compressed locally;
passing RAM remains temporary and bounded. Successful complete missions and
independent full-flight comparisons are next. Remaining callback contracts,
typed state, audio and broader performance stay open; the complete-port goal
remains active. This supersedes older open-gun and failed-probe claims below.

Latest acceptance (2026-10-08): normal-input infrared shoot-down now passes
impact, enemy expiry accounting and inactivation. All 634 consecutive bodies
in that interval match original compared RAM/drawing; the full check passes
55 input/stage intervals and 802 bodies. The shared observer checks the selected
missile kind and actual pilot-log counter. Debug/Release builds and four
affected CTests pass, including the radar-kill regression. Passing captures
remain temporary and bounded. This batch changes no gameplay rule or playable
executable. See `analysis/native_infrared_kill_milestone.md`. Gun shoot-downs
and successful complete missions are next; independent complete flights,
remaining contracts, typed state, audio and broader performance stay open.
The complete-port goal stays active; this entry supersedes older open-IR claims.

Restart checkpoint (2026-10-07): `coverage-accounting` has validated code
`f4606f9c`, pushed to origin. The playable runner is
`build/native/fa18_native.exe`, SHA256
`7c6e8a527a834d9cf78fbca2725392c03a41aebdd94214bb134de1fda2ea2c01`.
Read the current restart summary in `NEW_PORT_HANDOFF.md` for recent commits,
validation commands, artifact retention and the next work. Gun
shoot-downs and complete successful missions are next; independent complete
flights, remaining contracts, typed state, audio fidelity and broader
visible-window/combat performance remain open. The complete-port goal stays
active. This checkpoint supersedes older progress and next-work claims below;
dated entries preserve historical evidence. Preserve user-owned `.vscode/`.

Latest normal-input radar shoot-down (2026-10-07): enemy aircraft ten is hit,
starts its 15-tick expiry, advances the enemy-aircraft expiry counter and then
becomes inactive. All 634 consecutive bodies from impact through inactivation
match original compared RAM/drawing, with every body serial required. The full
probe passes 57 input/stage intervals and 802 bodies. Passing RAM is discarded
immediately after comparison; captures remain temporary and bounded. Native
Debug/Release builds and eight affected CTests pass. See
`analysis/native_radar_kill_milestone.md`. Gun/infrared shoot-downs, successful
missions, independent full flights, other contracts, audio, typed state and
wider performance stay open; the complete-port goal remains active.

Latest normal-input radar hit (2026-10-07): mode eight registers a player radar
missile hit; 57 input/stage intervals and 188 bodies, including the exact hit
body, match original compared RAM/drawing. Inactive C22AC0 now preserves its
caller's actual placement calculation instead of returning zero. Thirty-two
focused inactive-record contracts pass. Weapon probes check consumption before
the natural reset and replenishment afterward; mode-eight combat retains its
long-flight guards with a bounded pull-up. These source comparisons run in
CTest with temporary passing RAM. Debug/Release builds and thirteen affected
checks pass across targeted runs. See `analysis/native_radar_hit_milestone.md`.
Kills, successful missions, independent complete flights, other contracts,
audio, typed state and wider performance remain open.

Latest normal-input outcome and stream reset (2026-10-07): mode six loses three
aircraft, exhausts resets, returns to the menu and launches Free Flight using
ordinary keys. Its 86 input/stage intervals and 237 sampled bodies match
original compared RAM/drawing. Mode two's missing C28722 connection is fixed;
all seven streams, their wrap and Escape return pass 30 intervals/206 bodies.
Debug/Release builds and seven affected CTests pass. Both source comparisons
are registered in CTest and discard passing raw RAM. See
`analysis/native_natural_outcomes.md`. Successful missions, independent complete
sequences, remaining contracts, audio, typed state and performance stay open.

Latest connected postflight preservation (2026-10-07): nine childless callbacks
retain actual preceding input results. The expanded comparison passes 355 full
bodies, 372 recorder parents, 84 keyboard parents and 168 separate input/stage
parents against original compared RAM/drawing and defined returns. Three
controlled postflight outcomes also pass nine intervals/25 bodies and saved-log
persistence. Native Debug/Release and eight affected checks pass. See
`analysis/native_postflight_input_return.md`.
Other contracts and full mission/audio/state/performance acceptance remain open.

Latest connected setup preservation (2026-10-07): smoothing publication/restart,
context entry and viewport completion retain actual preceding input outputs.
Stage outputs are checked separately from final message/body results. Sixty
actual input/stage parents, 247 full bodies, 264 recorder parents and 84 keyboard
parents match original compared RAM/drawing and defined returns. Native
Debug/Release and eleven affected checks pass. See `analysis/native_setup_input_return.md`.
Other callback contracts and whole-game mission/audio/state/performance
acceptance remain open.

Latest connected smoothing cancel (2026-10-07): C10A24's previously unavailable
MC_CANCEL child now uses the existing native cancel/reset composition. A
validation-only cancel marker at a naturally reached mode-four stage resumes
flight and returns to the menu. All 44 actual input/stage intervals and 29
sampled bodies match original compared RAM/drawing. Native Debug/Release and
eight affected checks pass.
See `analysis/native_smoothing_cancel.md`. Full outcomes, other stage contracts,
audio, typed state and broader performance acceptance remain open.

Latest measured native demo (2026-10-07): all 10,910 frames call SDL presentation
in a normally paced hidden Direct3D window. Measured frame work peaks at
6.9374 ms, below 20 ms; headless work peaks at 1.6407 ms. Host pacing intervals
still exceed 20 ms on some frames. Timing instrumentation preserves complete
RAM, pixels and counters in ordinary Free Flight. Native Debug/Release and five
affected checks pass. See `analysis/native_frame_performance.md`. Visible display
and full mission/combat performance, outcomes, audio and typed state remain open.

Latest connected idle preservation (2026-10-07): the viewport-message stage
and idle frame retain their actual preceding input result. Fourteen idle
bodies, including twelve followed directly by recorder input, match original
outputs. The expanded suite matches 199 full bodies, 216 recorder parents and
84 keyboard parents against compared RAM/drawing and defined returns; twelve
separate original input/stage parents also pass. Ten affected checks and
native/reference builds pass. See `analysis/native_idle_input_return.md`.
Other stage/reset/HUD contracts and whole-game mission/audio/state/performance
acceptance remain open.

Latest connected context camera outputs (2026-10-07): record-relative and
preset calculations now call the existing matrix/observer owners; context,
map and request actions compose actual outputs into depleted recorder input.
The extended run also restores zoom text and skipped-message decimal-policy
outputs. 187 full bodies match compared RAM/drawing; 204 recorder parents and
84 keyboard parents match RAM and defined returns. Two added idle bodies keep
their inherited output explicitly unresolved. All 12,576 selected command
parents match RAM/returns. Ten affected checks and native/reference builds pass.
See `analysis/native_context_input_return.md`.
Idle/stage/reset contracts and whole-game mission/audio/state/performance
acceptance remain open.

Latest connected indexed outputs (2026-10-07): actual number/index selection,
throttle level/accumulator and recorder-$FD relative change now compose later
depleted input. The extended run also connects source release/model/vertex
paths and corrects negative throttle-request ordering and its recorder gate.
175 full bodies, 192 recorder parents and 72 intervening keyboard parents
match original compared RAM/drawing and defined returns; all 10,688 selected
command parents match RAM/returns. Nine affected checks and native/reference
builds pass. See
`analysis/native_indexed_input_return.md`. Context/reset/HUD contracts and
whole-game mission/audio/state/performance acceptance remain open.

Latest connected fire/countermeasure outputs (2026-10-07): actual fire
selection, saved flare/chaff event and nested eject queue index now compose
subsequent depleted input; modified/depleted gates preserve prior output.
163 full bodies, 180 recorder parents and 60 intervening keyboard parents
match original compared RAM/drawing and defined returns. The radar/weapon
runtime probe now uses the actual flight mode and requires its intended
owner/value, correcting the prior narrower integration claim. All 8,256
selected command parents match RAM/returns; nine affected checks and all
native/reference builds pass. See
`analysis/native_fire_countermeasure_input_return.md`. Indexed/context
contracts and full mission/audio/state/performance acceptance remain open.

Latest connected weapon/radar command outputs (2026-10-07): actual radar
range, masked weapon block and selected weapon mode now compose depleted
recorder input. Weapon $80-$10 follows the original signed-overflow branch,
selecting $30. Target/throttle/hook/ECM actions preserve prior output.
151 full bodies, 168 recorder parents and 48 intervening keyboard parents
match original compared RAM/drawing and returns; all 4,416 selected command
parents match non-stack RAM and defined returns. Nine affected checks and
all native/reference builds pass. See
`analysis/native_weapon_radar_input_return.md`. Other action families and
whole-game acceptance remain open. Gameplay acceptance notes now reflect
the user's permitted native cadence and corrected cold scene cores.

Latest connected view-action return (2026-10-07): detail, origin level,
record type, view mode and masked zoom flags now reach subsequent depleted
recorder input when publication skips. 139 full bodies, 156 recorder parents
and 36 intervening keyboard parents match original compared RAM/drawing and
defined returns. All 2,176 selected command parents match RAM and returns;
other command families remain outside this bounded suite. Nine affected checks
and all native/reference builds pass. See
`analysis/native_view_action_input_return.md`. Full port remains active.

Latest connected control-action return (2026-10-07): stick/rudder/throttle,
information-page and sign-input actions preserve actual prior/selection output;
HUD toggle and gear-gate assignments publish their existing domain values.
127 full bodies, 144 recorder parents and 24 intervening keyboard parents
match original compared RAM/drawing and defined returns. 832 focused command
parents match RAM; 826 defined returns pass and six stay unresolved. Nine
affected checks and all native/reference builds pass. See
`analysis/native_control_action_input_return.md`. Full port remains active.

Latest connected intervening-command return (2026-10-07): wait/modifier/empty
and queue-only exits preserve prior output or the actual selector block byte;
accepted publication supplies its signed translated index. 115 complete bodies,
132 recorder input parents and twelve intervening keyboard parents match the
original compared RAM/drawing and defined returns. 512 focused command parents
match RAM; 471 defined returns pass, with 41 action-owned returns still unresolved.
Nine affected checks and all native/reference builds pass.
See `analysis/native_intervening_command_input_return.md`. Full port stays active.

Latest connected lost-target cleanup return (2026-10-07): selection cleanup now
preserves its incoming output on skip/context exits and publishes the actual
view mode or signed translated-queue index on the view-key path. 103 complete
bodies and 120 recorder input parents match original compared RAM/drawing,
including twelve actual cleanup frames after ordinary Free Flight startup.
512 focused cleanup return/non-stack RAM cases and all native/reference builds pass.
Nine affected native checks pass.
See `analysis/native_selection_cleanup_input_return.md`. Full port remains active.

Latest connected grid/aircraft-marker return (2026-10-07): point depths,
heading/shape selections, clipped-segment heights, line results and number-text
outputs now reach first depleted recorder flare/chaff input in source order.
Inactive grids preserve the preceding result. 91 complete bodies and 108 input
parents match original compared RAM/drawing, including twelve selected grid
frames in ordinary Free Flight. Two snapshots each pass 401 complete grid/marker
returns and 128 clipped-segment returns with non-stack RAM. Eleven affected
checks and all native/reference builds pass. See
`analysis/native_grid_marker_input_return.md`. Full port remains active.

Latest connected HUD marker return (2026-10-07): the actual line renderer
publishes its blit size, or the signed X delta computed before a lower-start
last-row rejection. Other rejected starts preserve the preceding output.
Both HUD marker callers compose this result through existing bar/text ordering.
79 complete bodies and 96 recorder input parents match original compared RAM
and drawing, including twelve new marker frames from a fresh Free Flight.
Two snapshots each pass 128 line-return/non-stack RAM cases; focused HUD
checks pass 492 RAM cases and 252 defined returns. Ten affected checks and all
native/reference builds pass. The raster checker is now registered in CTest
with its current source-backed Free Flight stage expectation. See
`analysis/native_hud_marker_input_return.md`. Full port remains active.

Latest small-text fault return (2026-10-07): odd destinations retain the
selected glyph after setting error $46, as the original release C06C02 RTS
does. The existing drawing skip is unchanged. Focused HUD comparisons match
480 non-stack RAM cases and 237 defined returns, including 27 actual odd
fault returns across cockpit/context views. Six affected checks and all
native/reference builds pass. See `analysis/native_small_text_fault_return.md`.
Full port remains active.

Latest connected scene-label return (2026-10-07): row-Z loads, matrix products
and position-number text results now reach first depleted recorder flare/chaff
commands. Terminator loads and alternate skipped-row exits follow the original
source. Skipped labels preserve the preceding result, including the newly
connected context speed/altitude/heading HUD readouts. The suite matches 67
complete bodies and 84 recorder input parents, with twelve new label frames.
256 label returns/non-stack RAM cases and 450 HUD RAM cases with 207 defined
returns pass. Ten affected checks and native/reference builds pass. See
`analysis/native_scene_label_input_return.md`. Full port remains active.

Latest connected debug return (2026-10-07): the actual four-plane text owner
now returns its final character or glyph selection through the numeric debug
overlay to the next first depleted recorder command. Inactive overlays preserve
the preceding domain result. The suite matches 55 complete flight bodies and
72 recorder input parents, including twelve one-/three-field debug frames.
256 gated overlay returns match the original, including clipped and odd-window
characters. Ten affected checks pass; Release/Debug and both reference runners
build. Qualification fixtures now match the current native boundary signatures.
See `analysis/native_debug_text_input_return.md`. Full port remains active.

Latest connected HUD return (2026-10-07): bar destinations, small-text
character/glyph results and periodic cockpit redraw pass counts now reach the
next first depleted recorder flare/chaff command. The real HUD owners compose
these results in source order; selected cleanup/grid/label/debug work with
unfinished contracts invalidates them. The complete suite matches 43 flight
bodies and 60 recorder input parents, including twelve new HUD/redraw cases.
Focused HUD checks match 405 non-stack RAM cases and 117 defined returns,
including clipping and skipped redraws. Nine affected native checks pass;
Release/Debug and both reference MSVC runners build. See
`analysis/native_hud_input_returns.md`. Full port remains active.

Latest connected drawing return (2026-10-07): C2F582 now publishes its actual
page-clear pattern for the next first depleted pending flare/chaff command.
Twelve clears reached by the normal game counter and twelve following input
parents match independently derived original returns. The complete suite
matches 31 flight bodies and 48 recorder input parents, including all drawing
bytes. Message no-assignment exits preserve a known clear result; selected
labels/debug overlays invalidate it. Other drawing and intervening-command
returns remain UNKNOWN and fail if consumed. See
`analysis/native_page_clear_input_return.md`. Full port remains active.

Latest connected recorder return (2026-10-07): the completed C32CEE message
renderer now publishes its defined character/glyph byte for the first depleted
pending flare/chaff command with an unclaimed input queue. Twelve actual input
parents consume independently verified original frame returns; 19 complete
bodies and 36 recorder input parents match compared RAM/drawing. Two inactive
message exits leave no message assignment. The real message owner also passes 8,192 complete
CPU/SR/RAM component calls. Other drawing-derived and intervening-command returns
still require their source owners and fail explicitly if consumed. See
`analysis/native_message_input_carry_milestone.md`. Full port remains active.

Latest user-reported controls fix (2026-10-07): comma/period rudder and SDL
numeric keypad views now reach the native input queue. The former native mapper
used a typed-text table that omits control keys and did not accept SDL keypad
identities. Native and reference runners now share the unchanged physical Amiga
host mapper in `port/amiga/host_keys.c`. The SDL event boundary is shared by
the playable entry and a normal Free Flight integration test. Twenty-six real
SDL-queue input/stage intervals and 52 sampled bodies match original compared
RAM/display. Both rudder directions move the source control axis and releases
clear the input; external/cockpit view changes work with either Num Lock flag.
Passing captures are temporary. See `analysis/native_host_keys_milestone.md`.
The complete-port goal remains active.

Latest connected input fix (2026-10-07): depleted recorder flare/chaff commands
now complete when C1C23C's input queue is already claimed. Its gate makes the
inherited event dead while stock/message and modifier clearing still execute.
2,512 complete source input parents pass, including 1,536 new claimed-recorder
cases. After ordinary Free Flight startup, 24 controlled input parents using
the playable runtime match original RAM, including twelve successful-flare ->
depleted-chaff sequences. Four keyboard-driven flight bodies still match
gameplay/display. Comparisons discard passing RAM. First depleted commands
outside the defined message/page-clear returns still need their carry producer and fail loudly;
full gameplay acceptance remains unfinished. See
`analysis/native_countermeasures_milestone.md`.
The consumed-game-input gate now also requires the complete 164-byte player
core; stale assertions expecting the corrected flag/countdown gap are removed.
Original export stability and native consumed-input checkpoint comparisons pass.

Latest user direction (2026-10-07): stop workspace artifact growth. Cleanup
reduces 60.501 GiB to 3.719 GiB, reclaiming 56.782 GiB; active build output is
1.329 GiB. Source, media, recordings and Visual Studio setup are preserved.
Builds/CTest now prune disposable output against a 4 GiB budget; comparison
RAM is temporary by default, with only failed cases retained. Manual native
captures default to 512 MiB. Original comparison windows are hash-verified
`.dat.gz`; reports/logs survive. See `analysis/workspace_artifact_retention.md`.

Latest connected startup fix: native cold entry now calls original C08EE4/
C08EB8 defaults and saved-level owners before C08F26 bootstrap. This removes
the type-zero root construction that left a region flag and sixteen-update
countdown lead. All 16 initial cores and 9,376 complete cores across 586
independent demo boundaries match. Takeoff drawing still passes 223/223;
later player/camera/core state passes 363/363, drawing 83/363. The remaining
seconds-driven drawing assessment is open. 32 original cold parents, disk-backed
demo checks, eight affected runtime gates, Free Flight input/body and controlled
postflight comparisons pass. See `analysis/native_scene_startup_milestone.md`
and `analysis/native_later_demo_assessment.md`. Full goal remains unfinished.

Latest connected batch: the reported demo outside-view clipping is corrected.
C1F158/C1F2EE consumes the original model vertex; native composition had
rotated it twice. All 223 independently aligned takeoff boundaries now match
both drawing pages, player pose/motion and camera state. Ten assembled-body
cases pass, including three new attached-camera regressions. Extended mission
sampling also corrects C2E048's coarse angle clamp before word narrowing:
modes five through eight match 232 input/stage intervals and 726 sampled
bodies, including active postflight dynamics. All 576 complete matrix cases,
the failing record checkpoint and eleven selected runtime gates pass. See
`analysis/native_outside_camera_milestone.md`. Complete gameplay/audio/state/
performance acceptance remains unfinished.

Latest connected batch: sustained native mode-six/eight input now resumes
the existing guidance owner after C2C348 -> C06C02. Extended comparisons also
correct target-marker returned coordinates, signed attitude longs, the matrix
extractor's retained small divisor, and settling from previous roll. Normal
keyboard probes match 114 input/stage intervals and 465 sampled bodies against
compared original RAM/display, including three original guidance-child returns
in mode eight. No new exclusions were added. Component checks pass 512 point
returns, 128 signed-attitude parents, 512 matrix transforms, 256 settling tails
and 3,584 affected projection CPU contracts. All 36 selected native gates,
seven record checkpoints, mode-four region flight (56 intervals/49 bodies),
default models and native/reference builds pass. The public executable is
refreshed; final frontend/link checks pass. See
`analysis/native_guidance_limits_milestone.md`;
complete gameplay/audio/performance acceptance remains unfinished.

Latest connected batch: Delete now maps to source raw $46 and runs C06BF0's
callback removal/reinstallation via existing C1748C/C17456 domain owners.
Flight/menu callers preserve mouse state; 976 full input parents (64 new reset
variants) plus 16 menu command parents match original non-stack RAM and callback
state. Normal Free Flight Delete/Escape/restart matches 57 input/stage intervals
and 37 sampled bodies with unchanged exclusions. The eligible-pilot fixture
now enters enlistment before saving, checks readiness and verifies reload. A
fresh mode-eight failure showed cached eligibility files concealed that fixture
mistake in the previous batch. Fresh mode-seven/eight, ejection and all three
weapon checks match 264 intervals and 211 bodies. Full native build and six
selected native gates pass. See `analysis/native_callback_reset_milestone.md`;
complete gameplay/audio/performance acceptance remains unfinished.

Latest connected batch: native configuration writes now execute source C1643A
status/readiness policy, and enlistment refresh executes C162E4/C0EF08/C16386/
C1631C through existing Amiga host file services. The validated OFS mount stays
read-only; the overlay retains MODE_OLDFILE write behavior. Native startup now
sets source file readiness/load status. Nine input/stage intervals and 25 bodies
match original RAM/display with complete game file owners executing and
unchanged masks; the old C1643A comparison bypass is removed. 77 result/config
parents match RAM and persisted bytes; 2,560 CPU/child-entry contract cases pass.
Normal mode-four flight still matches 56 intervals/47 bodies; recorded carrier
qualification completes, saves, restarts and reloads. All 28 selected native
tests, 12 host/loading checks, default models and reference builds pass. The
qualification model probe now keeps carrier comparisons while limiting its
aircraft expiry fixtures to appropriate records; five aircraft cases pass on a
real demo record. See `analysis/native_config_owner_milestone.md`. Full mission,
independent sequence, audio and performance acceptance remain open.

Latest connected batch: C0A3EA readiness, mode-four C1BEE8 result view,
mode-five C0A12E record restores, reached C083A6 control request and
C110A4/C24FA4/C11350/C08ED0 result messages/log update. Controlled fixtures
through the actual shared runtime pass nine input/stage intervals and 25
sampled bodies with zero compared original RAM/display differences and unchanged
RAM/display masks. 63 result-parent cases pass with the existing C1643A shared save
boundary; its disk/status/readiness decisions remain unverified. Despite older
helper names, C539F4 is DOS Write, not Read. Normal mode-four flight still
passes 56 intervals/47 bodies. All 26 affected native tests, seven native-only
host/loading checks and both reference builds pass. The validated public native
executable is updated. See `analysis/native_postflight_schedule_milestone.md`;
run `python tools/native/check_postflight_schedule.py`. Normal-input mission
success, full gameplay/audio/performance acceptance remain open.

Latest connected batch: mode-4 throttle/stick flight runs region placement and
orientation, zone exits, draw $B4/C21EF8, NPC missile launches and the player's
hit/restart sequence (5,970 scene/HUD frames). The launch gate now tests the
owning aircraft; the carrier renderer retains its completion flag, and the
record sight tail retains its caller's viewer. 56 actual input/stage intervals
and 47 sampled bodies match compared original RAM/display with zero differences
and unchanged exclusions. Derived geometry passes 320 complete parents. The
shared runtime adds `fa18_native_region_flight`; default setup models, reference
runner builds, all 22 affected native tests and 12 host/loading checks pass.
Full flight sequences, other
combat/outcomes, audio and performance remain unaccepted. See
`analysis/native_region_flight_milestone.md`; use `check_mode_two.py --mode 4 --flight`.

Latest connected batch: normal mode-8 Return/Space firing exercises source
weapon modes $30/$20/$10 (two missile stocks and gun ammunition). Draw command
$98/C21E08 now has its readable workspace geometry. The gun comparison exposed
and corrected C2CCA0's use of the point child's returned plane destination for
its subsequent test/clear. 138 actual input/stage intervals and 112 sampled
bodies match compared original RAM/display with zero differences and unchanged
exclusions. Geometry/point components pass 256/64 complete parents. Actual
runner probes reach 2,458 scene/HUD frames. All 21 affected native tests pass.
Default setup models, countermeasure
parents/four bodies, reference build and 12 host/loading checks pass. Successful
hits, complete combat/outcomes and whole-game acceptance remain unfinished.
See `analysis/native_weapon_firing_milestone.md`.

Latest connected batch: normal mode-8 Shift-E runs through the source ejection
flag/command publication, programmed sound, action record clone/orientation,
lifetime-selected model and failure/menu return (739 scene/HUD frames).
46 actual input/stage intervals and 43 sampled bodies match compared original
RAM/display with zero differences and unchanged exclusions. C1C214, C23186,
C236AA's reached view/matrix children, C22B1A and draw commands $E0/$100 are
connected. 64 additional complete input parents and 128 additional derived
geometry parents pass, including wrapping/overlapping banks. This is the
demonstrated ejection/failure route. All 18 affected native integration tests,
three default setup-model comparisons, reference build and 12 host/loading
checks pass. Other mission outcomes, combat and whole
game acceptance remain open. See `analysis/native_ejection_milestone.md`.

Latest connected batch: mission-list F6 runs source mode 8 with an eligible
saved pilot (2,458 scene/HUD frames). The disk's locked pilot still rejects
F6. The fixture changes only availability byte $19 and reloads through the
normal loader. Native record composition calls readable C0A364; C0FECE
preserves saved A4=C29702's nonzero sort choice. 38 actual input/stage intervals
and 27 sampled bodies match compared original RAM/display with zero differences
and no new exclusions. Eleven affected native integration tests pass, including
current frontend/save/reload and locked/eligible F6 menu checks.
Complete mission outcomes, combat, typed-state migration,
whole gameplay sequence acceptance, audio fidelity and performance remain open.
See `analysis/native_mode_eight_milestone.md`.

Latest connected batch: mission-list F5 runs source mode 7 with an eligible
saved pilot (2,289 scene/HUD frames). The disk's locked pilot still cannot
select F5. 42 actual input/stage intervals and 29 sampled bodies, including
C11078/C110A4, match compared original RAM/display. C0A1E0's scheduler and
C1BEE8 record-view publication are connected; command $FC/C21FA4 derives its
five parallelogram completions. Complete publication/geometry checks pass
256/64 cases. C110A4 now uses typed host locals, removing its scratch-RAM
residue and the older result oracle's exclusion. 39 result/restart parents,
fifteen affected regressions and reference build/host checks pass. Standalone
mode-7 placement-driver snapshots fail at command $C3 in the controlled repeated
expiry fixture; original execution also fails there. That comparison remains
unaccepted. Complete outcomes/combat and whole-game acceptance remain open.
See `analysis/native_mode_seven_milestone.md`.

Latest connected batch: mission-list F3 now runs normal source mode 5 through
briefing/context setup into sustained flight (2,312 scene/HUD frames).
39 actual input/stage intervals and 27 sampled bodies match compared original
RAM/display. Native record composition calls C0A002's readable scheduler;
the transition preserves saved A4=C2968A's sort choice. Seven affected native
integration tests and current frontend checks pass. Complete mode-5 outcomes,
combat and whole-game acceptance remain open. See
`analysis/native_mode_five_milestone.md`.

Latest connected batch: mission-list F2 now runs normal source mode 4 through
briefing/context setup into sustained flight (2,507 scene/HUD frames).
39 actual input/stage intervals and 27 sampled bodies match compared original
RAM/display. Native record composition calls C09EC4's readable scheduler;
the transition preserves saved A4=C296E4's sort choice. Thirteen affected native
integration tests and current frontend checks pass. Complete mode-4 outcomes,
combat and whole-game acceptance remain open. See
`analysis/native_mode_four_milestone.md`.

Latest connected batch: mission-list F1 now runs normal source mode 3 through
aircraft choice and flight for both choices (969 scene/HUD frames). Each
choice passes 43 actual input/stage intervals and 29 sampled original frame
bodies. Draw commands $118/C1FE68 and $11C/C0CFB6 are connected and pass
65,588 selector cases / 24 complete circle-stream parents. Normal mode 3's
transition preserves its saved-pointer sort choice. Ten affected native
integration tests, current frontend and active/crash/map checks, reference
build and twelve host/loading checks pass. Full mission outcomes, combat and
whole-game acceptance remain open. See `analysis/native_mode_three_milestone.md`.

Latest connected batch: next-mission digit 7 now runs source mode 6 through
briefing/context/aircraft setup and sustained flight (401 scene/HUD frames).
38 actual input/stage intervals and 27 sampled bodies match compared original
RAM/display. Source message lookup, origin selection, setup engine/noise and
C0A15C's scheduler are composed into the native runner. The transition retains
its saved-pointer sort choice. Modes 2/125 source regressions, eight affected
native integration tests and frontend checks pass. Full mode-6 outcomes and
whole-game acceptance remain open. See `analysis/native_mode_six_milestone.md`.

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
