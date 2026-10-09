# Native port handoff — 2026-10-09

## Current restart summary — 2026-10-09

Latest acceptance direction (2026-10-09): the user waived the uninterrupted
qualification-plus-six-mission requirement. Repeated individual mission tests
with earned saves, objectives, landing, results and menu returns now qualify
that part of completion; cold starts are allowed. The continuous-campaign
driver and accepted five-mission prefix remain diagnostic evidence. Its final
flight failure is no longer a completion blocker. This overrides every older
uninterrupted-run requirement below. The latest failed bank/height input
experiments were reverted. Independent original whole-flight comparisons,
startup sorting, original recorded sound/filter fidelity, visible performance
and named-state cleanup remain outstanding.

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
requirement by the latest explicit user direction. Initial startup sorting,
audio recording fidelity, visible performance and named-state migration remain
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
