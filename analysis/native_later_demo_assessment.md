# Later independent demo assessment

2026-10-07, after `94127f67`. This batch extends evidence and the existing
comparison tools; it makes no playable behavior change. The complete-port
goal remains active.

Update: `native_scene_startup_milestone.md` corrects the cold bootstrap caller.
All 5,808 later complete record cores and all 363 complete record boundaries
now match. The table below records the preceding investigation; its drawing
and player/camera results remain unchanged. Original boundaries are now
compressed `.dat.gz`; the window checker reads these without a fresh replay.

## Independent segment

A focused original prefix of 6,000 PAL frames now retains source pre-input
boundaries 2401..2763, aligned with native 2364..2726 at game ticks 222..584.
The alignment comes from the already established gameplay transition, not a
search for similar pictures. Both processes start independently from their
normal state/disk/input paths; reference state never supplies native behavior.

| Checked scope | Result |
| --- | --- |
| Player motion, rates, pose, matrices, camera, phase and consumed controls | 363/363 boundaries match |
| Both complete drawing pages | 83/363 boundaries match; strict checker exits 1 |
| Named motion/rates/pose/matrices across all 16 records | 5,808/5,808 record instances match |
| Complete $A4-byte record cores, including flags/counters | 5,445/5,808 match |
| Boundaries with all 16 complete cores matching | 0/363 |

Only the player core differs: byte +04 is source 00/native 04, and the signed
countdown at +4C is sixteen updates ahead in native. At tick 222 it is source
FF1A/native FF2A. No other core byte or record differs in this segment. The
earlier takeoff snapshots already have that sixteen-update countdown lead and
bit-two flag difference. Their origin and gameplay implications remain open;
matching kinematics must not be called complete record-state parity.

The comparison tools now always report all sixteen cores separately from
their existing drawing/player/camera scope. No flag/counter exclusion is used
in that diagnostic. Cached hull vertices after +A4 remain renderer state, not
part of the core check. Mutation checks detect the player's flag/countdown and
the last record's state even when the initial pair has known core differences.

## Timed drawing difference

The first drawing difference remains tick 273's information line. The first
difference elsewhere appears at source 2687/native 2650, tick 508, including
the cached cockpit altitude. The aircraft/camera states still agree.

Source C25312's seconds-change path calls C25482 on C45891's view-hold timer.
At source tick 507, that timer has reached zero; native's is still four.
C12098 then calls C1B906 (`start_view_mode_zero`), which requests the existing
C082B8 cockpit redraw. At the next source boundary GAUGE_REFRESH=C45837 is
two, while native remains -1. The uncached altitude refresh and its two-page
cache lifecycle therefore differ. Source elapsed seconds differ because
original task/render/presentation cadence is slower. No fitted clock or
synthetic redraw was added to the game.

The actual native body at update 2649/tick 507, PAL 6148..6150, matches complete
original C0EFEA/C0F3C0 execution given the same starting state and clock
samples: zero compared gameplay or display differences. Complete C12098 and
C1C63E also match on the original tick-507 checkpoint whose view timer is
zero, including the source redraw branch. The record oracle's publication,
signed-attitude, transform and settling cases pass on that checkpoint.

These checks establish the observed timer/redraw dependency. They do not
accept all later drawing or replace the unfinished assessment at equivalent
elapsed gameplay time. Strict drawing comparisons still fail; no HUD pixels,
instrument regions or additional RAM fields were masked. Copper fade remains
the sole visual exclusion.

## Reproduction and next work

Artifacts are under `build/native-flight/demo-later-review/`, including the
source/native boundaries, `checked/comparison.json`, `record-comparison.json`,
`first-body-check.log` and `source-view-expiry-check.log`. Reuse the original
prefix rather than replaying it again:

```powershell
python tools/native/check_gameplay_window.py --runner build/native-cmake/native/Release/fa18_native.exe --source-prefix build/native-flight/demo-later-review/source --source-first 2401 --native-first 2364 --count 363 --out build/native-flight/demo-later-review/checked
```

Current strict result: exit 1, 83 drawing matches, 363 player/camera matches,
5,808 matching record cores and 363 complete record boundaries. Before the
cold-start fix it reported 5,445 and zero respectively. Existing
allocation/page/kinematic/camera rejection tests and the new core mutations
pass. No runtime build is needed for this Python-only batch; executables remain
those validated in `native_outside_camera_milestone.md`.

The player's +4C/+04 gap was traced to omitted C08EE4/C08EB8 startup calls;
their restored source order fixes it without seeding counters or clearing
flags after construction. Next assess equivalent elapsed gameplay events. Later/full
independent sequences, depleted pending recorder carry, normal mission success,
typed-state migration, audio fidelity and the 20 ms target remain unfinished.
