# Native port handoff — 2026-10-09

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

Independent whole demo recording: first flight matches 2,046 boundaries, all
32,736 cores and camera/control state. Twelve callback runs through automatic
restart preserve all2,062 distinct gameplay states. Strict drawing remains
1,235/2,046 and full fixed-offset failures remain reported. See
`analysis/native_independent_demo_trace_milestone.md`. All-mission independent
flights and remaining drawing assessment remain open.

Reported target-info timing is now assessed: all 374 transitions in the actual
mode-three recording's selected-target episode obey source elapsed/countdown,
context/redraw and full-text rules. Both first HDG events follow two sampled-second
changes; seven initial page transitions agree at equivalent countdown events. Release/Debug
full native trace hashes and assessments agree; eight selected CTests and four
wrong-result mutations pass. V2 corrects the mouse field mislabeled "selected
record" in V1 and adds the true selected-record word and HUD gates. All old
common trace bytes, counters and final RAM remain unchanged. Strict first-flight
drawing remains 1,235/2,046; other drawing, all-mission whole-flight, original
audio/filter, visible performance and state cleanup work remains open. See
`analysis/native_demo_hud_timing_milestone.md`.

Latest control direction (2026-10-09): keep original plain-Escape restart
behaviour. The user confirmed that automatic Free Flight reentry is expected
and requested dropping the proposed change. The uncommitted SDL Escape-to-menu
override and its tests were removed. C0F992 preserves the selected mode;
modified Escape uses the existing abandonment/reset path. The earlier
menu-return check used Shift+Escape. This is no longer an outstanding fix.
Release/Debug and canonical Release are rebuilt with the original controls;
host-key/frontend checks and cleanup pass in both configurations. Original
input parity passes all 2,512 pending cases, 12,576 command parents/returns and
16 menu Delete cases with a diagnostic 120-second allowance. The unchanged
CTest's 15-second oracle limit timed out twice; its test source was preserved.

## Current restart summary — 2026-10-09

Latest acceptance direction (2026-10-09): the user waived the uninterrupted
qualification-plus-six-mission requirement. Repeated individual mission tests
with earned saves, objectives, landing, results and menu returns now qualify
that part of completion; cold starts are allowed. The continuous-campaign
driver and accepted five-mission prefix remain diagnostic evidence. Its final
flight failure is no longer a completion blocker. This overrides every older
uninterrupted-run requirement below. The latest failed bank/height input
experiments were reverted. Independent original whole-flight comparisons,
original recorded sound/filter fidelity, visible performance
and named-state cleanup remain outstanding.

Initial startup and qualification-entry sorting are now verified. A read-only
startup observer captures the actual C08F26 call; the oracle executes C0F812's
real LINK -14 frame, then the complete bootstrap. All three lists and retained
output agree. Release/Debug match every record and RAM hash across 22 combined
intervals, 60 sorted lists and 40 incoming-factor probes. Initial startup alone
adds 10 intervals/30 lists/20 probes from five distinct real before-states.
No startup sort discrepancy or game-behavior change was found; normal frontend,
demo and artifact regressions pass in both builds. Canonical Release is refreshed.
See `analysis/native_initial_startup_sort_milestone.md` and
`analysis/figures/native_initial_startup_sort_checkpoint.json`. Older initial
startup TODO notes are superseded for these requested callers.

Repeated individual mission acceptance now passes on the current executables:
Release and Debug each earn qualification plus all six cold missions, save the
sixth result, return to menu and wrap. All nine stages match every counter/input
hash and all 78 saved bytes. Fresh Release qualification/final source gates pass
for original and newly enlisted pilot contexts; ten selected CTests pass overall.
See `analysis/native_individual_mission_acceptance.md` and
`analysis/figures/native_individual_mission_acceptance_checkpoint.json`.
The uninterrupted driver is optional diagnostic tooling. Visible-performance
qualification must be adapted to validated individual geared mission routes.

Free Flight location-three renderer crash fixed: model C3AAC4's command 4018
now dispatches the original C206E4 interpolated-segment routine. The model oracle
compares 72 complete command cases and 16 actual-model poses (eight source calls),
matching RAM, drawing, stream advancement and results. The user flew through
the affected area again and reported no crash. An automated 50,000-tick
location-three smoke also passes, but its straight flight did not reproduce the
original crash. Model error output now includes player X/Y/Z (stored coordinates
/256), pose index, mode and stage. See
`analysis/native_free_flight_interpolation_fix.md`.
Renderer, Free Flight startup and cleanup checks pass in Release and Debug;
both executables and canonical Release are refreshed, including diagnostics.

Latest title/music direction: wait on the title until any key, then duck music
in the pilot/menu flow; no music in gameplay. Startup now starts the original
disk music at master 63; the key targets 31. Native empty-voice service discards
current/next PCM buffers before a replacement voice starts, removing slow music
carried into flight. Music restarts on menu return. The timed title-to-credits
transition is removed per the user's explicit request. Four connected music
checks cover Demonstration, Free Flight, qualification and a mission; the
frontend check holds the title for 9,000 ticks without input. All ten selected
checks pass in Release and Debug, including original sample/voice oracles,
frontend, menu start and PCM ownership; active mission flight is also checked.
Canonical Release is refreshed. Complete original
recorded-audio/filter acceptance remains open.

Final-campaign diagnosis: an actual earned five-mission cold suffix first sets
the player's destroyed bit at tick 35318 with component damage still zero.
Five complete frame bodies around the hit match original instructions, including
the C2613A flag write and drawing. The subsequent thrust/fuel freeze follows
original C13D84's destruction branch. See
`analysis/figures/native_final_mission_destroyed_body_comparison.json`.
This is sampled, native-seeded source evidence; complete independent original
flights and the uninterrupted sixth mission remain open. Three unsuccessful
input-only defensive/radar experiments were reverted. Only read-only diagnostic
telemetry is retained; playable runtime and accepted gear-managed inputs remain
unchanged. Details are in `analysis/native_continuous_campaign_investigation.md`.

Native message 71 now displays `SECURITY CLEARANCE GRANTED`, as requested.
Release/Debug builds and the frontend check pass; the complete phrase is visible
at qualification-entry tick 3420. The fixed-width field and descriptor are
preserved; this wording intentionally differs from original recordings.

Qualification and five geared missions now pass in one actual Release playable
process. The final menu is C0FCB4/mode zero at tick 146862; all 78 earned saved
bytes match the observing driver, with no crash reset or pending input.
See `analysis/figures/native_geared_five_mission_prefix_checkpoint.json`.
Final combat remains unsuccessful. Five-mission Debug replay remains open;
the preceding four-mission prefix matches all counters and saved bytes in Debug.
The complete-port goal remains active.

Active completion work: the one-frontend campaign driver in
`tools/native/native_campaign_session_test.c` and checker
`tools/native/check_campaign_session.py` earn qualification, mission three
and mission four with real saved results and menu mode zero between missions.
Mode five remains unsuccessful; its opponents can land while active, and its
original failure phase F0 eventually returns to menu without a saved completion.
Latest experiment uses the already accepted legacy mode-five pilot instead of
the new-pilot controller; report directory is
`build/native-flight/campaign-session-legacy-five/`. No uninterrupted six-mission
pass or complete playable replay is accepted. The experimental executable target
is available; no failing campaign gate is registered in default CTest yet.

Later steering/evidence: the legacy mode-five controller completes the first
five missions in one frontend (`campaign-session-legacy-five/`): modes five,
six and seven save at ticks 100581, 123474 and 143480. Final combat crashes
before any aircraft expiry. The user identified gear-down flight and directed
gear up after takeoff, down before landing (also manually toggled gear in the
visible mission-three run). The campaign pilot now sends original G commands
and requires observed retracted gear in flight and lowered gear at landing.
Qualification's sealed input already has the two original gear events.
First geared attempt completes mission three, but misses the escort carrier
approach. Current rerun lowers gear at return start to slow before approach:
`build/native-flight/campaign-session-gear-return/`. The full goal remains open.

That return-stage lowering passes escort, but the legacy geared mode-five input
crashes on its attack. Current attempt uses the campaign's safer combat input,
ordinary gear management. Ground kills cannot satisfy mode five's original
airborne-expiry count. The radar interception attempt reaches both airborne
kills and the objective; its return stalls because the pilot does not reapply
controls after the source clears them for the result camera. Current attempt
addresses this handoff: `build/native-flight/campaign-session-gear-return-handoff/`.
That handoff passes mode five and rescue, saved/menu ticks 103321 and 125454.
Qualification and four geared missions pass in one frontend. The cruise
interception misses with fuel available. Latest attempt tightens radar launch
alignment: `build/native-flight/campaign-session-gear-cruise-alignment/`.
Full playable replay remains pending.

The prefix through rescue now also passes in one actual Release `fa18_native`
process. Qualification and four geared missions produce exactly the driver's
78 saved bytes, menu C0FCB4/mode zero at tick 125454, with no crash reset or
pending input. See `analysis/figures/native_geared_campaign_prefix_checkpoint.json`.
Debug now replays the same prefix with identical complete counters and saved
bytes. This is partial playable integration, not six-mission or independent-original
acceptance. The latest cruise gun approach earns the original objective at
tick 138725, then crashes during the low-altitude return handoff; no saved
cruise completion is claimed. Diagnostic targets build in Release/Debug and
the existing mission-five success comparison passes in both. No test is
currently running. See `campaign-session-gear-cruise-close-gun/` for that failure.
No visible measurement process is running after the interrupted old route.

The cold geared cruise diagnostic now earns interception, carrier wire,
save and menu (`build/native-flight/campaign-suffix-missile-retry/`). Its initial
save is the actual four-mission result. Final combat then crashes without an
aircraft expiry. This cold suffix cannot qualify the uninterrupted campaign;
the full Release rerun is `campaign-session-gear-missile-retry/`. That run now
earns five missions before the final combat crash, and the five-mission prefix
has also passed in the actual playable runner.

Visible measurement was run via `tools/native/measure_visible_performance.py`
under `build/native-flight/visible-performance-20261009/`: demo and six actual
earned cold mission routes, real window/audio and one CSV row per presentation.
The completed demo presents all 10,910 frames, work p99 4.0922 ms, maximum
544.8323 ms. Its worst frame (2322, C10CFE) spends 543.2488 ms in SDL input
polling. The strict 20 ms maximum therefore fails; no frame or stall is excluded.
Mission three later finishes with a crash/reset after live manual gear input,
so the old fixed-key suite stops and has no accepted mission result. Generate
new gear-managed routes before resuming visible mission measurements. SDL
returns are counted; compositor
scanout is not instrumented. See
`analysis/native_continuous_campaign_investigation.md` for continuation details.

Actual Demonstration entry now sorts all three display lists, matching the
original caller. The preceding test's labelled Demonstration case selected
qualification instead; mode assertions now prevent that mistake. The actual
demo revealed 22 RAM differences despite equal retained sort words. Original
saved A4 `$C47584` explains the caller's all-list decision; the native C0FECE
path now preserves it. Qualification entry also passes. Ten scenarios compare
12 intervals, 30 sorted lists and 20 factor probes in Release and Debug, with
identical interval/RAM hashes and a rejected wrong-output negative control.
Frontend/demo regressions and artifact checks pass; the Release executable is
refreshed. Initial startup remains unverified. See
`analysis/native_menu_sort_mode_audit_milestone.md` and its checkpoint.

Full independently started original flights remain the main playable-parity
check. Repeated individual mission tests replace the uninterrupted campaign
requirement by the latest explicit user direction. Initial startup sorting is
verified. Audio recording fidelity, visible performance and named-state migration remain
open. Historical summaries below are superseded
where they label qualification as Demonstration or describe all binaries as
unchanged. The complete-port goal remains active.

## Current restart summary — 2026-10-08

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

Final mission availability is now earned by ordinary rescue and cruise flights
starting from the original ADF pilot. The active final success gate loads their
actual saved log; the original byte-25 grade guard admits F6/internal mode eight.
Four aircraft expiries then admit success, followed by carrier landing, save,
messages/menu and cold Next Mission wrap. Completions advance 5 -> 6 and grade
0 -> 1. All 42 flight intervals/239 bodies plus one wrap interval/body match
original compared RAM/drawing, with matching playable saved bytes/menu/wrap.
The cruise gate reproduces exact retained input and all 78 prerequisite bytes.
Gameplay and masks are unchanged; the older patrol diagnostic still uses its
explicitly unearned reference log. See
`analysis/native_final_mission_earned_availability_milestone.md` and checkpoint.
No visible submarine explosion is required on this original counter path.
Earlier ADF pilot progress remains: a newly enlisted complete tour, independent
original whole flights and full-port acceptance are still open. Capture and
pruning limits remain unchanged; the complete-port goal stays active.
Six selected Release checks and four Debug checks pass. Final input/save hashes,
objective/landing/menu and cold wrap agree across configurations. The Release
playable remains unchanged and pruned build use is 2.00 GiB. Evidence is in
`analysis/figures/native_final_mission_earned_availability_checkpoint.json`.

Intercept Incoming Cruise Missile (F5/internal mode seven) now completes through
ordinary keys: a radar missile intercepts record four, the original FF objective
is admitted, and the player returns to the carrier, catches the wire, stops,
saves, finishes messages and presses Escape to reach the menu. Completions
advance 4 -> 5 and grade 0 -> 1. All 42 input/stage intervals and 171 bodies
match original compared RAM/drawing, including 20 interception/expiry bodies,
80 consecutive landing/result bodies and one config write. The playable runner
delivers all 1,533 host events and agrees on the saved bytes/menu; all 78 bytes
survive cold reload. Starting availability is earned by the accepted normal-key
rescue save from the original ADF pilot. Its existing gate now reproduces exact
retained rescue inputs and log bytes. This does not prove a complete tour from
a newly enlisted pilot. Gameplay and comparison masks are unchanged; no new
decompilation is claimed. The serial gate is `fa18_native_mission_7_sequence`;
see `analysis/native_cruise_mission_sequence_milestone.md` and its checkpoint.
All six menu missions now have successful native routes. Complete-tour
acceptance, independent whole flights, remaining caller contracts, typed state,
audio and wider performance remain open. Captures use 213 pairs within the
unchanged 240-pair / 480 MiB cap and passing RAM remains temporary.
Debug/Release fixture builds pass. Nine selected Release checks and three
Debug checks pass, with matching cruise input/save hashes and outcome/landing/
menu evidence. The playable Release executable is unchanged, its frontend/link
gate passes, and build-cache use remains 2.00 GiB after pruning.

Search and Rescue (F4/internal mode six) now completes through ordinary keys:
Shift+F deploys a pod, the original near-site test admits success, and the
player returns to the carrier, catches the wire, stops, saves, finishes messages
and presses Escape to return to the menu. All 44 input/stage intervals and 193
sampled bodies match original compared RAM/drawing, including both 20-body
rescue windows, 80 consecutive landing/result bodies and one config write.
Completions advance 3 -> 4 and grade 0 -> 1. The playable runner consumes the
same 870 host events and agrees on the saved bytes/menu; cold reload preserves
all 78 bytes. Debug/Release rescue gates pass with matching input, save and
sequence evidence. Gameplay and comparison masks are unchanged; no new
decompilation is claimed. The initial pilot is the unmodified original ADF log,
not a port-earned complete tour. Captures use 237 pairs inside the unchanged
240-pair / 480 MiB cap; passing RAM is temporary. See
`analysis/native_rescue_mission_sequence_milestone.md` and its checkpoint;
the serial gate is `fa18_native_mission_6_sequence`. Internal mode seven is
accepted above; earned tour availability and independent whole flights remain
open.
Eight selected Release checks and three Debug checks pass, including all five
accepted mission sequences in Release, the frontend/link gate and artifact
policy/cleanup. The playable Release executable is unchanged; build-cache use
remains 2.00 GiB after pruning.

Final Carrier Sub mission (F6/internal mode 8) now completes through ordinary
keys: four aircraft expiries satisfy this fixture's quota four, the FF objective
is admitted, and the player returns to the carrier wire, stops, saves and
finishes result messages. Completions advance 3 -> 4 and grade 0 -> 1. Escape
returns to the menu; a cold Next Mission selection wraps from saved mode eight
to mode three. Class-20 surface record 14 remains active at objective admission. All 42
flight input/stage intervals and 239 bodies plus one cold-wrap interval/body
match original compared RAM/drawing, including 101 combat bodies, 64 landing
bodies and one config write. The playable runner delivers all 2,626 flight
events and four wrap events and agrees on the saved log/menu/wrap.
The connected scheduler now preserves C230B0's returned selection word through
C0A364/C0A3A6, publishing original FF04 instead of 0004 at completion. Original
predicates, countdowns and comparison masks are unchanged. No newly decompiled
routine is claimed. The serial gate is `fa18_native_mission_8_sequence`; see
`analysis/native_final_mission_sequence_milestone.md` and its checkpoint.
This historical availability-only saved pilot was explicitly unearned; flight
state was never seeded. The active gate now uses earned final availability as
recorded above; independent complete original flights remain open. Two identical
flights
partition captures inside the existing 480 MiB cap, removing passing RAM before
the next partition; the 4 GiB build-cache pruning hooks stay active. The earlier
`fa18_native_final_patrol_diagnostic` remains an accepted three-aircraft route.
Debug/Release playable and fixture builds pass. Eight selected Release checks
and three Debug checks pass. Inputs, counters, saved bytes and cold wrap agree
across configurations. The canonical Release executable is refreshed, passing
RAM is removed and build-cache use remains 2.00 GiB within its 4 GiB budget.
Evidence is in `analysis/figures/native_final_mission_sequence_checkpoint.json`.

Mission four now completes a normal-key escort flight and saved result sequence
from the source-earned region pilot log. Record 8 is shot down, the escort lands,
the original objective is admitted at tick 17,346, and the player returns to the
carrier. Wire touchdown occurs at 26,834, the stopped-aircraft result at 27,000,
save at 27,023 and Escape/menu return at 28,037. Completions advance 1 -> 2 and
grade 0 -> 1; the saved log survives cold reload unchanged. All 43 input/stage
intervals and 159 sampled bodies match original compared RAM/drawing, including
20 hit/expiry bodies, 64 consecutive landing bodies and one actual config write.
The playable runner delivers all 3,883 host replay events and produces the same
saved bytes/menu. Host replay delivery and pending counts now appear separately
from game-input replay counters in JSON. The new serial CTest is
`fa18_native_mission_4_sequence`. See
`analysis/native_mission_four_sequence_milestone.md` and its checkpoint JSON.
Gameplay and comparison masks are unchanged. Original instructions receive
native before-states; independent full-flight parity remains open.
Debug/Release playable and fixture builds pass. Seven selected Release checks
and three Debug checks pass, with matching mission input/save hashes. Passing
RAM is removed; build-cache use remains 2.00 GiB within its 4 GiB budget.

Latest user direction: decompile any remaining instruction translation encountered
on the active path, including its required shared tails and child contracts.
The comparison-build inventory currently lists 75 already-readable source-only
entries and 84 deferred translations; use the actual deferred list rather than
treating 75 as a count of undecompiled functions. These counts do not establish
native integration or whole-game completion.
The user's mission six is the final Carrier Sub mission: F6 selects internal
mode 8. Internal mode 6 is Search and Rescue; the earlier reference to
`postflight_scheduler.c:mode_six` for this concern was a numbering error.
The supplied [Wikipedia passage](https://en.wikipedia.org/wiki/F/A-18_Interceptor#Gameplay)
reports that the submarine need not visibly explode and that destroying the
patrolling aircraft may suffice. The original mode-eight dispatch uses
`POSTFLIGHT_MODE_OTHER` -> `generic_mode`, comparing admitted/completed counters.
It has no direct submarine-explosion test. The successful ordinary-input final
flight/save/menu/wrap route and rescue/cruise-earned availability are accepted
above; a newly enlisted complete tour and independent original full-flight
parity remain open.
Preserve original rules; see `analysis/native_final_mission_completion_context.md`.

Earlier mission-four input diagnostic added `4-success` and `4-sequence`.
These modes read flight state and send ordinary keys, firing earlier on the
closing pass and pursuing regional enemy record 12 when eligible. Starting
with `region_pilot_fixture.load_region_pilot()` in a fresh saved-pilot directory,
the 1,747-event flight shoots down record 8: radar hits advance 0 -> 1 at body
6,434. All 26 input/stage intervals and 81 sampled bodies match original
compared RAM/drawing. Original instructions receive native before-states;
this is sampled boundary evidence, not an independent original full flight.
The flight still crashes and resets at tick 15,999 (C11830), with completions
unchanged at 1, no objective and no config write. The success fixture returns
failure as required. Successful escort/landing/save remains open. Existing
`4-mission` and mode-three/five pilot choices are preserved. Gameplay and the
playable executable are unchanged; passing RAM is removed after comparison.
Diagnostic reports and consumed keys are under
`build/native-flight/mission-four-earned-success-probe/` (ignored local output).
Debug/Release mission fixture builds pass. All five selected Release checks pass:
mission-four combat reset, mode-three/five sequences and artifact policy/cleanup.

Region-flight coverage is restored with a source-earned level-zero pilot log.
Normal menu reset/callsign entry, carrier qualification and mission-three
success/save/menu return reproduce its exact 78 bytes. Qualification matches
50 input/stage intervals and 174 sampled bodies; mission three matches 44/164,
including each real config write. The playable runner agrees on both saves
and the mission-three menu return. Region flight retains its existing keys
and strict guards: records 12/13 spawn, record 12 exits its zone, NPC missile
slots 9/13 activate, and all 57 intervals/49 bodies match original compared
RAM/drawing. See `analysis/native_region_pilot_progression_milestone.md`.
The region CTest now uses the source checker with temporary RAM. The new serial
`fa18_native_region_pilot_progression` gate reproduces the earned log.
Debug/Release playable and affected fixture builds pass. Seven selected Release
checks, three Debug checks and the final Release region/report repeat pass;
both builds reject a late unordered replay event. Results/hashes are in
`analysis/figures/native_region_pilot_progression_checkpoint.json`. Passing
captures are removed; build-cache use is 2.00 GiB under the existing 4 GiB policy.

The playable host replay loader now grows its event storage instead of rejecting
the mission's 1,652-key recording at event 1,025. Allocation, size and read
errors remain explicit. Game rules, arithmetic and comparison masks are unchanged.
The original level-two ADF pilot fills both admitted aircraft slots; it cannot
spawn the region aircraft in this scenario. Cold level loading is correct.
The retained earned log selects a reachable coverage case; it supplies no
flight state or outcomes. It also supplies the accepted mission-four route above.

Earlier validation diagnostics: the mode fixture services PCM after each
tick, matching the playable backend, and supports `FA18_MODE_END_TICK` plus
`FA18_MODE_FINAL_DATA` for aligned, bounded final-RAM snapshots. Default input
sequences and acceptance guards are preserved. With the established region
inputs, all 1 MiB of runner/fixture RAM agrees at ticks 16,003, 18,500 and
22,000. PCM servicing does not restore the missing region coverage. A separate
early pull-up/rudder experiment matches 61 original input/stage intervals and
62 sampled bodies but fails acceptance; its inputs are not retained in the
fixture. See `analysis/native_region_flight_diagnostics.md`. Gameplay and the
playable executable were unchanged at that earlier checkpoint; its open region
coverage is superseded by the earned-pilot acceptance above.
Debug/Release fixture builds, five selected CTests, fourteen invalid tick
checks and the three final aligned RAM comparisons pass. Passing captures
are removed within the existing retention policy.

The active playable runner is `fa18_native`. This checkpoint follows
`6d5f5c3f` on `coverage-accounting`.
Use `git log -1` for the latest pushed checkpoint.
The playable executable is `build/native/fa18_native.exe`; its SHA256 is
`24c84246d78e72afdc1f8aa876aac0a1fe1dae191863adead9ac069b877536de`.
Untracked `.vscode/` is user-owned and must remain untouched.

Latest connected fix: C2DEE0/C2DFF6's matrix product cursor now reaches the
following record's C2436A sight update through the native dynamics/record loop.
All 576 complete matrix component cases match original angles, divisor,
cursor and non-stack RAM. The normal-key mode-four diagnostic matches all
26 input/stage intervals and 59 sampled bodies, including C11788's crash
continuation and C11830's reset. Its new serial gate is
`fa18_native_mission_four_combat_reset`; Debug and Release pass. The native
playable executable above is refreshed. No arithmetic, gameplay rule or
comparison mask changes. See `analysis/native_matrix_sight_reference_milestone.md`
and `analysis/figures/native_matrix_sight_reference_checkpoint.json`.
Passing RAM stays temporary; the resolved failure is compressed with hashes.

Earlier regression: the original level-two region input failed its spawn/zone/
NPC-missile coverage guards. All 58 input/stage intervals and 47 sampled bodies
of the failing run match original compared RAM/drawing, with no config write.
A shorter validation pull-up also misses these events; original keys and
strict guards are preserved in the accepted earned-pilot case above.
The other nine additional Release checks and six selected Debug checks pass
across their initial runs and Debug sequence retry. Debug sequence captures
now allow 90 seconds; Release retains 60. Details are in the milestone above.

The established combat-reset diagnostic still has no hit/objective/save and
crashes at tick 22,148, with completions unchanged at 3. The separate successful
route is accepted above. Source `postflight_scheduler.c:record_mode` requires
record four grounded, active and slow, with enemy records eight/ten inactive,
before its countdown admits the objective. Successful modes seven/eight,
final Carrier Sub (internal mode-eight) completion verification,
independent complete flights, further play after the accepted result menu,
remaining caller contracts, typed state, audio and wider performance remain
open; the complete-port goal stays active.

Earlier mission result sequence acceptance: modes three and five complete
normal-key success, saved result, result messages, Escape, scene bootstrap,
menu return and cold log reload. All 44/42 input-stage intervals and 162/169
sampled bodies match original compared RAM/drawing, with 64 consecutive
landing bodies per mode and one actual config write. Completion count stays
4 and grade stays 2 through restart; all 78 saved log bytes remain unchanged.
The serial gates are `fa18_native_mission_3_sequence` and
`fa18_native_mission_5_sequence`. Passing RAM remains temporary and bounded;
Debug/Release fixture builds and all five selected checks pass in each
configuration, with identical consumed input and saved-result hashes.
Gameplay and the playable executable are unchanged. See
`analysis/native_mission_result_sequence_milestone.md`. This resolves result
message completion and Escape/menu restart for these two mission-list modes.
Successful modes four/six/seven/eight, independent complete flights, further
play after this result menu, remaining contracts, typed state, audio and wider
performance remain open; the complete-port goal stays active.

Earlier mode-five mission success acceptance: normal keys complete formation,
both radar shoot-downs, the original objective, carrier arrestor landing,
stopped-aircraft admission, result save and cold reload. All 34 input/stage
intervals and 155 sampled bodies match original compared RAM/drawing, including
all 59 consecutive touchdown-to-result bodies. Completion count advances
3 -> 4 and mode-five grade 1 -> 2 with no crash reset. The new serial gate is
`fa18_native_mission_five_success`. Debug fixture build and four selected checks
pass; Release fixture build and six selected checks pass. Gameplay and the
playable executable are unchanged. Passing RAM remains temporary and bounded.
See `analysis/native_mission_five_success_milestone.md` and
`analysis/figures/native_mission_five_success_checkpoint.json`. This supersedes
the earlier open mode-five return/landing/save claims below. Result message
completion/restart, successful modes four/six/seven/eight, independent complete
flights, remaining contracts, typed state, audio and wider performance remain
open; the complete-port goal stays active.

Earlier mode-five combat objective acceptance: normal keyboard input completes
formation, destroys both enemy aircraft with radar missiles and reaches the
original objective phase `$FF` at tick 19,304. All 29 input/stage intervals
and 118 sampled bodies match original compared RAM/drawing, including 32
consecutive formation-window and 40 consecutive combat-window bodies. The
new serial CTest is `fa18_native_mission_five_objective`. Debug fixture build
and all four selected checks pass; Release fixture build and all five selected
checks pass. Gameplay and the playable executable are unchanged. The bounded
run has no reset, keeps completion count 3 and writes no result config. Safe
return, landing and saved success remain open for mode five; an exploratory
longer continuation still crashes. See
`analysis/native_mission_five_objective_milestone.md` and
`analysis/figures/native_mission_five_objective_checkpoint.json`. This advances
the earlier forced-return milestone below; independent complete flights and
other whole-port work remain open. The complete-port goal stays active.

Earlier mode-five forced-return acceptance: normal keyboard input holds the
original stolen-aircraft proximity requirement through all 201 native scans,
then reaches C0A12E for both aircraft. All 24 input/stage intervals and 107
sampled bodies match original compared RAM/drawing, including 64 consecutive
proximity/restoration window bodies. Original execution reports 12 calls for
each aircraft; both native restored view sets match the original C295E0 table.
The new serial CTest is `fa18_native_mission_five_forced_return`. This advances
mission coverage without changing playable behavior or the executable.
Debug fixture build and all three selected checks pass; the Release fixture
build and all four selected checks pass, including mode three, mode-five
combat/reset, forced return and artifact cleanup.
See `analysis/native_mission_five_forced_return_milestone.md`. The bounded
fixture still reports an incomplete mission, with completion count 3 and no
saved result; combat, safe return, landing and cold reload remain open for
mode five. Independent complete flights and the other whole-port work remain
open. This supersedes the earlier pilot/countdown investigation below.

Earlier mode-five investigation: the optional validation argument `5-formation`
attempts the original stolen-aircraft proximity objective using ordinary keys.
The bounded run through tick 11,600 matches all 24 input/stage intervals and
36 sampled bodies against original compared RAM/drawing, including contact
bit-two transitions during the early turn. Its frozen attitude also occurs
in the original; no gameplay fix is justified by this evidence. The countdown
remains 200, completion count remains 3 and the fixture correctly reports an
incomplete mission. Default mode-three and numeric mode-five checks retain
their existing pilots. `FA18_MISSION_END_TICK` bounds diagnostic duration;
passing RAM remains temporary. See
`analysis/native_mission_five_formation_checkpoint.md` for the retained input,
comparison scope and next investigation. Mode-five success remains open.
The Release fixture build, identical bounded replay and both existing mission
CTest checks plus artifact cleanup pass.

Latest independent-camera depth acceptance: the final scaled view coefficient
now reaches context sorting through an explicit native return value. C2DACC's
matrix call replaces the earlier C29042 origin output; context projection
preserves its upper word. All 128 complete matrix and 512 view/projection/sort
component cases pass. Map and two moving camera routes each match 16 consecutive
full original bodies, covering both signs of the incoming factor and actual
preserved-factor sorting: 16 source batches and 16 cached-depth entries.
Debug builds and all three affected checks pass; the final Release build and
all 17 affected checks pass. The new serial CTest uses one bounded capture run per
window, with per-body timing sidecars. See
`analysis/native_context_depth_milestone.md`. Camera motion, matrix arithmetic,
gameplay rules and comparison masks are unchanged. Full mission acceptance
remains open.

Current camera checkpoint: `analysis/figures/native_context_depth_checkpoint.json`.
Regression logs: `build/native-flight/view-depth-{debug,release}-ctest.log`.
The camera gate covers map and moving views; regressions include both carrier
sequences, mission three, mode-five combat/reset and all three weapon kills.

Latest combat/reset acceptance: the mode-five normal-key diagnostic matches
all 26 input/stage intervals and all 115 sampled bodies, including every one
of the 63 consecutive combat-window bodies. Seven gun-hit increments, both
enemy-expiry accounting increments (0 -> 1 -> 2) and reset C11830 at tick
20,352 match original compared RAM/drawing. The actual control status pointer
and depth-sort output now reach early model expiry. Their retained result is
ordinary host renderer state until a placement consumes it, preserving startup
and restart scratch. Distance and sorting arithmetic, gameplay rules and
comparison masks are unchanged. All 64 candidate-reference, 128 depth-sort
and 256 distance/factor component cases pass. The combat/reset comparison is
now a serial CTest gate. The flight still crashes and completion count stays 3;
mode-five mission success remains unaccepted. See
`analysis/native_mission_five_combat_reset_milestone.md` and
`analysis/figures/native_mission_five_combat_reset_checkpoint.json`.

The reset-reference fix was committed as `48e3e130`. The former expanded
renderer investigation is resolved for this scenario; its failed body 96/97
RAM is compressed locally with hashes. Passing RAM remains temporary and
removed. The former C29042 context-flight claim is superseded by the final
scaled view output above. Startup/menu context refreshes without a projection
pass retain their named incoming-factor TODO when no template/distance call
replaces it. Preserve user-owned `.vscode/` and the protected validation files.

Recent completed batches:

- Mode-three mission success (2026-10-08): normal keys complete takeoff,
  selected-aircraft confirmation, safe terrain landing, taxi into the original
  runway polygon, stopped-aircraft admission, result save and fresh log reload.
  Completion count advances 3 -> 4 and mode grade 1 -> 2 without a crash reset.
  All 36 input/stage intervals and 184 sampled bodies match original compared
  RAM/drawing, including 96 consecutive landing bodies and the actual DOS Write.
  This is a CTest gate. The validation pilot only reads RAM and emits keys;
  gameplay and the playable executable are unchanged. See
  `analysis/native_mission_three_success_milestone.md` for scope and limits.
- Carrier qualification sequence (2026-10-08): both the original ADF log and
  a reopened unqualified saved pilot complete takeoff, landing, successful
  result, config save, restarted flight and fresh log reload with normal keys.
  Each passes 50 input/stage intervals and 174 bodies against original compared
  RAM/drawing, including all 96 consecutive landing bodies and the actual DOS
  Write. The new pilot earns qualification 0 -> 1. A retained consumed-key
  fixture makes both checks repeatable in CTest without local build-cache
  input. Gameplay and the playable executable are unchanged. See
  `analysis/native_qualification_sequence_milestone.md` for scope and limits.
- Gun shoot-down (2026-10-08): 317 input/stage intervals and 275 bodies match
  original compared RAM/drawing, including every body in the 101-body
  first-damage-to-inactivation interval. The actual map clipping owner now
  carries its retained output into early model expiry, fixing C4F6DE's cache.
  All 256 focused clipping residue/drawing cases match. Gun kill is a CTest
  acceptance gate; collision, damage, motion and timers are unchanged. See
  `analysis/native_gun_kill_milestone.md` for source ownership and proof limits.
- `e2919a29`: gun damage hit (2026-10-08): 317 input/stage intervals and 181 sampled bodies
  match original compared RAM/drawing, including the exact first gun hit.
  Pilot-log +60 and enemy ten's damage advance 0 -> 1. A validation-only pilot
  reads positions and emits ordinary pitch/rudder/fire keys; no gameplay
  source or playable executable changes. The gun-hit test is registered in
  CTest. See `analysis/native_gun_hit_milestone.md` for the historical failure
  now resolved by the gun shoot-down checkpoint.
- `5aea11c6`: gun diagnostics (2026-10-08): 59 input/stage intervals and 277 sampled bodies
  match original compared RAM/drawing, including 32 active gun-effect bodies.
  The fixed ordinary-input flight records zero gun hits. The failed feedback
  pilot was removed; read-only telemetry and strict gun-hit observation remain
  in the validation fixture. No playable behavior changes. See
  `analysis/native_gun_approach_checkpoint.md`. Its open-first-hit claim is
  superseded by the newer gun damage checkpoint.
- `373feace`: infrared acceptance (2026-10-08): normal-input missile impact, expiry
  accounting and inactivation pass 55 input/stage intervals and 802 bodies,
  including every body in the 634-body destruction interval. The selected
  missile kind and pilot-log +64 counter are checked. No gameplay rule changes;
  the playable executable is unchanged. See
  `analysis/native_infrared_kill_milestone.md`.
- `f4606f9c`: normal-input radar shoot-down through impact, enemy expiry
  accounting and inactivation. All 634 consecutive bodies in that interval
  match original compared RAM/drawing; the full probe passes 57 input/stage
  intervals and 802 bodies. This batch extends validation of existing gameplay.
- `1c7a2481`: preserve the incoming placement result on inactive C22AC0 paths;
  verify the actual radar hit, weapon consumption before reset and replenishment
  afterward. Thirty-two focused inactive-record contracts pass.
- `367139cf`: connect mode-two stream resets to the existing scene initializer;
  verify all seven streams and wrap. Mode six naturally loses three aircraft,
  exhausts resets, returns to the menu and relaunches Free Flight.

The preceding combat/expiry/reset checkpoint passed native Debug and Release
playable and affected fixture builds. Debug mission-three success, mode-five combat/reset
and cleanup pass. All 14 final Release checks pass: both mission gates, both
qualification sequences, gun/radar/infrared kills, mode five, record expiry,
models, raster, frontend, artifact policy and cleanup. Evidence is in
`analysis/figures/native_mission_five_combat_reset_checkpoint.json`; logs are
`build/native-flight/mission-five-model-state-{debug,release}-ctest.log`.
The preceding reset-reference checkpoint is
`analysis/figures/native_candidate_reference_reset_checkpoint.json`; the
preceding mission-three checkpoint is
`analysis/figures/native_mission_three_success_checkpoint.json`.

The preceding six Release checks pass: both qualification sequences, ordinary qualification,
ready-player postflight, frontend and artifact cleanup. Debug sequence checks
also pass. Evidence is in `analysis/figures/native_qualification_sequence_checkpoint.json`;
logs are in `build/native-flight/carrier-sequence/`.

The preceding gun checkpoint passed all eight affected
checks: gun hit, gun kill, radar kill, infrared kill, raster, models,
frontend and artifact cleanup.
Results are recorded in `analysis/figures/native_gun_kill_checkpoint.json`;
the local log is `build/native-flight/gun-kill-checkpoint-ctest.log`.
The preceding first-hit checkpoint is
`analysis/figures/native_gun_hit_checkpoint.json`.
The preceding gun approach checkpoint is
`analysis/figures/native_gun_approach_checkpoint.json`. The preceding infrared Release
run passed all four; its checkpoint is
`analysis/figures/native_infrared_kill_checkpoint.json`.
The prior radar Release run passed eight
affected CTests: combat eight, radar hit, radar kill, three weapon probes,
frontend and artifact cleanup. Evidence and executable hashes are in
`analysis/native_radar_kill_milestone.md` and
`analysis/figures/native_radar_kill_checkpoint.json`.
The comparisons execute original instructions from native before-states;
they do not establish an independent complete original mission replay.

Next work starts with finishing the mode-five objective and successful return
using normal keys, then comparing its successful landing, save and reload.
The combat/expiry/reset comparison now passes; it does not prove a successful
mission or complete independent flight. The context-flight factor is resolved;
trace startup/menu context callers without projection if cached sorting reaches
their still-unknown incoming factor.
Successful mission-list modes four through eight and independent full-flight
comparison remain open.
Mode three now has source comparisons through its
objective, landing, taxi, stopped-aircraft result, save and reload. Its mission
restart and independent full original replay remain open. Carrier qualification has repeatable source
comparisons through landing/result/save/restart/reload, including a newly earned
qualification, but independent complete original flight parity remains open.
The gun probe now passes:
`python tools/native/check_mode_two.py --mode 8 --kill --gun --out build/native-flight/gun-kill-fixed`.
The former failure's original C7FE78 word came from map clip stage C24956/C24970
return addresses, with C249D6 providing a second closing-boundary Y writer.
The pure clipping owner now publishes those outputs to model -$7C; full bodies
verify the connected ownership. Keep component and whole-frame evidence
separate: isolated model tests still seed caller scratch and alone cannot prove
its producer. The resolved `frame.79.*.dat.gz` case is compressed locally in
`build/native-flight/gun-kill/failure/`. Existing masks remain unchanged;
native gameplay never consumes reference RAM. Remaining callback contracts,
typed game state, audio fidelity and visible-window/combat performance remain
open. Preserve physics and source timers while allowing the user's native
presentation cadence. The complete-port goal remains active.

Builds and CTest automatically prune disposable artifacts against a 4 GiB
budget; passing comparison RAM is deleted immediately. The latest cleanup left
about 1.61 GiB of build files and small artifacts before this mission batch. Keep passing
captures temporary and bounded; use `--keep-captures` only for deliberate
debugging. Preserve sealed recordings and local media. Do not modify
`scripts/check_native_build.py`, `scripts/native_frame_count.py` or
`port/native_data_allowlist.txt`.

This summary supersedes older checkout, validation and unfinished-work claims
below. Dated entries retain the evidence available at their original milestone.

## Objective and user constraints

Latest normal-input radar shoot-down (2026-10-07): mode eight destroys enemy
aircraft ten through actual impact, 15-tick expiry, source enemy-aircraft expiry
accounting and eventual inactivation. All 634 consecutive bodies in this
interval match original compared RAM/drawing; the checker requires every body
serial. The full probe passes 57 input/stage intervals and 802 bodies. Passing
RAM is discarded immediately, and raw captures remain temporary and bounded.
Native Debug/Release builds and eight affected CTests pass. See
`analysis/native_radar_kill_milestone.md`. Gun/infrared shoot-downs, successful
missions, independent full flights, remaining contracts, typed state, audio and
wider performance stay unfinished; the complete-port goal remains active.

Latest normal-input radar hit (2026-10-07): mode eight registers a player radar
missile hit. All 57 input/stage intervals and 188 bodies, including the precise
hit body, match original compared RAM/drawing. Inactive C22AC0 preserves the
actual placement caller calculation; 32 focused inactive-record contracts pass.
Weapon checks now verify consumption before the natural reset and replenishment
afterward. Mode-eight combat keeps its long-flight guards with a bounded pull-up.
All five new/expanded source comparisons run in CTest with temporary passing RAM.
Debug/Release builds and thirteen affected checks pass across targeted runs.
See `analysis/native_radar_hit_milestone.md`. Kills, successful missions,
independent full flights, remaining contracts, audio, typed state and wider
performance stay unfinished; the complete-port goal remains active.

Latest normal-input outcome and stream reset (2026-10-07): mode six now has a
required three-loss/reset-exhaustion/menu-return/Free-Flight-relaunch scenario.
Its 86 input/stage intervals and 237 sampled bodies match original compared
RAM/drawing. Mode two's missing C28722 child is connected to the existing scene
initializer: seven streams, wrap and Escape return pass 30 intervals/206 bodies.
The old mode-two spontaneous-failure deadline is superseded by this verified
source loop. Debug/Release builds and seven affected CTests pass; both source
comparisons run in CTest with temporary passing RAM. See
`analysis/native_natural_outcomes.md`. Whole-game acceptance remains unfinished;
the complete-port goal stays active.

Latest connected postflight preservation (2026-10-07): nine childless callback
owners retain actual preceding input results. The expanded comparison passes
355 full bodies, 372 recorder parents, 84 keyboard parents and 168 separately
checked input/stage parents against original compared RAM/drawing and defined
returns. Three controlled outcomes pass nine intervals/25 bodies and saved-log
persistence. Native Debug/Release and eight affected checks pass. See
`analysis/native_postflight_input_return.md`.
Whole mission/audio/state/performance acceptance and remaining contracts stay
unfinished; the complete-port goal remains active.

Latest connected setup preservation (2026-10-07): smoothing publication/restart,
context entry and viewport completion preserve actual preceding input results.
Sixty separately checked input/stage parents, 247 full bodies, 264 recorder
parents and 84 keyboard parents match original compared RAM/drawing and defined
returns. Stage outputs are checked before later message rendering can replace
them. Native Debug/Release and eleven affected checks pass. See `analysis/native_setup_input_return.md`.
Other contracts and whole-game mission/audio/state/performance acceptance remain
unfinished; the complete-port goal stays active.

Latest connected smoothing cancel (2026-10-07): C10A24 -> MC_CANCEL now calls
the existing native cancel/reset composition instead of aborting. A controlled
source cancel marker at a naturally reached mode-four stage resumes flight
and returns to the menu. All 44 actual input/stage intervals and 29 sampled
bodies match original compared RAM/drawing. Native Debug/Release and eight
affected checks pass. See
`analysis/native_smoothing_cancel.md`. Other stage contracts and whole-game
mission/audio/state/performance acceptance remain unfinished; the goal stays active.

Latest measured native demo (2026-10-07): all 10,910 frames call SDL presentation
in a normally paced hidden Direct3D window; work peaks at 6.9374 ms, below 20 ms.
Headless work peaks at 1.6407 ms. Host pacing intervals sometimes exceed 20 ms;
visible display and full mission/combat performance remain unaccepted. Logging
preserves complete RAM, pixels and counters in Free Flight. Native Debug/Release
and five affected checks pass. See `analysis/native_frame_performance.md`.
Whole-game outcomes, rare contracts, audio and typed state remain unfinished;
the complete-port goal stays active.

Latest connected idle preservation (2026-10-07): the actual viewport-message
stage and idle frame preserve preceding input outputs. Fourteen idle bodies
now match their original outputs; twelve feed recorder input directly. The
expanded comparison passes 199 full bodies, 216 recorder parents, 84 keyboard
parents and twelve separate input/stage parents. Ten affected checks and
native/reference builds pass. See `analysis/native_idle_input_return.md`.
Other stage/reset/HUD contracts and whole-game mission/audio/state/performance
acceptance remain unfinished; the complete-port goal stays active.

Latest connected context camera outputs (2026-10-07): existing matrix and
observer owners now compose record-relative and preset camera calculations.
Actual context/map outputs and preserving requests reach depleted recorder
input. The connected run also fixes zoom-text and skipped-message formatting
outputs. 187 full bodies match compared RAM/drawing; 204 recorder parents and
84 keyboard parents match RAM and defined returns. Two new idle bodies retain
explicitly unresolved inherited outputs. All 12,576 selected command parents
match RAM/returns. Ten affected checks and native/reference builds pass.
See `analysis/native_context_input_return.md`. Idle/stage/reset
contracts and whole-game mission/audio/state/performance acceptance stay open.

Latest connected indexed outputs (2026-10-07): number/index selections,
throttle outputs and recorder-$FD relative change now reach later depleted
input. Extended frames also connect source release/model/vertex paths and fix
negative throttle-request ordering and the recorder-mode gate. 175 full bodies,
192 recorder parents and 72 keyboard parents match original compared RAM/drawing
and returns; all 10,688 selected command parents match. Nine affected checks and
native/reference builds pass. See `analysis/native_indexed_input_return.md`. Context/reset/HUD contracts
and whole-game mission/audio/state/performance acceptance remain open.

Latest connected fire/countermeasure outputs (2026-10-07): fire selection,
saved stock event and actual nested eject queue index now reach subsequent
depleted input; modified/depleted gates preserve prior output. 163 full bodies,
180 recorder parents and 60 intervening keyboard parents match original compared
RAM/drawing and returns. Corrected radar/weapon probes require actual action
owners/values; the earlier mode-zero integration claim is superseded. All 8,256
selected command parents, nine affected checks and all native/reference builds
pass. See `analysis/native_fire_countermeasure_input_return.md`. Remaining
indexed/context/HUD contracts and whole-game acceptance stay open.

Latest connected weapon/radar command outputs (2026-10-07): actual range,
weapon block/mode and preserving target/throttle/hook/ECM actions now compose
depleted recorder input. Weapon $80-$10 selects $30 under the original signed
overflow condition. 151 full bodies, 168 recorder parents and 48 intervening
keyboard parents match original compared RAM/drawing and returns. All 4,416
selected command parents pass RAM/return checks; other action families and
whole-game acceptance stay open. Nine affected checks and all native/reference
builds pass. See `analysis/native_weapon_radar_input_return.md`. Gameplay
acceptance notes now reflect permitted native cadence and corrected cold cores.

Latest connected view-action return (2026-10-07): actual detail, origin level,
record type, view mode and masked zoom flags compose subsequent depleted input.
139 full bodies, 156 recorder parents and 36 intervening keyboard parents match
original compared RAM/drawing and defined returns. All 2,176 selected command
parents match RAM and returns. Other action families and whole-game acceptance
remain open. Nine affected checks and all native/reference builds pass. See
`analysis/native_view_action_input_return.md`. Full port stays active.

Latest connected control-action return (2026-10-07): preserving control actions,
actual HUD-mode changes and gear-gate outputs now reach subsequent depleted
recorder input when publication skips. 127 full bodies, 144 recorder parents
and 24 intervening keyboard parents match original RAM/drawing and returns.
832 focused command parents match RAM and 826 returns pass; six stay unresolved.
Nine affected checks and all native/reference builds pass.
See `analysis/native_control_action_input_return.md`. Full acceptance stays open.

Latest connected intervening-command return (2026-10-07): wait/modifier/empty
and queue-only exits compose actual preservation/selection outputs, and accepted
publication supplies its signed queue index to subsequent depleted input.
115 full bodies, 132 recorder parents and twelve intervening keyboard parents
match original RAM/drawing. 512 focused command RAM cases and 471 defined returns
pass; 41 action-owned outputs stay unresolved. Nine affected checks and all
native/reference builds pass. See
`analysis/native_intervening_command_input_return.md`. Full acceptance remains open.

Latest connected lost-target cleanup return (2026-10-07): the actual view mode
or signed translated-queue index now reaches first depleted recorder input.
Skipped cleanup preserves its preceding result. 103 complete bodies and 120
input parents match original RAM/drawing, including twelve selected lost-target
frames. 512 focused cleanup return/RAM cases and all native/reference builds pass.
Nine affected native checks pass.
See `analysis/native_selection_cleanup_input_return.md`. Earlier HUD,
intervening-command outputs and whole-game acceptance remain open.

Latest connected grid/aircraft-marker return (2026-10-07): the existing point,
heading/shape, segment, line and number-text owners now compose first depleted
recorder input; inactive grids preserve the preceding result. 91 complete bodies
and 108 input parents match original RAM/drawing. Two snapshots each pass 401
grid/marker and 128 clipped-segment return/RAM cases. Eleven affected checks and
all native/reference builds pass. See
`analysis/native_grid_marker_input_return.md`. Selection-cleanup, earlier HUD,
intervening-command contracts and whole-game acceptance remain open.

Latest connected HUD marker return (2026-10-07): line sizes and defined
clipped-start X deltas now reach first depleted recorder flare/chaff commands.
Other rejected starts preserve the preceding result. 79 full bodies and 96
input parents match original RAM/drawing, including twelve fresh Free Flight
marker frames. Two snapshots each pass 128 line-return/RAM cases; focused HUD
checks pass 492 RAM cases and 252 returns. Ten affected checks and all builds
pass. See `analysis/native_hud_marker_input_return.md`. Active grid/aircraft
marker composition, remaining HUD/command contracts and full acceptance stay open.

Latest small-text fault return (2026-10-07): the original release fault hook
is RTS, so an odd destination preserves the selected glyph after setting $46.
The native renderer now retains that result through its existing drawing skip.
480 HUD non-stack RAM cases and 237 defined returns match, including 27 odd
fault returns across cockpit/context views. Six affected checks and all builds
pass. See `analysis/native_small_text_fault_return.md`. Full port remains open.

Latest connected scene-label return (2026-10-07): scene row-Z loads, matrix
products and position-number text results now reach first depleted recorder
flare/chaff commands. Terminator and alternate skipped-row exits match source;
skipped labels preserve the context speed/altitude/heading HUD result. The suite
matches 67 complete bodies and 84 recorder input parents, including twelve
label frames. 256 label returns/non-stack RAM cases and 450 HUD RAM cases with
207 defined returns pass. Ten affected checks and all native/reference builds
pass. See `analysis/native_scene_label_input_return.md`. Active grid/marker,
remaining HUD/command contracts and whole-game acceptance remain open.

Latest connected debug return (2026-10-07): four-plane text character/glyph
results now reach first depleted recorder flare/chaff commands through the
real numeric overlay. Inactive overlays preserve the preceding result.
Fifty-five complete bodies and 72 recorder input parents match original
compared RAM/drawing, including twelve one-/three-field debug frames.
256 gated debug returns match original source, with clipping and odd-window
selection cases. Ten affected checks and all native/reference builds pass.
See `analysis/native_debug_text_input_return.md`. Scene-label, marker, earlier
HUD and intervening-command returns and whole-game acceptance remain open.

Latest connected HUD return (2026-10-07): source bar destinations, small-text
character/glyph results and periodic cockpit redraw pass counts now reach the
first depleted recorder flare/chaff command. Forty-three actual flight bodies
and 60 recorder input parents match original compared RAM/drawing, including
twelve new HUD/redraw cases. Focused checks match 405 HUD RAM cases and 117
defined returns with clipped and skipped paths. Nine affected checks pass;
Release/Debug and both reference runners build. Selected unfinished drawing
and intervening-command returns remain explicit missing contracts. See
`analysis/native_hud_input_returns.md`. The complete-port goal remains active.

Latest connected drawing return (2026-10-07): periodic C2F582 page clears
publish their actual pattern for the next first depleted recorder command.
Twelve clears reached by the normal game counter and twelve following input
parents match original independent returns. The complete suite matches 31
flight bodies and 48 recorder input parents, including drawing bytes. Selected
labels/debug overlays invalidate this return; other drawing/command producers
remain explicit missing contracts. See `analysis/native_page_clear_input_return.md`.

Latest connected recorder return (2026-10-07): C32CEE's defined message/glyph
byte now reaches the next first depleted pending flare/chaff command. Nineteen
actual flight bodies and 36 recorder input parents match original compared
RAM/drawing; twelve first-depleted cases verify independently derived original
frame returns and both release-bit states. Inactive message frames do not assign
a new value. The real message owner passes 8,192 CPU/SR/RAM component calls.
Other drawing-derived and intervening-command returns remain unfinished.
See `analysis/native_message_input_carry_milestone.md`. The goal stays active.

Latest user controls report (2026-10-07): rudder and numeric keypad external
views did not work. Native host input now shares the reference physical Amiga
mapper; typed-text translation is no longer used to identify physical keys.
Comma/period and SDL/legacy keypad press/release events are connected. A normal
Free Flight test drives the SDL event queue through the playable host handler;
26 input/stage intervals and 52 bodies match original compared RAM/display,
including opposite rudder control-axis motion, releases and external/cockpit
view changes with either Num Lock flag. See
`analysis/native_host_keys_milestone.md`. Full goal remains unfinished.

Latest user direction (2026-10-07): clear generated output and prevent renewed
disk growth. Workspace 60.501 -> 3.719 GiB; 56.782 GiB reclaimed. Build/CTest
cleanup now uses a 4 GiB budget, native comparisons discard passing RAM, and
manual native captures default to 512 MiB. Active Visual Studio builds, source,
media and recordings are preserved. Original recent windows are verified
`.dat.gz` files, directly readable by the window checker. See
`analysis/workspace_artifact_retention.md` and AGENTS.md retention rules.

The active goal is **get gameplay going and matching the recorded runs**.
It is still active and unfulfilled. The user requested this file to transfer
context; this is not a request to cancel or mark the gameplay goal complete.

Latest startup correction (2026-10-07): cold native entry now invokes C08EE4/
C08EB8 before C08F26. All 16 initial cores and 9,376 complete record cores
across 586 independent demo boundaries match, correcting the player region
flag/countdown gap. Takeoff drawing passes 223/223; later drawing stays 83/363
while player/camera/complete cores pass 363/363. Thirty-two cold source parents,
disk-backed demo contracts, eight runtime gates and Free Flight/postflight
comparisons pass. See `analysis/native_scene_startup_milestone.md`. Full goal
remains active.

- Latest user clarification: exact Amiga frame timing is not a completion
  requirement; this is a native port. Preserve gameplay physics, rules, input
  behavior and source-defined timers, while allowing different rendering and
  presentation cadence. Compare equivalent gameplay states/events rather than
  requiring identical wall-clock timestamps or frame numbers. Intro, loading
  and preflight may also run faster.
- Accepted target: modern hardware completes each rendered frame within a
  20 ms budget and presents every frame without skipping. Do not reproduce
  Amiga missed frames or rendering delays. This is the intended pacing policy,
  not a measured performance guarantee or permission to change physics/timers.
- Ignore **Copper fade** when comparing frames. Do not silently mask HUD pixels,
  loosen physics/input comparisons or invent another exclusion.
- Commit completed, validated batches as work progresses. Give progress estimates
  with a concrete scope and evidence. Routine counts and emulator instruction
  percentages do not measure native gameplay completeness.
- Avoid frequent expensive full original replays. Reuse retained original
  captures and run focused affected-path checks; repeat broad checks only for a
  new change, failure or unresolved concern.
- Original source/disk behavior is authoritative. Do not supply captured game
  states, pixels or fitted clocks as native behavior; controlled test inputs
  belong in validation entries only.
- Preserve `.vscode/`, sealed recordings and unrelated changes. Do not edit
  `scripts/check_native_build.py`, `scripts/native_frame_count.py` or
  `port/native_data_allowlist.txt`.

Read `AGENTS.md`, `CURRENT_PORT_HANDOFF.md`, `PORT.md` and `port/README.md`.
Their historical restrictions against a new runner were explicitly superseded
by the user's native-port direction. Older milestone notes can contain now-fixed
TODOs; current code and the newest evidence take precedence.

## Current checkout and architecture

Branch: `coverage-accounting`. Latest validated code: **`f4606f9c`**, pushed to
origin. The playable executable and current acceptance scope are identified in
the restart summary above. Preserve untracked `.vscode/`.
No build/test process is pending and no user answer or approval is pending.

The playable native runner is **`fa18_native`**:

- Entry, replay, captures and build: `port/native/`.
- Connected game composition: `port/game/native/`, reusing domain C in
  `port/game/`. Shared disk/loading facilities remain in `port/amiga/`.
- `port/native/CMakeLists.txt` is included by `port/recomp/CMakeLists.txt`.
  `fa18_native_runtime` is an object library shared by the actual executable
  and native integration tests, not a separate gameplay implementation.
- The checked native link omits CPU, generated translations, glue, bus and
  chipset objects. Storage still uses original addresses; typed state migration
  and whole-game coverage remain unfinished.
- `fa18_recomp` and `fa18_romfree` are original-behavior reference tools.
  Do not restore the deleted top-level `fa18_port` or add C/H files under `port/`.

Normal build and launch:

```powershell
python scripts/build_native.py
build/native/fa18_native.exe --adf local/media/fa18.adf
```

The build script configures `BUILD_TESTING=OFF`. To build integration tests:

```powershell
cmake -S port/recomp -B build/native-cmake -DFA18_NATIVE_ONLY=ON -DBUILD_TESTING=ON
cmake --build build/native-cmake --config Release --target fa18_native fa18_native_scene_exit_test fa18_native_record_expiry_test --parallel 8
Copy-Item build/native-cmake/native/Release/fa18_native.exe build/native/fa18_native.exe
```

Configure from **`port/recomp`**, not `port/native`. GNU reference/oracle builds
through `scripts/build_recomp.py` share `build/recomp/obj`; do not run those
builds in parallel. Independent MSVC build directories can run concurrently.

## What runs and what parity actually proves

Latest independent later-demo assessment: source 2401..2763/native 2364..2726
at ticks 222..584 matches all 363 named player/camera/phase/control boundaries.
Named motion/rates/pose/matrices and complete record cores match all 5,808
record instances after the cold-start default/level correction. All 363
boundaries have all sixteen cores matching. Together with takeoff, complete
cores match 9,376/9,376 instances across 586 independently aligned boundaries.
Strict two-page drawing passes 83/363 and still exits 1. At tick 508, the
source view-hold timer expires and requests a cockpit redraw earlier in elapsed
time; same-state/clock frame-body and original expiry-parent checks pass. No
mask, fitted timer or game behavior change was introduced. See
`analysis/native_later_demo_assessment.md`; compressed source captures are
retained under `build/native-flight/demo-later-review/`. Independent complete
sequences and seconds-driven drawing assessment remain open.

Native intro, credits, callsign editing, menu/mission/log selection and config
save/reload work. Demonstration, Free Flight and carrier qualification execute
native input, record dynamics, terrain/models, cockpit/HUD, display, messages
and audio. The original cockpit assets `pix/inst5` and `pix/frnt5` are loaded
and drawn. The earlier missing-cockpit issue was real and has been fixed.
SDL stereo output and `--wav PATH` use native PCM; bit-exact original audio
fetch/filter/interrupt timing has not been established.

The three recordings have functional outcomes: **3/3**. The historical strict
recorded-frame sequence acceptance remains **0/3**, but exact Amiga frame timing
is no longer a completion gate. No revised complete sequence assessment has
been performed yet. Do not relabel old failures as new passes or report a
branch's 100% completion as whole-game parity.

Current independent comparison evidence:

| Scope | Result |
| --- | --- |
| Five selected demo/carrier checkpoints | Both 320x200 four-plane pages, phase/controls and named player motion/pose/matrices match |
| Consecutive demo window, source 2401..2528 / native 2364..2491 | Player motion/pose/matrices, phase and controls match 128/128 |
| Same window, both complete drawing pages | 53/128 match, **41.4% of this window** |
| Five retained demo boundaries, ticks 222/272/273/322/349 | Named motion/rates/orientation/matrices agree for all 16 flight records; sampled evidence only |
| Original frame-body composition | Eight previously sampled ordinary bodies passed; latest active/crash/map rechecks pass |

Frame-body composition compares original code on the native body's starting
state and clock interval. It proves assembled behavior given those inputs,
not independent original task/display pacing or a complete recording.
Its scratch, asynchronous voice and blitter-busy exclusions are explicit in
`analysis/native_frame_body_milestone.md` and the oracle. Independent checkpoints
compare a narrower named state scope; record flags/counters and full RAM parity
remain separate requirements.

## Main unresolved gameplay-frame difference

The first independent window failure is **source update 2452 / native update
2415, game tick 273**. Twenty differing bytes in plane 3 at y=192 belong to the
target-info line: original **HDG:166**, native still **ALT:5110**. Later drawing
differences remain in that information-line region; the last two boundaries
agree again. This is a real HUD difference, not Copper fade.

The source path is C25312 / `begin_main_loop_timers()` -> C25482 / `tick_timer()`
on INFO_REQUEST (`C45886`) -> `message_line.c:info_line()` -> INFO_PAGE (`C459C4`).
It cycles ALT/HDG/SPD when sampled seconds advance. Across the retained window
the original timer advances **18,000 ms**, native **8,560 ms**.

Both select C2502E rate-table entry 15, **67 ms**, and use the same timer
arithmetic. No wrong index or arithmetic bug has been found. Native approaches
that rate limit; original rendering takes longer. `native/clock.c` samples
whole 20 ms PAL ticks; `native/display.c` currently yields WaitBOVP until the
next whole host PAL boundary. Original viewport/task/sub-PAL pacing remains
unverified, but reproducing that pacing is not required. A seconds-driven HUD
can show a different page at the same update count when elapsed time differs;
verify its timer and page transitions at equivalent elapsed game time before
calling this a gameplay defect. Do not add a guessed multiplier, fitted delay
or captured clock to reproduce the original machine's rendering cost.

C16D04 also publishes fractional time at C45AF6. C28782/C28CAE can use its low
word/byte to derive geometry offsets at C45B18/C45B1A. This is a possible later
clock dependency, but it does not cause this retained window's HUD mismatch.

**Next priority:** finish unsupported gameplay commands/modes and assess later
gameplay against equivalent source states/events using retained evidence first.
Check physics, collisions, AI, mission outcomes and source-defined timer behavior;
do not spend implementation effort reproducing Amiga rendering delays. Keep
strict frame comparisons as diagnostics, and separately classify differences
caused only by elapsed-time/presentation alignment. Do not blanket-mask HUD
differences. The two latest commits fixed genuine aborts; the unchanged 53/128
strict drawing result is diagnostic evidence, not a completion percentage.

## Latest completed commits

### Demo attached camera and extended matrix limits — 2026-10-07

The reported clipping just after demo takeoff came from passing an already
rotated vertex into C1F2EE's attached-camera projection. The original reloads
the unrotated model vertex at A1-6; native composition now does the same.
The barrel roll also occurs in the original playback and is preserved.
Source/native camera position and plane pose already matched. All 223
consecutive takeoff boundaries now match both complete drawing pages, camera
state and named player motion/pose/matrices (previously 153 drawing matches).
The independent comparator now includes named camera state; mutation tests
reject incorrect observer/camera matrices. Ten actual frame-body cases pass,
including three new outside-takeoff/roll/bank regressions.

Extended modes five through eight also exposed C2E048's signed-long coarse
clamp: native narrowed before comparing. The clamp now precedes narrowing.
576 complete matrix cases and all seven actual record checkpoints pass,
including the formerly failing mode-seven body. Normal probes match 232
input/stage intervals and 726 sampled bodies, including active postflight
dynamics. Mode five's player stays stationary under these inputs while other
aircraft fly; its probe requires observed non-player aircraft movement.
All eleven selected runtime gates, the final renderer regressions, full
native/reference builds and final frontend/link check pass. The public native
executable is refreshed. See `analysis/native_outside_camera_milestone.md`.
Artifacts are in `build/native-flight/demo-takeoff-review/` and
`build/native-flight/combat-probe/clamp-5` through `clamp-8`.

This accepts the compared takeoff segment and connected mission samples,
not complete independent flights or mission success. Remaining whole-game,
typed-state, audio and performance requirements stay active; the later
seconds-driven HUD comparison remains separately unaccepted.

### Keyboard flare/chaff and connected control effects — 2026-10-07

Real Free Flight F/C now consume stock, post source messages and reach C1518C's
launch/update/draw children through the actual scene. C159AE/C15AD4/C257EC
derive launch direction; existing collision/ground owners and source
C2CCA0/C2CD28/C2CD94 drawing are connected. Source C1AE02/C1AE08 defines the
keyboard depleted-stock carry byte; successful pending commands save their
own event. SHIFT-F mode 6 calls source sound 6.

Validation: 688 complete input-parent cases; 192 complete control-effect-parent
cases including visible lines/layers; four actual keyboard-driven native bodies
covering flare/chaff launch and early ground-contact expiry. All four bodies
match original compared gameplay state and every display byte. Active/crash/map,
queued input, three native integration tests, frontend/link omission and twelve
reference host/loading checks pass. No full original replay repeated. Component
and runtime evidence are distinguished in
`analysis/native_countermeasures_milestone.md`.

Build `fa18_native_countermeasures_test` with the native runner, then run
`python tools/native/check_countermeasures.py`. Reports/logs persist under
`build/native-flight/countermeasure-check/`; passing raw RAM is temporary.
First depleted recorder commands with an unclaimed queue remain unsupported.
Full gameplay goal remains active.

Follow-up collision connection: C266AE now consumes source C26CC0/C26D8A
typed returns; the component child reuses C27456's existing face-plane owner.
Expanded C1518C source comparisons pass 256 cases, reaching 256 component,
32 face and 128 plane child calls (32 hits / 220 misses). Four controlled
C1518C parents using actual native runtime objects also match original RAM
and display: two face hits, two component misses. Those exports bracket only
the control parent; they are separate from the four keyboard-driven full frame
bodies, which still pass. Motion-helper contract/real-child regressions and
frontend/native omission checks pass. See the milestone's follow-up section.

Recorder $FD follow-up: its inherited selection is dead to publication
(C1BCEE -> C1BEDA -> C1C23C), so function keys now proceed without aborting.
Complete input comparisons pass 848 cases, including 160 new $FD combinations;
two controlled actual native C0F3C4 parents match original compared RAM.
The `frame.fd.N` files bracket only input, separate from complete frame bodies.
Native menu/function-key/pilot-log regression checks pass.

Claimed-recorder follow-up (2026-10-07): C1C23C tests KEY_TAKEN before inspecting
the event release bit or touching the queue. Depleted flare/chaff commands can
therefore finish once a prior command has claimed input without reconstructing
their dead inherited event. Stock/messages and modifier clears retain their
original owners. All 2,512 complete input parents match original compared RAM,
including 1,536 new cases covering every stock byte for each effect and recorder
modes 1/2/3 with nonzero original incoming D4. Twenty-four controlled C0F3C4
parents using the playable runtime also match, including twelve sequences
where a successful flare claims input before depleted chaff. These parents
bracket input only; four ordinary keyboard-driven full bodies still match
gameplay and every display byte. No comparison exclusions changed. A first
depleted command with an unclaimed queue still requires its actual producer;
no carry value is guessed for that case.

### `d5203c52` — C0DA38 alternate selection and enclosing-frame exit

Actual call chain: frontend -> flight -> scene -> UPDATE_BUFFERS -> C0D730.
Bit $2000 selects C0DA38. It writes the four viewport corners at CORNER_RECORDS,
tests a signed threshold against 14400 and returns accept/reject. **Both**
outcomes unlink C0EFD4's frame on this path, skipping the remaining scene,
HUD, timers, counter and C32CEE final message; outer display still proceeds.
C0DA38 itself is not a page-presentation service.

`display_records.c:prepare_full_display_selection()` shares the existing
ordinary rectangle behavior. `submit_update_display_buffers()` now returns
`UpdateSequenceResult` so `owner_finished` is propagated. Flight returns
NATIVE_FLIGHT_OWNER_EXIT; frontend skips the final message and resumes display.

Validation: 18 original signed-threshold/mode/direct/enclosing-call cases;
native disk/input-backed frontend integration; complete original C0EFEA-to-exit
prefix, **5,253 instructions, zero compared gameplay/display differences**.
The controlled fixture is demo update 2365, tick 223, PAL 5189..5189.
It does not prove a particular player input naturally activates this branch.
See `analysis/native_scene_exit_milestone.md`.

### `311d09b8` — C22ADE destroyed-record expiry and target cleanup

C22AC0's reached destruction branch now sets timer +$4C to 15, clears $0200,
sets $0400, and calls existing C09DD0 cleanup before descriptor drawing.
Only a matching selected record is cleared; WARNING_CAUSES is masked with
$FFFFBDFF and C25704 posts **TARGET DESTROYED**, $4016. The historical
CONTEXT_PUBLISH_SELECTION_TONE hook name is misleading: it posts a message,
not an immediate tone. C22C70 is an empty source render hook. Repeated rendering
does not restart expiry or repost the message.

Twenty controlled source descriptor cases and a disk/input-backed native scene
test pass. The test derives the visible record from the actual render list;
the cockpit need not render the player's own exterior model. This verifies
rendering/cleanup, not a complete input-driven kill or collision sequence.
See `analysis/native_record_expiry_milestone.md`; its older remaining-C0DA38
statement is superseded by `d5203c52`.

### Prior comparison/input commits

- `e2cd4eab`: independent comparisons at equal **pre-input** boundaries and
  source-defined draw/display page roles. Previous key-release and physical
  buffer comparisons produced false mismatches. Verifier mutation checks keep
  HUD, input, motion, publication and wrong-frame comparisons strict.
- `85d302bf`: retained consecutive 128-update original window and native capture
  ranges; isolated the real HUD clock mismatch.
- `2f216caf`: complete source PAL mouse/button/input callback connection,
  including source wrap/clamps/bounds and waits. Existing audio, HUD, cockpit,
  stores, labels and frame-tail milestones are listed in the current handoff.

## Capture alignment and retained artifacts

Source dumps at C0EFD4 are **before input/stage**. Native `.entry.dat` observes
that same phase. `.before.dat` is later, at C0EFEA, after input/stage; do not
compare it to a source pre-input dump across a key edge. `.after.dat` normally
ends at C0F3C0. C0DA38 exits instead report `frame_owner_exit: true`; their
`.after.dat` is at the owner return. The normal body checker rejects this
alternate boundary; the oracle supports explicit `owner-exit` fixtures.

C2F558 selects the drawing table and C1612C publishes then swaps DRAW_PAGE.
Compare draw/display roles, not physical buffer numbering. Carrier source
draw page 1 corresponds to native page 0. Validate PAGE_PLANE_TABLE against
each run's own DRAW_PAGE; this is not permission to search for similar frames.

Native `--frame-capture FIRST+COUNT PREFIX` produces
`PREFIX.ITERATION.entry.dat`, `.before.dat`, `.after.dat`. For a single capture,
the iteration is omitted from filenames. Reference `FA18_LOOP_DUMP=FIRST+COUNT:PREFIX`
produces `PREFIX.ITERATION.dat`; its single form uses the supplied path.

Useful retained files under `build/native-flight/`:

- `gameplay-window-source.2401.dat` through `.2528.dat`: original pre-input
  boundaries. Their one bounded original prefix agrees with the previously
  retained final RAM/registers; no need to repeat it casually.
- `gameplay-window-comparison/native.2364.entry.dat` through `.2491.entry.dat`,
  corresponding body exports and `comparison.json`.
- `reference-demo2401.dat`: accepted against native entry 2364, tick 222.
  `reference-demo-initial.dat` and `reference-demo6000frames.dat` are additional
  reference evidence; the latter ends mid-phase and needs phase-aware handling.
- `reference-carrier5101.dat`, `reference-carrier5501.dat`,
  `reference-carrier6001.dat`, `reference-carrier-approach.dat`: accepted native
  entries 5101/5501/6001/6289, ticks 412/812/1312/1600.
- **`carrier-game-input.fa18in`**: corrected carrier consumed-key input,
  554 events, end update 8038. Earlier carrier input variants were wrong.
- `scene-exit-check/`: current controlled native `.before/.after` captures,
  original `.source.dat` and `capture.json` for the alternate-exit proof.
- `input-callback-aligned-frame2364.before.dat` and `.after.dat`: prior normal
  body fixture. **`input-callback-aligned2364.dat` is not a matching pre-input
  checkpoint**; it is a different frame phase.
- `sample-freeflight6100.png` and `.wav`: actual cockpit/audio review artifacts.

Sealed demo input: `captures/native/demo01/input.fa18in`, end 4892.
Sealed crash input: `captures/native/qual_fail_crashes/input.fa18in`, end 3082.
Original `captures/native/*/state.bin` files belong only to reference/oracle
loading; native starts from disk/input.

RAM exports contain 1 MiB (512 KiB chip + 512 KiB slow), sometimes followed by
72 original register bytes. Address offset is `a` below $80000, otherwise
`a - $C00000 + $80000`. Compare phases before interpreting any byte difference.

## Useful commands and latest validation

These are available checks, not instructions to rerun every check on resumption.

```powershell
# Reuse independent original checkpoints; starts native from disk/input.
python tools/native/check_gameplay_checkpoint.py --source build/native-flight/reference-demo2401.dat --iteration 2364
python tools/native/check_gameplay_checkpoint.py --source build/native-flight/reference-carrier-approach.dat --input build/native-flight/carrier-game-input.fa18in --iteration 6289

# Known HUD difference: currently exits 1 and retains every failure.
python tools/native/check_gameplay_window.py --source-prefix build/native-flight/gameplay-window-source --source-first 2401 --native-first 2364 --count 128 --out build/native-flight/gameplay-window-comparison

# Strictness using an existing accepted pair; no game run.
python tools/native/check_gameplay_comparison.py --source build/native-flight/gameplay-window-source.2401.dat --native build/native-flight/gameplay-window-comparison/native.2364.entry.dat

# Focused original/native alternate-exit proof; GNU builds sequentially.
python tools/native/check_scene_exit.py
ctest --test-dir build/native-cmake -C Release --output-on-failure -R '^fa18_native_(scene_exit|record_expiry)$'

# Ordinary assembled frame bodies and native/frontend omission gate.
python tools/native/check_frame_body.py --case active --case crash-flight --case map
python tools/native/check_frontend.py --runner build/native/fa18_native.exe --map build/native-cmake/native/fa18_native.map
```

Latest validated batch: native Debug/Release builds and four affected Release
CTests pass. The missile-kill checks compare the continuous impact-to-inactivation
interval; each original oracle build must run sequentially.

```powershell
ctest --test-dir build/native-cmake -C Release --output-on-failure -R '^fa18_native_(radar_kill|infrared_kill|frontend|artifact_cleanup)$'
python tools/native/check_mode_two.py --mode 8 --kill --missile infrared --out build/native-flight/infrared-kill
```

Latest log: `build/native-flight/infrared-ctest.log`. No independent complete
original mission replay was performed. Do not rerun this batch without a new
change, failure or unresolved concern.

The earlier scene-exit batch rebuilt MSVC `fa18_recomp` and passed twelve
reference host/loader tests:

```powershell
ctest --test-dir build/recomp-cmake -C Release --output-on-failure -R '^(fa18_host_|fa18_clean_machine|fa18_emulation_meter|amiga_)'
```

Logs: `build/native-flight/scene-exit-check.log`, `scene-exit-frame-check.log`,
`scene-exit-frontend-check.log`, `scene-exit-reference-build.log`.
These scene-exit logs describe historical validation.

## Other unfinished scope

Radar and infrared destruction through expiry accounting and inactivation are
now verified. Gun shoot-downs, complete successful missions, independent complete
flights, remaining callback contracts, typed state, audio fidelity and broader
visible-window/combat performance remain open. Later input-return milestones
supersede the old first-depleted-recorder limitation; do not restore guessed
inherited results or treat earlier missing-child claims as current bugs without
checking the connected caller.

Native flight includes mode eight. Mode two now passes all seven streams, wrap
and Escape return (30 input/stage intervals and 206 bodies); its source playback
loop supersedes the earlier spontaneous-failure deadline. Mode six passes the
natural three-loss/reset-exhaustion/menu-return/Free-Flight-relaunch scenario
(86 intervals and 237 bodies). See `analysis/native_natural_outcomes.md`.
Native page allocation now retains the original descending 320x200 plane
layout, independently confirmed in retained original demo/carrier RAM.
Earlier mode-two startup evidence is in `analysis/native_mode_two_milestone.md`;
`python tools/native/check_mode_two.py` reuses the actual runtime objects.

Digit 4's mode 125 now connects its source restore setup and reaches sustained
flight. Model command $120 retains C1FEE4's clock-controlled 18-byte skip;
the native record loop dispatches mode 125 to source scheduler C0A334.
The normal disk/input run reaches 2,211 scene/HUD frames, Escape/menu return
and re-entry. 55 actual input/stage intervals and 35 bodies match compared
original RAM/display. Mode-2 source regression and seven affected native
integration tests pass. No new comparison exclusions were added.
See `analysis/native_mode_125_milestone.md` and run
`python tools/native/check_mode_two.py --mode 125 --out build/native-flight/restore-check/original-check`.
Complete mode-125 outcomes and sequence acceptance remain open.

Next-mission digit 7 now uses the original pilot-log field to select mode 6
and runs briefing/context/aircraft setup into sustained flight. The normal
disk/input run reaches 401 scene/HUD frames. 38 actual input/stage intervals
and 27 sampled bodies match compared original RAM/display. The source
C0FECE transition sorts all lists from its saved A4=C29662 byte, independent
of the request mask; mode-2's already preserved choice remains intact.
C24FA4 message lookup, C29368 origin selection, setup engine/noise and
C0A15C's scheduler are connected. Eight affected native integration tests,
mode-2/mode-125 source regressions and current frontend checks pass.
See `analysis/native_mode_six_milestone.md`; run
`python tools/native/check_mode_two.py --mode 6 --out build/native-flight/mission-six/original-check`.
Mode-6 completion/outcome children, cancellations and variants remain open.
Mission-list F1 now runs normal mode 3 (recorder mode 0) through briefing,
source aircraft selection and flight. Both aircraft choices reach 969
scene/HUD frames and each passes 43 actual input/stage intervals / 29 sampled
bodies against compared original RAM/display. Source draw commands $118
(C1FE68 workspace selector) and $11C (C0CFB6 circle stream) are connected.
65,588 selector cases and 24 complete circle-stream parents match original
instructions; the transition preserves saved A4=C296EE's nonzero sort byte.
The model oracle now verifies the actual descending 320x200 plane layout,
replacing its obsolete ascending 256-row assumption. Ten affected native
integration tests and current frontend/frame-body/reference checks pass.
See `analysis/native_mode_three_milestone.md`; run the shared checker with
`--mode 3`, and with `--mode 3 --aircraft 2` for the second choice, using
separate `--out` directories. Mission completion/combat and variants remain open.
Mission-list F2 now runs normal mode 4 through briefing/context setup into
flight, reaching 2,507 scene/HUD frames. 39 actual input/stage intervals and
27 sampled bodies match compared original RAM/display. The native record
loop calls readable C09EC4 through its source mode-4 scheduler; C0FECE retains
saved A4=C296E4's nonzero sort byte. Thirteen affected native integration tests
and current frontend checks pass. No comparison exclusions were added.
See `analysis/native_mode_four_milestone.md`; run the shared checker with
`--mode 4 --out build/native-flight/mission-four/original-check`.
Complete mode-4 outcomes, combat and other variants remain open.
Mission-list F3 now runs normal mode 5 through briefing/context setup into
flight, reaching 2,312 scene/HUD frames. 39 actual input/stage intervals and
27 sampled bodies match compared original RAM/display. The native record
loop calls readable C0A002 through its source mode-5 scheduler; C0FECE retains
saved A4=C2968A's nonzero sort byte. Seven affected native integration tests
and current frontend checks pass. No comparison exclusions were added.
See `analysis/native_mode_five_milestone.md`; run the shared checker with
`--mode 5 --out build/native-flight/mission-five/original-check`.
Complete mode-5 outcomes, record-restoration branches and combat remain open.
Mission-list F5 now runs source mode 7 using an eligible saved-pilot fixture,
with 2,289 scene/HUD frames. The original disk pilot's availability byte at
offset $18 is zero and still rejects F5. Tests change only that saved-pilot
flag, then reopen through the normal loader before input. 42 actual input/stage
intervals and 29 sampled bodies, including C11078/C110A4, match compared original
RAM/display. Native C0A1E0 calls C1BEE8's readable record-view publication with
the source record-word event and STREAM_MODE; 256 complete publication parents
pass. Model command $FC/C21FA4 passes 64 complete parallelogram-tail cases.
C0FECE preserves saved A4=C296DA's nonzero sort byte. C110A4 uses typed host
locals, leaving gameplay RAM untouched by its private frame; the older result
oracle's six-byte scratch exclusion is removed and 39 result/restart parents
pass. No new comparison exclusions were added. Direct standalone placement
drivers on mode-7 snapshots fail at draw command $C3 in their controlled repeated
expiry fixture; original execution also fails there. That fixture comparison
remains unaccepted, alongside complete outcomes/combat and other variants.
See `analysis/native_mode_seven_milestone.md`; run the shared checker with
`--mode 7 --out build/native-flight/mission-seven/original-check`.
Mission-list F6 now runs source mode 8 with an eligible saved pilot, reaching
2,458 scene/HUD frames. The disk's availability byte at offset $19 remains zero
and still rejects F6. Only the saved-pilot fixture changes that byte, then reloads
through the normal loader before input. Native C0A364's existing readable
scheduler is connected; C0FECE preserves saved A4=C29702's nonzero sort choice.
38 actual input/stage intervals and 27 sampled bodies match compared original
RAM/display with zero differences and no new comparison exclusions. Eleven
affected native integration tests, including current frontend and menu, pass.
See `analysis/native_mode_eight_milestone.md`; run the shared checker with
`--mode 8 --out build/native-flight/mission-eight/original-check`.
Every numbered mission entry now has a connected sampled route. Complete mission
outcomes, reached combat/weapon children and other input/state variants remain
unfinished; startup gates do not measure whole-game completeness.
Normal mode-8 Shift-E now reaches source ejection/failure and returns to the
menu after 739 scene/HUD frames. 46 actual input/stage intervals and 43 sampled
bodies match compared original RAM/display. The reached flag publication,
programmed sound, action-record clone/view/matrix, C22B1A lifetime stream and
draw commands $E0/$100 are connected. 64 additional input parents and 128
additional geometry parents pass. No new comparison exclusions were added.
18 affected native integration tests, three default setup-model comparisons,
reference build and 12 host/loading checks pass.
See `analysis/native_ejection_milestone.md`; run the shared checker with
`--mode 8 --eject --out build/native-flight/ejection/original-check`.
Other input/state variants, successful outcomes and combat remain unfinished.

Whole gameplay sequences under the revised timing scope, complete
record/flag/counter state and post-result behavior remain unverified. Exact
audio fidelity is also unverified. The native link's omission
of emulation objects is real evidence for this runner's current connected scope,
not proof that every original game path has been ported. The gameplay goal must
remain active until its full requested end state is implemented and verified.

Normal mode-8 Return/Space now runs three firing probes: source weapon high
nibbles $30/$20 consume their respective missile stocks, and $10 consumes gun
ammunition. Draw $98/C21E08 has connected readable geometry. The gun path also
preserves C2CCA0's test/clear through the point child's returned plane-word
destination, fixing a one-pixel original-body mismatch. 138 input/stage intervals
and 112 sampled bodies match compared original RAM/display with zero differences
and no new exclusions. Geometry/point components pass 256/64 complete parents.
Actual runner probes reach 2,458 scene/HUD frames. All 21 affected native tests
pass. Default setup models,
countermeasure parents/four bodies, reference build and 12 host/loading checks
pass. These probes do not prove successful hits, full combat or mission outcomes.
See `analysis/native_weapon_firing_milestone.md`; the shared checker accepts
`--mode 8 --weapon 1`, `2` or `3`. The complete-port goal remains active.

Normal mode-4 throttle/stick flight now runs region placement/orientation,
zone exit, draw $B4/C21EF8, NPC missile launches and the player's hit/restart
sequence, reaching 5,970 scene/HUD frames. Native composition fixes the owning
aircraft input to the paired-missile readiness gate, the carrier's retained
whole-list completion flag and the sight tail's inherited viewer. 56 actual
input/stage intervals and 47 sampled bodies match compared original RAM/display
with zero differences and no new exclusions. Derived geometry passes 320
complete parents. The new shared-runtime test is `fa18_native_region_flight`;
run `check_mode_two.py --mode 4 --flight`. Default setup-model checks, reference
runner builds, all 22 affected native tests and 12 host/loading checks pass.
These sampled bodies do not
prove an independent whole flight, all collisions or mission outcomes. Audio
fidelity and the 20 ms target remain unaccepted; the complete-port goal stays
active. See `analysis/native_region_flight_milestone.md`.


Controlled postflight conditions now connect C0A3EA readiness, mode-four
C1BEE8 result view, mode-five C0A12E record restoration, reached C083A6 control
request and C110A4/C24FA4/C11350/C08ED0 result messages and pilot-log updates.
Normal menu startup precedes validation-only terminal flag/position seeding;
all subsequent frames use the actual shared runtime. Nine input/stage intervals
and 25 sampled bodies match compared original RAM/display, with unchanged
RAM/display masks. 63 derived result-parent cases pass, including signed level and
unsigned attempt limits and completion-count wrap. The ready fixture verifies
its 78-byte saved log equals the result table. C1643A remains the explicit shared
native save boundary in these result comparisons; its disk/status/readiness
gates remain to be integrated and verified. C539F4 is DOS Write despite older
load/read helper names. Normal mode-four flight still passes 56 intervals and
47 bodies. All 26 affected native tests, seven native-only host/loading checks
and both reference builds pass. Public build/native/fa18_native.exe is updated.
Run python tools/native/check_postflight_schedule.py; see
analysis/native_postflight_schedule_milestone.md. Normal-input successful
missions, whole independent flights, audio and 20 ms performance acceptance
remain unfinished; the complete-port goal stays active.


Native configuration calls now compose complete source C1643A status/readiness
policy. Enlistment runs C162E4/C0EF08/C16386/C1631C refresh/read/create through
existing Amiga host file services. The read-only OFS mount is retained for the
frontend lifetime; writes use the overlay, including MODE_OLDFILE=1005 behavior.
Source readiness/load-result fields are now initialized. Nine postflight
input/stage intervals and 25 bodies match original RAM/display with complete
game file owners executing and unchanged masks; the C1643A comparison bypass
is removed. 77 result/config parents match RAM and persisted bytes; 2,560
CPU/child-entry contract cases pass. Normal mode-four flight still matches 56
intervals and 47 bodies. Retained recorded carrier input completes native
qualification, persists success, restarts and reloads. All 28 selected native
tests, 12 host/loading checks, default models and reference builds pass. An
existing model probe incorrectly forced aircraft lifetime fixtures onto the
carrier; both previous native and original rendering fail that artificial
case. Normal carrier comparisons remain, and five expiry cases/11 descriptors
pass on a real demo aircraft record. Public native executable is updated.
See analysis/native_config_owner_milestone.md; full mission successes,
independent sequences, audio fidelity and 20 ms acceptance remain unfinished.
The complete-port goal remains active.

Delete callback follow-up: host Delete maps to raw $46; C06BF0 removes and
reinstalls the input callback through existing C1748C/C17456 owners, preserving
mouse state. 976 full input-parent comparisons include 64 new reset cases; 16
additional menu command parents pass. Actual Free Flight Delete/Escape/restart
matches 57 input/stage intervals and 37 sampled bodies. The eligible-pilot
validation fixture now establishes source file readiness by entering
enlistment before saving, then verifies reload. A fresh mode-eight probe exposed
that cached eligible pilots concealed this fixture error in the previous
28-test run. Six fresh pilot runs (seven/eight, ejection and three weapons)
match 264 intervals and 211 bodies. Full native build and six selected native
gates pass. See analysis/native_callback_reset_milestone.md; full gameplay,
audio and performance acceptance remain open. The complete-port goal stays active.

Sustained native mode-six/eight flight now consumes C2C348 -> C06C02 and resumes
the existing readable guidance owner. Original-body checks also correct target
marker returned coordinates, signed attitude publication, retained scaled
matrix divisor and settling from previous roll. Normal throttle/stick/target/
fire probes match 114 input/stage intervals and 465 sampled bodies with unchanged
RAM/display exclusions, including three original guidance-child returns in
mode eight. Mode-four region flight still matches 56 intervals and now 49 bodies.
Component checks pass 512 point-return cases, 128 signed-attitude parents, 512
matrix transforms, 256 settling tails and 3,584 affected projection CPU contracts.
All 36 selected native gates, seven record checkpoints, default models and
native/reference builds pass. Final frontend/link checks pass and the public
native executable is refreshed.
The shared-runtime gates are fa18_native_combat_6 and fa18_native_combat_8; use
check_mode_two.py --mode 6/8 --combat. See
analysis/native_guidance_limits_milestone.md. Full mission successes,
independent complete sequences, readable typed state, audio fidelity and
performance acceptance remain unfinished. The complete-port goal stays active.
