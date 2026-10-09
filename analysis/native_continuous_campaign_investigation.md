# Continuous campaign and visible performance investigation - 2026-10-09

## Superseding acceptance direction - 2026-10-09

The user explicitly waived the uninterrupted campaign requirement, accepting
repeated individual mission tests instead because final-mission spawn variation
makes the continuous route difficult. Cold starts with actual earned saves are
allowed. The completed five-mission single-process prefix remains useful
evidence, but final-flight success in that same process is no longer required.
This overrides the older campaign requirements and next steps below. Visible
performance, independent original whole flights and other acceptance work remain.

Two subsequent ordinary-key bank/height experiments were unsuccessful and were
reverted. The banked-turn diagnostic reached two enemy expiries but touched down
before completing combat and then exhausted fuel. Adding height control during
turns instead ended in player destruction at tick 24046, fuel 6214047. These are
test-pilot failures, not new evidence of a native runtime defect. Their bounded
reports are in `build/native-flight/campaign-suffix-final-bank-turn/` and
`build/native-flight/campaign-suffix-final-bank-height/`. No experimental control
changes or early-failure changes remain in the test driver.

The renewed individual qualification uses the existing Release/Debug mission
sequence gates and earned cold tour, including original and newly enlisted
pilot contexts where supported. Fixed-input repeats show reproducibility;
they do not establish exhaustive coverage of random spawns.

Completed follow-up: current Release and Debug earned cold tours pass with all
nine stages exactly equal, including runtime counters and complete saves. Fresh
Release qualification/final source comparisons pass in both pilot contexts;
ten selected checks pass overall. Repeated individual mission qualification is
accepted. See `native_individual_mission_acceptance.md` and its checkpoint.
The visible-performance tool's continuous-report prerequisite must be replaced
with validated individual geared routes; the waived campaign is not required.

This is work in progress, not an acceptance milestone. Completed sort fix and
its Release/Debug evidence are committed as `1dd08521`; all files described here
remain uncommitted pending validation. The complete-port goal remains active.

## Continuous campaign

`fa18_native_campaign_session_test` links the existing playable runtime and opens
one frontend. It records ordinary host inputs, observes flight/result state
without writing it, renders audio like the runner, and checks each saved log.
`check_campaign_session.py` first obtains the actual normally enlisted save from
the runner. Qualification and every intended mission then run in one process.
Enlistment setup is outside that uninterrupted flight campaign. An optional
`--live-enlistment` keeps enlistment inside it, including the original ADF's
retained startup difficulty. No config bytes or flight state are fabricated.

The source advances an existing pilot's tour word on startup without immediately
saving it. Only the initial checkpoint permits that source-defined word advance;
all earned-result checkpoints compare the entire actual 78-byte file with RAM.
Qualification uses the sealed ordinary input and answers the original empty-code
prompt only if it is active. Shift+Escape returns to the actual menu; modifiers
stay held until the original queued press is consumed. Plain Escape can briefly
expose a menu while retaining the mission and restarting it, so acceptance
requires C0FCB4, selected mode zero and empty input.

Observed checkpoints from `campaign-session-projectiles/driver.log`:

| Checkpoint | Host tick | Result |
| --- | ---: | --- |
| Normally enlisted save | 9000 | Unqualified, zero completions |
| Qualification | 25726 | Original successful qualification saved |
| Mission 3 | 50818 | Objective, runway landing, stop, grade one, saved count one, menu mode zero |
| Mission 4 | 76223 | Objective, carrier wire, stop, grade one, saved count two, menu mode zero |
| Mission 5 | 128100 | Failure phase F0 followed by C0F920; no completion |

The campaign's retained difficulty rises between missions, unlike the older
cold-start routes. Early escort attempts exhausted fuel; starting from the real
enlisted save allows escort completion at difficulty one. Mode five runs at
difficulty two. Its proximity countdown reaches -1, but a missile misses, the
opponents remain active and eventually land. The test pilot can circle above
those records without firing again; their expiry count stays zero. Failure F0
returns through C118FC/C11934/C0F920. This is not evidence of a native transition
fault. A lower-throttle experiment conserved fuel but could not close formation;
dry maximum also lost flying speed. No gameplay or physics rule was changed.

Current run: `build/native-flight/campaign-session-legacy-five/`. Mode five uses
the pre-existing accepted `force_return`/complete-flight controller, with
`tour_flight` and `campaign_flight` disabled for that mission. Other missions use
the opt-in campaign controller, which can apply ordinary defenses and select
available weapons. Shared default fixture behavior remains guarded from these
changes. The executable target builds; its campaign CTest is deliberately not
registered until all six flights and the actual playable replay pass.

Update after that run: legacy mode five succeeds, then rescue and cruise also
succeed in the same frontend. Saved menu checkpoints are mode five at 100581,
mode six at 123474 and mode seven at 143480, with completion counts three,
four and five. Mode eight crashes at 158188 before any enemy expiry. No whole
campaign or playable replay is accepted.

Latest user direction: retract gear after takeoff and lower it before landing.
The user manually operated gear in the visible mission-three measurement and
identified gear-down flight as a possible cause of excessive fuel use. Source
G/raw $24 toggles C46200 bit seven; set means retracted. The airborne speed-limit
owner reduces its limit when this bit is clear. Qualification's sealed input
already contains gear operations at recorded iterations 1603 and 5647.

`MissionPilot.manage_gear` is now enabled in the campaign driver, including the
legacy mode-five branch. It sends ordinary G events after leaving the surface,
releases after two host ticks, observes the resulting source flag, and lowers
before the return approach. The checker requires actual gear-up flight and
gear-down landing per mission. Default fixtures remain unchanged because this
flag is opt-in. No native runtime gear, drag or fuel code changed.

The first geared run completes mission three but misses the escort's carrier
approach, coming to rest outside its wire region. It stops with no accepted
escort result. Current run `campaign-session-gear-return/` lowers gear at return
start, allowing slowing before the final approach. Its process is active; inspect
the streamed `driver.log` before launching another campaign. The earlier
gear-down mode-five tuning is inactive under the accepted legacy branch and
should be removed if no longer needed by the eventual validated pilot.

Update: lowering gear at return start succeeds for both mode three (saved/menu
tick 50865) and escort (75334). The legacy mode-five attack then crashes at
90083 with gate -1 and no expiry. The campaign combat controller stays aloft
with gear raised and much more fuel remaining, but circles a landed opponent
without a launch opportunity. The latest candidate reads the actual landed
record and uses a straight outbound leg until 12,000 units away, then an inbound
attack. This only chooses ordinary keys; no position, target, hit, expiry or
fuel value is written. Current run is `campaign-session-gear-ground-pass/`.
Gear lowering is now observed while still airborne after the observed retraction,
so ground contact alone cannot satisfy the landing-gear check. This candidate
builds in Release; complete validation and Debug remain pending.

The ground-pass attempt ends in a player crash at tick 93940. Ignoring the
already falling enemy in the next `campaign-session-gear-wreck-wait/` attempt
keeps the player airborne to tick 140334, with fuel 4,235,136 and no reset, but
does not complete the objective: enemy expiries remain one of the required two.
C25B66 (`flight_dynamics.c`) increments C458AB only if the expiring aircraft
is airborne. Destroying the already landed opponent therefore cannot complete
this mission. This is an original rule, not evidence of a missing transition.
The next `campaign-session-gear-radar-pass/` attempt uses the previously accepted
radar intercept lead/launch choices with the campaign attitude controller;
it must destroy both opponents before landing. Acceptance remains pending.

The user's fuel observation is consistent with the source. Gear down reduces
the airborne +78 performance term in `control_records.c`, which participates
in +6E motion publication. `indexed_record_update.c` consumes $600 fuel per
afterburner update; ordinary thrust uses the absolute signed phase divided by
two (60 at phase 120). Gear does not directly alter those fuel deductions.
Its fuel effect is indirect through the extra thrust required by the pilot.
The gear-managed attempt retains much more fuel; this is not an isolated,
equal-trajectory fuel-consumption measurement.

`campaign-session-gear-radar-pass/` reaches two airborne enemy expiries and
the objective at tick 91481. It remains alive with ample fuel but flies straight
after the FF-to-one result transition: the source clears held input and the
mode-five pilot had not reapplied it. The campaign-only return handoff now
releases/represses ordinary controls on phase changes, as the already accepted
escort pilot does. Current run: `campaign-session-gear-return-handoff/`.
The preceding incomplete run is a failed pilot route, not accepted campaign
evidence. Latest code needs Release/Debug completion and playable replay.

That input handoff reaches successful geared mode-five and mode-six saves/menu
returns at ticks 103321 and 125454. Qualification and four missions therefore
pass in one frontend. Mode seven's initial radar missile misses the low cruise
target and the original reports failure; fuel is still available. The pilot's
campaign height floor had held it at 2,100 units while the cruise target flew
at 176, preventing another aligned pass. Current input selects the usual
400-unit margin for this low target (`campaign-session-gear-cruise-height/`).
The driver now stops promptly on original failure phases FE/two rather than
running the rest of the cap after a known failed mission. This is diagnostic
efficiency, not a bypass of a mission condition. Full campaign remains pending.

The lower cruise approach also misses and produces FE at tick 139018, fuel
4,720,481. Original radar guidance reaches the ground close to the moving target;
the subsequent heat shots also miss. The earlier accepted gear-down route hit
on its near-zero-bearing launch at 134952. The current geared attempt restores
the established 2,000-unit height margin and tightens launch bearing from .035
to .005 radians (`campaign-session-gear-cruise-alignment/`). These remain pilot
key choices; no missile guidance, hit test or failure outcome is changed.

Tighter cruise alignment still misses; FE arrives at tick 139018. The next
input fallback selects the original gun after an inactive missed radar missile
and uses the existing gun-aiming controller for record four. It follows the
low target but has no hit before failure in `campaign-session-gear-cruise-gun-held/`.
Current `campaign-session-gear-cruise-gun-aim/` adds bounded read-only aiming/
fire-state diagnostics. No acceptance is claimed. The preceding `...-gun/` run
was stopped early to correct a weapon-choice feedback bug in the test pilot.

The aiming diagnostic confirms normal fire alignment, but its range/absolute
speed estimate does not account for the receding target. An intercept estimate
using observed velocity also fails before FE at tick 139018. Further read-only
gates show COMMAND_WORD=8 and gun budget falling 296 -> 185 while fire remains
held: the gun is firing, and the remaining problem is achieving a hit, not an
empty magazine or blocked Space command. Current output is
`campaign-session-gear-cruise-gun-gates/`. A subsequent ordinary-throttle
closing approach (`campaign-session-gear-cruise-close-gun/`) earns the original
cruise objective at tick 138725 but crashes during the low-altitude return
handoff. This is an objective observation, not a saved mission completion.
No test is running now.

## Verified playable prefix

The larger Release prefix now passes qualification and five geared missions in
one actual `fa18_native` process. Ordinary radar/heat missile retries finish
cruise interception, carrier wire, save and menu. At tick 146862, the complete
78-byte saved result matches the observing driver, five grades/completions are
earned, menu is C0FCB4/mode zero, and no crash reset or pending input exists.
Evidence is `figures/native_geared_five_mission_prefix_checkpoint.json`, with
inputs/logs under `campaign-session-gear-five-playable/`. The driver executable
hash was not sealed before a later telemetry rebuild; it is deliberately not
claimed in this checkpoint. Replay/runner/ADF/save hashes are sealed.
Final combat still crashes before an aircraft expiry; full six-mission and
five-mission Debug acceptance remain open.

The existing recorded keys through rescue were independently replayed in one
actual Release `fa18_native` process with an actual normally enlisted save.
Qualification and missions 3..6 finish at tick 125454, menu C0FCB4/mode zero,
with zero crash resets and no pending host/game input. All 78 saved bytes agree
with the observing driver; completion count is four and the four earned grades
are one. Driver observations require raised gear in flight and lowered gear
before touchdown. This establishes playable integration for that prefix only.
Debug independently replays this same prefix and matches every reported
Release counter plus all 78 saved bytes. The compact proof now includes its
runner hash and complete counter object. The six-mission campaign remains open.

The compact verified checkpoint is
`figures/native_geared_campaign_prefix_checkpoint.json`; local input/report/logs
are in `build/native-flight/campaign-session-gear-partial-playable/`. No passing
RAM is retained. Source/runner/ADF and key hashes preserve the evidence scope.

On success the checker must replay the entire generated `session.e9k` in one
fresh `fa18_native` process with the same actual enlisted save. It requires all
saved bytes, scene counts, host events and final menu/reset counters to agree.
No such full pass has been reached yet. Diagnostic executables build in both
Release and Debug; the existing mission-five success comparison and artifact
cleanup pass in each (Release 24.85 s, Debug 49.89 s). Python syntax and both
CLI help paths pass. This validates a diagnostic-tool checkpoint, not the full
campaign or visible-performance workflow. Full Release/Debug campaign checks
remain necessary before registering an accepted continuous-campaign gate.

Cold suffix diagnostics can set `FA18_CAMPAIGN_START_MODE=7` and supply
`saved-pilot` with the actual mode-six save from the compact checkpoint. They
observe the normal loader and ordinary flight keys, emit a distinct
`diagnostic_campaign_suffix` summary and cannot satisfy the full checker.
The missile-retry suffix completes geared cruise interception, carrier wire,
save and menu; final combat subsequently crashes without an aircraft expiry.
It is not uninterrupted campaign evidence. The full Release rerun is
`campaign-session-gear-missile-retry/`; see `campaign-suffix-missile-retry/`
for the cold input diagnostics. No passing RAM is retained.

Final-flight telemetry is in `campaign-suffix-final-view-trace/`. The pilot
fires one heat missile and remains alive in combat for many updates, then fuel
and thrust stop changing while pitch falls toward the ground. Player component
damage remains zero, but that alone does not establish whether the original
interception/destruction bit at record +32 is clear. Inspect that owner and
compare the affected complete original body before changing game behavior.
The ordinary-key radar-first / higher-speed candidate
(`campaign-suffix-final-radar-speed/`) earns one aircraft expiry, then crashes
at tick 24629. Those three final-flight input changes were reverted for the
validated cruise checkpoint. No test is currently running. Final guidance and
the original destruction marker remain the next investigation.

The cruise input checkpoint retains its slower closing speed, ordinary radar
retries without the extra test-only alignment restriction, and heat missiles
before gun fallback. Actual native runtime/physics are unchanged. Both fixture
targets build in Release/Debug; invalid diagnostic suffix modes are rejected
before opening either runtime. The full and cold cruise paths plus the actual
playable five-mission replay exercise these connected input changes. The
default six-mission acceptance test remains unregistered until it passes.

```powershell
cmake --build build/native-cmake --config Release --target fa18_native_campaign_session_test --parallel 8
$env:FA18_MISSION_TRACE='1'
python tools/native/check_campaign_session.py --runner build/native-cmake/native/Release/fa18_native.exe --out build/native-flight/campaign-session-legacy-five
```

## Original destruction-frame diagnosis — 2026-10-09

The connected continuous driver now observes the first complete body setting
CONTROL_RECORDS +32 bit one, separately from component damage at +60. Its
read-only mission trace also prints each record's motion flags. The actual
earned five-mission saved file, SHA256
`94ce2bc0faef4c0bfeecdd223a8a64e96759883546f5b18e1878ca29ed521e59`,
starts the cold final-flight diagnostic in
`build/native-flight/campaign-suffix-final-destroyed/`. No flight RAM or result
state is seeded in that driver. It observes destruction at host tick 35318,
height 11850, fuel 6054134, thrust 106 and component damage zero; the route
crashes and returns to menu without a sixth saved result.

The actual Release playable runner reproduces the emitted ordinary host keys.
Raw replay iterations 19332..19336 export five complete before/after bodies;
`native_frame_body_oracle` runs the original instructions for each body's
observed timer interval. All five compare with zero gameplay and display-byte
differences under the existing scratch/voice/blitter-busy exclusions. Iteration
19334 changes player motion flags from 0 to 2. The original watch trace identifies
`C2613A: bset #1, ($20,A1)`, with word C461A4 changing 0001 to 0201.
C2612A-C26136 checks the original chaff/flare timers before this destruction;
native `flight_dynamics.c`'s collision-speed branch has the same ownership.
C13D84's destroyed branch explains the later thrust/fuel freeze. This hit is
source-matching behavior, not evidence of a native motion defect.
Release and Debug diagnostic targets build; the Debug cold suffix produces
the identical ordinary host-input replay and the same destruction event at
tick 35318, with all reported event fields matching Release. Both routes
still fail the campaign objective, as expected for this diagnostic.

The compact report and hashes are retained as
`analysis/figures/native_final_mission_destroyed_body_comparison.json`.
Only the hit's before/after/original RAM is kept compressed under the diagnostic
directory; all other passing raw captures were temporary. This original oracle
is seeded from native pre-body RAM and is **not** an independently started
original whole flight. A cold earned suffix likewise does not qualify the
uninterrupted campaign.

Three ordinary-key pilot candidates were tried and reverted:

- `campaign-suffix-final-close-defense/`: deploy on approaching missiles within
  3 km while allowing existing decoy timers to finish; destroyed at tick 29378,
  no aircraft expiry. Repeated close passes still drain flare stock.
- `campaign-suffix-final-radar-close-defense/`: radar first, minimum combat
  speed 125, decoys within 1 km; one aircraft expiry, then fuel exhausted and
  crash/menu failure at tick 41639. The second radar shot was poorly aligned.
- `campaign-suffix-final-radar-cruise-defense/`: normal minimum cruise speed 90,
  radar first, wider launch alignment and decoys within 1 km; three launches,
  no aircraft expiry, destroyed at tick 42392 and failure at tick 43084 with
  fuel still available. This improves survival duration but does not complete
  the objective.

These failed candidates are not retained in either the game or the test pilot.
Only read-only destruction telemetry is retained. Original game mechanics,
the accepted five-mission input controller and canonical playable binary are
unchanged. The next campaign work is a viable final-flight input route, followed
by a complete qualification-to-sixth-menu rerun and actual playable replay.

## Visible performance

The historical `measure_visible_performance.py` case obtains actual enlistment/qualification saves,
then runs the established demo and six earned cold mission routes in real,
visible Direct3D windows with the audio device enabled. Each mission consumes
the preceding actual saved file and must match its sealed earned result. This
measures performance across missions; it does not prove uninterrupted campaign
or independent original fidelity. Normal pacing is unchanged.

Current output: `build/native-flight/visible-performance-20261009/`.
The demo completes 10,910 frames and SDL presentations, p99 work 4.0922 ms,
maximum 544.8323 ms, one frame over 20 ms. Frame 2322, C10CFE, spends 543.2488 ms
in input polling; game work is 0.9181 ms and presentation 0.5454 ms. The cause of
that SDL input stall is unresolved. Keep the failing CSV/report and do not
exclude it, move input work out of the measurement or declare the maximum met.
SDL presentation returns do not instrument compositor scanout.

The visible mission-three case later finishes with one crash/reset. The user
explicitly reported a live gear change, which the sealed fixed-key replay does
not record; its original expected route can no longer qualify this case. Its
CSV/log remain for diagnosis, with no mission acceptance claimed. The suite
has ended; no visible measurement process is running. Generate and validate
new gear-managed input routes before relaunching mission measurements.

The current tool accepts `--campaign-report` pointing to a fully accepted
geared continuous-campaign report and verifies the runner/replay/ADF hashes.
It enlists normally, replays the entire accepted session visibly with sound,
and compares the earned save, frame/scene counts, menu and pending-input/reset
counters. Every frame is counted; per-mission scene work and observed camera
views are reported. This new workflow is only syntax/CLI checked so far:
there is no accepted continuous report to run it against yet. Historical
gear-down inputs now require `--allow-historical-gear-down-inputs`, strictly
for reproducing old failures, and are not the normal qualification path.

The script is designed to continue all six mission measurements after a budget overrun and
fails at the end if any case exceeds 20 ms. Each normally paced mission takes
about nine to fourteen minutes. A case failure retains its CSV/log. Do not
relaunch a visible suite with the old gear-down inputs. Complete reports and
inspect stall locations before deciding which focused recheck is justified.

```powershell
python tools/native/measure_visible_performance.py --campaign-report build/native-flight/campaign-session/comparison.json --runner build/native-cmake/native/Release/fa18_native.exe --out build/native-flight/visible-geared-campaign
```

Both workflows retain compact reports/inputs/CSV rather than passing RAM and
run the existing 4 GiB pruner. Workspace cache was 2.12 GiB after the committed
sort batch. `.vscode/`, sealed recordings and the protected native-check scripts
remain untouched. Independent original full flights, HUD event-time assessment,
initial startup sorting, recorded audio/filtering and remaining named-state
migration are still open.
