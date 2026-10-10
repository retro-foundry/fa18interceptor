# Original escort approach-height recording — 2026-10-10

Waiting for the validation controller's existing approach-height gate reduces
the original escort landing overshoot, but **does not earn an escort grade**.
All **65,000 observations**, complete final RAM/register bytes and consumed
keys reproduce exactly in an independent run of the unmodified original runner.
The complete earned qualification/mission-three prefix also reproduces exactly.
Native whole-flight parity and successful original escort landing remain open.

Optional `--wait-for-approach-height` enables the existing final-mission
controller's `home height + 1400` gate for the external original escort pilot.
Its ordinary keyboard choices wait for both that height and the existing
1,800-unit horizontal approach gate before turning onto final. No game physics,
clock, source state, RNG or landing rules change. Default controllers retain
their previous gate; the new field is zero-initialized in every existing caller.
The adapter and shared `MissionPilot` remain validation-only, outside the
playable runner. There are no new gameplay allocations or removed runtime
dependencies.

The actual path is `check_original_mission_recording.py -> externally built
fa18_original_mission_pilot.exe -> fa18_loop_iteration -> mission_pilot_tick ->
fa18_machine_key -> original keyboard IRQ`. It repeats held steering keys as
in [the preceding recording](native_original_escort_steering_milestone.md),
with the approach option recorded in report metadata. Mismatched retained
input profiles reject before replay.

Measured outcomes:

| Observation | Result |
| --- | --- |
| 1–45,254 | Every complete observation equals the preceding no-height-gate recording |
| 42,816 / PAL 68,379 | Original combat success, unchanged |
| 45,255 | First observation affected by the changed return input |
| 46,126 / PAL 88,277 | Final approach begins at height 1,518.015625, home height 119.03125, horizontal approach distance 1,008.354871 |
| 46,462 | Closest final pass to the carrier center: 42.42 horizontal units away, aircraft height 249.890625 |
| 46,585 | First surface contact on return, beyond the carrier |
| 65,000 | Incomplete: mode four, player phase one, one completed mission, no escort grade |

The final player is `(1136678.765625, 2.9296875, 1076518.34375)` and the carrier
is `(1136640, 0, 1071104)`. Speed is zero and contact is `8080`.
There are **3,275** repeated steering make events. The aircraft passes near the
carrier's horizontal center while still above its observed starting height,
then stops about 5,414 units beyond the center. The preceding route stopped
about 14,803 units beyond it. This narrows the external input problem to the
final descent; neither route is accepted as successful completion.

Validation includes the external runner's compile/link, complete independent
unmodified replay, exact full-prefix comparison, Python compilation and clean
diff checks. Three controls reject a wrong height profile on either retained
recording and a height option on the wrong mission. The exact unmodified
replay executable is SHA-256
`9da30b7cd2d0776c2b016077e683b50e9687f5ad43ba5e75c1e69384058c91e7`.
The playable native executable remains unchanged at SHA-256
`36cad74383f5beaad810378f03a973b8a93a9778b906655de112ffad39f80287`.

Reports, actual keys and verified losslessly compressed trace/RAM remain under
`build/native-flight/original-escort-approach-height`. The recording and replay
have trace SHA-256
`56a1774facdb81b765c2359808659c4f8a04273f976fe9983f74ee2c81c929cc`
and final RAM/register SHA-256
`69e8c9baf701806c068d13a29f6ec390806e0f1e29d2851a0c3ae9c94e760f6f`.
Raw passing captures and duplicate replay files are removed by the existing
temporary-storage workflow, and the pruner completes successfully. Prior
recordings remain unchanged. [The checkpoint](figures/native_original_escort_approach_checkpoint.json)
retains source/executable identities, outcomes, guards and evidence hashes.

```powershell
python tools/native/check_original_mission_recording.py --mode 4 --source-prefix build/native-flight/original-mission-three-patrol-runway --repeat-steering --wait-for-approach-height --out build/native-flight/original-escort-approach-height
```

Next: investigate the external controller's final descent using its actual
position, speed and source landing conditions before another complete original
recording. Independent broader full-flight and original sound onset/handoff
checks remain open. Campaign continuity remains waived; named-state cleanup
remains outside this goal.
