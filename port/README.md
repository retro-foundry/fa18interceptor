# Active port source ownership

Bounded radar history (2026-10-09): ordered original crosshair, cache erasure
and marker writes predict all 70 complete owner page sets across 35 observations.
Actual instrument bitmap redraws explain all requested plane XOR bytes
(944,000), including the later background-bit interaction. Negative controls
and the existing five-frame delta check pass. Runtime is unchanged; complete
flight drawing remains 287/4,967, with message differences still open. See
[paint history evidence](../analysis/native_radar_paint_history_milestone.md).

Free Flight model crash fix (2026-10-09): valid command $90 at C3BA04
now derives the original six-point block through C20F78; its shared $94
entry is also connected. 184 complete command cases and 32 actual-model
camera poses match original instructions. Release/Debug model, flight-start
and preallocation checks pass; canonical Release is refreshed. Precise
reported flight route is unsealed. See [crash evidence](../analysis/native_free_flight_six_point_block_fix.md).

Current whole-flight allocation/cockpit check (2026-10-09): Release/Debug
preserve all qualification/mission-three traces, final RAM, counters and
earned saves, with zero gameplay heap violations. Five bounded complete
page deltas match selected-radar marker history. Corrected original owner
returns match all pages in both bounded windows (5/5 and 11/11); the previous
end address included the next HUD child and is now rejected. Bounded pixel
interaction is now explained above; broader drawing stays open. Runtime is unchanged. See
[current evidence](../analysis/native_cockpit_radar_milestone.md).

Original voice ownership (2026-10-09): reference tracing now resolves the
loaded sound hunks, correcting false empty voices in a separate launch with
a 480-byte relocation. Complete original Demo and separate-launch execution,
PCM and register writes remain exact. All 48 initial music requests match
native sample bytes/order/period/volume. Original right-channel onset follows
left by 67 samples; native starts together, so onset/whole-flight sound remain
open. Native executable and gameplay storage are unchanged. See
[voice and handoff evidence](../analysis/native_original_voice_layout_milestone.md).

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
cleanup remain separate. See [filter integration evidence](../analysis/native_pcm_filter_milestone.md).

Original filter evidence (2026-10-09): the existing LED interface reports
filter enabled at all 21,069 Demo boundaries, preserving complete original
PCM/execution and every earlier event row. Two independent cold starts also
produce 13,230,014 stereo frames matching unchanged A500/LED filter functions
exactly, including the reference mixer gain; non-silent output and every
sample are checked. Wrong filters and corrupted LED traces are rejected.
Native executable is unchanged; filter integration, onset/handoff alignment
and whole-flight sound acceptance remain open. See [filter evidence](../analysis/native_original_filter_state_milestone.md).

Native PCM averaging (2026-10-09): playable output now uses original source
interval averaging instead of sample holding, with fixed integer storage.
196,608 intervals match unchanged original accumulator functions in both
builds. Intro, Free Flight and final combat preserve complete game state;
Debug/Release agree on the intentionally changed PCM. Complete Demo requests,
stops, voices and stream phases remain exact, with zero heap violations.
Canonical Release is refreshed. Original onset/handoff alignment and Amiga
filter acceptance remain open; broader completion and state cleanup scopes
are unchanged. See [averaging evidence](../analysis/native_pcm_averaging_milestone.md).

The playable native audio trace now covers complete Demo requests/stops and
per-frame voices, preserving all PCM/state/counters. Release/Debug rows agree
with zero heap violations; tracing and preallocation checks pass. Source/native
onset/handoffs and filtering remain open. See [trace evidence](../analysis/native_audio_trace_milestone.md).

The complete original demo now has a validated PCM recording: all 21,069
reference calls preserve original execution and complete sample coverage.
Their 406,753-row audio-write/voice trace also preserves full state and PCM.
LED/filter pin is known at the retained endpoints only.
Native onset/handoffs and interpolation/filter fidelity remain open; the native
executable is unchanged. See [audio evidence](../analysis/native_original_audio_recording_milestone.md).

Gameplay storage is reserved before the playable frame loop. Project heap and
buffered-file open/close calls are guarded; SDL uses a fixed startup arena,
PCM a fixed ring, and pilot/capture I/O direct OS handles. Release/Debug preserve
complete native PCM/state/saves. OS/driver heaps are outside the measured scope;
original RNG/timing remain unchanged. See [evidence](../analysis/native_preallocation_milestone.md).

Visible qualification/mission-three playback now finishes with real audio,
exact RAM/counters/earned save, no resets and all 42,706 frames presented.
Recorded controls are isolated by an opt-in diagnostic. Two SDL poll stalls
exceed the 20 ms work budget, so performance acceptance remains open. All rows
remain counted. Other missions/views/combat are separate. See
[visible evidence](../analysis/native_visible_replay_input_milestone.md).

Complete mission-three drawing localization (2026-10-09): all 4,967 observations
match rows 0..127 of both complete pages. Optional adjoining bands preserve
every earlier trace, page, final RAM byte, counter and save in Release/Debug.
The first row-191 difference is sliding target-info text at the same offset.
Strict pages remain 287/4,967; later cockpit drawing and broader completion
remain open. See [localization evidence](../analysis/native_mission_drawing_bands_milestone.md).

Complete mission-three message timing (2026-10-09): all 4,964 real transitions
obey original elapsed/countdown, delay/redraw, full text and colour-cache rules,
including 1,362 paused HUD periods. Nine priority-message producer fields match
at all 4,967 observations. Both first HDG events follow two sampled-second
changes (original tick168/native185); this is assessed under the accepted
cadence policy. Release/Debug agree; nine wrong results are rejected. Optional
read-only fields preserve all earlier traces, pages, final RAM, counters and
saves. Strict pages remain 287/4,967; other drawing and completion work remain
open. State cleanup stays deferred. See [message evidence](../analysis/native_mission_message_timing_milestone.md).

Ground-strip startup fix (2026-10-09): native now calls original C2527C,
generating all 40 disk-backed strip corner records before gameplay. Missing
corners caused empty ground draws and later placement skips, consistent with
the Free Flight disappearing-road report. Initializer RAM and all 240 corners
match original instructions; the omitted-call regression is rejected.
Release/Debug mission-three gameplay remains exact; first missing-ground-line
pages now match and strict drawing improves 267 -> 287/4,967. The next strict
difference is observation 22,303. Free Flight callback/restart, connected model,
frontend and new CTests pass. Other drawing remains open; state cleanup stays
outside this goal. See [ground evidence](../analysis/native_ground_bounds_startup_milestone.md).

Native raster addressing (2026-10-09): negative-X lines now use the original
logical-shift starting offset and aligned word address. The connected raster
checks pass all existing cases and 32 new negative-X cases per checkpoint.
Release/Debug complete mission-three gameplay, traces and RAM remain identical
to the preceding evidence; strict drawing stays 267/4,967. A bounded capture
localizes the first visible difference to a skipped ground segment and a
different placement cache; its earlier cause remains open. Canonical Release
is refreshed. State cleanup remains deferred outside this goal. See
[drawing evidence](../analysis/native_negative_line_and_drawing_window_milestone.md).

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
[execution evidence](../analysis/native_independent_mission_execution_milestone.md).

The original and independent native now complete mission three with gear
raised after takeoff, lowered before runway landing, grade and menu return.
The complete original recording reproduces without its controller; native
Release/Debug agree. Whole-flight state parity remains unaccepted after a
repeated original entry at tick 360. Strict comparisons and verified snapshots
retain the failure. Gameplay and Escape are unchanged. See
[success recording](../analysis/native_independent_mission_success_milestone.md).

Independent mission-three failed flight now matches all 5,216 aircraft cores
and camera/control/target state through its first crash/reset callback. The
original recording reproduces all 22,467 boundaries without its input pilot.
Native uses an ordinarily enlisted, earned pilot at the same scene level zero;
Release/Debug agree. Drawing matches 259/326 and 67 failures remain unassessed.
Successful all-mission whole flights and other completion work stay open.
Gameplay and original Escape are unchanged. See [recording evidence](../analysis/native_independent_mission_recording_milestone.md).

Independent complete qualification now matches all 43,648 record cores and
camera/control/target fields across 2,728 first-flight boundaries; 2,722
complete drawings match. Nine result/restart callback runs preserve all
1,611 distinct gameplay states. Six message differences follow the original
expiry/blink cadence at a one-step phase offset, with 83 observed states,
267 original-owner checks and four rejection probes. Release/Debug agree.
Initial pilot contexts differ, and second-flight/menu observation limits
remain explicit. Gameplay and original Escape restart are unchanged;
all-mission independent flights and other completion items remain open.
See [qualification flight evidence](../analysis/native_independent_qualification_trace_milestone.md).

The independently recorded target-info episode now obeys original clock,
countdown, context/redraw and complete-text rules across all 374 transitions.
This assesses the reported ALT/HDG difference under the accepted cadence policy.
V2 fixes a diagnostic mouse field label and observes true target selection;
Release/Debug agree and eight selected CTests pass. Strict whole-flight drawing
and all-mission comparisons remain open. See
[HUD timing evidence](../analysis/native_demo_hud_timing_milestone.md).

Initial startup and qualification-entry sorting are verified. The real C0F812
caller frame and complete C08F26 bootstrap match native lists, retained output
and compared RAM. Release/Debug agree on 22 combined intervals, 60 lists and
40 factor probes; frontend/demo regressions pass. Optional startup observation
does not change gameplay. See [startup evidence](../analysis/native_initial_startup_sort_milestone.md).

Current acceptance (2026-10-09): repeated individual mission tests now replace
the uninterrupted campaign gate by explicit user direction. Cold save/load
boundaries are allowed; retain actual earned results and objective, landing,
save and menu-return checks. Older uninterrupted-tour requirements below are
superseded. Independent original whole-flight parity and other completion work
remain open.

Repeated individual mission acceptance now passes: current Release and Debug
earned cold tours agree on all nine stages, counters and 78-byte saves. Fresh
Release qualification/final source comparisons also pass for both supported
pilot contexts; ten selected checks pass overall. See
[individual mission evidence](../analysis/native_individual_mission_acceptance.md).

Actual Demonstration drawing order now matches its original caller; its native
menu transition sorts all three lists. Mode assertions corrected a fixture that
had selected qualification under the Demonstration label. Qualification entry
also passes. Release/Debug agree on 12 intervals, 30 lists and 20 factor probes;
frontend/demo regressions pass. Initial startup is now verified above; other completion
items stay open. See [mode audit](../analysis/native_menu_sort_mode_audit_milestone.md).
The earlier sorting summary below is historical.

Menu/restart retained sorting now has direct original-caller comparisons:
eleven intervals, 27 sorted lists and eighteen incoming-factor probes agree.
Release and Debug pass; gameplay and playable binaries are unchanged. Reached
paths replace the factor through templates or uncached distances. Initial
startup and rare callers remain open. See
[menu/context evidence](../analysis/native_menu_context_sort_milestone.md).

PCM playback now retains frontend-owned host buffer spans rather than resolving
an addressed game byte for every sample. Three ordinary-key audio/state/pixel
replays and the full earned tour preserve their previous results. Original
sample/voice comparisons and ownership checks pass. Voice/request state still
uses the source arena; full typed-state migration and original recorded-audio
fidelity remain open. See [PCM ownership evidence](../analysis/native_pcm_buffer_ownership_milestone.md).

A newly enlisted pilot now earns qualification and all six menu missions,
including final aircraft objective, carrier wire landing, saved sixth result,
menu return and cold Next Mission wrap. The submarine remains active at
objective admission; a visible explosion is not required. Gameplay is unchanged.
The new per-mission gates compare sampled original instructions and playable
saved bytes. `fa18_native_new_pilot_tour` starts with an empty save directory
and carries only actual game saves across cold starts. Independent original
whole flights and uninterrupted single-process tour checks remain open; see
[native_new_pilot_tour_milestone.md](../analysis/native_new_pilot_tour_milestone.md).

Earlier acceptance notes below describe the preceding milestones.

The active final success gate now uses availability earned by normal-key rescue
and cruise saves from the original ADF pilot. It completes the aircraft
objective, carrier landing, save, menu and cold wrap with matching sampled
original comparisons and playable results. Gameplay is unchanged. See
[`../analysis/native_final_mission_earned_availability_milestone.md`](../analysis/native_final_mission_earned_availability_milestone.md).
The newly enlisted tour is accepted above; independent whole flights remain open.

Intercept Incoming Cruise Missile (F5/internal mode seven) now completes
interception, carrier deck/wire landing, save, messages, Escape/menu and cold
reload through ordinary keys. The serial `fa18_native_mission_7_sequence`
compares 42 intervals and 171 bodies with original RAM/drawing; the playable
replay agrees on saved bytes/menu. Availability comes from the normal-key
rescue save, now reproduced by its existing gate. See
[`../analysis/native_cruise_mission_sequence_milestone.md`](../analysis/native_cruise_mission_sequence_milestone.md).
Gameplay and comparison masks are unchanged. All six menu missions have
successful routes; independent original whole flights remain open.

Search and Rescue (F4/internal mode six) now completes its pod objective,
carrier deck/wire landing, save, messages, Escape/menu and cold reload with
ordinary keys. The serial `fa18_native_mission_6_sequence` compares 44 intervals
and 193 bodies with original RAM/drawing; the playable replay agrees on saved
bytes/menu. See [`../analysis/native_rescue_mission_sequence_milestone.md`](../analysis/native_rescue_mission_sequence_milestone.md).
Existing readable game owners are unchanged. The initial pilot is the original
ADF log; port-earned tour availability and independent whole flights remain
open; internal mode seven is accepted above.

Latest final-mission correction: the native record scheduler preserves C230B0's
returned selection word through C0A364/C0A3A6. Ordinary controls now complete
the final aircraft objective, carrier landing, save, menu return and cold Next
Mission wrap, with sampled original comparisons and matching playable output.
See [`../analysis/native_final_mission_sequence_milestone.md`](../analysis/native_final_mission_sequence_milestone.md).
The original success fixture was explicitly unearned; the active gate now uses
the earned log above. The earlier patrol diagnostic retains its unearned log.
Independent original whole flights and full-port acceptance remain open.

Latest connected fixes: demo attached-camera projection avoids rotating the
model vertex twice; 223 independent takeoff boundaries match both drawing
pages, player motion and camera state. The shared coarse angle clamp now uses
source long arithmetic; extended mission samples match 232 intervals/726
bodies. See [`../analysis/native_outside_camera_milestone.md`](../analysis/native_outside_camera_milestone.md).
Full game acceptance remains unfinished.

Latest direction (2026-10-06): `fa18_native` reuses the game source with a native
intro/menu entry, mission/log selection and connected demonstration/Free
Flight/carrier qualification gameplay, omitting CPU,
translations, glue and chipset objects. Build
with `python ../scripts/build_native.py` from this directory, or
`python scripts/build_native.py` from the repository root. See
[`native/README.md`](native/README.md). The historical restriction below is
superseded for this explicitly requested runner; the deleted implementation
remains retired.

The sustained mode-six/eight guidance continuation and projection/matrix fixes
reuse `game/` owners through `game/native/`. Their shared-runtime comparisons
and remaining whole-game limits are recorded in
[`../analysis/native_guidance_limits_milestone.md`](../analysis/native_guidance_limits_milestone.md).

The active game implementation is `game/`, with temporary CPU adapters in
`game/glue/`. The playable build is defined in `recomp/CMakeLists.txt` and
`../scripts/build_recomp.py`; it produces `fa18_recomp` and `fa18_romfree`.

The abandoned top-level `fa18_port` implementation, its build definition,
contract tests and disconnected native replacement modules have been removed.
Do not recreate that implementation or use historical standalone test counts
as evidence of progress in the playable runner. Previous sources remain in git
history. Some historical analysis and oracle tools reference those removed
sources and cannot be run against the current tree.

All 48 retained shared C source/header files now live in `game/`; no C sources
or headers remain at the top level of `port/`. Build and include paths use the
new locations. These retained dependencies are:

| Files | Active use |
| --- | --- |
| `game/disk.c/.h`, `game/hunk.c/.h` | Original ADF/OFS reading and Hunk parsing, used by `amiga/` loading |
| `game/map_packet_*.c/.h`, `game/map_detail_*.c/.h` | Shared map-packet core called by `game/map_packet.c` and its glue |
| `game/projection_packet.h` | Map-packet depth/result types |
| `game/command_types.h`, `game/context_command_types.h`, `game/flight_command_types.h` | Types included by the active command implementation in `game/` |

Build from the repository root:

```sh
cmake -S port/recomp -B build/recomp-cmake
cmake --build build/recomp-cmake --config Release
python scripts/build_recomp.py
python scripts/build_recomp.py --romfree
```

Removing the abandoned source does not remove the active runner's remaining
Musashi, guest-memory or chipset dependencies. Their removal must happen through
the actual game/runtime call graph.

`game/native_call_graph.json` records C-only ownership for retired CPU entry
adapters. Tooling checks known original callers and actual C call sites before
excluding these entries from deferred/unported lists. This inventory is scoped
to discovered calls; runtime edge counts in profile `_native_edges` demonstrate
exercise in the fixed suite. Neither proves whole-game CPU independence.

Headless frame diagnostics use `--rgb444 OUT.bin --index8 OUT.index8`. The
index stream contains one selected palette index per 320x256 pixel per frame,
including during black fades; it does not change guest state or the RGB stream.
`../scripts/compare_recomp_frames.py` ignores only same-index colours belonging
to the original game's 16-stage Copper fade table at C08510. It checks drawing
indices, all other colours and frame counts. The live gate, timing probe,
comparison image report and emulation meter use that policy. Python comparison
tools and their regression check require NumPy; image reports also use Pillow.

The active emulation meter uses schema 2. Its instruction total includes
`interpreted`, `generated`, `residual` and `adapter`: handwritten CPU steps
fetching original opcodes still count as emulation. Retained C scheduling
without an opcode fetch does not count. Schema-1 percentages omitted adapters
and are superseded. The canonical meter report states whether its scenario
coverage is partial or the full acceptance suite. Raw counts with failed parity
do not establish accepted independence; subsystem omission builds remain the
completion gate.
